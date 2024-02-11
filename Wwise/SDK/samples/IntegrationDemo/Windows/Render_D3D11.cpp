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

// Drawing.cpp
/// \file 
// Implements the Drawing.h functions for the Windows platform


/////////////////////////
//  INCLUDES
/////////////////////////

#include "stdafx.h"

#include "Platform.h"
#include "Render.h"

#include <d3d11.h>
#include <vector>

#if defined(INTDEMO_RENDER_D3D11)

/////////////////////////
//  BACKEND CORE
/////////////////////////

namespace d3d11Core
{
	ID3D11Device* g_pDevice = nullptr;  // The D3D11 Device Object
	ID3D11DeviceContext* g_pDeviceCtx = nullptr;  // The D3D11 context
	std::vector<IUnknown*> g_resourceDestructionQueue; // List of device resources to be destroyed on frame completion 
	
	IDXGISwapChain* g_pSwapChain = nullptr; // swap chain
	ID3D11Texture2D* g_pFrameRtTexture = nullptr; // D3D11 Texture representing the swapchain's backbuffer
	ID3D11RenderTargetView* g_pFrameRtv = nullptr; // D3D11 RTV wrapping the backbufferTexture
	ID3D11Texture2D* g_pFrameDsTexture = nullptr; // D3D11 depth-stencil texture for the frame
	ID3D11DepthStencilView* g_pFrameDsv = nullptr; // D3D11 DSV wrapping the FrameDsTexture
	AkUInt32 g_iFrameWidth = 0;
	AkUInt32 g_iFrameHeight = 0;

	bool Init(void* in_pParam, AkOSChar* in_szErrorBuffer, unsigned int in_unErrorBufferCharCount)
	{
		HWND hWnd = (HWND)(in_pParam);

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		memset(&swapChainDesc, 0, sizeof(DXGI_SWAP_CHAIN_DESC));
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.OutputWindow = hWnd;
		swapChainDesc.Windowed = true;

		UINT deviceFlags = 0;
#if 0
		deviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D_FEATURE_LEVEL featureLevels[] = {
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
		};

		HRESULT hr = D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			deviceFlags,
			featureLevels,
			sizeof(featureLevels) / sizeof(D3D_FEATURE_LEVEL),
			D3D11_SDK_VERSION,
			&swapChainDesc,
			&g_pSwapChain,
			&g_pDevice,
			nullptr,
			&g_pDeviceCtx
		);

		if (FAILED(hr))
		{
			if (in_szErrorBuffer)
				__AK_OSCHAR_SNPRINTF(in_szErrorBuffer, in_unErrorBufferCharCount, AKTEXT("D3D11CreateDeviceAndSwapChain() returned HRESULT %d"), hr);
		
			RenderTerm();
			return false;
		}

		// Fetch Texture2D and RTV for swapchain backbuffer
		{
			hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&g_pFrameRtTexture);
			AKASSERT(SUCCEEDED(hr));
			hr = d3d11Core::g_pDevice->CreateRenderTargetView(g_pFrameRtTexture, nullptr, &g_pFrameRtv);
			AKASSERT(SUCCEEDED(hr));

			D3D11_TEXTURE2D_DESC desc;
			g_pFrameRtTexture->GetDesc(&desc);
			g_iFrameWidth = desc.Width;
			g_iFrameHeight = desc.Height;
		}

		// Create Texture2D and DSV for frame depth-stencil
		{
			D3D11_TEXTURE2D_DESC desc;
			memset(&desc, 0, sizeof(D3D11_TEXTURE2D_DESC));
			desc.Width = g_iFrameWidth;
			desc.Height = g_iFrameHeight;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			desc.SampleDesc.Count = 1;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			desc.CPUAccessFlags = 0;
			HRESULT hr = g_pDevice->CreateTexture2D(&desc, nullptr, &g_pFrameDsTexture);
			AKASSERT(SUCCEEDED(hr));
			hr = g_pDevice->CreateDepthStencilView(g_pFrameDsTexture, nullptr, &g_pFrameDsv);
			AKASSERT(SUCCEEDED(hr));
		}

		return true;
	}

	void FlushResourceDestructionQueue()
	{
		if (g_resourceDestructionQueue.size())
		{
			for (auto& resource : d3d11Core::g_resourceDestructionQueue)
			{
				resource->Release();
			}
			g_resourceDestructionQueue.clear();
		}
	}

	template<typename T>
	void DestroyResource(T **resource)
	{
		if (*resource)
		{
			(*resource)->Release();
			(*resource) = nullptr;
		}
	}

	void Term()
	{
		FlushResourceDestructionQueue();

		DestroyResource(&g_pFrameRtTexture);
		DestroyResource(&g_pFrameRtv);
		DestroyResource(&g_pFrameDsTexture);
		DestroyResource(&g_pFrameDsv);
		DestroyResource(&g_pSwapChain);
		DestroyResource(&g_pDeviceCtx);
		DestroyResource(&g_pDevice);
	}

}

