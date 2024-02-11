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

#include "AkFilePackageLowLevelIO.h"
#include "AkFileHelpers.h"
#include <AK/Tools/Common/AkPlatformFuncs.h>
#include <AK/SoundEngine/Common/AkCommonDefs.h>

template <class T_LLIOHOOK, class T_PACKAGE>
CAkFilePackageLowLevelIO<T_LLIOHOOK,T_PACKAGE>::CAkFilePackageLowLevelIO()
: m_bRegisteredToLangChg( false )
, m_bFallback( true )
{
}

template <class T_LLIOHOOK, class T_PACKAGE>
CAkFilePackageLowLevelIO<T_LLIOHOOK,T_PACKAGE>::~CAkFilePackageLowLevelIO()
{
}

// Initialize/terminate.
template <class T_LLIOHOOK, class T_PACKAGE>
void CAkFilePackageLowLevelIO<T_LLIOHOOK,T_PACKAGE>::Term()
{
    UnloadAllFilePackages();
	m_packages.Term();
	if ( m_bRegisteredToLangChg )
		AK::StreamMgr::RemoveLanguageChangeObserver( this );
	T_LLIOHOOK::Term();
}

// Override Open: Search file in each LUT first. If it cannot be found, use base class services.
// If the file is found in the LUTs, open is always synchronous.
template <class T_LLIOHOOK, class T_PACKAGE>
AKRESULT CAkFilePackageLowLevelIO<T_LLIOHOOK,T_PACKAGE>::Open(const AkFileOpenData& in_FileOpen, AkFileDesc*& out_pFileDesc)
{
	{
		AkAutoLock<CAkLock> lock(m_lock);
		AkPackageFileDesc* pDesc = nullptr;
		AKRESULT eResult = FindInPackages(in_FileOpen, pDesc);
		if (eResult == AK_Success)
		{
			out_pFileDesc = pDesc;
			return AK_Success;
		}
		if (!m_bFallback && in_FileOpen.eOpenMode == AK_OpenModeRead)
		{
			return eResult;
		}
	}

    // It is not a soundbank, or it is not in the file package LUT. Use default implementation.
    return T_LLIOHOOK::Open(in_FileOpen, out_pFileDesc);
}

template<class T_LLIOHOOK, class T_PACKAGE>
inline void CAkFilePackageLowLevelIO<T_LLIOHOOK, T_PACKAGE>::BatchOpen(AkUInt32 in_uNumFiles, AkAsyncFileOpenData** in_ppItems)
{
	AKRESULT eResult;
	AkUInt32 uNumItemsNotInPackage = 0;
	AkAsyncFileOpenData** arItemsNotInPackage = (AkAsyncFileOpenData **)AkAlloca(sizeof(AkAsyncFileOpenData*) * in_uNumFiles);

	// First, look for all files in the batch in the packages, and keep track of those not found
	{
		AkAutoLock<CAkLock> lock(m_lock);
		for (int i = 0; i < (int)in_uNumFiles; i++)
		{
			AkPackageFileDesc* pDesc = nullptr;
			AkAsyncFileOpenData* pItem = in_ppItems[i];
			eResult = FindInPackages(*pItem, pDesc);
			if (eResult == AK_FileNotFound && (m_bFallback || pItem->eOpenMode != AK_OpenModeRead))
			{
				arItemsNotInPackage[uNumItemsNotInPackage] = pItem;
				uNumItemsNotInPackage++;
			}
			else
			{
				pItem->pFileDesc = pDesc;
				pItem->pCallback(pItem, eResult);
			}
		}
	}

	if (uNumItemsNotInPackage > 0)
	{
		// Fall back on default implementation for files that were not found in the packages.
		T_LLIOHOOK::BatchOpen(uNumItemsNotInPackage, arItemsNotInPackage);
	}
}

template<class T_LLIOHOOK, class T_PACKAGE>
inline AKRESULT CAkFilePackageLowLevelIO<T_LLIOHOOK, T_PACKAGE>::OutputSearchedPaths(
	AKRESULT in_result,
	const AkFileOpenData& in_FileOpen,
	AkOSChar* out_searchedPath,
	AkInt32 in_pathSize
)
{
	if (!out_searchedPath)
		return AK_InvalidParameter;

	AkOSChar buf[256];
	for (ListFilePackages::Iterator it = m_packages.Begin(); it != m_packages.End(); ++it)
	{
		AK_OSPRINTF(buf, 256, AKTEXT("Package #%u; "), (*it)->ID());
		AKPLATFORM::SafeStrCat(out_searchedPath, buf, in_pathSize);
	}
	if (!m_bFallback && in_FileOpen.eOpenMode == AK_OpenModeRead)
		return AK_Success;

	return T_LLIOHOOK::OutputSearchedPaths(
		in_result,
		in_FileOpen,
		out_searchedPath,
		in_pathSize);
}

