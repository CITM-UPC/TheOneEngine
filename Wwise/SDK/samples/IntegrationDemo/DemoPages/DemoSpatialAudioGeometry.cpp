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

// DemoSpatialAudioGeometry.cpp
/// \file
/// Defines all methods declared in DemoSpatialAudioGeometry.h

#include "stdafx.h"

#include <math.h>
#include <float.h>
#include "Menu.h"
#include "Helpers.h"
#include "DemoSpatialAudioGeometry.h"
#include <AK/SoundEngine/Common/AkSoundEngine.h>    // Sound engine

#define _USE_MATH_DEFINES
#include <math.h>

//If you get a compiling error here, it means the file wasn't generated with the banks.  Did you generate the soundbanks before compiling?
#include "../WwiseProject/GeneratedSoundBanks/Wwise_IDs.h"		

// Our game object ID.  Completely arbitrary.
const AkGameObjectID EMITTER = 100;
const AkGameObjectID LISTENER = 103;
const AkGameObjectID DISTANCE_PROBE = 104;
const AkGeometrySetID GEOMETRY_VERTICAL_WALL = 0;
const AkGeometrySetID GEOMETRY_HORIZONTAL_WALL = 1;

#define REPEAT_TIME 20

/////////////////////////////////////////////////////////////////////
// DemoSpatialAudio Public Methods
/////////////////////////////////////////////////////////////////////

DemoSpatialAudioGeometry::DemoSpatialAudioGeometry(Menu& in_ParentMenu)
	: Page( in_ParentMenu, "Geometry Demo")
	, m_pListenerChip(NULL)
	, m_pEmitterChip(NULL)
	, m_aLines(NULL)
	, m_numLines(0)
	, m_diffraction( 0 )
	, m_fWidth( 0.0f )
	, m_fHeight(0.0f)
	, m_bTestNoise(false)
	, m_bMoved(false)
{
	for (int i = 0; i < k_numGeometryPlanes; i++)
		m_geometry[i] = NULL;

	m_szHelp =
		"*** Please connect Wwise and open the 3D Game Object Viewer (F12 layout). ***\n"
		"This demo showcases the Wwise Spatial Audio Geometry API, usable for direct (dry) path diffraction and transmission.\n \n"
		"In this demo we have 2 walls, a listener/[L], and an emitter/[E].\n"
		"The diffraction path(s) are displayed, with the resulting diffraction amount in the lower-left corner. "
		"Spatial Audio is set up so that diffraction/transmission controls both project-wide obstruction and the built-in diffraction/transmission parameter, although only the former is used in the project.\n"
		"Refer to Spatial Audio Geometry in the SDK documentation for more details.\n \n"
		"Listener and emitter controls are specified under the Spatial Audio Demos menu help page.";
}

bool DemoSpatialAudioGeometry::Init()
{
	// Load the sound bank
	AkBankID bankID; // Not used
	if ( AK::SoundEngine::LoadBank( "Spatial_Audio_Demo.bnk", bankID ) != AK_Success )
	{
		SetLoadFileErrorMessage( "Spatial_Audio_Demo.bnk" );
		return false;
	}

	return Page::Init();
}

void DemoSpatialAudioGeometry::InitControls()
{
	float fMargin = (float)m_pEmitterChip->GetRightBottomMargin();
	m_fWidth = (float)m_pParentMenu->GetWidth() - fMargin;
	m_fHeight = (float)m_pParentMenu->GetHeight() - fMargin;
	float centerX = m_fWidth / 2.0f;
	float centerY = m_fHeight / 2.0f;

	// Register listener in spatial audio and init chip controls
	m_pListenerChip = new SpatialAudioListener(*this, LISTENER, DISTANCE_PROBE, "Listener L");
	m_pListenerChip->SetLabel("<L>");
	m_pListenerChip->UseJoystick(UG_STICKRIGHT);
	m_pListenerChip->SetNonLinear();
	m_pListenerChip->SetPos(centerX + 30, centerY + 30);
	m_pListenerChip->SetMaxSpeed(3.f);

	// Register emitter in spatial audio and init chip controls
	m_pEmitterChip = new SpatialAudioGameObject(*this, EMITTER, LISTENER, "Emitter E");
	m_pEmitterChip->SetLabel( "<E>" );
	m_pEmitterChip->UseJoystick(UG_STICKRIGHT);
	m_pEmitterChip->SetNonLinear();
	m_pEmitterChip->SetPos(centerX - 100, centerY - 100);
	m_pEmitterChip->SetMaxSpeed(3.f);

	Init_Geometry();
	AK::SoundEngine::PostEvent("Play_Room_Emitter", EMITTER);

	// Update movable chips the first time.
	UniversalInput::Iterator it;
	for (it = m_pParentMenu->Input()->Begin(); it != m_pParentMenu->Input()->End(); it++)
	{
		// Skip this input device if it's not connected
		if (!it->IsConnected())
			continue;

		m_pEmitterChip->Update(*it);
		m_pListenerChip->Update(*it);
		m_pEmitterChip->SetPosition();
		m_pListenerChip->SetPosition();
	}
}

