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
// AkFilePackageLowLevelIO.h
//
// CAkFilePackageLowLevelIO extends a Low-Level I/O device by providing 
// the ability to reference files that are part of a file package.
//
// It can extend either blocking or deferred I/O hooks (both inheriting from
// AK::StreamMgr::IAkLowLevelIOHook), since its base class is templated. 
// In either case, the base class must also implement 
// AK::StreamMgr::IAkFileLocationResolver. This interface defines both overloads
// for Open(), and this is where the package's look-up table is searched.
// If no match is found, then it falls back on the base implementation.
// 
// Clients of devices that use this class' functionnality simply need to call
// LoadFilePackage(), which loads and parses file packages that were created with 
// the AkFilePackager utility app (located in ($WWISESDK)/samples/FilePackager/). 
// The header of these file packages contains look-up tables that describe the 
// internal offset of each file it references, their block size (required alignment), 
// and their language. Each combination of AkFileID and Language ID is unique.
//
// LoadFilePackage() returns a package ID that can be used to unload it. Any number
// of packages can be loaded simultaneously. When Open() is called, the last package 
// loaded is searched first, then the previous one, and so on.
//
// The language ID was created dynamically when the package was created. The header 
// also contains a map of language names (strings) to their ID, so that the proper 
// language-specific version of files can be resolved. The language name that is stored
// matches the name of the directory that is created by the Wwise Bank Manager,
// except for the trailing slash.
//
// The type of package is also a template argument. By default, it is a disk package
// (see AkDiskPackage.h).
//
//////////////////////////////////////////////////////////////////////

#ifndef _AK_FILE_PACKAGE_LOW_LEVEL_IO_H_
#define _AK_FILE_PACKAGE_LOW_LEVEL_IO_H_

#include <AK/SoundEngine/Common/AkStreamMgrModule.h>
#include "AkFilePackage.h"
#include <AK/Tools/Common/AkAutoLock.h>
#include <AK/Tools/Common/AkLock.h>

//-----------------------------------------------------------------------------
// Name: AkFilePackageReader 
// Desc: This class wraps an AK::IAkStdStream to read a file package.
//-----------------------------------------------------------------------------
class AkFilePackageReader
{
public:
	AkFilePackageReader()
		: m_pStream( NULL ) {}
	~AkFilePackageReader() 
	{
		// IMPORTANT: Do not close file. This object can be copied.
	}

	AKRESULT Open(
		const AkOSChar*	in_pszFilePackageName,	// File package name. Location is resolved using base class' Open().
		bool in_bReadFromSFXOnlyDir		// If true, the file package is opened from the language agnostic directory only. Otherwise, it tries to open it 
										// from the current language-specific directory first, and then from the common directory if it fails, similarly to the soundbanks loader of the Sound Engine (Default).
		)
	{
		AkFileSystemFlags flags;
		flags.uCompanyID = AKCOMPANYID_AUDIOKINETIC;
		flags.uCodecID = AKCODECID_FILE_PACKAGE;
		flags.uCustomParamSize = 0;
		flags.pCustomParam = NULL;
		flags.bIsLanguageSpecific = !in_bReadFromSFXOnlyDir;

		return AK::IAkStreamMgr::Get()->CreateStd(AkFileOpenData(in_pszFilePackageName, &flags), m_pStream, true);
	}

	AKRESULT Read(
		void *			in_pBuffer,			// Buffer. Must be aligned with value returned by GetBlockSize().
		AkUInt32		in_uSizeToRead,		// Size to read. Must be a multiple of value returned by GetBlockSize().
		AkUInt32 &		out_uSizeRead,		// Returned size read.
		AkPriority		in_priority = AK_DEFAULT_PRIORITY,	// Priority heuristic.
		AkReal32		in_fThroughput = 0	// Throughput heuristic. 0 means "not set", and results in "immediate".
		)
	{
		AKASSERT( m_pStream );
		AkReal32 fDeadline = ( in_fThroughput > 0 ) ? in_uSizeToRead / in_fThroughput : 0;
		return m_pStream->Read( 
			in_pBuffer, 
			in_uSizeToRead,
			true,
			in_priority,
			fDeadline,
			out_uSizeRead );
	}

