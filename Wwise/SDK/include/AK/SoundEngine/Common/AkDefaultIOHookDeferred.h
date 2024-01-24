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
// AkDefaultIOHookDeferred.h
//
// Default deferred low level IO hook (AK::StreamMgr::IAkLowLevelIOHook) 
// and file system (AK::StreamMgr::IAkFileLocationResolver) implementation.
// 
// AK::StreamMgr::IAkFileLocationResolver: 
// Resolves file location using simple path concatenation logic.
// It can be used as a standalone
// Low-Level IO system, or as part of a multi device system. 
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
// Examples of streaming initialization:
// 
// Standalone (registered as the one and only File Location Resolver):
/* 
	// Create Stream Manager.
	AkStreamMgrSettings stmSettings;
	AK::StreamMgr::GetDefaultSettings( stmSettings );
	AK::IAkStreamMgr * pStreamMgr = AK::StreamMgr::Create( stmSettings );
	AKASSERT( pStreamMgr );

	// Create deferred device.
	AkDeviceSettings deviceSettings;
	AK::StreamMgr::GetDefaultDeviceSettings( deviceSettings );
	CAkDefaultIOHookDeferred hookIODeferred;
	AKRESULT eResult = hookIODeferred.Init( deviceSettings );
	AKASSERT( AK_SUCCESS == eResult );
*/
//
// As part of a system with multiple devices (the File Location Resolver is 
// implemented by CAkDefaultLowLevelIODispatcher):
/* 
	// Create Stream Manager.
	AkStreamMgrSettings stmSettings;
	AK::StreamMgr::GetDefaultSettings( stmSettings );
	AK::IAkStreamMgr * pStreamMgr = AK::StreamMgr::Create( stmSettings );
	AKASSERT( pStreamMgr );

	// Create and register the File Location Resolver.
	CAkDefaultLowLevelIODispatcher lowLevelIODispatcher;
	AK::StreamMgr::SetFileLocationResolver( &lowLevelIODispatcher );

	// Create deferred device.
	AkDeviceSettings deviceSettings;
	AK::StreamMgr::GetDefaultDeviceSettings( deviceSettings );
	CAkDefaultIOHookDeferred hookIODeferred;
	AKRESULT eResult = hookIODeferred.Init( deviceSettings );
	AKASSERT( AK_SUCCESS == eResult );

	// Add it to the global File Location Resolver.
	lowLevelIODispatcher.AddDevice( hookIODeferred );

	// Create more devices.
	// ...
*/
//
//////////////////////////////////////////////////////////////////////

#ifndef _AK_DEFAULT_IO_HOOK_DEFERRED_H_
#define _AK_DEFAULT_IO_HOOK_DEFERRED_H_

#include <AK/SoundEngine/Common/AkStreamMgrModule.h>
#include "../Common/AkMultipleFileLocation.h"
#include <AK/Tools/Common/AkAssert.h>
#include <AK/Tools/Common/AkObject.h>
#include <AK/Tools/Common/AkObjectPool.h>

