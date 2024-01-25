/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

  Copyright (c) 2023 Audiokinetic Inc.
*******************************************************************************/
#include "stdafx.h"
#include "RenderDevice.h"

#include "CommandQueue.h"

using namespace Microsoft::WRL;

namespace
{
	static void EnableDebugLayer()
	{
#ifndef AK_OPTIMIZED
		ComPtr<ID3D12Debug> debugInterface;
		ThrowIfFailed(D3D12GetDebugInterface(PLATFORM_IDD_PPV_ARGS(debugInterface.GetAddressOf())));
		debugInterface->EnableDebugLayer();
#endif
	}
}

namespace d3d12
{
	RenderDevice& RenderDevice::Instance()
	{
#ifdef AK_WIN
		static RenderDeviceWindows instance;
#elif defined(AK_XBOXGC)
		static RenderDeviceXboxGC instance;
#else
#error "No D3D device definition for this platform!"
#endif
		return instance;
	}

	bool RenderDevice::Init(void* data)
	{
		m_device = CreateDevice();

		m_commandQueue = std::make_unique<CommandQueue>(D3D12_COMMAND_LIST_TYPE_DIRECT, RenderingSettings::kConcurrentFrameCount);
		m_resourcesToDestroy.resize(RenderingSettings::kConcurrentFrameCount);

		CreateSwapChain();

		// Create Descriptor Heap
		m_RTVDescriptorHeap = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, kNbBackBuffers);
		m_RTVDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		
		UpdateRenderTargetViews();

		return true;
	}

	void RenderDevice::Term()
	{
		DestroyObsoleteResources();

		m_frameCommandList.Reset();
		m_commandQueue.reset();
		
		m_RTVDescriptorHeap.Reset();

		for (auto& backBuffer : m_backBuffers)
			backBuffer.Reset();

		m_device.Reset();
	}

	void RenderDevice::UpdateBufferResource(
		ComPtr<ID3D12GraphicsCommandList> commandList,
		ID3D12Resource** pDestinationResource,
		ID3D12Resource** pIntermediateResource,
		size_t numElements, size_t elementSize, const void* bufferData,
		D3D12_RESOURCE_FLAGS flags)
	{
		size_t bufferSize = numElements * elementSize;

		// Create a committed resource for the GPU resource in a default heap.
		ThrowIfFailed(m_device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(bufferSize, flags),
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			PLATFORM_IDD_PPV_ARGS(pDestinationResource)));

		// Create a committed resource for the upload.
		if (bufferData)
		{
			ThrowIfFailed(m_device->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				PLATFORM_IDD_PPV_ARGS(pIntermediateResource)));

			D3D12_SUBRESOURCE_DATA subresourceData = {};
			subresourceData.pData = bufferData;
			subresourceData.RowPitch = bufferSize;
			subresourceData.SlicePitch = subresourceData.RowPitch;

			UpdateSubresources(commandList.Get(),
				*pDestinationResource, *pIntermediateResource,
				0, 0, 1, &subresourceData);
		}
	}

	ComPtr<ID3D12DescriptorHeap> RenderDevice::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors)
	{
		ComPtr<ID3D12DescriptorHeap> descriptorHeap;

		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.NumDescriptors = numDescriptors;
		desc.Type = type;

		ThrowIfFailed(m_device->CreateDescriptorHeap(&desc, PLATFORM_IDD_PPV_ARGS(descriptorHeap.GetAddressOf())));

		return descriptorHeap;
	}

	void RenderDevice::BeginFrame()
	{
		m_frameCommandList = m_commandQueue->CreateCommandList();
		DestroyObsoleteResources();
	}

	void RenderDevice::RenderBegin()
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(
			m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			m_currentBackBufferIndex,
			m_RTVDescriptorSize
		);

		// Set mandatory state
		CD3DX12_VIEWPORT viewport{ 0.0f, 0.0f, static_cast<float>(m_clientWidth), static_cast<float>(m_clientHeight) };
		CD3DX12_RECT scissorRect{ 0, 0, static_cast<LONG>(m_clientWidth), static_cast<LONG>(m_clientWidth) };
		m_frameCommandList->RSSetViewports(1, &viewport);
		m_frameCommandList->RSSetScissorRects(1, &scissorRect);

		// Clear the render target.
		{
			CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
				m_backBuffers[m_currentBackBufferIndex].Get(),
				D3D12_RESOURCE_STATE_PRESENT,
				D3D12_RESOURCE_STATE_RENDER_TARGET
			);
			m_frameCommandList->ResourceBarrier(1, &barrier);

			// Set render target
			m_frameCommandList->OMSetRenderTargets(1, &rtv, FALSE, nullptr);

			// Clear
			FLOAT clearColor[] = { 0.0f, 0.15f, 0.40f, 1.0f };
			m_frameCommandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
		}
	}

	void RenderDevice::RenderEnd()
	{
		PreparePresent();
		Present();
		MoveToNextFrame();
	}

	void RenderDevice::PreparePresent()
	{
		// Prepare back buffer for presentation
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			m_backBuffers[m_currentBackBufferIndex].Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT
		);
		m_frameCommandList->ResourceBarrier(1, &barrier);

		// and send GPU commands to render the frame
		m_commandQueue->ExecuteCommandList();
	}

	void RenderDevice::MoveToNextFrame()
	{
		// Move to the next frame
		m_currentBackBufferIndex = (m_currentBackBufferIndex + 1) % kNbBackBuffers;
	}

	void RenderDevice::DestroyObsoleteResources()
	{
		auto index = GetCommandListIndex();
		auto& commandListResources = m_resourcesToDestroy[index];

		for (auto resource : commandListResources)
			resource.Reset();

		commandListResources.clear();
	}

