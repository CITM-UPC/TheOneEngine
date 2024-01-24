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

#include "Render.h"

#include "Utilities.h"

namespace d3d12
{
	class DynamicTexture2D : public IDynamicTexture2D
	{
	public:
		DynamicTexture2D(AkUInt32 width, AkUInt32 height);

		void UpdateResource(const void* buffer, size_t bufferPitch, size_t bufferLength);

		// When refcount is 0, the surface will be unavailable for further CPU usage,
		// but destruction of resource is deferred to end-of-frame
		void AddRef() override { ++m_uRefCnt; }
		void Release() override; // Schedules deferred destruction

		ID3D12DescriptorHeap* GetDescriptorHeap() { return m_SRVDescriptorHeap.Get(); }

	private:
		AkUInt32 m_width = 0;
		AkUInt32 m_height = 0;

		Microsoft::WRL::ComPtr<ID3D12Resource> m_texture;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_textureUploadHeap;

		// Shader Resource Views descriptors
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_SRVDescriptorHeap{};

		AkUInt32 m_uRefCnt;
	};
}
