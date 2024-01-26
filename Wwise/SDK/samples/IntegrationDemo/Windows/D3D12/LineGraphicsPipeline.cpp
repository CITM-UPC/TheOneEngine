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
#include "LineGraphicsPipeline.h"

#include "RenderDevice.h"

namespace d3d12
{
	namespace LineShaders
	{
		#include <Shader_Line2DSimpleVs.h>
		#include <Shader_Line2DSimplePs.h>
	}

	void LineGraphicsPipeline::Init()
	{
		auto device = RenderDevice::Instance().GetDevice();

		auto vertexShader = CD3DX12_SHADER_BYTECODE(LineShaders::g_VsMain, sizeof(LineShaders::g_VsMain) / sizeof(BYTE));
		auto pixelShader = CD3DX12_SHADER_BYTECODE(LineShaders::g_PsMain, sizeof(LineShaders::g_PsMain) / sizeof(BYTE));

		// Create root signature from compiled vertex shader RootSignature attribute
		ThrowIfFailed(device->CreateRootSignature(0, vertexShader.pShaderBytecode,
			vertexShader.BytecodeLength, PLATFORM_IDD_PPV_ARGS(m_rootSignature.GetAddressOf())));

		const D3D12_INPUT_ELEMENT_DESC inputLayoutDesc[] = {
			{ "POS2D",   0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // float2
			{ "COLOR",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // float3
		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = { inputLayoutDesc, _countof(inputLayoutDesc) };
		psoDesc.pRootSignature = m_rootSignature.Get();
		psoDesc.VS = vertexShader;
		psoDesc.PS = pixelShader;
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState = CD3DX12_BLEND_DESC(CD3DX12_DEFAULT{});
		psoDesc.DepthStencilState.DepthEnable = FALSE;
		psoDesc.DepthStencilState.StencilEnable = FALSE;
		psoDesc.DSVFormat = {};
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;

		ThrowIfFailed(
			device->CreateGraphicsPipelineState(&psoDesc,
				PLATFORM_IDD_PPV_ARGS(m_pipelineState.ReleaseAndGetAddressOf())));
	}

	void LineGraphicsPipeline::Term()
	{
		m_rootSignature.Reset();
		m_pipelineState.Reset();

		for (auto& buffer : m_vertexBuffers)
			buffer.Reset();

		for (auto& buffer : m_intermediateVertexBuffers)
			buffer.Reset();
	}

	void LineGraphicsPipeline::SendDrawCommands()
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
		device.UpdateBufferResource(commandList,
			&vertexBuffer, &intermediateVertexBuffer,
			m_vertices.size(), sizeof(LineVertex), m_vertices.data());

		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			vertexBuffer.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
		);
		commandList->ResourceBarrier(1, &barrier);

		vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
		vertexBufferView.SizeInBytes = static_cast<UINT>(m_vertices.size() * sizeof(LineVertex));
		vertexBufferView.StrideInBytes = sizeof(LineVertex);

		commandList->SetPipelineState(m_pipelineState.Get());
		commandList->SetGraphicsRootSignature(m_rootSignature.Get());
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
		commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
		commandList->DrawInstanced(static_cast<UINT>(m_vertices.size()), 1, 0, 0);
	}
}
