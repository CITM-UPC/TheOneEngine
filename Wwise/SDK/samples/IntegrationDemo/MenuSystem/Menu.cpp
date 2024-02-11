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

// Menu.cpp
/// \file 
/// Defines the methods declared in Menu.h

#include "stdafx.h"

#include <AK/Tools/Common/AkAssert.h>
#include "Menu.h"
#include "MessagePage.h"

/////////////////////////////////////////////////////////////////////
// Menu Public Methods
/////////////////////////////////////////////////////////////////////

Menu::Menu()
{
	m_iTickCount = 0;
	m_pInput = NULL;
	m_bQuitApp = false;
}

Menu::~Menu()
{
	Release();
}

void Menu::Init( UniversalInput* in_pInput, int in_iWidth, int in_iHeight, int in_iFrameRate, Page& in_BasePage )
{
	m_pInput = in_pInput;
	m_iMenuWidth = in_iWidth;
	m_iMenuHeight = in_iHeight;
	m_iFrameRate = in_iFrameRate;
	StackPage( &in_BasePage );
}

void Menu::Release()
{
	while ( ! m_PageStack.empty() )
	{
		PopPageStack();
	}
}

void Menu::Back()
{
	if ( m_PageStack.empty() )
		return;

	// The Page wants to close.. remove it from the stack.
	Page* p = m_PageStack.top();
	m_PageStack.pop();

	p->Release();
}

bool Menu::Update()
{
	++m_iTickCount;

	if ( m_PageStack.size() > 0 )
	{
		// Call the Page's update..
		if ( ! m_PageStack.top()->Update() )
		{
			Back();
		}
	}
	else
	{
		// Nothing left on the stack, quit the app
		QuitApplication();
	}

	return !m_bQuitApp;
}

void Menu::OnPointerEvent( PointerEventType in_eType, int in_x, int in_y )
{
	if ( m_PageStack.size() > 0 )
	{
		bool bStay = m_PageStack.top()->OnPointerEvent( in_eType, in_x, in_y );
		if ( !bStay )
		{
			Back();
		}
	}
		
}

void Menu::Draw()
{
	BeginDrawing();
	if ( m_PageStack.size() > 0 )
	{
		m_PageStack.top()->Draw();
	}
	DoneDrawing();
}

void Menu::QuitApplication()
{
	m_bQuitApp = true;
}

UniversalInput* Menu::Input() const
{
	return m_pInput;
}

int Menu::GetFrameRate() const
{
	return m_iFrameRate;
}

int Menu::GetHeight() const
{
	return m_iMenuHeight;
}

int Menu::GetTickCount() const
{
	return m_iTickCount;
}

int Menu::GetWidth() const
{
	return m_iMenuWidth;
}

void Menu::StackPage( Page* in_pPage )
{
	// First, push a blank page to catch any Monitor errors that might occur while initializing the real page.
	MessagePage* pBlankPage = new MessagePage(*this);
	AKVERIFY(true == pBlankPage->Init());
	m_PageStack.push(pBlankPage);

	// Initialize the page.
	if ( in_pPage->Init() )
	{
		// Initialization succeeded
		// Remove the blank page, then stack the page.
		pBlankPage->Release();
		m_PageStack.pop();
		m_PageStack.push( in_pPage );
	}
	else
	{
		// Error occured during initialization, keep the error page.
		pBlankPage->SetTitle("An Error Occurred");
		pBlankPage->SetMessage(in_pPage->RetrieveErrorMsg(), DrawStyle_Error);

		// Release the page that had the error
		in_pPage->Release();
	}
}

void Menu::PopPageStack()
{
	Page* pg = m_PageStack.top();
	pg->Release();
	m_PageStack.pop();
}

Page* Menu::GetCurrentPage()
{
	if (m_PageStack.size() > 0)
		return m_PageStack.top();
	return nullptr;
}

void Menu::SetPluginLog( const char* in_szError )
{
	if ( ! m_szPluginLog.empty() )
	{
		m_szPluginLog.append("\n");	//Allows multiline display
		m_szPluginLog.append(in_szError);
	}
	else
		m_szPluginLog = in_szError;
}

const char* Menu::RetrievePluginLog()
{
	return m_szPluginLog.c_str();
}
