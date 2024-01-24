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

#include <vector>

namespace d3d12
{
	class DynamicTexture2D;

	// Graphics pipeline to draw textures using Shader_Tex2DSimple*.hlsl
	class TextureGraphicsPipeline
	{
	public:
		void Init();

		void Term();

		void SendDrawCommands();

		void AddTexture(DrawRect src, DrawRect dest, DynamicTexture2D* texture)
		{
			m_vertices.push_back({ { dest.x1, dest.y1 }, { src.x1, src.y1 } });
			m_vertices.push_back({ { dest.x2, dest.y1 }, { src.x2, src.y1 } });
			m_vertices.push_back({ { dest.x2, dest.y2 }, { src.x2, src.y2 } });
			m_vertices.push_back({ { dest.x2, dest.y2 }, { src.x2, src.y2 } });
			m_vertices.push_back({ { dest.x1, dest.y2 }, { src.x1, src.y2 } });
			m_vertices.push_back({ { dest.x1, dest.y1 }, { src.x1, src.y1 } });

			m_textures.push_back(texture);
		}

		void ClearTextures()
		{
			m_vertices.clear();
			m_textures.clear();
		}

	private:
		// Root signature
		ComPtr<ID3D12RootSignature> m_rootSignature;

		// Pipeline state object.
		ComPtr<ID3D12PipelineState> m_pipelineState;

		// Vertex buffer (one per in-flight frame)
		ComPtr<ID3D12Resource> m_vertexBuffers[RenderingSettings::kConcurrentFrameCount];
		ComPtr<ID3D12Resource> m_intermediateVertexBuffers[RenderingSettings::kConcurrentFrameCount];
		D3D12_VERTEX_BUFFER_VIEW m_vertexBufferViews[RenderingSettings::kConcurrentFrameCount];

		static constexpr int kNbVertexPerTexture = 6; // We only support rendering textures on rectangles

		struct TriangleVertex
		{
			vec2 pos;
			vec2 texCoord;
		};

		std::vector<TriangleVertex> m_vertices;
		std::vector<DynamicTexture2D*> m_textures;
	};
}