void DemoSpatialAudioGeometry::Init_Geometry()
{
	float wall0_height_px = m_fHeight / 2.f - 60;
	float wall1_width_px = m_fWidth / 2.f - 60;

	// 2 disjoint meshes: an horizontal wall (on the bottom left) and a vertical wall (on the right).
	// For diffraction, it is important to join meshes to avoid sound leaking through cracks.
	// Here in our 2D geometry, we do not assign triangles to the top and bottom planes (through screen), and utilize
	// AkGeometryParams::EnableDiffractionOnBoundaryEdges = false so that edges on top and bottom will not allow sound to pass 
	// through the ceiling or floor. Likewise, we do not assign triangles on the left of the horizontal wall. 
	//       
	//        |
	// ----   |

	// Vertical wall.
	//

	// Draw Box.
	m_geometry[0] = new Box(*this);
	int posX_px = (int) (m_fWidth / 2.f - 1.f);
	int posY_px = (int) (m_fHeight / 2.f - wall0_height_px);
	m_geometry[0]->SetPosition(posX_px, posY_px);
	m_geometry[0]->SetDimensions(1, (int) wall0_height_px);
	
	// Register to SpatialAudio
	SpatialAudioHelpers::BoxGeometryParams wallParams { 0, 0, 0, 1 };

	AkAcousticSurface surface;
	surface.strName = "Outside";
	surface.textureID = AK::SoundEngine::GetIDFromString("Brick");
	wallParams.surfaces.push_back(surface);

	surface.strName = "Inside";
	surface.textureID = AK::SoundEngine::GetIDFromString("Drywall");
	wallParams.surfaces.push_back(surface);

	SpatialAudioHelpers::AddBoxGeometry(GEOMETRY_VERTICAL_WALL, wallParams);
	SpatialAudioHelpers::AddBoxGeometryInstance(GEOMETRY_VERTICAL_WALL, GEOMETRY_VERTICAL_WALL, m_geometry[0], m_fWidth, m_fHeight);

	// Horizontal wall
	//

	// Draw Box.
	m_geometry[1] = new Box(*this);
	posX_px = 0;
	posY_px = (int) (m_fHeight / 2.f - 1.f);
	m_geometry[1]->SetPosition(posX_px, posY_px);
	m_geometry[1]->SetDimensions((int) wall1_width_px, 1);

	// Use same two surfaces as above (0 is outside and 1 is inside)
	// Top of the wall is now inside and we do not assign triangles on the left of the horizontal wall.
	wallParams.topWall = 1;
	wallParams.leftWall = AK_INVALID_SURFACE;

	SpatialAudioHelpers::AddBoxGeometry(GEOMETRY_HORIZONTAL_WALL, wallParams);
	SpatialAudioHelpers::AddBoxGeometryInstance(GEOMETRY_HORIZONTAL_WALL, GEOMETRY_HORIZONTAL_WALL, m_geometry[1], m_fWidth, m_fHeight);
}

void DemoSpatialAudioGeometry::Release()
{
	AK::SoundEngine::StopAll();
	AK::SpatialAudio::RemoveGeometry(0);
	AK::SpatialAudio::RemoveGeometry(1);
	delete m_pEmitterChip;
	delete m_pListenerChip;
	AK::SoundEngine::UnloadBank( "Spatial_Audio_Demo.bnk", NULL );

	Page::Release();
}