// Override Close: Do not close handle if file descriptor is part of the current packaged file.
template <class T_LLIOHOOK, class T_PACKAGE>
AKRESULT CAkFilePackageLowLevelIO<T_LLIOHOOK, T_PACKAGE>::Close(
	AkFileDesc* in_pFileDesc      // File descriptor.
)
{
	if (!in_pFileDesc)
		return AK_Success;

	{
		AkAutoLock<CAkLock> lock(m_lock);
		// Do not close handle if it is that of the file package (closed only in UnloadFilePackage()).
		if (IsInPackage(in_pFileDesc))
		{
			CastFileDesc(in_pFileDesc)->pPackage->Release();
			AkDelete(AkMemID_Streaming, in_pFileDesc);
			return AK_Success;
		}
	}
	return T_LLIOHOOK::Close(in_pFileDesc);
}

// Override GetBlockSize: Get the block size of the LUT if a file package is loaded.
template <class T_LLIOHOOK, class T_PACKAGE>
AkUInt32 CAkFilePackageLowLevelIO<T_LLIOHOOK,T_PACKAGE>::GetBlockSize(
    AkFileDesc &  in_fileDesc     // File descriptor.
    )
{
	{
		AkAutoLock<CAkLock> lock(m_lock);
		if (IsInPackage(&in_fileDesc))
			return CastFileDesc(&in_fileDesc)->uBlockSize;
	}

	return T_LLIOHOOK::GetBlockSize( in_fileDesc );
}

// Override Cancel: The CAkDefaultIOHookDeferred implementation only permits cancellations of all transfers 
// for a given file handle. Since the packaged files share the same handle, we cannot do this.
template <class T_LLIOHOOK, class T_PACKAGE>
void CAkFilePackageLowLevelIO<T_LLIOHOOK, T_PACKAGE>::BatchCancel(
	AkUInt32				in_uNumTransfers,		///< Number of transfers to cancel
	AK::StreamMgr::IAkLowLevelIOHook::BatchIoTransferItem* in_pTransferItems,		///< List of transfer items to cancel
	bool** io_ppbCancelAllTransfersForThisFile		///< Flag for each transfer indicating whether all transfers should be cancelled for this file (see notes in function description).
)
{
	for (AkUInt32 i = 0; i < in_uNumTransfers; ++i)
	{
		//Can't cancel transfers in a package (may cancel more than expected)
		if (!IsInPackage(in_pTransferItems->pFileDesc))
			T_LLIOHOOK::BatchCancel(1, &in_pTransferItems[i], &io_ppbCancelAllTransfersForThisFile[i]);
	}
}

template <class T_LLIOHOOK, class T_PACKAGE>
AkFileDesc* CAkFilePackageLowLevelIO<T_LLIOHOOK, T_PACKAGE>::CreateDescriptor(const AkFileDesc* in_pCopy)
{
	if (!in_pCopy)
		return AkNew(AkMemID_Streaming, AkFileDescType());

	return AkNew(AkMemID_Streaming, AkFileDescType(*(AkFileDescType*)in_pCopy));
}

// Updates language of all loaded packages. Packages keep a language ID to help them find 
// language-specific assets quickly.
template <class T_LLIOHOOK, class T_PACKAGE>
void CAkFilePackageLowLevelIO<T_LLIOHOOK,T_PACKAGE>::OnLanguageChange( 
	const AkOSChar * const in_pLanguageName	// New language name.
	)
{
	AkAutoLock<CAkLock> lock(m_lock);
	// Set language on all loaded packages.
	ListFilePackages::Iterator it = m_packages.Begin();
	while ( it != m_packages.End() )
	{
		(*it)->lut.SetCurLanguage( in_pLanguageName );
		++it;
	}
}