//-----------------------------------------------------------------------------
// Name: class CAkDefaultIOHookDeferred.
// Desc: Implements IAkLowLevelIOHook low-level I/O hook, and 
//		 IAkFileLocationResolver. Can be used as a standalone Low-Level I/O
//		 system, or as part of a system with multiple devices.
//		 File location is resolved using simple path concatenation logic.
//-----------------------------------------------------------------------------
class CAkDefaultIOHookDeferred : public AK::StreamMgr::IAkFileLocationResolver
								,public AK::StreamMgr::IAkLowLevelIOHook
								,public CAkMultipleFileLocation
{
public:

	typedef AkFileDesc AkFileDescType;

	CAkDefaultIOHookDeferred();
	virtual ~CAkDefaultIOHookDeferred();

	// Initialization/termination. Init() registers this object as the one and 
	// only File Location Resolver if none were registered before. Then 
	// it creates a streaming device.
	AKRESULT Init(
		const AkDeviceSettings &	in_deviceSettings	// Device settings.
		);
	void Term();

	// Allow creation of specialized AkFileDesc class for derivatives of this class.
	// Default implementation allocates a AkFileDesc with AkNew, which should be freed in Close().
	virtual AkFileDesc* CreateDescriptor(const AkFileDesc* in_pCopy = nullptr);

	//
	// IAkFileLocationAware interface.
	//-----------------------------------------------------------------------------

	virtual AKRESULT OutputSearchedPaths(
		AKRESULT in_result,						///< Result of the open call
		const AkFileOpenData& in_FileOpen,		///< File name or file ID (only one should be valid!), open flags, open mode
		AkOSChar* out_searchedPath,				///< String containing all searched paths
		AkInt32 in_pathSize						///< The maximum size of the string
	);

	//
	// IAkLowLevelIOHook interface.
	//-----------------------------------------------------------------------------

	// Cleans up a file.
	virtual AKRESULT Close(
        AkFileDesc *			in_pFileDesc		// File descriptor.
        ) override;

	// Returns the block size for the file or its storage device. 
	virtual AkUInt32 GetBlockSize(
        AkFileDesc &  			in_fileDesc			// File descriptor.
        ) override;

	// Returns a description for the streaming device above this low-level hook.
    virtual void GetDeviceDesc(
        AkDeviceDesc &  		out_deviceDesc      // Description of associated low-level I/O device.
        ) override;

	// Returns custom profiling data: 1 if file opens are asynchronous, 0 otherwise.
	virtual AkUInt32 GetDeviceData() override;

	virtual void BatchOpen(AkUInt32 in_uNumFiles, AkAsyncFileOpenData** in_ppItems) override;

	virtual void BatchRead(AkUInt32 in_uNumTransfers, BatchIoTransferItem* in_pTransferItems) override;

	virtual void BatchWrite(AkUInt32 in_uNumTransfers, BatchIoTransferItem* in_pTransferItems) override;

	virtual void BatchCancel(AkUInt32 in_uNumTransfers, BatchIoTransferItem* in_pTransferItems, bool** io_ppbCancelAllTransfersForThisFile) override;

protected:
	
	// Local callback for overlapped I/O.
	static VOID CALLBACK FileIOCompletionRoutine(
		DWORD dwErrorCode,
		DWORD dwNumberOfBytesTransfered,
		LPOVERLAPPED lpOverlapped
		);

protected:
	using OverlappedPool = AK::ObjectPool<OVERLAPPED, AK::ObjectPoolDefaultAllocator<AkMemID_Streaming>>;

	AkDeviceID			m_deviceID;
	OverlappedPool		m_pool;

	// Structures for concurrent asynchronous transfers bookkeeping.
	// Note 1: The pool is a fixed block size pool. It returns OVERLAPPED objects. Allocation is guaranteed
	// because the pool size is MaxConcurrentIO * sizeof(OVERLAPPED).
	// Note 2: accesses to memory pool are not locked, because we only use the platform SDK here,
	// which is executed by the I/O thread when it is in an alertable state.
	// If you release overlapped objects from your own thread, a lock is mandatory.
	// Note 3: we use the hEvent field to store a pointer to the AkAsyncIOTransferInfo structure.
	
	// Get a free slot for an OVERLAPPED I/O transfer.
	// Transfer that will use this OVERLAPPED. Its address is stored in OVERLAPPED::hEvent.
	OVERLAPPED * GetFreeOverlapped(AkAsyncIOTransferInfo * in_pTransfer)
	{
		OVERLAPPED* pOverlapped = m_pool.Allocate();
		AKASSERT( pOverlapped || !"Too many concurrent transfers in the Low-Level IO" );

		if (!pOverlapped)
			return nullptr;

		pOverlapped->hEvent = in_pTransfer;
		return pOverlapped;
	}

	// Release a slot after an OVERLAPPED I/O transfer.
	inline void ReleaseOverlapped(OVERLAPPED * in_pOverlapped)
	{
		m_pool.Deallocate(in_pOverlapped);
	}

	// Open a file (synchronous)
	// This is a helper for subclasses.
	// Note: In this implementation, Open is ALWAYS synchronous, so subclasses can rely on the return code to know the result of the operation.
	virtual AKRESULT Open(
		const AkFileOpenData& in_FileOpen,		///< File open information (name, flags, etc)
		AkFileDesc*& out_pFileDesc              ///< File descriptor produced
	);

private:

	// Reads data from a file (asynchronous).
	void Read(
		AkFileDesc& in_fileDesc,        // File descriptor.
		const AkIoHeuristics& in_heuristics,		// Heuristics for this data transfer.
		AkAsyncIOTransferInfo& io_transferInfo		// Asynchronous data transfer info.
	);

	// Writes data to a file (asynchronous).
	void Write(
		AkFileDesc& in_fileDesc,        // File descriptor.
		const AkIoHeuristics& in_heuristics,		// Heuristics for this data transfer.
		AkAsyncIOTransferInfo& io_transferInfo		// Platform-specific asynchronous IO operation info.
	);

	// Notifies that a transfer request is cancelled. It will be flushed by the streaming device when completed.
	void Cancel(
		AkFileDesc& in_fileDesc,		// File descriptor.
		AkAsyncIOTransferInfo& io_transferInfo,	// Transfer info to cancel.
		bool& io_bCancelAllTransfersForThisFile	// Flag indicating whether all transfers should be cancelled for this file (see notes in function description).
	);
};

#endif //_AK_DEFAULT_IO_HOOK_DEFERRED_H_