	AKRESULT Seek( 
		AkUInt32		in_uPosition,
		AkUInt32 &		out_uRealOffset
		)
	{
		AKRESULT eResult = m_pStream->SetPosition( in_uPosition, AK_MoveBegin);
		AKASSERT( eResult == AK_Success || !"Failed changing stream position" );
		out_uRealOffset = in_uPosition;
		return eResult;
	}

	void Close()
	{
		if ( m_pStream )
			m_pStream->Destroy();
		m_pStream = NULL;
	}

	void SetName( 
		const AkOSChar* 
#ifndef AK_OPTIMIZED
		in_pszName 
#endif
		)
	{
#ifndef AK_OPTIMIZED
		AKASSERT( m_pStream );
		m_pStream->SetStreamName( in_pszName );
#endif
	}

	AkUInt64 GetSize()
	{
		AKASSERT( m_pStream );
		AkStreamInfo info;
		m_pStream->GetInfo( info );
		return info.uSize;
	}

	AkUInt32 GetBlockSize()
	{
		return m_pStream->GetBlockSize();
	}

	AkFileHandle GetHandle()
	{
		AKASSERT( m_pStream );
		AkFileDesc * pFileDesc = (AkFileDesc*)m_pStream->GetFileDescriptor();
		AKASSERT( pFileDesc );
		return pFileDesc->hFile;
	}

	AkFileDesc *GetFileDesc()
	{
		return (AkFileDesc*)m_pStream->GetFileDescriptor();
	}

private:
	AK::IAkStdStream *	m_pStream;
};

//-----------------------------------------------------------------------------
// Name: CAkDiskPackage 
// Desc: This class extends the CAkFilePackage class by providing system handle
// management.
// It keeps a copy of the file package reader that was used to read the file package
// header from disk, and uses it to query and cache its low-level system handle
// (AkFileDesc::hFile). This handle is kept open and used directly to read portions
// of the package from disk, corresponding to read requests for the files it 
// contains. The streaming object / package handle is closed when the package
// is destroyed.
//-----------------------------------------------------------------------------
class CAkDiskPackage : public CAkFilePackage
{
public:
	// Factory for disk package.
	// Instantiates a file package object, queries its file handle once and keep in package.
	// Also keeps a copy of its reader object, which is used to close the file handle on destruction.
	static CAkDiskPackage * Create( 
		AkFilePackageReader & in_reader,		// File package reader.
		const AkOSChar*		in_pszPackageName,	// Name of the file package (for memory monitoring).
		AkUInt32 			in_uHeaderSize,		// File package header size, including the size of the header chunk AKPK_HEADER_CHUNK_DEF_SIZE.
		AkUInt32 &			out_uReservedHeaderSize, // Size reserved for header, taking mem align into account.
		AkUInt8 *&			out_pHeaderBuffer	// Returned address of memory for header.
		)
	{
		CAkDiskPackage * pPackage = CAkFilePackage::Create<CAkDiskPackage>( 
			in_pszPackageName,
			in_uHeaderSize,
			in_reader.GetBlockSize(),
			out_uReservedHeaderSize,
			out_pHeaderBuffer );
		if ( pPackage )
		{
			pPackage->m_reader = in_reader;				// Copy reader.
			AkFileDesc* pFileDesc = in_reader.GetFileDesc();
			pPackage->m_hFile = pFileDesc->hFile;	// Cache handle.
		}
		return pPackage;
	}

	CAkDiskPackage(AkUInt32 in_uPackageID, AkUInt32 in_uHeaderSize, void * in_pToRelease)
		: CAkFilePackage(in_uPackageID, in_uHeaderSize, in_pToRelease)
	{ }

	// Override Destroy(): Close 
	virtual void Destroy()
	{
		m_reader.Close();
		CAkFilePackage::Destroy();
	}

	// Fills an AkFileHandle with a value that will be useable by the low-level I/O hook.
	// Disk packages return the package's system handle: the hook reads from the package file itself, with
	// proper offset, to get the data it needs.
	inline AkFileHandle GetHandleForFileDesc() { return m_hFile; }	