/////////////////////////
//  BACKEND TEXTURE HANDLING
/////////////////////////

DXGI_FORMAT GetDxgiFormat(TextureFormat format)
{
	switch (format)
	{
	case TextureFormat_R8G8B8A8:
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	};
	AKASSERT(!"TextureFormat not supported");
	return DXGI_FORMAT_UNKNOWN;
}

class D3D11DynamicTexture2D : public IDynamicTexture2D
{
public:
	D3D11DynamicTexture2D(AkUInt32 width, AkUInt32 height, TextureFormat format)
		: m_uRefCnt(0)
		, m_pTexture(nullptr)
		, m_pSrv(nullptr)
	{
		HRESULT hr;
		{
			D3D11_TEXTURE2D_DESC desc;
			memset(&desc, 0, sizeof(D3D11_TEXTURE2D_DESC));
			desc.Width = width;
			desc.Height = height;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.Format = GetDxgiFormat(format);
			desc.SampleDesc.Count = 1;
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			hr = d3d11Core::g_pDevice->CreateTexture2D(&desc, nullptr, &m_pTexture);
			AKASSERT(SUCCEEDED(hr));
		}
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			desc.Format = GetDxgiFormat(format);
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipLevels = 1;
			desc.Texture2D.MostDetailedMip = 0;
			hr = d3d11Core::g_pDevice->CreateShaderResourceView(m_pTexture, &desc, &m_pSrv);
			AKASSERT(SUCCEEDED(hr));
		}
	}

	void UpdateResource(const void* buffer, size_t bufferPitch,size_t bufferLength)
	{
		D3D11_TEXTURE2D_DESC desc;
		m_pTexture->GetDesc(&desc);
		
		D3D11_MAPPED_SUBRESOURCE subresMap;
		HRESULT hr = d3d11Core::g_pDeviceCtx->Map(m_pTexture, 0, (D3D11_MAP)(D3D11_MAP_WRITE_DISCARD), 0, &subresMap);
		AKASSERT(SUCCEEDED(hr));
		AKASSERT(bufferPitch <= subresMap.RowPitch);
		AKASSERT(bufferLength >= bufferPitch * desc.Height);
		for (AkUInt32 i = 0; i < desc.Height; ++i)
		{
			memcpy((void*)((uintptr_t)subresMap.pData + i * subresMap.RowPitch),
				(void*)((uintptr_t)buffer + i * bufferPitch), bufferPitch);
		}
		d3d11Core::g_pDeviceCtx->Unmap(m_pTexture, 0);
	}

	// When refcount is 0, the texture will be unavailable for further CPU usage,
	// but destruction of resource is deferred to end-of-frame
	void AddRef() { ++m_uRefCnt;  }
	void Release()
	{
		--m_uRefCnt;
		if (!m_uRefCnt)
		{
			if (m_pTexture)
				d3d11Core::g_resourceDestructionQueue.push_back(m_pTexture);
			if (m_pSrv)
				d3d11Core::g_resourceDestructionQueue.push_back(m_pSrv);
		}
	}

	ID3D11Texture2D* getTexture() { return m_pTexture; }
	ID3D11ShaderResourceView* getSrv() { return m_pSrv; }

private:
	AkUInt32 m_uRefCnt;
	ID3D11Texture2D *m_pTexture;
	ID3D11ShaderResourceView* m_pSrv;
};

IDynamicTexture2D* RenderCreateDynamicTexture(AkUInt32 width, AkUInt32 height)
{
	return new D3D11DynamicTexture2D(width, height, TextureFormat_R8G8B8A8);
}

//////////////////////////////////////////////////////////////////////////
// Draw commands
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// texture 2d drawing

namespace drawTexture2D
{
	#include <Shader_Tex2DSimpleVs.h>
	#include <Shader_Tex2DSimplePs.h>

