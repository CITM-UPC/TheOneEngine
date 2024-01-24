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
// and file system (AK::StreamMgr::IAkFileLocationResolver) implementation 
// on Windows.
// 
// AK::StreamMgr::IAkFileLocationResolver: 
// Resolves file location using simple path concatenation logic.
// It can be used as a 
// standalone Low-Level IO system, or as part of a multi device system. 
// In the latter case, you should manage multiple devices by implementing 
// AK::StreamMgr::IAkFileLocationResolver elsewhere (you may take a look 
// at class CAkDefaultLowLevelIODispatcher).
//
// AK::StreamMgr::IAkLowLevelIOHook: 
// Uses platform API for I/O. Calls to ::ReadFile() and ::WriteFile() 
// do not block because files are opened with the FILE_FLAG_OVERLAPPED flag. 
// Transfer completion is handled by internal FileIOCompletionRoutine function,
// which then calls the AkAIOCallback.
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


// Device info.
#define WIN32_DEFERRED_DEVICE_NAME		(L"Win32 Deferred")	// Default deferred device name.

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
	const AkDeviceSettings &	in_deviceSettings		// Device settings.
	)
{	
	AKRESULT poolResult = m_pool.Init(in_deviceSettings.uMaxConcurrentIO);
	if (poolResult != AK_Success)
		return poolResult;

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
	m_pool.Term();
}

//This should always be overridden by the final child class to create the correct type, a descendent of AkFileDesc.
AkFileDesc* CAkDefaultIOHookDeferred::CreateDescriptor(const AkFileDesc* in_pCopy)
{
	if (!in_pCopy)
		return AkNew(AkMemID_Streaming, AkFileDescType());

	return AkNew(AkMemID_Streaming, AkFileDescType(*(AkFileDescType*)in_pCopy));
}

//
// IAkFileLocationAware implementation.
//-----------------------------------------------------------------------------

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

// Local callback for overlapped I/O.
VOID CALLBACK CAkDefaultIOHookDeferred::FileIOCompletionRoutine(
  DWORD dwErrorCode,
  DWORD 
#ifdef _DEBUG
  dwNumberOfBytesTransfered
#endif
  ,
  LPOVERLAPPED lpOverlapped
)
{
	AkAsyncIOTransferInfo * pXferInfo = (AkAsyncIOTransferInfo*)(lpOverlapped->hEvent);

	CAkDefaultIOHookDeferred* ioHook = (CAkDefaultIOHookDeferred*)pXferInfo->pUserData;
	ioHook->ReleaseOverlapped( lpOverlapped );

	AKRESULT eResult = AK_Fail;
	if ( ERROR_SUCCESS == dwErrorCode )
	{
		eResult = AK_Success;
		AKASSERTD( dwNumberOfBytesTransfered >= pXferInfo->uRequestedSize && dwNumberOfBytesTransfered <= pXferInfo->uBufferSize );
	}

	pXferInfo->pCallback( pXferInfo, eResult );
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
		out_pFileDesc = nullptr;
	}

	return eResult;
}

// Reads data from a file (asynchronous overload).
void CAkDefaultIOHookDeferred::Read(
	AkFileDesc &			in_fileDesc,        // File descriptor.
	const AkIoHeuristics & /*in_heuristics*/,	// Heuristics for this data transfer (not used in this implementation).
	AkAsyncIOTransferInfo & io_transferInfo		// Asynchronous data transfer info.
	)
{
	AKASSERT( in_fileDesc.hFile != INVALID_HANDLE_VALUE 
			&& io_transferInfo.uRequestedSize > 0 
			&& io_transferInfo.uBufferSize >= io_transferInfo.uRequestedSize );

	io_transferInfo.pUserData = this;
	OVERLAPPED * pOverlapped = GetFreeOverlapped( &io_transferInfo );

	AKASSERT(pOverlapped);
	if (!pOverlapped)
	{
		io_transferInfo.pCallback(&io_transferInfo, AK_InsufficientMemory);
		return;
	}

	// Set file offset in OVERLAPPED structure.
	pOverlapped->Offset = (DWORD)( io_transferInfo.uFilePosition & 0xFFFFFFFF );
	pOverlapped->OffsetHigh = (DWORD)( ( io_transferInfo.uFilePosition >> 32 ) & 0xFFFFFFFF );

	// File was open with asynchronous flag. 
    // Read overlapped. 
	// Note: With a file handle opened with FILE_FLAG_NO_BUFFERING, ::ReadFileEx() supports read sizes that go beyond the end
	// of file. However, it does not support read sizes that are not a multiple of the drive's sector size.
	// Since the buffer size is always a multiple of the block size, let's use io_transferInfo.uBufferSize
	// instead of io_transferInfo.uRequestedSize.
    if ( ::ReadFileEx( in_fileDesc.hFile,
                      io_transferInfo.pBuffer,
                      io_transferInfo.uBufferSize,
                      pOverlapped,
					  CAkDefaultIOHookDeferred::FileIOCompletionRoutine ) )
	{
		return; // Callback will be called by FileIOCompletionRoutine
	}
	else
	{
		// Completion routine will not be called, complete the transfer now.
		ReleaseOverlapped(pOverlapped);
		AKRESULT eResult = GetLastError() == ERROR_ACCESS_DENIED ? AK_FilePermissionError : AK_Fail;
		io_transferInfo.pCallback(&io_transferInfo, eResult);
	}
}