	AkFileDesc* GetFileDesc() {return m_reader.GetFileDesc();}

protected:
	
	AkFilePackageReader	m_reader;	// READER object. Holds the stream used to read the package. Closed only upon package destruction.
	AkFileHandle		m_hFile;	// Platform-independent file handle (cached from READER).
};

//-----------------------------------------------------------------------------
// Name: class CAkFilePackageLowLevelIO.
// Desc: Extends default Low-level IO implementation with packaged file support.
//		 Base class must implement one of the low-level I/O hooks 
//		 (AK::StreamMgr::IAkLowLevelIOHook)
//		 It must also define the following methods:
//			- void Term()
//-----------------------------------------------------------------------------
template <class T_LLIOHOOK, class T_PACKAGE = CAkDiskPackage>
class CAkFilePackageLowLevelIO : public T_LLIOHOOK
{
public:

	struct AkPackageFileDesc : public T_LLIOHOOK::AkFileDescType
	{
		AkPackageFileDesc()	{}
		AkPackageFileDesc(const AkPackageFileDesc& in_copy) 
			: T_LLIOHOOK::AkFileDescType(in_copy)		
			, pPackage(in_copy.pPackage)
			, uBlockSize(in_copy.uBlockSize)
		{}

		CAkFilePackage* pPackage = nullptr;			///< If this file is in a File Package, this will point to our package.
		AkUInt32 uBlockSize = 1;					///< Block size for files within a package.
	};

	typedef AkPackageFileDesc AkFileDescType;

    CAkFilePackageLowLevelIO();
    virtual ~CAkFilePackageLowLevelIO();

	// File package loading:
    // Opens a package file, parses its header, fills LUT.
    // Overrides of Open() will search files in loaded LUTs first, then use default Low-Level I/O 
	// services if they cannot be found.
	// Any number of packages can be loaded at a time. Each LUT is searched until a match is found.
	// Returns AK_Success if successful, AK_InvalidLanguage if the current language 
	// does not exist in the LUT (not necessarily an error), AK_Fail for any other reason.
	// Also returns a package ID which can be used to unload it (see UnloadFilePackage()).
	// WARNING: This method is not thread safe. Ensure there are no I/O occurring on this device
	// when loading a file package.
    virtual AKRESULT LoadFilePackage(
        const AkOSChar* in_pszFilePackageName,	// File package name. Location is resolved using base class' Open().
		AkUInt32 &		out_uPackageID			// Returned package ID.
        );
	
	// Unload a file package.
	// Returns AK_Success if in_uPackageID exists, AK_Fail otherwise.
	// WARNING: This method is not thread safe. Ensure there are no I/O occurring on this device
	// when unloading a file package.
	virtual AKRESULT UnloadFilePackage( 
		AkUInt32	in_uPackageID			// Returned package ID.
		);

	// Unload all file packages.
	// Returns AK_Success;
	// WARNING: This method is not thread safe. Ensure there are no I/O occurring on this device
	// when unloading a file package.
    virtual AKRESULT UnloadAllFilePackages();

	// Change the policy when a file is not found in a package.
	// By default, when a file is not found in a package, it will fall back on T_LLIOHOOK
	// This behavior can be turned off by passing false to this function.
	// By turning off this behavior, files will be searched in packages exclusively, with no fallback.
	void SetPackageFallbackBehavior(bool bFallback) { m_bFallback = bFallback; }

	//
	// Overriden base class policies.
	// ---------------------------------------------------------------

    // Clean up.
    void Term();

	// Returns true if file described by in_fileDesc is in a package.
	inline bool IsInPackage(
		AkFileDesc* in_pFileDesc		// File descriptor.
	) const
	{
		return CastFileDesc(in_pFileDesc)->pPackage != nullptr;
	}

	//
	// IAkLowLevelIOHook interface overriden methods.
	// ---------------------------------------------------------------