	ID3D11InputLayout* pInputLayout;
	ID3D11VertexShader* pVertShader;
	ID3D11PixelShader* pPxShader;
	ID3D11Buffer* pVertBuffer;
	struct VertShaderInput
	{
		float pos[2];
		float texcoord[2];
	};
	const size_t VERTEX_BUFFER_SIZE = 64 * 1024;
	ID3D11SamplerState* pTexSampler;
	ID3D11BlendState* pBlendState;

	void Init()
	{
		HRESULT hr;
		{
			D3D11_INPUT_ELEMENT_DESC desc[] =
			{
				{"POS2D", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA},
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA}
			};
			hr = d3d11Core::g_pDevice->CreateInputLayout(desc, sizeof(desc) / sizeof(D3D11_INPUT_ELEMENT_DESC),
				g_VsMain, sizeof(g_VsMain) / sizeof(BYTE), &pInputLayout);
			AKASSERT(SUCCEEDED(hr));
		}

		hr = d3d11Core::g_pDevice->CreateVertexShader(g_VsMain, sizeof(g_VsMain) / sizeof(BYTE), nullptr, &pVertShader);
		AKASSERT(SUCCEEDED(hr));
		hr = d3d11Core::g_pDevice->CreatePixelShader(g_PsMain, sizeof(g_PsMain) / sizeof(BYTE), nullptr, &pPxShader);
		AKASSERT(SUCCEEDED(hr));

		{
			D3D11_BUFFER_DESC desc;
			desc.ByteWidth = VERTEX_BUFFER_SIZE;
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.MiscFlags = 0;
			desc.StructureByteStride = 0;
			hr = d3d11Core::g_pDevice->CreateBuffer(&desc, nullptr, &pVertBuffer);
			AKASSERT(SUCCEEDED(hr));
		}
		{
			D3D11_SAMPLER_DESC desc;
			memset(&desc, 0, sizeof(D3D11_SAMPLER_DESC));
			desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			desc.MipLODBias = 0.0f;
			desc.MaxAnisotropy = 1;
			desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			desc.MinLOD = 0.0f;
			desc.MaxLOD = D3D11_FLOAT32_MAX;
			hr = d3d11Core::g_pDevice->CreateSamplerState(&desc, &pTexSampler);
			AKASSERT(SUCCEEDED(hr));
		}
		{
			D3D11_BLEND_DESC desc;
			memset(&desc, 0, sizeof(D3D11_BLEND_DESC));
			desc.RenderTarget[0].BlendEnable = true;
			desc.RenderTarget[0].SrcBlend =  D3D11_BLEND_ONE;
			desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			hr = d3d11Core::g_pDevice->CreateBlendState(&desc, &pBlendState);
			AKASSERT(SUCCEEDED(hr));
		}
	}

	void Term()
	{
		pInputLayout->Release();
		pVertShader->Release();
		pPxShader->Release();
		pVertBuffer->Release();
		pTexSampler->Release();
		pBlendState->Release();
	}

	struct DrawCommand
	{
		ID3D11ShaderResourceView* srv;
		DrawRect src;
		DrawRect dest;
	};
	std::vector<DrawCommand> commands;

	void ExecuteCommands()
	{
		// Fill up the vertex buffer with data from the command list
		AkUInt32 vertexCount = 0;
		{
			size_t bufferPos = 0;
			AkUInt8 bufferData[VERTEX_BUFFER_SIZE];
			for (std::vector<DrawCommand>::iterator commandIt = commands.begin(); commandIt != commands.end(); ++commandIt)
			{
				VertShaderInput vertData[] =
				{
					{ {commandIt->dest.x1, commandIt->dest.y1},	{commandIt->src.x1, commandIt->src.y1} },
					{ {commandIt->dest.x2, commandIt->dest.y1},	{commandIt->src.x2, commandIt->src.y1} },
					{ {commandIt->dest.x1, commandIt->dest.y2},	{commandIt->src.x1, commandIt->src.y2} },
					{ {commandIt->dest.x2, commandIt->dest.y2},	{commandIt->src.x2, commandIt->src.y2} },
				};
				memcpy((void*)&bufferData[bufferPos], &vertData, sizeof(vertData));
				bufferPos += sizeof(vertData);
				vertexCount += 4;
				if (bufferPos > VERTEX_BUFFER_SIZE)
				{
					break;
				}
			}

			D3D11_MAPPED_SUBRESOURCE subresMap;
			d3d11Core::g_pDeviceCtx->Map(pVertBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresMap);
			memcpy(subresMap.pData, (void*)bufferData, bufferPos);
			d3d11Core::g_pDeviceCtx->Unmap(pVertBuffer, 0);
		}

		// Perform context binding and draw
		d3d11Core::g_pDeviceCtx->IASetInputLayout(pInputLayout);
		d3d11Core::g_pDeviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		{
			ID3D11Buffer* buffers[] = { pVertBuffer };
			UINT strides[] = { sizeof(VertShaderInput) };
			UINT offsets[] = { 0 };
			d3d11Core::g_pDeviceCtx->IASetVertexBuffers(0, 1, buffers, strides, offsets);
		}
		d3d11Core::g_pDeviceCtx->VSSetShader(pVertShader, nullptr, 0);
		d3d11Core::g_pDeviceCtx->PSSetShader(pPxShader, nullptr, 0);
		d3d11Core::g_pDeviceCtx->PSSetSamplers(0, 1, &pTexSampler);
		d3d11Core::g_pDeviceCtx->OMSetBlendState(pBlendState, nullptr, 0xFFFFFFFF);

		AkUInt32 startVertex = 0;
		for (std::vector<DrawCommand>::iterator commandIt = commands.begin(); commandIt != commands.end(); ++commandIt)
		{
			ID3D11ShaderResourceView* srv = commandIt->srv;
			d3d11Core::g_pDeviceCtx->PSSetShaderResources(0, 1, &srv);
			d3d11Core::g_pDeviceCtx->Draw(4, startVertex);
			startVertex += 4;
		}
		// consumed the commands, so clear them out
		commands.clear();

		// reset the default blend state for later draws
		d3d11Core::g_pDeviceCtx->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
	}
}

