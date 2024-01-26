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

#if defined(AK_RENDER_SDL)

#include "Platform.h"
#include "Render.h"

#if defined(AK_USE_SYSTEM_SDL)
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif
#include <vector>

namespace sdlCore
{
	SDL_Renderer* g_pRenderer = NULL;
	int g_iRenderWidth = 0;
	int g_iRenderHeight = 0;
	std::vector<SDL_Texture*> g_textureDestructionQueue; // List of SDL textures to be destroyed on frame completion 

	bool Init(void* in_pParam, AkOSChar* in_szErrorBuffer, unsigned int in_unErrorBufferCharCount)
	{
		g_pRenderer = SDL_CreateRenderer((SDL_Window*)(in_pParam), -1, SDL_RENDERER_ACCELERATED);
		if (g_pRenderer == NULL)
		{
			if (in_szErrorBuffer)
				AKPLATFORM::SafeStrFormat(in_szErrorBuffer, in_unErrorBufferCharCount, AKTEXT("Renderer could not be created! SDL Error: %s\n"), SDL_GetError());
			return false;
		}

		int width, height;
		SDL_GetWindowSize((SDL_Window*)(in_pParam), &width, &height);
		g_iRenderWidth = width;
		g_iRenderHeight = height;
		return true;
	}

	void FlushResourceDestructionQueue()
	{
		if (g_textureDestructionQueue.size())
		{
			for (size_t i = 0; i < g_textureDestructionQueue.size(); ++i)
			{
				SDL_DestroyTexture(g_textureDestructionQueue[i]);
			}
			g_textureDestructionQueue.clear();
		}
	}

	void Term()
	{
		FlushResourceDestructionQueue();
		SDL_DestroyRenderer(g_pRenderer);
	}
}

class SDLDynamicTexture2D : public IDynamicTexture2D
{
public:
	SDLDynamicTexture2D(AkUInt32 width, AkUInt32 height)
		: m_uRefCnt(0)
		, m_textureHeight(height)
		, m_pTexture(NULL)
	{
		m_pTexture = SDL_CreateTexture(
			sdlCore::g_pRenderer,
			SDL_PIXELFORMAT_ABGR8888, 
			SDL_TEXTUREACCESS_STREAMING, 
			width, 
			height);
	}

	void UpdateResource(const void* buffer, size_t bufferPitch, size_t bufferLength)
	{
		void* textureData;
		int texturePitch;
		SDL_LockTexture(m_pTexture, NULL, &textureData, &texturePitch);
		AKASSERT(bufferPitch <= (size_t)texturePitch);
		AKASSERT(bufferLength >= bufferPitch * m_textureHeight);
		for (AkUInt32 i = 0; i < m_textureHeight; ++i)
		{
			memcpy((void*)((uintptr_t)textureData + i * texturePitch),
				(void*)((uintptr_t)buffer + i * bufferPitch), bufferPitch);
		}
		SDL_UnlockTexture(m_pTexture);
	}

	// When refcount is 0, the surface will be unavailable for further CPU usage,
	// but destruction of resource is deferred to end-of-frame
	void AddRef() { ++m_uRefCnt; }
	void Release()
	{
		--m_uRefCnt;
		if (!m_uRefCnt && m_pTexture)
		{
			sdlCore::g_textureDestructionQueue.push_back(m_pTexture);
		}
	}

	SDL_Texture* getTexture() { return m_pTexture; }

private:
	AkUInt32 m_uRefCnt;
	AkUInt32 m_textureHeight;
	SDL_Texture *m_pTexture;
};

IDynamicTexture2D* RenderCreateDynamicTexture(AkUInt32 width, AkUInt32 height)
{
	return new SDLDynamicTexture2D(width, height);
}

namespace drawTexture2D
{
	struct DrawCommand
	{
		SDL_Texture* tex;
		DrawRect src;
		DrawRect dest;
	};
	std::vector<DrawCommand> commands;

