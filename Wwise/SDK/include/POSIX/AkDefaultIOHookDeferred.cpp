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
// AkDefaultIOHookDeferred.cpp
//
// Default deferred low level IO hook (AK::StreamMgr::IAkLowLevelIOHook) 
// and file system (AK::StreamMgr::IAkFileLocationResolver) implementation.
// 
// AK::StreamMgr::IAkFileLocationResolver: 
// Resolves file location using simple path concatenation logic 
// (implemented in ../Common/CAkFileLocationBase). It can be used as a 
// standalone Low-Level IO system, or as part of a multi device system. 
// In the latter case, you should manage multiple devices by implementing 
// AK::StreamMgr::IAkFileLocationResolver elsewhere (you may take a look 
// at class CAkDefaultLowLevelIODispatcher).
//
// AK::StreamMgr::IAkLowLevelIOHook: 
// Uses standard C IO functions, into seperate thread for async IO.
// AioFuncRead and AioFuncWrite handle the IO operation asynchronously.
//
// Init() creates a streaming device (by calling AK::StreamMgr::CreateDevice()).
// If there was no AK::StreamMgr::IAkFileLocationResolver previously registered 
// to the Stream Manager, this object registers itself as the File Location Resolver.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AkDefaultIOHookDeferred.h"
#include <AK/SoundEngine/Common/AkMemoryMgr.h>
#include "AkFileHelpers.h"
#include <AK/Tools/Common/AkObject.h>


// Device info.
#define POSIX_DEFERRED_DEVICE_NAME		("POSIX Deferred")	// Default deferred device name.

CAkDefaultIOHookDeferred::CAkDefaultIOHookDeferred()
: m_deviceID( AK_INVALID_DEVICE_ID )
{
}

CAkDefaultIOHookDeferred::~CAkDefaultIOHookDeferred()
{
}

// Initialization/termination. Init() registers this object as the one and 
// only File Location Resolver if none were registered before. Then 
// it creates a streaming device.
AKRESULT CAkDefaultIOHookDeferred::Init(
	const AkDeviceSettings &	in_deviceSettings,			// Device settings.
	AkOSChar * 					in_pszMountPoint/*=NULL*/	// Mount point (optional, can be NULL).
	)
{	
	// If the Stream Manager's File Location Resolver was not set yet, set this object as the 
	// File Location Resolver (this I/O hook is also able to resolve file location).
	if ( !AK::StreamMgr::GetFileLocationResolver() )
		AK::StreamMgr::SetFileLocationResolver( this );

	// Create a device in the Stream Manager, specifying this as the hook.
	return AK::StreamMgr::CreateDevice( in_deviceSettings, this, m_deviceID);
}

void CAkDefaultIOHookDeferred::Term()
{
	CAkMultipleFileLocation::Term();

	if ( AK::StreamMgr::GetFileLocationResolver() == this )
		AK::StreamMgr::SetFileLocationResolver( NULL );
	
	AK::StreamMgr::DestroyDevice( m_deviceID );
}

AkFileDesc* CAkDefaultIOHookDeferred::CreateDescriptor(const AkFileDesc* in_pCopy)
{
	if (!in_pCopy)
		return AkNew(AkMemID_Streaming, AkFileDescType());

	return AkNew(AkMemID_Streaming, AkFileDescType(*(AkFileDescType*)in_pCopy));
}

AKRESULT CAkDefaultIOHookDeferred::Open(const AkFileOpenData& in_FileOpen, AkFileDesc*& out_pFileDesc)
{
	out_pFileDesc = CreateDescriptor();
	if (!out_pFileDesc)
		return AK_InsufficientMemory;

	AKRESULT eResult = CAkMultipleFileLocation::Open(in_FileOpen, true, *out_pFileDesc);
	if (eResult == AK_Success)
	{
		out_pFileDesc->deviceID = m_deviceID;
	}
	else
	{
		AkDelete(AkMemID_Streaming, out_pFileDesc);
		out_pFileDesc = NULL;
	}

	return eResult;	
}