// Searches the LUT to find the file data associated with the FileID.
// Returns AK_Success if the file is found.
template <class T_LLIOHOOK, class T_PACKAGE> 
template <class T_FILEID>
AKRESULT CAkFilePackageLowLevelIO<T_LLIOHOOK,T_PACKAGE>::FindPackagedFile( 
	T_PACKAGE *			in_pPackage,	// Package to search into.
	T_FILEID			in_fileID,		// File ID.
	AkFileSystemFlags * in_pFlags,		// Special flags. Can pass NULL.
	AkPackageFileDesc*&	out_pFileDesc	// Returned file descriptor.
	)
{
	AkAutoLock<CAkLock> lock(m_lock);
	AKASSERT( in_pPackage && in_pFlags );
	const CAkFilePackageLUT::AkFileEntry<T_FILEID> * pEntry = in_pPackage->lut.LookupFile( in_fileID, in_pFlags );

	if ( pEntry )
	{
		out_pFileDesc = CastFileDesc(CreateDescriptor(in_pPackage->GetFileDesc()));
		if (!out_pFileDesc)
			return AK_InsufficientMemory;

		// Fill file descriptor.
		out_pFileDesc->iFileSize = pEntry->uFileSize;
		out_pFileDesc->uSector	= pEntry->uStartBlock;
		out_pFileDesc->uBlockSize = pEntry->uBlockSize;
		out_pFileDesc->pPackage = in_pPackage;
		in_pPackage->AddRef();
		
		return AK_Success;
	}
	return AK_FileNotFound;
}

// File package loading:
// Opens a package file, parses its header, fills LUT.
// Overrides of Open() will search files in loaded LUTs first, then use default Low-Level I/O 
// services if they cannot be found.
// Any number of packages can be loaded at a time. Each LUT is searched until a match is found.
// Returns AK_Success if successful, AK_InvalidLanguage if the current language 
// does not exist in the LUT (not necessarily an error), AK_Fail for any other reason.
// Also returns a package ID which can be used to unload it (see UnloadFilePackage()).
template <class T_LLIOHOOK, class T_PACKAGE>
AKRESULT CAkFilePackageLowLevelIO<T_LLIOHOOK,T_PACKAGE>::LoadFilePackage(
    const AkOSChar *    in_pszFilePackageName,	// File package name. 
	AkUInt32 &			out_uPackageID			// Returned package ID.
    )
{
	// Open package file.
	AkFilePackageReader filePackageReader;
	AKRESULT eRes = filePackageReader.Open( in_pszFilePackageName, true );	// Open from SFX-only directory.
	if ( eRes != AK_Success )
        return eRes;

	filePackageReader.SetName( in_pszFilePackageName );

	T_PACKAGE * pPackage;
	eRes = _LoadFilePackage( in_pszFilePackageName, filePackageReader, AK_DEFAULT_PRIORITY, pPackage );
	if ( eRes == AK_Success
		|| eRes == AK_InvalidLanguage )
	{
		AKASSERT( pPackage );
		// Add to packages list.
		AkAutoLock<CAkLock> lock(m_lock);
		m_packages.AddFirst( pPackage );
		
		out_uPackageID = pPackage->ID();
	}
	return eRes;
}


template <class T_LLIOHOOK, class T_PACKAGE>
AKRESULT CAkFilePackageLowLevelIO<T_LLIOHOOK, T_PACKAGE>::FindInPackages(
	const AkFileOpenData& in_FileOpen,	///< File name or file ID (only one should be valid!), open flags, open mode
	AkPackageFileDesc*& out_pFileDesc        // Returned file descriptor.
)
{
	if (in_FileOpen.eOpenMode != AK_OpenModeRead	//Can't write file in a package.
		|| in_FileOpen.pFlags == NULL)				//And we need the codec ID.
	{
		return AK_FileNotFound;
	}

	AkAutoLock<CAkLock> lock(m_lock);

	const AkOSChar* pFileName = in_FileOpen.pszFileName;
	AkOSChar szFileName[20];
	if (in_FileOpen.pszFileName == NULL)
	{
		AK_OSPRINTF(szFileName, 20, AKTEXT("%u.wem"), (unsigned int)in_FileOpen.fileID);
		pFileName = szFileName;
	}

	ListFilePackages::Iterator it = m_packages.Begin();
	AKRESULT eResult;
	while (it != m_packages.End())
	{
		if (in_FileOpen.pFlags->uCompanyID == AKCOMPANYID_AUDIOKINETIC)
		{
			AkFileID fileID = in_FileOpen.fileID;
			if (AK::IsBankCodecID(in_FileOpen.pFlags->uCodecID) && in_FileOpen.pszFileName)
				fileID = (*it)->lut.GetSoundBankID(in_FileOpen.pszFileName);

			eResult = FindPackagedFile((T_PACKAGE*)(*it), fileID, in_FileOpen.pFlags, out_pFileDesc);
		}
		else
		{
			AkUInt64 fileID = (*it)->lut.GetExternalID(pFileName);
			 eResult = FindPackagedFile((T_PACKAGE*)(*it), fileID, in_FileOpen.pFlags, out_pFileDesc);
		}

		if (eResult == AK_Success) // Found the ID in the lut.
		{
			AKASSERT(out_pFileDesc != nullptr);
			return AK_Success;
		}

		if (eResult == AK_InsufficientMemory)
			return eResult; // File was found, but ran out of memory.

		// Only remaining possibility is that the file was not found; move on to the next package
		AKASSERT(eResult == AK_FileNotFound);
		++it;
	}

	return AK_FileNotFound;
}