// Writes data to a file (asynchronous overload).
void CAkDefaultIOHookDeferred::Write(
	AkFileDesc &			in_fileDesc,        // File descriptor.
	const AkIoHeuristics & /*in_heuristics*/,	// Heuristics for this data transfer (not used in this implementation).
	AkAsyncIOTransferInfo & io_transferInfo		// Platform-specific asynchronous IO operation info.
	)
{
	AKASSERT( in_fileDesc.hFile != INVALID_HANDLE_VALUE 
			&& io_transferInfo.uRequestedSize > 0 );

	// If this assert comes up, it might be beacause this hook's GetBlockSize() return value is incompatible 
	// with the system's handling of file reading for this specific file handle.
	// Are you using the File Package Low-Level I/O with incompatible block size? (check -blocksize argument in the File Packager command line)
	AKASSERT( io_transferInfo.uFilePosition % GetBlockSize( in_fileDesc ) == 0
			|| !"Requested file position for I/O transfer is inconsistent with block size" );

	io_transferInfo.pUserData = this;
	OVERLAPPED * pOverlapped = GetFreeOverlapped( &io_transferInfo );

	AKASSERT(pOverlapped);
	if (!pOverlapped)
	{
		io_transferInfo.pCallback(&io_transferInfo, AK_InsufficientMemory);
		return;
	}

	// Set file offset in OVERLAPPED structure.
	pOverlapped->Offset = (DWORD)( io_transferInfo.uFilePosition & 0xFFFFFFFF );
	pOverlapped->OffsetHigh = (DWORD)( ( io_transferInfo.uFilePosition >> 32 ) & 0xFFFFFFFF );
	io_transferInfo.pUserData = this;

    // File was open with asynchronous flag. 
    if ( ::WriteFileEx( in_fileDesc.hFile,
                      io_transferInfo.pBuffer,
                      io_transferInfo.uRequestedSize,
                      pOverlapped,
					  CAkDefaultIOHookDeferred::FileIOCompletionRoutine ) )
	{
		return; // Callback will be called by FileIOCompletionRoutine
	}
	else
	{
		// Completion routine will not be called, complete the transfer now.
		ReleaseOverlapped(pOverlapped);
		AKRESULT eResult = GetLastError() == ERROR_ACCESS_DENIED ? AK_FilePermissionError : AK_Fail;
		io_transferInfo.pCallback(&io_transferInfo, eResult);
	}
}

// Cancel transfer(s).
void CAkDefaultIOHookDeferred::Cancel(
	AkFileDesc &			in_fileDesc,		// File descriptor.
	AkAsyncIOTransferInfo & /*io_transferInfo*/,// Transfer info to cancel (this implementation only handles "cancel all").
	bool & io_bCancelAllTransfersForThisFile	// Flag indicating whether all transfers should be cancelled for this file (see notes in function description).
	)
{
	if ( io_bCancelAllTransfersForThisFile )
	{
		CancelIo( in_fileDesc.hFile );
		// Leave io_bCancelAllTransfersForThisFile to true: all transfers were cancelled for this file,
		// so we don't need to be called again.
	}
}

// Close a file.
AKRESULT CAkDefaultIOHookDeferred::Close(
    AkFileDesc * in_pFileDesc      // File descriptor.
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
    AkFileDesc &  in_fileDesc     // File descriptor.
    )
{
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
	AKASSERT( m_deviceID != AK_INVALID_DEVICE_ID || !"Low-Level device was not initialized" );

	// Deferred scheduler.
	out_deviceDesc.deviceID       = m_deviceID;
	out_deviceDesc.bCanRead       = true;
	out_deviceDesc.bCanWrite      = true;
	AKPLATFORM::SafeStrCpy( out_deviceDesc.szDeviceName, WIN32_DEFERRED_DEVICE_NAME, AK_MONITOR_DEVICENAME_MAXLENGTH );
	out_deviceDesc.uStringSize   = (AkUInt32)wcslen( out_deviceDesc.szDeviceName ) + 1;
#endif
}

// Returns custom profiling data: 1 if file opens are asynchronous, 0 otherwise.
// Tip: An interesting application for custom profiling data in deferred devices is to display
// the number of requests currently pending in the low-level IO.
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
