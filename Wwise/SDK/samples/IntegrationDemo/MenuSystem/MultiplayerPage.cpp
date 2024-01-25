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

// MultiplayerPage.cpp
/// \file
/// Defines the methods declared in MultiplayerPage.h

#include "stdafx.h"

#include <AK/Tools/Common/AkPlatformFuncs.h>
#include "MultiplayerPage.h"
#include "Menu.h"


/////////////////////////////////////////////////////////////////////
// MultiplayerPage Public Methods
/////////////////////////////////////////////////////////////////////

MultiplayerPage::MultiplayerPage( Menu& in_ParentMenu, string in_strPageTitle ):Page( in_ParentMenu, in_strPageTitle )
{
	for ( AkUInt16 i = 0; i < UG_MAX_GAMEPADS; i++ )
	{
		// Initialize the selected indices
		m_iSelectedIndex[i] = 0;

		// Check if a gamepad exists for this player
		m_bPlayerConnected[i] = m_pParentMenu->Input()->IsPlayerConnected( i + 1 );
	}
	
	m_bResetControlPositions = true;
}

bool MultiplayerPage::Init()
{
	bool ret = Page::Init();
	for ( int i = 0; i < UG_MAX_GAMEPADS; i++ )
	{
		if (m_bPlayerConnected[i])
			OnPlayerConnect(i);
	}

	return ret;
}

void MultiplayerPage::Release()
{
	for ( int i = 0; i < UG_MAX_GAMEPADS; i++ )
	{
		// Delete all the controls for the player
		while ( ! m_Controls[i].empty() )
		{
			delete m_Controls[i].back();
			m_Controls[i].pop_back();
		}
	}

	Page::Release();
}

bool MultiplayerPage::Update()
{
	// Update for player 0 (keyboard) if they are present
	const UniversalGamepad* gp = m_pParentMenu->Input()->GetGamepad( 0 );
	if ( gp && gp->IsConnected() )
	{
		// Check if the user wants to go "back"
		if ( gp->IsButtonTriggered( UG_BUTTON2 ) )
		{
			return false;
		}

		// Check if the user wants to see the "Help" page
		if ( gp->IsButtonTriggered( UG_BUTTON7 ) )
		{
			DisplayHelp();
			return true;
		}

		// If m_bPlayerConnected[0] is true, that means that player 1 in the game is represented
		// by a gamepad. Let Player 0 participate by having his device also count as input from
		// player 1.
		if ( m_bPlayerConnected[0] )
		{
			// Check if the user pressed "Up" or "Down" to change the selected control
			if ( gp->IsButtonTriggered( UG_DPAD_UP ) && 
				 m_iSelectedIndex[0] > 0 )
			{
				m_Controls[0][m_iSelectedIndex[0]]->OnLoseFocus();
				--m_iSelectedIndex[0];
				m_Controls[0][m_iSelectedIndex[0]]->OnFocus();
			}
			else if ( gp->IsButtonTriggered( UG_DPAD_DOWN ) && 
					  m_iSelectedIndex[0] < m_Controls[0].size() - 1 )
			{
				m_Controls[0][m_iSelectedIndex[0]]->OnLoseFocus();
				++m_iSelectedIndex[0];
				m_Controls[0][m_iSelectedIndex[0]]->OnFocus();
			}
			
			// Call the current player's selected control's Update
			m_Controls[0][m_iSelectedIndex[0]]->Update( *gp );
		}
		
	}

	for ( AkUInt16 i = 0; i < UG_MAX_GAMEPADS; i++ )
	{
		bool bConnStatus = m_pParentMenu->Input()->IsPlayerConnected( i + 1 );
		if ( ! m_bPlayerConnected[i] && bConnStatus )
		{
			// Player connected!
			m_bPlayerConnected[i] = bConnStatus;
			OnPlayerConnect( i );
		}
		else if ( m_bPlayerConnected[i] && ! bConnStatus )
		{
			// Player disconnected!
			m_bPlayerConnected[i] = bConnStatus;
			OnPlayerDisconnect( i );
		}

		// Only update for this player if they exist!
		if ( m_bPlayerConnected[i] )
		{
			gp = m_pParentMenu->Input()->GetGamepad( i + 1 );

			if (!gp)
			{
				return false;
			}

			// Check if the user wants to go "back"
			if ( gp->IsButtonTriggered( UG_BUTTON2 ) )
			{
				return false;
			}

			// Check if the user wants to see the "Help" page
			if ( gp->IsButtonTriggered( UG_BUTTON7 ) )
			{
				DisplayHelp();
				return true;
			}

			// Check if the user pressed "Up" or "Down" to change the selected control
			if ( gp->IsButtonTriggered( UG_DPAD_UP ) && 
				 m_iSelectedIndex[i] > 0 )
			{
				m_Controls[i][m_iSelectedIndex[i]]->OnLoseFocus();
				--m_iSelectedIndex[i];
				m_Controls[i][m_iSelectedIndex[i]]->OnFocus();
			}
			else if ( gp->IsButtonTriggered( UG_DPAD_DOWN ) && 
					  m_iSelectedIndex[i] < m_Controls[i].size() - 1 )
			{
				m_Controls[i][m_iSelectedIndex[i]]->OnLoseFocus();
				++m_iSelectedIndex[i];
				m_Controls[i][m_iSelectedIndex[i]]->OnFocus();
			}
			
			// Call the current player's selected control's Update
			m_Controls[i][m_iSelectedIndex[i]]->Update( *gp );
		}
	}

	return true;
}