bool DemoSpatialAudioGeometry::Update()
{
	const float kAngleIncrement = M_PI / 180.f;

	UniversalInput::Iterator it;
	for ( it = m_pParentMenu->Input()->Begin(); it != m_pParentMenu->Input()->End(); it++ )
	{
		// Skip this input device if it's not connected
		if ( ! it->IsConnected() )
			continue;

		if ((*it).IsButtonDown(UG_BUTTON3))
		{
			if ((*it).IsButtonDown(UG_BUTTON5))
				m_pListenerChip->IncrementElevation(-kAngleIncrement);
			else
				m_pListenerChip->IncrementAzimuth(-kAngleIncrement);

			m_bMoved = true;
		}

		if ((*it).IsButtonDown(UG_BUTTON4))
		{
			if ((*it).IsButtonDown(UG_BUTTON5))
				m_pListenerChip->IncrementElevation(kAngleIncrement);
			else
				m_pListenerChip->IncrementAzimuth(kAngleIncrement);

			m_bMoved = true;
		}

		if ((*it).IsButtonDown(UG_BUTTON5))
		{
			m_pEmitterChip->Update(*it);
			m_bMoved = true;
		}
		else
		{
			m_pListenerChip->Update(*it);
			m_bMoved = true;
		}

		if ((*it).IsButtonTriggered(UG_BUTTON6))
		{
			m_pListenerChip->AddToPlayerOffset(5.f);
			m_bMoved = true;
		}

		if ((*it).IsButtonTriggered(UG_BUTTON9))
		{
			AK::SoundEngine::StopAll(EMITTER);
			if (m_bTestNoise)
			{
				AK::SoundEngine::PostEvent("Play_Room_Emitter", EMITTER);
				m_bTestNoise = false;
			}
			else
			{
				AK::SoundEngine::PostEvent("Play_Test_Noise", EMITTER);
				m_bTestNoise = true;
			}
		}
	}

	if ( m_bMoved )
	{
		// Update emitters and Listener.
		m_pEmitterChip->SetPosition();
		m_pListenerChip->SetPosition();

		//
		// Query propagation for UI display.
		//
		
		for (int i = 0; i < m_numLines; i++)
		{
			if (m_aLines[i])
				delete m_aLines[i];
		}
		delete[] m_aLines;
		m_aLines = NULL;
		m_numLines = 0;

		AkVector64 emitterPos, listenerPos;
		AkDiffractionPathInfo paths[8];
		AkUInt32 uNumPaths = 8;	// 8 arbitrary
		AK::SpatialAudio::QueryDiffractionPaths(
			EMITTER,	///< The ID of the game object that the client wishes to query.
			0,				///< Position index 0, we only have one.
			listenerPos,	///< Returns the position of the listener game object that is associated with the game object \c in_gameObjectID.
			emitterPos,		///< Returns the position of the emitter game object \c in_gameObjectID.
			paths,			///< Pointer to an array of \c AkPropagationPathInfo's which will be filled after returning.
			uNumPaths		///< The number of slots in \c out_aPaths, after returning the number of valid elements written.
			);

		// Count lines to draw.
		for (AkUInt32 path = 0; path < uNumPaths; path++)
		{
			int numNodes = paths[path].nodeCount;
			int numSegments = numNodes + 1;
			m_numLines += numSegments;
		}

		m_diffraction = 0.f;
		m_tramsissionLoss = 0.f;

		float diffraction = 1.f;
		float transmissionLoss = 0.f;

		if (m_numLines > 0)
		{
			m_aLines = new Line*[m_numLines];

			int line = 0;
			for (AkUInt32 path = 0; path < uNumPaths; path++)
			{
				int numNodes = paths[path].nodeCount;

				if (numNodes > 0)
				{
					// Listener to node 0.
					m_aLines[line] = new Line(*this);
					m_aLines[line]->SetPoints(IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, listenerPos.X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, listenerPos.Z),
						IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, paths[path].nodes[0].X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, paths[path].nodes[0].Z));
					++line;

					// node i-1 to node i
					for (int i = 1; i < numNodes; i++)
					{
						m_aLines[line] = new Line(*this);
						m_aLines[line]->SetPoints(IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, paths[path].nodes[i - 1].X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, paths[path].nodes[i - 1].Z),
							IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, paths[path].nodes[i].X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, paths[path].nodes[i].Z));
						++line;
					}
					
					// Last node to emitter.
					m_aLines[line] = new Line(*this);
					m_aLines[line]->SetPoints(IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, paths[path].nodes[numNodes - 1].X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, paths[path].nodes[numNodes - 1].Z),
						IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, emitterPos.X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, emitterPos.Z));
					++line;
				}
				else
				{
					// A path with no node: completely obstructed. Draw a line from emitter to listener, with style "Selected" (Draw() checks m_diffraction)
					m_aLines[line] = new Line(*this);
					m_aLines[line]->SetPoints(IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, emitterPos.X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, emitterPos.Z),
						IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, listenerPos.X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, listenerPos.Z));
					++line;
				}

				// Display the minimum diffraction angle of the (possibly) two propagation paths. Since we only have Room (i.e. game object) scoped RTPCs in Wwise at the moment, this is the actual value that will be pushed
				// to our built-in game parameter "Diffraction".
				if (paths[path].transmissionLoss == 0.f)
					diffraction = AkMin(diffraction, paths[path].diffraction);
				else
					transmissionLoss = paths[path].transmissionLoss;
			
			}
			m_diffraction = diffraction * 100.f;
			m_tramsissionLoss = transmissionLoss * 100.f;
		}
		else
		{
			m_diffraction = 0.f;
			m_tramsissionLoss = 0.f;
		}
	}

	// Reset m_bMoved.
	m_bMoved = false;

	return Page::Update();
}