#ifdef AK_WIN
	/////////////////////////////////////////////////////////////////////////////////////
	//                                WINDOWS RENDERING                                //
	/////////////////////////////////////////////////////////////////////////////////////

	// Windows rendering uses DXGI swapchains

	bool RenderDeviceWindows::Init(void* data)
	{
		m_hWnd = reinterpret_cast<HWND>(data);

		RECT rect;
		::GetClientRect(m_hWnd, &rect);
		m_clientWidth = rect.right - rect.left;
		m_clientHeight = rect.bottom - rect.top;

		return RenderDevice::Init(data);
	}

	void RenderDeviceWindows::Term()
	{
		m_swapChain.Reset();
		return RenderDevice::Term();
	}

	ComPtr<ID3D12Device> RenderDeviceWindows::CreateDevice()
	{
		EnableDebugLayer();

		auto adapter = GetAdapter();

		ComPtr<ID3D12Device3> device;
		ThrowIfFailed(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, PLATFORM_IDD_PPV_ARGS(&device)));
		EnableDebugMessages(device);

		return device;
	}

	void RenderDeviceWindows::EnableDebugMessages(ComPtr<ID3D12Device3> device)
	{
	#ifndef AK_OPTIMIZED
		ComPtr<ID3D12InfoQueue> pInfoQueue;
		if (SUCCEEDED(device.As(&pInfoQueue)))
		{
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

			// Suppress whole categories of messages
			//D3D12_MESSAGE_CATEGORY Categories[] = {};

			// Suppress messages based on their severity level
			D3D12_MESSAGE_SEVERITY Severities[] =
			{
				D3D12_MESSAGE_SEVERITY_INFO
			};

			// Suppress individual messages by their ID
			D3D12_MESSAGE_ID DenyIds[] = {
				D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,   // I'm really not sure how to avoid this message.
				D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         // This warning occurs when using capture frame while graphics debugging.
				D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       // This warning occurs when using capture frame while graphics debugging.
			};

			D3D12_INFO_QUEUE_FILTER NewFilter = {};
			//NewFilter.DenyList.NumCategories = _countof(Categories);
			//NewFilter.DenyList.pCategoryList = Categories;
			NewFilter.DenyList.NumSeverities = _countof(Severities);
			NewFilter.DenyList.pSeverityList = Severities;
			NewFilter.DenyList.NumIDs = _countof(DenyIds);
			NewFilter.DenyList.pIDList = DenyIds;

			ThrowIfFailed(pInfoQueue->PushStorageFilter(&NewFilter));
		}
	#endif
	}

	ComPtr<IDXGIAdapter1> RenderDeviceWindows::GetAdapter()
	{
		ComPtr<IDXGIFactory4> dxgiFactory;
		UINT createFactoryFlags = 0;
	#ifndef AK_OPTIMIZED
		createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
	#endif

		ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory)));

		ComPtr<IDXGIAdapter1> dxgiAdapter;

		SIZE_T maxDedicatedVideoMemory = 0;
		for (UINT i = 0; dxgiFactory->EnumAdapters1(i, &dxgiAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
		{
			DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
			dxgiAdapter->GetDesc1(&dxgiAdapterDesc1);

			// Check to see if the adapter can create a D3D12 device without actually 
			// creating it. The adapter with the largest dedicated video memory
			// is favored.
			if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
				SUCCEEDED(D3D12CreateDevice(dxgiAdapter.Get(),
					D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)) &&
				dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory)
			{
				maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
			}
		}

		return dxgiAdapter;
	}

	void RenderDeviceWindows::CreateSwapChain()
	{
		ComPtr<IDXGIFactory4> dxgiFactory4;
		UINT createFactoryFlags = 0;
	#ifndef AK_OPTIMIZED
		createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
	#endif

		ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, PLATFORM_IDD_PPV_ARGS(&dxgiFactory4)));

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = m_clientWidth;
		swapChainDesc.Height = m_clientHeight;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.Stereo = FALSE;
		swapChainDesc.SampleDesc = { 1, 0 };
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = kNbBackBuffers;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		swapChainDesc.Flags = 0;

		ComPtr<IDXGISwapChain1> swapChain1;
		ThrowIfFailed(dxgiFactory4->CreateSwapChainForHwnd(
			m_commandQueue->GetD3D12CommandQueue(),
			m_hWnd,
			&swapChainDesc,
			nullptr,
			nullptr,
			&swapChain1)
		);

		ThrowIfFailed(swapChain1.As(&m_swapChain));

		m_currentBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
	}

	void RenderDeviceWindows::UpdateRenderTargetViews()
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

		for (size_t i = 0; i < m_backBuffers.size(); ++i)
		{
			auto& backBuffer = m_backBuffers[i];

			ThrowIfFailed(m_swapChain->GetBuffer((AkUInt32)i, PLATFORM_IDD_PPV_ARGS(backBuffer.GetAddressOf())));

			m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);

			backBuffer->SetName(L"BackBuffer");

			rtvHandle.Offset(m_RTVDescriptorSize);
		}
	}

	void RenderDeviceWindows::Present()
	{
		ThrowIfFailed(m_swapChain->Present(0, 0));
	}

