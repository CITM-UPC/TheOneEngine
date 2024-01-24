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
#include "DynamicTexture2D.h"

#include "RenderDevice.h"

namespace d3d12
{
	DynamicTexture2D::DynamicTexture2D(AkUInt32 width, AkUInt32 height)
		: m_uRefCnt(0)
		, m_width(width)
		, m_height(height)
	{
		auto device = RenderDevice::Instance().GetDevice();

		// Create Descriptor Heap
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		ThrowIfFailed(device->CreateDescriptorHeap(&dsvHeapDesc, PLATFORM_IDD_PPV_ARGS(m_SRVDescriptorHeap.GetAddressOf())));

		// Describe and create a Texture2D.
		D3D12_RESOURCE_DESC textureDesc = {};
		textureDesc.MipLevels = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.Width = m_width;
		textureDesc.Height = m_height;
		textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		textureDesc.DepthOrArraySize = 1;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

		ThrowIfFailed(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&textureDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			nullptr,
			PLATFORM_IDD_PPV_ARGS(m_texture.GetAddressOf())));

		m_texture->SetName(L"Texture");

		// Create the GPU upload buffer.
		const UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_texture.Get(), 0, 1);

		ThrowIfFailed(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			PLATFORM_IDD_PPV_ARGS(m_textureUploadHeap.GetAddressOf())));

		// Describe and create a Shader Resource View for the texture.
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		device->CreateShaderResourceView(m_texture.Get(), &srvDesc, m_SRVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

		m_textureUploadHeap->SetName(L"TextureUploadHeap");
	}

	void DynamicTexture2D::Release()
	{
		--m_uRefCnt;
		if (m_uRefCnt == 0)
		{
			RenderDevice::Instance().ScheduleResourceForDestruction(m_texture.Detach());
			RenderDevice::Instance().ScheduleResourceForDestruction(m_textureUploadHeap.Detach());
			RenderDevice::Instance().ScheduleResourceForDestruction(m_SRVDescriptorHeap.Detach());
		}
	}

	void DynamicTexture2D::UpdateResource(const void* buffer, size_t bufferPitch, size_t bufferLength)
	{
		auto commandList = RenderDevice::Instance().GetCurrentCommandList();

		{
			CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
				m_texture.Get(),
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
				D3D12_RESOURCE_STATE_COPY_DEST
			);
			commandList->ResourceBarrier(1, &barrier);
		}

		D3D12_SUBRESOURCE_DATA textureData = {};
		textureData.pData = buffer;
		textureData.RowPitch = m_width * 4L; // R8G8B8A8
		textureData.SlicePitch = textureData.RowPitch * m_height;

		UpdateSubresources(commandList, m_texture.Get(), m_textureUploadHeap.Get(), 0, 0, 1, &textureData);

		{
			CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
				m_texture.Get(),
				D3D12_RESOURCE_STATE_COPY_DEST,
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
			);
			commandList->ResourceBarrier(1, &barrier);
		}
	}
}