	// Override BatchOpen: Files contained in package must be opened differently
	virtual void BatchOpen(AkUInt32 in_uNumFiles, AkAsyncFileOpenData** in_ppItems) override;

	// Override Close: Do not close handle if file descriptor is part of the current packaged file.
	virtual AKRESULT Close(
		AkFileDesc* in_pFileDesc			// File descriptor.
	) override;

	// Override GetBlockSize: Get the block size of the LUT if a file package is loaded.
	virtual AkUInt32 GetBlockSize(
		AkFileDesc& in_fileDesc			// File descriptor.
	) override;

	virtual void BatchCancel(
		AkUInt32				in_uNumTransfers,		///< Number of transfers to cancel
		AK::StreamMgr::IAkLowLevelIOHook::BatchIoTransferItem* in_pTransferItems,		///< List of transfer items to cancel
		bool** io_ppbCancelAllTransfersForThisFile		///< Flag for each transfer indicating whether all transfers should be cancelled for this file (see notes in function description).
	) override;

protected:

	virtual AKRESULT OutputSearchedPaths(
		AKRESULT in_result,						///< Result of the open call
		const AkFileOpenData& in_FileOpen,		///< File name or file ID (only one should be valid!), open flags, open mode
		AkOSChar* out_searchedPath,				///< String containing all searched paths
		AkInt32 in_pathSize		   				///< The maximum size of the string
		) override;

	// Allow creation of specialized AkFileDesc class for derivatives of this class.
	// Default implementation allocates a AkFileDesc with AkNew, which should be freed in Close().
	virtual AkFileDesc* CreateDescriptor(const AkFileDesc* in_pCopy = nullptr) override;

	// Language change handling.
    // ------------------------------------------

	// Handler for global language change.
	static AK_FUNC( void, LanguageChangeHandler )( 
		const AkOSChar * const in_pLanguageName,// New language name.
		void * in_pCookie						// Cookie that was passed to AddLanguageChangeObserver().
		)
	{
		((CAkFilePackageLowLevelIO<T_LLIOHOOK, T_PACKAGE>*)in_pCookie)->OnLanguageChange( in_pLanguageName );
	}

	// Updates language of all loaded packages. Packages keep a language ID to help them find 
	// language-specific assets quickly.
	void OnLanguageChange( 
		const AkOSChar * const in_pLanguageName	// New language name.
		);


	// File package handling methods.
    // ------------------------------------------

	virtual AKRESULT FindInPackages(
		const AkFileOpenData& in_FileOpen,
		AkPackageFileDesc*& out_pFileDesc        // Returned file descriptor.
	);

		// Loads a file package, with a given file package reader.
	AKRESULT _LoadFilePackage(
        const AkOSChar*			in_pszFilePackageName,	// File package name. Location is resolved using base class' Open().
		AkFilePackageReader &	in_reader,				// File package reader.
		AkPriority				in_readerPriority,		// File package reader priority heuristic.
		T_PACKAGE *&			out_pPackage			// Returned package
        );

    // Searches the LUT to find the file data associated with the FileID.
    // Returns AK_Success if the file is found.
	template <class T_FILEID>
    AKRESULT FindPackagedFile( 
		T_PACKAGE *			in_pPackage,	// Package to search into.
		T_FILEID			in_fileID,		// File ID.
		AkFileSystemFlags * in_pFlags,		// Special flags. Can pass NULL.
		AkPackageFileDesc*&		out_pFileDesc	// Returned file descriptor.
		);	

	inline AkPackageFileDesc* CastFileDesc(AkFileDesc* in_pFileDesc) const { return static_cast<AkPackageFileDesc*>(in_pFileDesc); }

protected:

	virtual AKRESULT Open(const AkFileOpenData& in_FileOpen, AkFileDesc*& out_pFileDesc) override;

	// List of loaded packages.
	ListFilePackages	m_packages;
	CAkLock				m_lock;
	bool				m_bRegisteredToLangChg;	// True after registering to language change notifications.
	bool				m_bFallback;
};

#include "AkFilePackageLowLevelIO.inl"

#endif //_AK_FILE_PACKAGE_LOW_LEVEL_IO_H_