// Loads a file package, with a given file package reader.
template <class T_LLIOHOOK, class T_PACKAGE>
AKRESULT CAkFilePackageLowLevelIO<T_LLIOHOOK,T_PACKAGE>::_LoadFilePackage(
    const AkOSChar*			in_pszFilePackageName,	// File package name. 
	AkFilePackageReader &	in_reader,				// File package reader.
	AkPriority				in_readerPriority,		// File package reader priority heuristic.
	T_PACKAGE *&			out_pPackage			// Returned package
    )
{
	// Read header chunk definition.
	struct AkFilePackageHeader
	{
		AkUInt32 uFileFormatTag;
		AkUInt32 uHeaderSize;
	};

	AkUInt32 uReadBufferSize = AkMax( 2 * in_reader.GetBlockSize(), sizeof(AkFilePackageHeader) );
	AkUInt8 * pBufferForHeader = (AkUInt8*)AkAlloca( uReadBufferSize );
	AkUInt32 uSizeToRead;
	bool bAligned = ( sizeof(AkFilePackageHeader) % in_reader.GetBlockSize() ) > 0;
	if ( bAligned )
	{
		// Header size is not a multiple of the required block size. Allocate an aligned buffer on the stack.
		pBufferForHeader += ( in_reader.GetBlockSize() - (AkUIntPtr)pBufferForHeader % in_reader.GetBlockSize() );
		uSizeToRead = in_reader.GetBlockSize();
	}
	else
	{
		// Header size is a multiple of the required block size. 
		uSizeToRead = sizeof(AkFilePackageHeader);
	}

	AKRESULT eRes;
	AkUInt32 uSizeRead;
	{
		void* pReadBuffer = AkAlloc(AkMemID_Streaming, uReadBufferSize);
		eRes = in_reader.Read(pReadBuffer, uSizeToRead, uSizeRead, in_readerPriority );
		if ( eRes != AK_Success || uSizeRead < sizeof(AkFilePackageHeader) )
		{
			in_reader.Close();
			AkFree(AkMemID_Streaming, pReadBuffer);
			AK::Monitor::PostString("Could not read package, or package is invalid", AK::Monitor::ErrorLevel_Error);
			return AK_Fail;
		}
		AKPLATFORM::AkMemCpy(pBufferForHeader, pReadBuffer, uSizeRead);
		AkFree(AkMemID_Streaming, pReadBuffer);
	}

	const AkFilePackageHeader & uFileHeader = *(AkFilePackageHeader*)pBufferForHeader;

	if ( uFileHeader.uFileFormatTag != AKPK_FILE_FORMAT_TAG 
		|| 0 == uFileHeader.uHeaderSize )
	{
		AKASSERT( !"Invalid file package header" );
		in_reader.Close();
        return AK_Fail;
	}

	// Create file package.
	AkUInt32 uReservedHeaderSize;
	AkUInt8 * pFilePackageHeader;
	out_pPackage = T_PACKAGE::Create(
		in_reader,
		in_pszFilePackageName,
		uFileHeader.uHeaderSize + AKPK_HEADER_CHUNK_DEF_SIZE,	// NOTE: The header size written in the file package excludes the AKPK_HEADER_CHUNK_DEF_SIZE.
		uReservedHeaderSize,
		pFilePackageHeader );
	if ( !out_pPackage )
	{
		in_reader.Close();
		AK::Monitor::PostString("Could not create file package", AK::Monitor::ErrorLevel_Error);
        return AK_Fail;
	}

	AkUInt32 uHeaderSize = uFileHeader.uHeaderSize;
	AkUInt32 uHeaderReadOffset = AKPK_HEADER_CHUNK_DEF_SIZE;

	// If we had already read more than sizeof(AkFilePackageHeader), copy the rest now.
	if ( uSizeRead > sizeof(AkFilePackageHeader) )
	{
		pBufferForHeader += sizeof(AkFilePackageHeader);
		AkUInt32 uSizeToCopy = uSizeRead - sizeof(AkFilePackageHeader);
		AKPLATFORM::AkMemCpy( pFilePackageHeader+AKPK_HEADER_CHUNK_DEF_SIZE, pBufferForHeader, uSizeToCopy );
		// Adjust header size and read offset.
		if ( uSizeToCopy > uHeaderSize )
			uSizeToCopy = uHeaderSize;
		uHeaderSize -= uSizeToCopy;
		uHeaderReadOffset += uSizeToCopy;
		// Round it up to required block size. It should be equal to the size that was reserved (minus what was already read).
		uHeaderSize = ( ( uHeaderSize + in_reader.GetBlockSize() - 1 ) / in_reader.GetBlockSize() ) * in_reader.GetBlockSize();
		AKASSERT( uHeaderSize == uReservedHeaderSize - uSizeRead );
	}

	// Stream in remaining of the header.
	if ( uHeaderSize > 0 )
	{
		AKASSERT( uHeaderReadOffset % in_reader.GetBlockSize() == 0 );
		if ( in_reader.Read( pFilePackageHeader+uHeaderReadOffset, uHeaderSize, uSizeRead, in_readerPriority ) != AK_Success
			|| uSizeRead < uHeaderSize )
		{
			AKASSERT( !"Could not read file package" );
			out_pPackage->Release();
			return AK_Fail;
		}
	}

	// Parse LUT.
	eRes = out_pPackage->lut.Setup( pFilePackageHeader, uFileHeader.uHeaderSize + AKPK_HEADER_CHUNK_DEF_SIZE );
	if ( eRes != AK_Success )
	{
		out_pPackage->Release();
		return eRes;
	}

	// Register to language change notifications if it wasn't already done
	if ( !m_bRegisteredToLangChg )
	{
		if ( AK::StreamMgr::AddLanguageChangeObserver( LanguageChangeHandler, this ) != AK_Success )
		{
			out_pPackage->Release();
			return AK_Fail;
		}
		m_bRegisteredToLangChg = true;
	}

	// Use the current language path (if defined) to set the language ID, 
    // for language specific file mapping.
	return out_pPackage->lut.SetCurLanguage( AK::StreamMgr::GetCurrentLanguage() );
}

