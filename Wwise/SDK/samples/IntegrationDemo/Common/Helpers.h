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

// IntegrationDemo.h
/// \file 
/// Declares various helpers for the Integration Demo

#pragma once
#include <string>
#include <AK/SoundEngine/Common/AkTypes.h>

namespace IntegrationDemoHelpers
{
	/// Returns the number of elements in an array
	template <class T, size_t N> AkForceInline size_t AK_ARRAYSIZE(T(&)[N])
	{
		return N;
	}

	void WordWrapString( std::string& io_strWrapped, const size_t in_nMaxWidth );
	std::string WordWrapString( const char* in_szOriginal, const size_t in_nMaxWidth );

	float PixelsToAKPos_X(float m_fWidth, float in_X);
	float PixelsToAKPos_Y(float m_fHeight, float in_Y);
	float PixelsToAKLen_X(float m_fWidth, float in_X);
	float PixelsToAKLen_Y(float m_fHeight, float in_Y);
	int AKPosToPixels_X(float m_fWidth, double in_X);
	int AKPosToPixels_Y(float m_fHeight, double in_Y);
}

/// Replaces tags by their platform-specific values
void ReplaceTags(std::string& io_strTagString);

class IRefCountable
{
public:
	virtual void AddRef() = 0;
	virtual void Release() = 0;
};