void RenderDrawTexture2D(IDynamicTexture2D& texture, DrawRect src, DrawRect dest)
{
	D3D11DynamicTexture2D& d3dTexture = static_cast<D3D11DynamicTexture2D&>(texture);
	drawTexture2D::commands.push_back({ d3dTexture.getSrv(), src, dest });
}


//////////////////////////////////////////////////////////////////////////
// line 2D drawing

namespace drawLine2D
{
	#include <Shader_Line2DSimpleVs.h>
	#include <Shader_Line2DSimplePs.h>	

	ID3D11InputLayout* pInputLayout;
	ID3D11VertexShader* pVertShader;
	ID3D11PixelShader* pPxShader;
	ID3D11Buffer* pVertBuffer;

	struct VertShaderInput
	{
		float pos[2];
		float color[3];
	};
	const size_t VERTEX_BUFFER_SIZE = 64 * 1024;

	void Init()
	{
		HRESULT hr;
		{
			D3D11_INPUT_ELEMENT_DESC desc[] = 
			{
				{"POS2D", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA},
				{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA}
			};
			hr = d3d11Core::g_pDevice->CreateInputLayout(desc, sizeof(desc)/sizeof(D3D11_INPUT_ELEMENT_DESC),
				g_VsMain, sizeof(g_VsMain) / sizeof(BYTE), &pInputLayout);
			AKASSERT(SUCCEEDED(hr));
		}

		hr = d3d11Core::g_pDevice->CreateVertexShader(g_VsMain, sizeof(g_VsMain) / sizeof(BYTE), nullptr, &pVertShader);
		AKASSERT(SUCCEEDED(hr));
		hr = d3d11Core::g_pDevice->CreatePixelShader(g_PsMain, sizeof(g_PsMain) / sizeof(BYTE), nullptr, &pPxShader);
		AKASSERT(SUCCEEDED(hr));

		{
			D3D11_BUFFER_DESC desc;
			desc.ByteWidth = VERTEX_BUFFER_SIZE;
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.MiscFlags = 0;
			desc.StructureByteStride = 0;
			hr = d3d11Core::g_pDevice->CreateBuffer(&desc, nullptr, &pVertBuffer);
			AKASSERT(SUCCEEDED(hr));
		}
	}

	void Term()
	{
		pInputLayout->Release();
		pVertShader->Release();
		pPxShader->Release();
		pVertBuffer->Release();
	}

	struct DrawCommand
	{
		float x1, y1, x2, y2;
		float color[3]; // rgb; 0 = black, 1 = white
	};
	std::vector<DrawCommand> commands;

