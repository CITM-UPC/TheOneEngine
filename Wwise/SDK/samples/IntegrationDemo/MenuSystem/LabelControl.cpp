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

// LabelControl.cpp
/// \file 
/// Defines the methods declared in LabelControl.h

#include "stdafx.h"

#include "LabelControl.h"

LabelControl::LabelControl( Page& in_pParentPage ):Control( in_pParentPage )
{
	m_szLabel = "[Label]";
}

LabelControl::~LabelControl()
{
}

void LabelControl::Draw( DrawStyle in_eDrawStyle )
{
	string szDisplayText = m_szLabel +" " + m_labelText;

	DrawTextOnScreen( szDisplayText.c_str(), m_iXPos, m_iYPos, in_eDrawStyle );
}
