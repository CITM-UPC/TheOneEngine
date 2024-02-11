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
// Uses standard C IO functions, into seperate thread for async IO.
// AioFuncRead and AioFuncWrite handle the IO operation asynchronously.
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
	AK:IAkStreamMgr * pStreamMgr = AK::StreamMgr::Create( stmSettings );
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
	AK:IAkStreamMgr * pStreamMgr = AK::StreamMgr::Create( stmSettings );
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

#include <AK/Tools/Common/AkLock.h>
#include <AK/Tools/Common/AkAutoLock.h>
#include <sys/stat.h>
#include <AK/Tools/Common/AkAssert.h>

#define AK_MAX_MOUNT_POINT_STRLENGTH	(12)

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
		const AkDeviceSettings &	in_deviceSettings,	// Device settings.		
		AkOSChar * 					in_pszMountPoint=NULL	// Mount point (if NULL, SYS_DEV_HDD0 is used).
		);
	void Term();

	// Allow creation of specialized AkFileDesc class for derivatives of this class.
	// On Android, we use the CAkFileHelpers class which has its own derivative.
	// Should be freed in Close();
	virtual AkFileDesc* CreateDescriptor(const AkFileDesc* in_pCopy = nullptr);

	//
	// IAkFileLocationAware interface.
	//-----------------------------------------------------------------------------

	virtual AKRESULT OutputSearchedPaths(
		AKRESULT in_result,						///< Result of the open call
		const AkFileOpenData& in_FileOpen,		///< File name or file ID (only one should be valid!), open flags, open mode
		AkOSChar* out_searchedPath,				///< String containing all searched paths
		AkInt32 in_pathSize						///< The maximum size of the string
	) override;

	//
	// IAkLowLevelIOHook interface.
	//-----------------------------------------------------------------------------

	virtual void BatchOpen(AkUInt32 in_uNumFiles, AkAsyncFileOpenData** in_ppItems) override;

	virtual void BatchRead(AkUInt32 in_uNumTransfers, BatchIoTransferItem* in_pTransferItems) override;

	virtual void BatchWrite(AkUInt32 in_uNumTransfers, BatchIoTransferItem* in_pTransferItems) override;

	virtual void BatchCancel(AkUInt32 in_uNumTransfers, BatchIoTransferItem* in_pTransferItems, bool** io_ppbCancelAllTransfersForThisFile) override;

	// Cleans up a file.
    virtual AKRESULT Close(
        AkFileDesc *			in_pFileDesc			// File descriptor.
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

protected:

	// Open a file (synchronous)
	// This is a helper for subclasses.
	// Note: In this implementation, Open is ALWAYS synchronous, so subclasses can rely on the return code to know the result of the operation.
	virtual AKRESULT Open(
		const AkFileOpenData& in_FileOpen,		///< File open information (name, flags, etc)
		AkFileDesc*& out_pFileDesc              ///< File descriptor produced
	);

	AkDeviceID			m_deviceID;

private:

    // Reads data from a file (asynchronous).
    void Read(
		AkFileDesc &			in_fileDesc,        // File descriptor.
		const AkIoHeuristics &	in_heuristics,		// Heuristics for this data transfer.
		AkAsyncIOTransferInfo & io_transferInfo		// Asynchronous data transfer info.
		);

    // Writes data to a file (asynchronous).
    void Write(
		AkFileDesc &			in_fileDesc,        // File descriptor.
		const AkIoHeuristics &	in_heuristics,		// Heuristics for this data transfer.
		AkAsyncIOTransferInfo & io_transferInfo		// Platform-specific asynchronous IO operation info.
		);

    // Notifies that a transfer request is cancelled. It will be flushed by the streaming device when completed.
    void Cancel(
		AkFileDesc &			in_fileDesc,		// File descriptor.
		AkAsyncIOTransferInfo & io_transferInfo,	// Transfer info to cancel.
		bool & io_bCancelAllTransfersForThisFile	// Flag indicating whether all transfers should be cancelled for this file (see notes in function description).
		);
};

#endif //_AK_DEFAULT_IO_HOOK_DEFERRED_H_