// Unload a file package.
template <class T_LLIOHOOK, class T_PACKAGE>
AKRESULT CAkFilePackageLowLevelIO<T_LLIOHOOK,T_PACKAGE>::UnloadFilePackage( 
	AkUInt32	in_uPackageID			// Package ID.
	)
{
	AkAutoLock<CAkLock> lock(m_lock);
	ListFilePackages::IteratorEx it = m_packages.BeginEx();
	while ( it != m_packages.End() )
	{
		if ( (*it)->ID() == in_uPackageID )
		{
			CAkFilePackage * pPackage = (*it);
			it = m_packages.Erase( it );

			// Destroy package.
			pPackage->Release();

			return AK_Success;
		}
		else
			++it;
	}

	AK::Monitor::PostString("Invalid package id", AK::Monitor::ErrorLevel_Error);
	return AK_IDNotFound;
}

// Unload all file packages.
template <class T_LLIOHOOK, class T_PACKAGE>
AKRESULT CAkFilePackageLowLevelIO<T_LLIOHOOK,T_PACKAGE>::UnloadAllFilePackages()
{
	AkAutoLock<CAkLock> lock(m_lock);
	ListFilePackages::IteratorEx it = m_packages.BeginEx();
	while ( it != m_packages.End() )
	{
		CAkFilePackage * pPackage = (*it);
		it = m_packages.Erase( it );

		// Destroy package.
		pPackage->Release();
	}

	return AK_Success;
}


