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

#include <math.h>

#include "UniversalInput.h"
#include "SDLInputMgr.h"
#include "Platform.h"

SDLInputMgr::SDLInputMgr()
{}

SDLInputMgr::~SDLInputMgr()
{
}

bool SDLInputMgr::Init(
	AkOSChar* in_szErrorBuffer,
	unsigned int in_unErrorBufferCharCount
)
{
	m_pUInput = new UniversalInput;
	m_pUInput->AddDevice( 1 , UGDeviceType_GAMEPAD );

	m_btnKeyboard = 0;
	m_bDidMotion = false;
	m_bShouldQuit = false;

	return true;
}


UniversalInput* SDLInputMgr::UniversalInputAdapter() const
{
	return m_pUInput;
}


void SDLInputMgr::Release()
{
	if ( m_pUInput )
	{
		delete m_pUInput;
		m_pUInput = NULL;
	}
}

void SDLInputMgr::Update()
{
	// Basic Gesture handling (implemented in 10 minutes in a "just to get it working" fashion.
	SDL_Scancode motionCh = SDL_SCANCODE_UNKNOWN;

	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		switch (e.type) {
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			switch (e.key.keysym.scancode)
			{
			case SDL_SCANCODE_ESCAPE:
				m_btnKeyboard = 0;
			case SDL_SCANCODE_UP:
			case SDL_SCANCODE_W:
			case SDL_SCANCODE_DOWN:
			case SDL_SCANCODE_S:
			case SDL_SCANCODE_LEFT:
			case SDL_SCANCODE_A:
			case SDL_SCANCODE_RIGHT:
			case SDL_SCANCODE_D:
			case SDL_SCANCODE_RETURN:
			case SDL_SCANCODE_BACKSPACE:
			case SDL_SCANCODE_F1:
				{
					UGBtnState btnState = ScanCodeToBtnState( e.key.keysym.scancode );
					if (e.type == SDL_KEYDOWN)
						m_btnKeyboard |= btnState;
					else
						m_btnKeyboard &= ~btnState;
				}
				break;
			default:
				break;
			}
			break;

		case SDL_FINGERMOTION:
			{
				bool bHandleX = fabs(e.tfinger.dx) > fabs(e.tfinger.dy);

				if (bHandleX)
				{
					if (fabs(e.tfinger.dx) > 100)
					{
						// User is largely swiping on sides, consider an escape.
						motionCh = SDL_SCANCODE_ESCAPE;
						printf("Swiping Back\n");
						m_bDidMotion = true;
					}
					else if (e.tfinger.dx > 0)
					{
						motionCh = SDL_SCANCODE_RIGHT;
						m_bDidMotion = true;
					}
					else if (e.tfinger.dx < 0)
					{
						motionCh = SDL_SCANCODE_LEFT;
						m_bDidMotion = true;
					}
				}
				else //y
				{
					if (e.tfinger.dy > 0)
					{
						motionCh = SDL_SCANCODE_DOWN;
						m_bDidMotion = true;
					}
					else if (e.tfinger.dy < 0)
					{
						motionCh = SDL_SCANCODE_UP;
						m_bDidMotion = true;
					}
				}
			}
			break;

		case SDL_FINGERDOWN:
			m_bDidMotion = false;
			break;

		case SDL_MULTIGESTURE:
			printf("SDL_MULTIGESTURE\n");
			break;

		case SDL_FINGERUP:
			if (!m_bDidMotion)
			{
				// condider a touch.
				motionCh = SDL_SCANCODE_RETURN;
			}
			break;

		case SDL_QUIT:
			m_bShouldQuit = true;
			break;

		default:
			break;
		}
	}

	UGStickState stickState[] = {{0.f, 0.f},{0.f,0.f} };

	UGBtnState btnMotion = ScanCodeToBtnState( motionCh );

	m_pUInput->SetState(1, true, m_btnKeyboard | btnMotion, stickState);
}

UGBtnState SDLInputMgr::ScanCodeToBtnState( SDL_Scancode in_scanCode ) const
{
	UGBtnState btnState = 0;

	switch ( in_scanCode )
	{
	case SDL_SCANCODE_UP:
	case SDL_SCANCODE_W:
		btnState = UG_DPAD_UP;
		break;
	case SDL_SCANCODE_DOWN:
	case SDL_SCANCODE_S:
		btnState = UG_DPAD_DOWN;
		break;
	case SDL_SCANCODE_LEFT:
	case SDL_SCANCODE_A:
		btnState = UG_DPAD_LEFT;
		break;
	case SDL_SCANCODE_RIGHT:
	case SDL_SCANCODE_D:
		btnState = UG_DPAD_RIGHT;
		break;
	case SDL_SCANCODE_RETURN:
		btnState = UG_BUTTON1;
		break;
	case SDL_SCANCODE_BACKSPACE:
	case SDL_SCANCODE_ESCAPE:
		btnState = UG_BUTTON2;
		break;
	case SDL_SCANCODE_F1:
		btnState = UG_BUTTON7;
		break;
	default:
		break;
	}

	return btnState;
}
