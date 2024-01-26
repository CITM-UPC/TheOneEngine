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

//////////////////////////////////////////////////////////////////////
//
// AkDefaultLowLevelIODispatcher.cpp
//
// Canvas for implementation of the AK::StreamMgr::IAkFileLocationResolver,
// meant to be used in a multiple streaming devices system. It is this
// object that should be registered as the one and only 
// AK::StreamMgr::IAkFileLocationResolver of the Stream Manager
// (by calling AK::StreamMgr::SetFileLocationResolver()).
// 
// It forwards the calls to Open() to one of the low level I/O devices 
// that were added to it using AddDevice(). These devices must thus also 
// implement AK::StreamMgr::IAkFileLocationResolver.
// 
// The strategy for device dispatching is that of a chain of responsibility:
// the dispatcher asks the first file resolver hook to open the file. If it 
// fails, then it tries with the second, and so on, until a hook succeeds. 
// This is inefficient. In your game, you should implement a strategy of 
// your own (see CAkDefaultLowLevelIODispatcher::Open()).
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AkDefaultLowLevelIODispatcher.h"
#include <AK/Tools/Common/AkAssert.h>


CAkDefaultLowLevelIODispatcher::CAkDefaultLowLevelIODispatcher()
:m_uNumDevices( 0 )
{
	RemoveAllDevices();
}

CAkDefaultLowLevelIODispatcher::~CAkDefaultLowLevelIODispatcher()
{
}

AKRESULT CAkDefaultLowLevelIODispatcher::GetNextPreferredDevice(
	AkAsyncFileOpenData& in_FileOpen,		///< File name and flags passed to the Open call.
	AkDeviceID& io_idDevice					///< In: last device used. Out: next device to use to open the file
)
{
	// Here, you need to define a strategy to determine which device is going to handle this file's I/O.
	// You could use some naming convention, or use the AkFileSystemFlags or file name extension if it depends 
	// on file type, or define a map, or read the mapping from an XML file... it is up to the game's organization.
	// Since this default implementation doesn't know anything about that, it forwards the calls to each 
	// device until one of them succeeds. 

	if (io_idDevice == AK_INVALID_DEVICE_ID) //First call always starts with AK_INVALID_DEVICE_ID. Return the first device we know
	{		
		io_idDevice = m_arDevices[0];
	}
	else
	{
		//Find the next device.
		AkUInt32 i = 0;
		while (i < AK_MAX_IO_DEVICES && m_arDevices[i] != io_idDevice)
			i++;

		if (i >= AK_MAX_IO_DEVICES-1)
			io_idDevice = AK_INVALID_DEVICE_ID;
		else
			io_idDevice = m_arDevices[i+1];
	}

	return io_idDevice == AK_INVALID_DEVICE_ID ? AK_FileNotFound : AK_Success;	//Return AK_FileNotFound if all devices were searched.
}

AKRESULT CAkDefaultLowLevelIODispatcher::AddDevice( 
	AkDeviceID in_idDevice /// Device ID as returned by AK::Stream::CreateDevice
)
{
	// Add the device in a free slot.
	for ( AkUInt32 uRecord = 0; uRecord < AK_MAX_IO_DEVICES; uRecord++ )
	{
		if ( m_arDevices[uRecord] == AK_INVALID_DEVICE_ID)
		{
			m_arDevices[uRecord] = in_idDevice;
			++m_uNumDevices;
			return AK_Success;
		}
		if (m_arDevices[uRecord] == in_idDevice)
		{
			AKASSERT(!"Cannot add same device twice");
			return AK_InvalidParameter;
		}
	}
	AKASSERT( !"Cannot hold any more I/O devices" );
	return AK_Fail;
}

void CAkDefaultLowLevelIODispatcher::RemoveAllDevices()
{
	for ( AkUInt32 uRecord = 0; uRecord < AK_MAX_IO_DEVICES; uRecord++ )
		m_arDevices[uRecord] = AK_INVALID_DEVICE_ID;
	m_uNumDevices = 0;
}