	void ExecuteCommands()
	{
		SDL_BlendMode blendMode = SDL_ComposeCustomBlendMode(
			SDL_BLENDFACTOR_ONE,
			SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
			SDL_BLENDOPERATION_ADD,
			SDL_BLENDFACTOR_ONE,
			SDL_BLENDFACTOR_ZERO,
			SDL_BLENDOPERATION_ADD);

		for (std::vector<DrawCommand>::iterator commandIt = commands.begin(); commandIt != commands.end(); ++commandIt)
		{
			// blend isn't what we actually want but it's the closest available
			SDL_SetTextureBlendMode(commandIt->tex, blendMode);
			
			int flip = (int)SDL_FLIP_NONE;
			
			int x1 = (int)(commandIt->dest.x1 * sdlCore::g_iRenderWidth);
			int x2 = (int)(commandIt->dest.x2 * sdlCore::g_iRenderWidth);
			int y1 = (int)(commandIt->dest.y1 * sdlCore::g_iRenderHeight);
			int y2 = (int)(commandIt->dest.y2 * sdlCore::g_iRenderHeight);

			SDL_Rect dstRect;
			dstRect.w = x2 - x1;
			if (dstRect.w >= 0)
			{
				dstRect.x = x1;
			}
			else
			{
				dstRect.x = x2;
				dstRect.w *= -1;
				flip ^= (int)SDL_FLIP_HORIZONTAL;
			}
			
			dstRect.h = y2 - y1;
			if (dstRect.h >= 0)
			{
				dstRect.y = y1;
			}
			else
			{
				dstRect.y = y2;
				dstRect.h *= -1;
				flip ^= (int)SDL_FLIP_VERTICAL;
			}

			int texWidth, texHeight;
			SDL_QueryTexture(commandIt->tex, NULL, NULL, &texWidth, &texHeight);
			x1 = (int)(commandIt->src.x1 * texWidth);
			x2 = (int)(commandIt->src.x2 * texWidth);
			y1 = (int)(commandIt->src.y1 * texHeight);
			y2 = (int)(commandIt->src.y2 * texHeight);
			
			SDL_Rect srcRect;
			srcRect.w = x2 - x1;
			if (srcRect.w >= 0)
			{
				srcRect.x = x1;
			}
			else
			{
				srcRect.x = x2;
				srcRect.w *= -1;
				flip ^= (int)SDL_FLIP_HORIZONTAL;
			}
			
			srcRect.h = y2 - y1;
			if (srcRect.h >= 0)
			{
				srcRect.y = y1;
			}
			else
			{
				srcRect.y = y2;
				srcRect.h *= -1;
				flip ^= (int)SDL_FLIP_VERTICAL;
			}

			SDL_RenderCopyEx(sdlCore::g_pRenderer, commandIt->tex, &srcRect, &dstRect, 0, NULL, (SDL_RendererFlip)flip);
		}
		// consumed the commands, so clear them out
		commands.clear();
	}
}

void RenderDrawTexture2D(IDynamicTexture2D& texture, DrawRect src, DrawRect dest)
{
	SDLDynamicTexture2D& sdlTexture = static_cast<SDLDynamicTexture2D&>(texture);
	drawTexture2D::DrawCommand command;
	command.tex = sdlTexture.getTexture();
	command.src = src;
	command.dest = dest;
	drawTexture2D::commands.push_back(command);
}

namespace drawLine2D
{
	struct DrawCommand
	{
		float x1, y1, x2, y2;
		float color[3]; // rgb; 0 = black, 1 = white
	};
	std::vector<DrawCommand> commands;

	void ExecuteCommands()
	{
		for (std::vector<DrawCommand>::iterator commandIt = commands.begin(); commandIt != commands.end(); ++commandIt)
		{
			SDL_SetRenderDrawColor(sdlCore::g_pRenderer,
				(Uint8)commandIt->color[0] * 255,
				(Uint8)commandIt->color[1] * 255,
				(Uint8)commandIt->color[2] * 255,
				255);
			SDL_RenderDrawLine(sdlCore::g_pRenderer,
				(int)(sdlCore::g_iRenderWidth * commandIt->x1),
				(int)(sdlCore::g_iRenderHeight * commandIt->y1),
				(int)(sdlCore::g_iRenderWidth * commandIt->x2),
				(int)(sdlCore::g_iRenderHeight * commandIt->y2));
		}
		// consumed the commands, so clear them out
		commands.clear();
	}
}

void RenderDrawLine2D(float x1, float y1, float x2, float y2, float color[3])
{
	drawLine2D::DrawCommand command;
	command.x1 = x1;
	command.y1 = y1;
	command.x2 = x2;
	command.y2 = y2;
	command.color[0] = color[0];
	command.color[1] = color[1];
	command.color[2] = color[2];
	drawLine2D::commands.push_back(command);
}

bool RenderInit(void* in_pParam, AkOSChar* in_szErrorBuffer, unsigned int in_unErrorBufferCharCount)
{
	if (!sdlCore::Init(in_pParam, in_szErrorBuffer, in_unErrorBufferCharCount))
	{
		return false;
	}
	return true;
}

void RenderTerm()
{
	sdlCore::Term();
}

void RenderBeginFrame()
{
}

void RenderEndFrame()
{
	// Clear RT
	SDL_SetRenderDrawColor(sdlCore::g_pRenderer, 0, 38, 102, 255);
	SDL_RenderFillRect(sdlCore::g_pRenderer, NULL);

	// Submit all other queued commands
	drawTexture2D::ExecuteCommands();
	drawLine2D::ExecuteCommands();

	SDL_RenderPresent(sdlCore::g_pRenderer);
	
	// Now that all commands have been submitted, related resources can be cleaned up
	sdlCore::FlushResourceDestructionQueue();
}

#endif // AK_RENDER_SDL
