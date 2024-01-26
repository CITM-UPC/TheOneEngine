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
#pragma once

///////////////////////////
//  BACKEND CORE UTILITY //
///////////////////////////

#include "Utilities.h"
#include "CommandQueue.h"

#include <vector>
#include <memory>

namespace d3d12
{
	class RenderDevice
	{
	private:
		const wchar_t* kAppName = L"IntegrationDemo";

	public:
		static const uint32_t kNbBackBuffers = 3;

		static RenderDevice& Instance();

		virtual bool Init(void* data);
		
		void WaitForFrame() { m_commandQueue->Flush(); }
		
		virtual void Term();
		
		// Should be called before sending GPU commands for a new frame
		virtual void BeginFrame();

		// Should be called after BeginFrame, before sending draw calls
		virtual void RenderBegin();

		// Should be called after draw calls to submit commands and present the frame buffer
		virtual void RenderEnd();

	public:
		Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() { return m_device; }

		// ------ Render commands for current frame ------ //

		ID3D12GraphicsCommandList* GetCurrentCommandList() { return m_frameCommandList.Get(); }

		UINT GetCommandListIndex() const { return m_commandQueue->GetCommandListIndex(); }

		// ------ Resource management ------ //

		void ScheduleResourceForDestruction(Microsoft::WRL::ComPtr<IGraphicsUnknown> resource)
		{
			// Resources used by a command list are destroyed
			// when command list execution is complete
			auto index = GetCommandListIndex();
			m_resourcesToDestroy[index].push_back(resource);
		}

		void UpdateBufferResource(
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList,
			ID3D12Resource** pDestinationResource,
			ID3D12Resource** pIntermediateResource,
			size_t numElements, size_t elementSize, const void* bufferData,
			D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);

	protected:
		// ------ Init ------ //
		
		virtual Microsoft::WRL::ComPtr<ID3D12Device> CreateDevice() = 0;
		virtual void CreateSwapChain() = 0;
		virtual void UpdateRenderTargetViews() = 0;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors);

		// ------ Render ------ //

		void DestroyObsoleteResources();
		virtual void PreparePresent();
		virtual void Present() = 0;
		virtual void MoveToNextFrame();

	protected:

		uint32_t m_clientWidth = 1920;
		uint32_t m_clientHeight = 1080;

		// Associate resources used by a command list to destroy them when execution is complete
		std::vector<std::vector<Microsoft::WRL::ComPtr<IGraphicsUnknown>>> m_resourcesToDestroy;

		Microsoft::WRL::ComPtr<ID3D12Device> m_device{};

		UINT m_currentBackBufferIndex{};
		std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, kNbBackBuffers> m_backBuffers = {}; // render targets

		// Render Target Views descriptors
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RTVDescriptorHeap{};
		UINT m_RTVDescriptorSize{};

		std::unique_ptr<CommandQueue> m_commandQueue{};
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_frameCommandList; // for the current frame
	};

#ifdef AK_WIN
	class RenderDeviceWindows : public RenderDevice
	{
	public:
		// ------ Init ------ //

		bool Init(void* data) override;

		void Term() override;

	protected:
		// Device Creation
		ComPtr<ID3D12Device> CreateDevice() override;

		void CreateSwapChain() override;

		// Use swapchain backbuffer as a render target
		void UpdateRenderTargetViews() override;

		// ------ Render ------ //

		// Present using DXGI swapchain
		void Present() override;

	private:
		ComPtr<IDXGIAdapter1> GetAdapter();

		// Windows uses DXGI swapchains
		ComPtr<IDXGISwapChain4> CreateSwapChain(uint32_t width, uint32_t height, uint32_t bufferCount);

		void EnableDebugMessages(ComPtr<ID3D12Device3> device);

	private:
		ComPtr<IDXGISwapChain4> m_swapChain{};
		HWND m_hWnd{};
	};
#elif defined(AK_XBOXGC)
	class RenderDeviceXboxGC : public RenderDevice
	{
	public:
		// ------ Init ------ //

		void Term() override;

		void Suspend();

		void Resume();

	protected:

		Microsoft::WRL::ComPtr<ID3D12Device> CreateDevice() override;

		void CreateSwapChain() override;

		// Use commited resources as a render target
		void UpdateRenderTargetViews() override;

		// ------ Render ------ //

		void BeginFrame() override;

		// Present using PresentX API
		void Present() override;

	private:
		D3D12XBOX_FRAME_PIPELINE_TOKEN m_framePipelineToken = D3D12XBOX_FRAME_PIPELINE_TOKEN_NULL;
	};
#else
#error "No D3D device definition for this platform!"
#endif
}
