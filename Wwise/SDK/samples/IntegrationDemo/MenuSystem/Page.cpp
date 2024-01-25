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

// Page.cpp
/// \file 
/// Defines the methods declared in Page.h

#include "stdafx.h"

#include "Page.h"
#include "Menu.h"
#include "MessagePage.h"
#include "Platform.h"
#include "Helpers.h"
#include <AK/Tools/Common/AkPlatformFuncs.h>
#include "IntegrationDemo.h"
#include <sstream>

/////////////////////////////////////////////////////////////////////
// Page Public Methods
/////////////////////////////////////////////////////////////////////

bool Page::ToggledResourceMonitor = false;

Page::Page( Menu& in_ParentMenu, string in_strPageTitle )
{
	m_pParentMenu = &in_ParentMenu;
	m_iSelectedIndex = 0;
	m_bResetControlPositions = true;
	m_szPageTitle = in_strPageTitle;
	m_szHelp = "No help is available for this page.";
}

bool Page::Init()
{
	ReplaceTags( m_szHelp );

	// We start writing the help text at 1/10 the width of the screen. We want
	// a similar margin on the right, so let's wrap at 8/10 of the width:
	IntegrationDemoHelpers::WordWrapString( m_szHelp, (size_t)( INTEGRATIONDEMO_SCREEN_CHAR_WIDTH * 0.8 ) );

	AKPLATFORM::PerformanceCounter(&m_uLastErrorTimeStamp);
	InitControls();
	return true;
}

void Page::Release()
{
	while ( ! m_Controls.empty() )
	{
		delete m_Controls.back();
		m_Controls.pop_back();
	}

	// Goodbye cruel world!
	delete this;
}

bool Page::Update()
{
	// Iterate through the inputs
	UniversalInput::Iterator it;
	for ( it = m_pParentMenu->Input()->Begin(); it != m_pParentMenu->Input()->End(); it++ )
	{
		// Skip this input device if it's not connected
		if ( ! it->IsConnected() )
		{
			continue;
		}

		// Check if the user wants to go "Back"
		if ( it->IsButtonTriggered( UG_BUTTON2 ) )
		{
			return false;
		}

		// Check if the user wants to see the "Help" page
		if ( it->IsButtonTriggered( UG_BUTTON7 ) )
		{
			if ( WarningOccured() && it->IsButtonDown(UG_BUTTON5) )
				DisplayWarning();
			else
				DisplayHelp();
			return true;
		}

		// Check if the user wants to toggle ON/OFF the resource monitor
		if (it->IsButtonTriggered(UG_BUTTON11))
		{
			ToggleResourceMonitor();
			return true;
		}

		// Check if the user pressed "Up" or "Down" to change the selected control
		if ( ! m_Controls.empty() )
		{
			if ( it->IsButtonTriggered( UG_DPAD_UP ))
			{
				m_Controls[m_iSelectedIndex]->OnLoseFocus();
				m_iSelectedIndex = (int)((m_iSelectedIndex - 1 + m_Controls.size()) % m_Controls.size());
				m_Controls[m_iSelectedIndex]->OnFocus();
			}
			else if ( it->IsButtonTriggered( UG_DPAD_DOWN ) )
			{
				if ( m_iSelectedIndex != -1 )
					m_Controls[m_iSelectedIndex]->OnLoseFocus();
				m_iSelectedIndex = (int)((m_iSelectedIndex + 1) % m_Controls.size());
				m_Controls[m_iSelectedIndex]->OnFocus();
			}
		
			// Call the selected control's Update, give it the current input
			if ( m_iSelectedIndex != -1 )
				m_Controls[m_iSelectedIndex]->Update( *it );
		}
	}

	return true;
}

bool Page::OnPointerEvent(PointerEventType in_eType, int in_x, int in_y)
{
	// Determine control at position
	int iHitIndex = -1;
	if (!m_Controls.empty())
	{
		int iTopControl, iControlsDist;
		GetControlSpacing(iTopControl, iControlsDist);

		for (unsigned int i = 0; i < m_Controls.size(); i++)
		{
			int c_x, c_y;
			m_Controls[i]->GetPosition(c_x, c_y);
			if (in_y >= c_y && in_y < (c_y + iControlsDist))
			{
				iHitIndex = i;
				break;
			}
		}
	}

	if (in_eType == PointerEventType_Pressed)
	{
		if (iHitIndex != m_iSelectedIndex)
		{
			if (m_iSelectedIndex != -1
				&& m_iSelectedIndex < (int)m_Controls.size())
			{
				m_Controls[m_iSelectedIndex]->OnLoseFocus();
			}

			m_iSelectedIndex = iHitIndex;

			if (m_iSelectedIndex != -1
				&& m_iSelectedIndex < (int)m_Controls.size())
				m_Controls[m_iSelectedIndex]->OnFocus();
		}
	}
	else if (in_eType == PointerEventType_DoubleTapped)
	{
		if (m_iSelectedIndex != -1
			&& m_iSelectedIndex < (int)m_Controls.size())
			m_Controls[m_iSelectedIndex]->OnFocus();
		else
			return false; // back
	}

	if (iHitIndex != -1
		&& m_iSelectedIndex != -1
		&& m_iSelectedIndex < (int)m_Controls.size())
	{
		m_Controls[m_iSelectedIndex]->OnPointerEvent(in_eType, in_x, in_y);
	}

	return true;
}