#elif defined(AK_XBOXGC)
	////////////////////////////////////////////////////////////////////////////////
	//                             XboxOneGC RENDERING                            //
	////////////////////////////////////////////////////////////////////////////////

	void RenderDeviceXboxGC::Term()
	{
		m_commandQueue->GetD3D12CommandQueue()->PresentX(0, nullptr, nullptr);
		RenderDevice::Term();
	}

	void RenderDeviceXboxGC::Suspend()
	{
		m_commandQueue->GetD3D12CommandQueue()->SuspendX(0);
	}

	void RenderDeviceXboxGC::Resume()
	{
		m_commandQueue->GetD3D12CommandQueue()->ResumeX();
		CreateSwapChain();
	}

	ComPtr<ID3D12Device> RenderDeviceXboxGC::CreateDevice()
	{
		ComPtr<ID3D12Device2> device;

		// Create the DX12 API device object.
		D3D12XBOX_CREATE_DEVICE_PARAMETERS params = {};
		params.Version = D3D12_SDK_VERSION;

#ifdef AK_OPTIMIZED
		// Enable the debug layer.
		params.ProcessDebugFlags = D3D12_PROCESS_DEBUG_FLAG_DEBUG_LAYER_ENABLED;
#endif

		params.GraphicsCommandQueueRingSizeBytes = static_cast<UINT>(D3D12XBOX_DEFAULT_SIZE_BYTES);
		params.GraphicsScratchMemorySizeBytes = static_cast<UINT>(D3D12XBOX_DEFAULT_SIZE_BYTES);
		params.ComputeScratchMemorySizeBytes = static_cast<UINT>(D3D12XBOX_DEFAULT_SIZE_BYTES);

		ThrowIfFailed(D3D12XboxCreateDevice(
			nullptr,
			&params,
			IID_GRAPHICS_PPV_ARGS(device.ReleaseAndGetAddressOf())
		));
		device->SetName(L"RendererXboxOneGC");

		return device;
	}

	// Register frame events
	void RenderDeviceXboxGC::CreateSwapChain()
	{
		// First, retrieve the underlying DXGI device from the D3D device.
		ComPtr<IDXGIDevice1> dxgiDevice;
		ThrowIfFailed(m_device.As(&dxgiDevice));

		// Identify the physical adapter (GPU or card) that this device is running on.
		ComPtr<IDXGIAdapter> dxgiAdapter;
		ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));

		// Retrieve the outputs for the adapter.
		ComPtr<IDXGIOutput> dxgiOutput;
		ThrowIfFailed(dxgiAdapter->EnumOutputs(0, dxgiOutput.GetAddressOf()));

		// Set the frame interval, and register for frame events.
		ThrowIfFailed(m_device->SetFrameIntervalX(
			dxgiOutput.Get(),
			D3D12XBOX_FRAME_INTERVAL_60_HZ,
			RenderingSettings::kConcurrentFrameCount,
			D3D12XBOX_FRAME_INTERVAL_FLAG_NONE));

		ThrowIfFailed(m_device->ScheduleFrameEventX(
			D3D12XBOX_FRAME_EVENT_ORIGIN,
			0,
			nullptr,
			D3D12XBOX_SCHEDULE_FRAME_EVENT_FLAG_NONE));

		m_currentBackBufferIndex = 0;
	}

	void RenderDeviceXboxGC::UpdateRenderTargetViews()
	{
		CD3DX12_HEAP_PROPERTIES swapChainHeapProperties(D3D12_HEAP_TYPE_DEFAULT);

		D3D12_RESOURCE_DESC swapChainBufferDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_R8G8B8A8_UNORM,
			m_clientWidth,
			m_clientHeight,
			1, // This resource has only one texture.
			1  // Use a single mipmap level.
		);
		swapChainBufferDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		D3D12_CLEAR_VALUE swapChainOptimizedClearValue = {};
		swapChainOptimizedClearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		for (int i = 0; i < kNbBackBuffers; ++i)
		{
			ThrowIfFailed(m_device->CreateCommittedResource(
				&swapChainHeapProperties,
				D3D12_HEAP_FLAG_ALLOW_DISPLAY,
				&swapChainBufferDesc,
				D3D12_RESOURCE_STATE_PRESENT,
				&swapChainOptimizedClearValue,
				PLATFORM_IDD_PPV_ARGS(m_backBuffers[i].GetAddressOf())));

			D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
			rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), i, m_RTVDescriptorSize);

			m_device->CreateRenderTargetView(m_backBuffers[i].Get(), &rtvDesc, rtvHandle);

			m_backBuffers[i]->SetName(L"BackBuffer");
		}
	}

	void RenderDeviceXboxGC::BeginFrame()
	{
		// Throttle
		m_framePipelineToken = D3D12XBOX_FRAME_PIPELINE_TOKEN_NULL;
		ThrowIfFailed(m_device->WaitFrameEventX(
			D3D12XBOX_FRAME_EVENT_ORIGIN,
			D3D12XBOX_WAIT_FRAME_EVENT_INFINITE,
			nullptr,
			D3D12XBOX_WAIT_FRAME_EVENT_FLAG_NONE,
			&m_framePipelineToken
		));

		RenderDevice::BeginFrame();
	}

	void RenderDeviceXboxGC::Present()
	{
		D3D12XBOX_PRESENT_PLANE_PARAMETERS planeParameters = {};
		planeParameters.Token = m_framePipelineToken;
		planeParameters.ResourceCount = 1;
		planeParameters.ppResources = m_backBuffers[m_currentBackBufferIndex].GetAddressOf();

		// Present
		ThrowIfFailed(m_commandQueue->GetD3D12CommandQueue()->PresentX(1, &planeParameters, nullptr));
	}

#else
#error "No D3D device definition for this platform!"
#endif
}