AKRESULT CAkDefaultIOHookDeferred::OutputSearchedPaths(
	AKRESULT in_result,
	const AkFileOpenData& in_FileOpen,
	AkOSChar* out_searchedPath,
	AkInt32 in_pathSize
)
{
	return CAkMultipleFileLocation::OutputSearchedPaths(in_FileOpen, out_searchedPath, in_pathSize);
}

//
// IAkLowLevelIOHook implementation.
//-----------------------------------------------------------------------------
// Async Read
void AioFuncRead(AkAsyncIOTransferInfo& transferInfo)
{
	
	AkFileHandle fileHandle = ((AkFileDesc*)transferInfo.pUserData)->hFile;

	// Seek in the file
	if( lseek( fileno(fileHandle), transferInfo.uFilePosition, SEEK_SET ) == transferInfo.uFilePosition )
	{
		// Read the data
		AkUInt32 byteTransferred = (AkUInt32) read( fileno(fileHandle), transferInfo.pBuffer, transferInfo.uRequestedSize );
		
		if( byteTransferred == transferInfo.uRequestedSize )
		{
			transferInfo.pCallback( &transferInfo, AK_Success );
			return;
		}
	}
	
	transferInfo.pCallback( &transferInfo, AK_Fail );
}

// Async Write
void AioFuncWrite(AkAsyncIOTransferInfo& transferInfo)
{
	AkFileHandle fileHandle = ((AkFileDesc*)transferInfo.pUserData)->hFile;
	
	// Seek in the file
	if( lseek( fileno(fileHandle), transferInfo.uFilePosition, SEEK_SET ) == transferInfo.uFilePosition )
	{
		// Read the data
		AkUInt32 byteTransferred = (AkUInt32) write( fileno(fileHandle), transferInfo.pBuffer, transferInfo.uRequestedSize );
		
		if( byteTransferred == transferInfo.uRequestedSize )
		{
			transferInfo.pCallback( &transferInfo, AK_Success );
			fflush(fileHandle);
			return;
		}
	}
	
	transferInfo.pCallback( &transferInfo, AK_Fail );
}

// Reads data from a file (asynchronous overload).
void CAkDefaultIOHookDeferred::Read(
	AkFileDesc &			in_fileDesc,        // File descriptor.
	const AkIoHeuristics & /*in_heuristics*/,	// Heuristics for this data transfer (not used in this implementation).
	AkAsyncIOTransferInfo & io_transferInfo		// Asynchronous data transfer info.
	)
{
	io_transferInfo.pUserData = (void*)&in_fileDesc;
	
	//Call the low level read which takes care of error handling with the callback 
	AioFuncRead(io_transferInfo);
}

// Writes data to a file (asynchronous overload).
void CAkDefaultIOHookDeferred::Write(
	AkFileDesc &			in_fileDesc,        // File descriptor.
	const AkIoHeuristics & /*in_heuristics*/,	// Heuristics for this data transfer (not used in this implementation).
	AkAsyncIOTransferInfo & io_transferInfo		// Platform-specific asynchronous IO operation info.
	)
{
	io_transferInfo.pUserData = (void*)&in_fileDesc;
	
	//Call the low level read which takes care of error handling with the callback 
	AioFuncWrite(io_transferInfo);
}

// Cancel transfer(s).
void CAkDefaultIOHookDeferred::Cancel(
	AkFileDesc &			/*in_fileDesc*/,	// File descriptor.
	AkAsyncIOTransferInfo & io_transferInfo,	// Transfer info to cancel.
	bool & io_bCancelAllTransfersForThisFile	// Flag indicating whether all transfers should be cancelled for this file (see notes in function description).
	)
{
	// not implemented
}