void Page::Draw()
{
	int iHeight = m_pParentMenu->GetHeight();
	int iWidth  = m_pParentMenu->GetWidth();
	
	// Set/Update the positions of the Controls if necessary
	if ( m_bResetControlPositions && ( m_Controls.size() > 0 ) )
	{
		PositionControls();
		m_bResetControlPositions = false;
	}

	// Draw the page title
	DrawTextOnScreen( m_szPageTitle.c_str(), iWidth / 8, iHeight / 10, DrawStyle_Title );

	if (Page::ToggledResourceMonitor)
	{
		int xPosition = 2*iWidth/3;
		//Draw resource monitor data summary
		std::stringstream strs;
		strs << IntegrationDemo::ResourceDataSummary.totalCPU;
		std::string str = "Total CPU % : " + strs.str();
		DrawTextOnScreen(str.c_str(), xPosition, 0, DrawStyle_Text);

		strs.str(std::string()); //clear stream
		strs << IntegrationDemo::ResourceDataSummary.pluginCPU;
		str = "Plugin CPU % : " + strs.str();
		DrawTextOnScreen(str.c_str(), xPosition, 20, DrawStyle_Text);

		strs.str(std::string()); //clear stream
		strs << IntegrationDemo::ResourceDataSummary.virtualVoices;
		str = "Virtual Voices : " + strs.str();
		DrawTextOnScreen(str.c_str(), xPosition, 40, DrawStyle_Text);

		strs.str(std::string()); //clear stream
		strs << IntegrationDemo::ResourceDataSummary.physicalVoices;
		str = "Physical Voices : " + strs.str();
		DrawTextOnScreen(str.c_str(), xPosition, 60, DrawStyle_Text);

		strs.str(std::string()); //clear stream
		strs << IntegrationDemo::ResourceDataSummary.totalVoices;
		str = "Total Voices : " + strs.str();
		DrawTextOnScreen(str.c_str(), xPosition, 80, DrawStyle_Text);

		strs.str(std::string()); //clear stream
		strs << IntegrationDemo::ResourceDataSummary.nbActiveEvents;
		str = "Active events : " + strs.str();
		DrawTextOnScreen(str.c_str(), xPosition, 100, DrawStyle_Text);
	}

	// Tell the controls to draw themselves
	for ( int i = 0; i < (int) m_Controls.size(); i++ )
	{
		if ( i == m_iSelectedIndex )
		{
			m_Controls[i]->Draw( DrawStyle_Selected );
		}
		else
		{
			m_Controls[i]->Draw();
		}
	}

	// Display instructions at the bottom of the page
	int iBottomY = iHeight - 2 * GetLineHeight(DrawStyle_Text);
	if (ErrorOccured())
	{
		int lines = 0;
		size_t errorLength = AkMin(INTEGRATIONDEMO_MAX_MESSAGE, m_szError.length());
		for (size_t i = 0; i < errorLength; i++)
			lines += m_szError.c_str()[i] == '\n';
		
		int height = GetLineHeight(DrawStyle_Error);
		DrawTextOnScreen(m_szError.c_str(), 20 , iBottomY - lines * height, DrawStyle_Error);
	}

	if (WarningOccured())
	{
		DrawTextOnScreen("(Warning <<UG_BUTTON5>> + <<UG_BUTTON7>>)", (int)(iWidth * 0.7), 0, DrawStyle_SelectedText);
	}

	DrawTextOnScreen("(Press <<UG_BUTTON7>> For Help...)", (int)(iWidth * 0.4), iBottomY, DrawStyle_Text);
}

bool Page::ErrorOccured()
{
	return ( ! m_szError.empty() );
}

bool Page::WarningOccured()
{
	return ( ! m_szWarning.empty() );
}

const char* Page::RetrieveErrorMsg()
{
	return m_szError.c_str();
}

void Page::SetTitle( const char* in_szTitle )
{
	m_szPageTitle = in_szTitle;
}

const Menu* Page::ParentMenu()
{
	return m_pParentMenu;
}