void DemoSpatialAudioGeometry::Draw()
{
	Page::Draw();

	if (m_pEmitterChip)
		m_pEmitterChip->Draw();
	if (m_pListenerChip)
		m_pListenerChip->Draw();

	for (int i = 0; i < k_numGeometryPlanes; i++)
	{
		if (m_geometry[i])
			m_geometry[i]->Draw(DrawStyle_Selected);
	}

	for (int i = 0; i < m_numLines; i++)
	{
		if (m_aLines[i])
			m_aLines[i]->Draw( DrawStyle_Control );
	}

	float fAkPosListenerX, fAkPosListenerZ, fAkPosEmitterX, fAkPosEmitterZ;
	m_pListenerChip->GetAkPos(fAkPosListenerX, fAkPosListenerZ);
	m_pEmitterChip->GetAkPos(fAkPosEmitterX, fAkPosEmitterZ);

	char strBuf[128];
	if (m_numLines)
		snprintf(strBuf, 128, "[L](X,Z): %.2f  %.2f\n[E](X,Z): %.2f  %.2f\nDiffraction: %.2f\nTransmission Loss: %.2f",
			fAkPosListenerX, fAkPosListenerZ, fAkPosEmitterX, fAkPosEmitterZ, m_diffraction, m_tramsissionLoss);
	else
		snprintf(strBuf, 128, "[L](X,Z): %.2f  %.2f\n[E](X,Z): %.2f  %.2f", fAkPosListenerX, fAkPosListenerZ, fAkPosEmitterX, fAkPosEmitterZ);

	static const int MAX_LINES_FOR_TEXT = 5;
	static int s_nOffset = MAX_LINES_FOR_TEXT * GetLineHeight( DrawStyle_Text );

	DrawTextOnScreen( strBuf, 5, m_pParentMenu->GetHeight() - s_nOffset, DrawStyle_Text );
	
	// Display instructions at the bottom of the page
	int iInstructionsY = m_pParentMenu->GetHeight() - 3 * GetLineHeight( DrawStyle_Text );
	DrawTextOnScreen( "(Press <<UG_BUTTON2>> To Go Back...)", (int)(m_pParentMenu->GetWidth() * 0.4), iInstructionsY, DrawStyle_Text );
}

bool DemoSpatialAudioGeometry::OnPointerEvent(PointerEventType in_eType, int in_x, int in_y)
{
	if ( in_eType == PointerEventType_Moved )
	{
		bool bMoveListener = true;
		for (UniversalInput::Iterator it = m_pParentMenu->Input()->Begin(); it != m_pParentMenu->Input()->End(); it++)
		{
			if ((*it).IsButtonDown(UG_BUTTON5))
			{
				bMoveListener = false;
				break;
			}
		}

		if (bMoveListener)
			m_pListenerChip->SetPos((float)in_x, (float)in_y);
		else
			m_pEmitterChip->SetPos((float)in_x, (float)in_y);

		m_bMoved = true;	// Force update of game objects and GUI of sound propagation.
	}

	return Page::OnPointerEvent( in_eType, in_x, in_y );
}