void MultiplayerPage::Draw()
{
	int iHeight = m_pParentMenu->GetHeight();
	int iWidth  = m_pParentMenu->GetWidth();
	
	// Set/Update the positions of the Controls if necessary
	if ( m_bResetControlPositions )
	{
		PositionControls();
		m_bResetControlPositions = false;
	}

	// Draw the page title
	DrawTextOnScreen( m_szPageTitle.c_str(), iWidth / 10, iHeight / 10, DrawStyle_Title );

	for ( int i = 0; i < UG_MAX_GAMEPADS; i++ )
	{
		char szHeading[50];
		int iHeadingX = ( i * iWidth / UG_MAX_GAMEPADS ) + ( iWidth / 25 );
		int iHeadingY = iHeight / 3;

		if ( m_bPlayerConnected[i] )
		{
			// Draw the column heading
			snprintf( szHeading, 50, "Player %d", i + 1 );
			DrawTextOnScreen( szHeading, iHeadingX, iHeadingY );

			// Tell the player's controls to draw themselves.
			for ( unsigned int j = 0; j < m_Controls[i].size(); j++ )
			{
				if ( j == m_iSelectedIndex[i] )
				{
					m_Controls[i][j]->Draw( DrawStyle_Selected );
				}
				else
				{
					m_Controls[i][j]->Draw();
				}
			}
		}
		else
		{
			// Draw the column heading
			snprintf( szHeading, 50, "***P%d***", i + 1 );
			DrawTextOnScreen( szHeading, iHeadingX, iHeadingY );
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
		DrawTextOnScreen(m_szError.c_str(), 20, iBottomY - lines * height, DrawStyle_Error);
	}
	else
	{
		bool isPlayerConnected = false;
		for (int i = 0; i < UG_MAX_GAMEPADS; i++)
		{
			if (m_bPlayerConnected[i])
			{
				isPlayerConnected = true;
				break;
			}
		}

		if (!isPlayerConnected)
		{
			static int s_nOffset = 5 + GetLineHeight( DrawStyle_Text );
			DrawTextOnScreen( "Connect a controller to participate", 10, m_pParentMenu->GetHeight() - s_nOffset, DrawStyle_Text );
		}

		DrawTextOnScreen("(Press <<UG_BUTTON7>> For Help...)", (int)(iWidth * 0.4), iBottomY, DrawStyle_Text);
	}
}


/////////////////////////////////////////////////////////////////////
// MultiplayerPage Protected Methods
/////////////////////////////////////////////////////////////////////

void MultiplayerPage::ResetControlPositions()
{
	m_bResetControlPositions = true;
}


/////////////////////////////////////////////////////////////////////
// MultiplayerPage Private Methods
/////////////////////////////////////////////////////////////////////

void MultiplayerPage::PositionControls()
{
	int iHeight = m_pParentMenu->GetHeight();
	int iWidth  = m_pParentMenu->GetWidth();
	int iControlsDist;              // The vertical distance between controls
	int iTopControl = iHeight / 2;  // Y-Position of the top control


	// Set the positions of the coordinates
	for ( int i = 0; i < UG_MAX_GAMEPADS; i++ )
	{
		int xPos = ( i * iWidth / UG_MAX_GAMEPADS ) + 30;

		// Figure out how far apart (vertically) the controls should be
		int iLineHeight = GetLineHeight( DrawStyle_Control );
		iControlsDist = ( ( iHeight - iTopControl ) - iLineHeight ) / (int)m_Controls[i].size();
		if ( iControlsDist > iLineHeight )
		{
			iControlsDist = iLineHeight;
		}

		for ( unsigned int j = 0; j < m_Controls[i].size(); j++ )
		{
			m_Controls[i][j]->SetPosition( xPos, iTopControl + ( j * iControlsDist ) );
		}
	}
}