void Page::SetErrorMessage( const char* in_szError )
{
	//Keeps a timer between error reports so grouped errors can be displayed together. If too long between errors, display is reset.
	AkInt64 uTime;
	AKPLATFORM::PerformanceCounter(&uTime);
	char errorMessage[INTEGRATIONDEMO_MAX_MESSAGE] = {};
	AKPLATFORM::SafeStrCat(errorMessage, m_szError.c_str(), INTEGRATIONDEMO_MAX_MESSAGE);
	if (AKPLATFORM::Elapsed(uTime, m_uLastErrorTimeStamp) < 2000)
	{
		AKPLATFORM::SafeStrCat(errorMessage, "\n", INTEGRATIONDEMO_MAX_MESSAGE);	//Allows multiline display
		AKPLATFORM::SafeStrCat(errorMessage, in_szError, INTEGRATIONDEMO_MAX_MESSAGE);
	}
	else
	{
		AKPLATFORM::SafeStrCpy(errorMessage, in_szError, INTEGRATIONDEMO_MAX_MESSAGE);
	}

	m_szError = errorMessage;
	
	m_uLastErrorTimeStamp = uTime;
}

void Page::SetPluginErrorMessage( const char* in_szError )
{
	char errorMessage[INTEGRATIONDEMO_MAX_MESSAGE];

	AKPLATFORM::SafeStrCpy(errorMessage, in_szError, INTEGRATIONDEMO_MAX_MESSAGE);
	AKPLATFORM::SafeStrCat(errorMessage, "\n \nRelated Errors:\n", INTEGRATIONDEMO_MAX_MESSAGE);
	AKPLATFORM::SafeStrCat(errorMessage, m_pParentMenu->RetrievePluginLog(), INTEGRATIONDEMO_MAX_MESSAGE);

	m_szError = errorMessage;
}

/////////////////////////////////////////////////////////////////////
// Page Protected Methods
/////////////////////////////////////////////////////////////////////

void Page::SetLoadFileErrorMessage( const char* fileName )
{
	char errorMessage[INTEGRATIONDEMO_MAX_MESSAGE];
#if defined (AK_IOS) || defined (AK_MAC_OS_X)
	const char* soundBankPath = SOUND_BANK_PATH;
	AKPLATFORM::SafeStrFormat(errorMessage, INTEGRATIONDEMO_MAX_MESSAGE, "Failed to load file \"%s\" under Soundbank path: \n\n%s", fileName, soundBankPath);
#else
	char* soundBankPath = NULL;
	CONVERT_OSCHAR_TO_CHAR(SOUND_BANK_PATH, soundBankPath);
	AKPLATFORM::SafeStrFormat(errorMessage, INTEGRATIONDEMO_MAX_MESSAGE, "Failed to load file \"%s\" under Soundbank path: \n\n%s", fileName, soundBankPath);
#endif //#if defined (AK_IOS) || defined (AK_MAC_OS_X)
	SetErrorMessage(errorMessage);
}

void Page::ResetControlPositions()
{
	m_bResetControlPositions = true;
}

void Page::DisplayHelp()
{
	MessagePage* pHelpPage = new MessagePage( *m_pParentMenu );

	string title( "Help: " );
	title.append( m_szPageTitle );
	pHelpPage->SetTitle( title.c_str() );
	pHelpPage->SetMessage( m_szHelp.c_str() );
	m_pParentMenu->StackPage( pHelpPage );
}

void Page::DisplayWarning()
{
	MessagePage* pWarningPage = new MessagePage( *m_pParentMenu );

	string title( "Warning: " );
	title.append( m_szPageTitle );
	pWarningPage->SetTitle( title.c_str() );
	pWarningPage->SetMessage( m_szWarning.c_str(), DrawStyle_SelectedText );
	m_pParentMenu->StackPage( pWarningPage );
}

void Page::ToggleResourceMonitor()
{
	Page::ToggledResourceMonitor = (Page::ToggledResourceMonitor) ? false : true;
}

/////////////////////////////////////////////////////////////////////
// Page Private Methods
/////////////////////////////////////////////////////////////////////

void Page::GetControlSpacing( int & out_iTopControl, int & out_iControlsDist )
{
	int iHeight = m_pParentMenu->GetHeight();
	int iLineHeight = GetLineHeight( DrawStyle_Control );

	// Figure out how far apart (vertically) the controls should be
	out_iTopControl = (int)(iHeight * 0.175);  // Y-Position of the top control
	out_iControlsDist = ( ( iHeight - out_iTopControl ) - iLineHeight ) / (int)m_Controls.size();
	if ( out_iControlsDist > iLineHeight )
	{
		out_iControlsDist = iLineHeight;
	}
}

void Page::PositionControls()
{
	int iTopControl, iControlsDist;
	GetControlSpacing( iTopControl, iControlsDist );

	// Set the positions of the coordinates
	int iWidth  = m_pParentMenu->GetWidth();
	for ( unsigned int i = 0; i < m_Controls.size(); i++ )
	{
		m_Controls[i]->SetPosition( iWidth / 5, iTopControl + ( i * iControlsDist ) );
	}
}
