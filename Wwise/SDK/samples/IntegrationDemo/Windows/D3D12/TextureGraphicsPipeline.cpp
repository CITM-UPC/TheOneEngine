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
#include "TextureGraphicsPipeline.h"

#include "RenderDevice.h"

#include "DynamicTexture2D.h"

namespace d3d12
{
	namespace TextureShaders
	{
		#include <Shader_Tex2DSimpleVs.h>
		#include <Shader_Tex2DSimplePs.h>
	}

	void TextureGraphicsPipeline::Init()
	{
		auto device = RenderDevice::Instance().GetDevice();

		auto vertexShader = CD3DX12_SHADER_BYTECODE(TextureShaders::g_VsMain, sizeof(TextureShaders::g_VsMain) / sizeof(BYTE));
		auto pixelShader = CD3DX12_SHADER_BYTECODE(TextureShaders::g_PsMain, sizeof(TextureShaders::g_PsMain) / sizeof(BYTE));

		// Create root signature from compiled vertex shader RootSignature attribute
		ThrowIfFailed(device->CreateRootSignature(0, vertexShader.pShaderBytecode,
			vertexShader.BytecodeLength, PLATFORM_IDD_PPV_ARGS(m_rootSignature.GetAddressOf())));

		static const D3D12_INPUT_ELEMENT_DESC inputLayoutDesc[] = {
			{ "POS2D",      0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // float2
			{ "TEXCOORD",   0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // float2
		};

		CD3DX12_BLEND_DESC blendDesc(CD3DX12_DEFAULT{});
		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = { inputLayoutDesc, _countof(inputLayoutDesc) };
		psoDesc.pRootSignature = m_rootSignature.Get();
		psoDesc.VS = vertexShader;
		psoDesc.PS = pixelShader;
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState = blendDesc;
		psoDesc.DepthStencilState.DepthEnable = FALSE;
		psoDesc.DepthStencilState.StencilEnable = FALSE;
		psoDesc.DSVFormat = {}; // dunno this one
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;

		ThrowIfFailed(
			device->CreateGraphicsPipelineState(&psoDesc,
				PLATFORM_IDD_PPV_ARGS(m_pipelineState.ReleaseAndGetAddressOf())));
	}

	void TextureGraphicsPipeline::Term()
	{
		m_rootSignature.Reset();
		m_pipelineState.Reset();

		for (auto& buffer : m_vertexBuffers)
			buffer.Reset();

		for (auto& buffer : m_intermediateVertexBuffers)
			buffer.Reset();
	}

	void TextureGraphicsPipeline::SendDrawCommands()
	{
		if (m_vertices.empty())
			return;

		auto& device = RenderDevice::Instance();
		auto* commandList = device.GetCurrentCommandList();

		UINT frameIndex = device.GetCommandListIndex();
		auto& vertexBuffer = m_vertexBuffers[frameIndex];
		auto& intermediateVertexBuffer = m_intermediateVertexBuffers[frameIndex];
		auto& vertexBufferView = m_vertexBufferViews[frameIndex];

		// Upload vertex data
		{
			device.UpdateBufferResource(commandList,
				&vertexBuffer, &intermediateVertexBuffer,
				m_vertices.size(), sizeof(TriangleVertex), m_vertices.data());

			vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
			vertexBufferView.SizeInBytes = static_cast<UINT>(m_vertices.size() * sizeof(TriangleVertex));
			vertexBufferView.StrideInBytes = sizeof(TriangleVertex);

			CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
				vertexBuffer.Get(),
				D3D12_RESOURCE_STATE_COPY_DEST,
				D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
			);
			commandList->ResourceBarrier(1, &barrier);
		}

		commandList->SetPipelineState(m_pipelineState.Get());
		commandList->SetGraphicsRootSignature(m_rootSignature.Get());
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->IASetVertexBuffers(0, 1, &vertexBufferView);

		UINT vertexOffset = 0;
		for (auto* texture : m_textures)
		{
			auto* srvDescriptorHeap = texture->GetDescriptorHeap();

			ID3D12DescriptorHeap* heaps[] = { srvDescriptorHeap };
			commandList->SetDescriptorHeaps(1, heaps);
			commandList->SetGraphicsRootDescriptorTable(0, srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

			commandList->DrawInstanced(static_cast<UINT>(kNbVertexPerTexture), 1, vertexOffset, 0);
			vertexOffset += kNbVertexPerTexture;
		}
	}
}