// Close a file.
AKRESULT CAkDefaultIOHookDeferred::Close(
	AkFileDesc *	in_pFileDesc      // File descriptor.
    )
{
	if (in_pFileDesc)
	{
		CAkFileHelpers::CloseFile(*in_pFileDesc);
		AkDelete(AkMemID_Streaming, in_pFileDesc);
	}
	return AK_Success;
}

// Returns the block size for the file or its storage device. 
AkUInt32 CAkDefaultIOHookDeferred::GetBlockSize(
    AkFileDesc &  /*in_fileDesc*/     // File descriptor.
    )
{
	// There is no limitation nor performance degradation with unaligned
	// seeking on any mount point with asynchronous cell fs API.
    return 1;
}

// Returns a description for the streaming device above this low-level hook.
void CAkDefaultIOHookDeferred::GetDeviceDesc(
    AkDeviceDesc & 
#ifndef AK_OPTIMIZED
	out_deviceDesc      // Description of associated low-level I/O device.
#endif
    )
{
#ifndef AK_OPTIMIZED
	// Deferred scheduler.
	out_deviceDesc.deviceID       = m_deviceID;
	out_deviceDesc.bCanRead       = true;
	out_deviceDesc.bCanWrite      = true;
	AK_CHAR_TO_UTF16( out_deviceDesc.szDeviceName, POSIX_DEFERRED_DEVICE_NAME, AK_MONITOR_DEVICENAME_MAXLENGTH );
	out_deviceDesc.szDeviceName[AK_MONITOR_DEVICENAME_MAXLENGTH - 1] = '\0';
	out_deviceDesc.uStringSize = (AkUInt32)AKPLATFORM::AkUtf16StrLen(out_deviceDesc.szDeviceName) + 1;
#endif
}

// Returns custom profiling data: 1 if file opens are asynchronous, 0 otherwise.
AkUInt32 CAkDefaultIOHookDeferred::GetDeviceData()
{
	return 1;
}

void CAkDefaultIOHookDeferred::BatchOpen(AkUInt32 in_uNumFiles, AkAsyncFileOpenData** in_ppItems)
{
	for (int i = 0; i < (int)in_uNumFiles; i++)
	{
		AkAsyncFileOpenData* pItem = in_ppItems[i];
		AKRESULT eResult = Open(*pItem, pItem->pFileDesc);
		pItem->pCallback(pItem, eResult);
	}
}

void CAkDefaultIOHookDeferred::BatchRead(AkUInt32 in_uNumTransfers, BatchIoTransferItem* in_pTransferItems)
{
	for (int i = 0; i < (int)in_uNumTransfers; ++i)
	{
		BatchIoTransferItem ioTransferItem = in_pTransferItems[i];
		Read(*(ioTransferItem.pFileDesc), ioTransferItem.ioHeuristics, *(ioTransferItem.pTransferInfo));
	}
}

void CAkDefaultIOHookDeferred::BatchWrite(AkUInt32 in_uNumTransfers, BatchIoTransferItem* in_pTransferItems)
{
	for (int i = 0; i < (int)in_uNumTransfers; ++i)
	{
		BatchIoTransferItem ioTransferItem = in_pTransferItems[i];
		Write(*(ioTransferItem.pFileDesc), ioTransferItem.ioHeuristics, *(ioTransferItem.pTransferInfo));
	}
}

void CAkDefaultIOHookDeferred::BatchCancel(
	AkUInt32						in_uNumTransfers,
	BatchIoTransferItem* in_pTransferItems,
	bool** io_ppbCancelAllTransfersForThisFile
)
{
	for (int i = 0; i < (int)in_uNumTransfers; ++i)
	{
		BatchIoTransferItem ioTransferItem = in_pTransferItems[i];
		Cancel(*(ioTransferItem.pFileDesc), *(ioTransferItem.pTransferInfo), *io_ppbCancelAllTransfersForThisFile[i]);
	}
}