	void ExecuteCommands()
	{
		// Fill up the vertex buffer with data from the command list
		AkUInt32 vertexCount = 0;
		{
			size_t bufferPos = 0;
			AkUInt8 bufferData[VERTEX_BUFFER_SIZE];
			for (std::vector<DrawCommand>::iterator commandIt = commands.begin(); commandIt != commands.end(); ++commandIt)
			{
				VertShaderInput vertData[] =
				{
					{ {commandIt->x1, commandIt->y1}, { commandIt->color[0], commandIt->color[1], commandIt->color[2] } },
					{ {commandIt->x2, commandIt->y2}, { commandIt->color[0], commandIt->color[1], commandIt->color[2] } },
				};
				memcpy((void*)&bufferData[bufferPos], &vertData, sizeof(vertData));
				bufferPos += sizeof(vertData);
				vertexCount += 2;
				if (bufferPos > VERTEX_BUFFER_SIZE)
				{
					break;
				}
			}
			// consumed the commands, so clear them out
			commands.clear();

			D3D11_MAPPED_SUBRESOURCE subresMap;
			d3d11Core::g_pDeviceCtx->Map(pVertBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresMap);
			memcpy(subresMap.pData, (void*)bufferData, bufferPos);
			d3d11Core::g_pDeviceCtx->Unmap(pVertBuffer, 0);
		}

		// Perform context binding and draw
		d3d11Core::g_pDeviceCtx->IASetInputLayout(pInputLayout);
		d3d11Core::g_pDeviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		{
			ID3D11Buffer* buffers[] = { pVertBuffer };
			UINT strides[] = { sizeof(VertShaderInput) };
			UINT offsets[] = { 0 };
			d3d11Core::g_pDeviceCtx->IASetVertexBuffers(0, 1, buffers, strides, offsets);
		}
		d3d11Core::g_pDeviceCtx->VSSetShader(pVertShader, nullptr, 0);
		d3d11Core::g_pDeviceCtx->PSSetShader(pPxShader, nullptr, 0);
		d3d11Core::g_pDeviceCtx->Draw(vertexCount, 0);
	}
}

void RenderDrawLine2D(float x1, float y1, float x2, float y2, float color[3])
{
	drawLine2D::commands.push_back({ x1, y1, x2, y2, {color[0], color[1], color[2]} });
}

//////////////////////////////////////////////////////////////////////////
// Render implementation
//////////////////////////////////////////////////////////////////////////

bool RenderInit(void* in_pParam, AkOSChar* in_szErrorBuffer, unsigned int in_unErrorBufferCharCount)
{
	if (!d3d11Core::Init(in_pParam, in_szErrorBuffer, in_unErrorBufferCharCount))
	{
		return false;
	}

	drawLine2D::Init();
	drawTexture2D::Init();
	return true;
}

void RenderTerm()
{
	drawTexture2D::Term();
	drawLine2D::Term();
	d3d11Core::Term();
}

void RenderBeginFrame()
{
}

void RenderEndFrame()
{
	// Set relevant RTs and clear them
	d3d11Core::g_pDeviceCtx->OMSetRenderTargets(1, &d3d11Core::g_pFrameRtv, d3d11Core::g_pFrameDsv);

	float rtClearColor[] = { 0.0f, 0.15f, 0.40f, 1.0f };
	d3d11Core::g_pDeviceCtx->ClearRenderTargetView(d3d11Core::g_pFrameRtv, rtClearColor);
	d3d11Core::g_pDeviceCtx->ClearDepthStencilView(d3d11Core::g_pFrameDsv, D3D11_CLEAR_DEPTH, 1.0f, 0);
	{
		D3D11_VIEWPORT viewport = { 0.0f, 0.0f, (float)d3d11Core::g_iFrameWidth, (float)d3d11Core::g_iFrameHeight, 0.0f, 1.0f, };
		d3d11Core::g_pDeviceCtx->RSSetViewports(1, &viewport);
	}

	// Submit all other queued commands
	drawTexture2D::ExecuteCommands();
	drawLine2D::ExecuteCommands();
	
	// Present
	HRESULT hr = d3d11Core::g_pSwapChain->Present(1, 0);
	AKASSERT(SUCCEEDED(hr));

	// Now that all commands have been submitted, related resources can be cleaned up
	d3d11Core::FlushResourceDestructionQueue();
}

#endif
