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

#include "stdafx.h"

#include "Perforce.h"
#include "Resource.h"

#include <AK/AkWwiseSDKVersion.h>

#include <memory>
#include <vector>

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

using namespace AK;
using namespace Wwise;

namespace
{
	using WideToUtf8 = CW2AEX<4 * MAX_PATH>; // Most of the UCS-2 encoding fits into 4 bytes
    static const CString k_csIndentLevel = L"   ";
	static const CString k_csFolderBackslash = L"\\";
	static const CString k_csAllFiles = L"...";
}

bool PerforceSourceControl::ReInitClient(Error & out_error)
{
	TermClient(out_error);
	return InitClient(&out_error);
}

bool PerforceSourceControl::InitClient(Error* out_error)
{
	Error error;
	if (!m_pClient)
	{
		m_pClient = new ClientApi();
		m_pClientTag = new ClientApi();

		InitClientFromRegistry(*m_pClient);
		InitClientFromRegistry(*m_pClientTag);
		ResetOverridePassword();

		m_pClient->SetProg("Wwise Perforce Plug-in");
		m_pClientTag->SetProg("Wwise Perforce Plug-in");

		m_pClientTag->SetProtocol("tag","");

		m_pClient->Init(&error);
		m_pClientTag->Init(&error);

		if (out_error)
			*out_error = error;
	}
	return !error.Test();
}

void PerforceSourceControl::PrepareClient(ClientApi* in_pClient)
{
	in_pClient->SetVersion(AK_WWISESDK_VERSIONNAME);
	in_pClient->SetVar(P4Tag::v_enableStreams);

	int apiLevel = atoi(P4Tag::l_client);

	// Enable graph if perforce version allows it
	if (apiLevel > 81)
		in_pClient->SetVar(P4Tag::v_enableGraph);
}

ISourceControl::OperationResult PerforceSourceControl::RunCommand(
	const char* in_acOperation,
	std::optional<StringListView> in_pFilenameList,
	std::optional<StringListView> in_pArgumentList,
	DWORD in_dwTimeoutMs, // = INFINITE
	bool in_bShowErrorMsgs) // = true
{
	ASSERT(in_acOperation != nullptr);
	Error error;
	
	m_operationResult = OperationResult_Succeed;
	m_bDismissNextErrorMessageBox = !in_bShowErrorMsgs;
	m_bCancelOperation = false;

	// Init
	InitClient(&error);

	ClientApi* pClient = m_pClient;

	// Run certain operations with the tag mode
	if ( !m_currentOperation.empty() && 
		(m_currentOperation.top() == OperationID_GetSubmitFiles ||
		 m_currentOperation.top() == OperationID_Opened ||
		 m_currentOperation.top() == OperationID_Workspaces) )
	{
		pClient = m_pClientTag;
	}

	if ( error.Test() )
	{
		// Failed to Init
		m_operationResult = OperationResult_Failed;
		Message( &error );

		TermClient( error );
	}
	else
	{
		// Argument and filename list allocation
		size_t uiArgListSize = in_pArgumentList ? in_pArgumentList->size() : 0;
		size_t uiFileListSize = in_pFilenameList ? in_pFilenameList->size() : 0;
		size_t uiArgc = uiArgListSize + uiFileListSize;

		// Most operations should have non-empty filelist, otherwise they make no sense
		ASSERT(
			strcmp(in_acOperation, "workspaces") == 0 ||
			strcmp(in_acOperation, "submit") == 0 ||
			strcmp(in_acOperation, "opened") == 0 ||
			uiFileListSize > 0
		);

		static std::vector<char*> asArgv;
		static std::vector<std::unique_ptr<WideToUtf8>> strFiles;
		asArgv.clear();
		strFiles.clear();
		asArgv.reserve(uiArgc);
		strFiles.reserve(uiArgc);

		// Argument list
		if ( in_pArgumentList )
		{
			for (const auto& arg : *in_pArgumentList)
			{
				strFiles.push_back(std::make_unique<WideToUtf8>(arg, CP_UTF8));
				asArgv.push_back(static_cast<char*>(*strFiles.back()));
			}
		}

		// File list
		if ( in_pFilenameList )
		{
			for (const auto& filename : *in_pFilenameList)
			{
				strFiles.push_back(std::make_unique<WideToUtf8>(filename, CP_UTF8));
				asArgv.push_back(static_cast<char*>(*strFiles.back()));
			}
		}

		// Run the operation
		pClient->SetBreak( this );

		m_dwOperationTimeout = in_dwTimeoutMs;
		m_dwOperationBeginTime = GetTickCount();

		// Always assume we are logged-in
		m_bNeedLogin = false;

		do
		{
			// The operation failed because not logged-in
			while( m_bNeedLogin && !m_bUserCancelledLogin )
			{
				// Login
				Operation operation( this, OperationID_Login );

				PrepareClient(pClient);
				pClient->SetArgv( 0, NULL );
				pClient->Run( "login", this );
			}

			m_bRetryUnicodeServer = false;

			// Run the operation
			PrepareClient(pClient);
			pClient->SetArgv(uiArgc, asArgv.data());
			pClient->Run(in_acOperation, this);
		}
		while( (m_bNeedLogin && !m_bUserCancelledLogin) || m_bRetryUnicodeServer );

		if( m_pOperationDialog )
		{
			AFX_MANAGE_STATE( AfxGetStaticModuleState() );
			delete m_pOperationDialog;
			m_pOperationDialog = NULL;
		}

		// Check if the connection was dropped
		if( pClient->Dropped() )
		{
			// The connection was dropped, we will re-initialize the client
			TermClient( error );
		}

		if ( error.Test() )
		{
			Message( &error );
		}
	}

	return m_operationResult;
}

void PerforceSourceControl::TermClient( Error& io_error )
{
	if( m_pClient )
	{
		m_pClient->Final( &io_error );

		delete m_pClient;
		m_pClient = NULL;
	}

	if( m_pClientTag )
	{
		m_pClientTag->Final( &io_error );

		delete m_pClientTag;
		m_pClientTag = NULL;
	}
}

void PerforceSourceControl::PrintText( char in_cLevel, const char* in_pInfoBuffer )
{
	if ( m_bPrintTextToDialog )
	{
		CString csInfoBuffer( CA2W( in_pInfoBuffer, CP_UTF8 ) );
		CString csInfoLine;
		CString csLevel;
		int iFoundAt = 0;

		// Create the indent level string
		for ( int i=0 ; i<in_cLevel ; ++i )
		{
			csLevel += k_csIndentLevel;
		}

		// Replace tabulation
		csInfoBuffer.Replace( L"\t", L"   " );
		csInfoBuffer.Remove( L'\r' );

		// Find strings to print lines one by one
		while ( true )
		{
			iFoundAt = csInfoBuffer.Find( L"\n" );

			if ( iFoundAt == -1 )
			{
				// Final message line
				if ( csInfoBuffer.GetLength() )
				{
					m_pUtilities->GetProgress()->AddLogMessage( AK::Wwise::ISourceControlProgress::Severity_Info, csLevel + csInfoBuffer );
				}

				break;
			}
			else
			{
				// Get the message line
				csInfoLine = csInfoBuffer.Left( iFoundAt );
				m_pUtilities->GetProgress()->AddLogMessage( AK::Wwise::ISourceControlProgress::Severity_Info, csLevel + csInfoLine );
				csInfoBuffer = csInfoBuffer.Right( csInfoBuffer.GetLength() - iFoundAt - 1 );
			}

		}
	}
}

void PerforceSourceControl::DisplayErrorMessageBox( Error* in_pError )
{
	if (SourceControlHelpers::IsMainThread())
	{
		AFX_MANAGE_STATE( AfxGetStaticModuleState() );

		// Display a message box only if we can't display it to the progress dialog
		if ( !m_bPrintTextToDialog && !m_bDismissNextErrorMessageBox )
		{
			StrBuf errorMsg;
			CString csCaption;
			CString csMessage;

			in_pError->Fmt( &errorMsg, EF_PLAIN );

			csCaption = _T("Perforce plug-in");
			csMessage.Format( _T("Perforce command failed:\n\"%s\""), CString( CA2W( errorMsg.Text(), CP_UTF8 ) ) );

			if ( m_pUtilities && m_pUtilities->MessageBox( NULL, csMessage, csCaption, MB_OKCANCEL | MB_ICONERROR ) == IDCANCEL )
			{
				m_bDismissNextErrorMessageBox = true;
				m_bCancelOperation = true;
			}
		}
	}
}

void PerforceSourceControl::GetValidPathList( const StringListView& in_pFilenameList, StringVector& out_pFilenameList, bool in_bSkipDirectories, bool in_skipDirectoryExpansion )
{
	out_pFilenameList.reserve(in_pFilenameList.size());

	// Add a '*' to the end of directory paths
	for (const auto& filename : in_pFilenameList)
	{
		CString csFilename = EscapeWildcards(filename);

		bool bIsDirectory = false;

		if (!in_skipDirectoryExpansion)
		{
			DWORD dwFileAttributes = ::GetFileAttributes(filename); 
			bIsDirectory = dwFileAttributes != INVALID_FILE_ATTRIBUTES && (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);

			if( dwFileAttributes == INVALID_FILE_ATTRIBUTES )
			{
				// The file might not exist yet, use the extension presence to determine
				wchar_t szSrcDrive[MAX_PATH] = {0}, szSrcDir[MAX_PATH] = {0}, szSrcName[MAX_PATH] = {0}, szSrcExt[MAX_PATH] = {0};
				_wsplitpath_s(filename, szSrcDrive, szSrcDir, szSrcName, szSrcExt );
				bIsDirectory = (szSrcExt[0] == NULL);
			}

			if( bIsDirectory )
			{
				if ( csFilename.Right( 1 ) != k_csFolderBackslash )
					csFilename += k_csFolderBackslash;

				csFilename += k_csAllFiles;
			}
		}

		if( !bIsDirectory || !in_bSkipDirectories )
			out_pFilenameList.push_back(csFilename);
	}
}

void PerforceSourceControl::AddEnumerationString( CString& out_rEnumeration, const CString& in_rStringToAdd )
{
	if ( out_rEnumeration.IsEmpty() )
	{
		out_rEnumeration = in_rStringToAdd;
	}
	else
	{
		out_rEnumeration += CString( L", " ) + in_rStringToAdd;
	}
}

bool PerforceSourceControl::IsOperationValid( OperationID in_operationID, const FileToPerforceStatusMap& in_rFileStatusMap )
{
	bool bResult = true;
	bool bMultiSelection = ( in_rFileStatusMap.GetSize() == 1 ) ? false : true;
	POSITION fileStatusPos = in_rFileStatusMap.GetStartPosition();

	while ( fileStatusPos && bResult )
	{
		FileStatus fileStatus;
		CString csFilename;

		in_rFileStatusMap.GetNextAssoc( fileStatusPos, csFilename, fileStatus );
		
		bResult = PerforceSourceControl::IsOperationValid( in_operationID, fileStatus, csFilename, bMultiSelection );

	}

	return bResult;
}

bool PerforceSourceControl::IsOperationValid( 
	OperationID in_operationID, 
	const FileStatus& in_rFileStatus, 
	LPCWSTR in_pszFilename,
	bool in_bMultiSelection )
{
	bool bResult = false;

	BOOL bIsDirectory = PathIsDirectory( in_pszFilename );

	switch ( in_operationID )
	{
		case OperationID_GetLatestVersion:
			bResult =  TestStatusCondition( in_rFileStatus, 
											( FileStatus_OnServer ),		// Necessary
											( FileStatus_Deleted |			// Unacceptable
											  FileStatus_Branch )
											);
			break;

		case OperationID_Submit:
			bResult =  TestStatusCondition( in_rFileStatus, 
											( FileStatus_Added |		// Necessary
											  FileStatus_Deleted | 
											  FileStatus_Branch |
											  FileStatus_LocalEdit | 
											  FileStatus_LocalLock |
											  FileStatus_MoveAdd |
											  FileStatus_MoveDelete ), 
											( FileStatus_Unresolved |	// Unacceptable
											  FileStatus_OtherLock |
											  FileStatus_Outdated )
											);
			break;

		case OperationID_CheckOut:
			bResult =  TestStatusCondition( in_rFileStatus, 
											bIsDirectory ? FileStatus_LocalOnly : ( FileStatus_OnServer ),	// Necessary
											( FileStatus_LocalEdit |	// Unacceptable
											  FileStatus_LocalLock |
											  FileStatus_Deleted |
											  FileStatus_Branch )
											);
			break;

		case OperationID_Lock:
			bResult =  TestStatusCondition( in_rFileStatus, 
											( FileStatus_LocalEdit ),	// Necessary
											( FileStatus_LocalLock |	// Unacceptable
											  FileStatus_OtherLock )
											);
			break;

		case OperationID_Unlock:
			bResult =  TestStatusCondition( in_rFileStatus, 
											( FileStatus_LocalLock ),	// Necessary
											( (FileStatus)0 )	// Unacceptable
											);
			break;
			
		case OperationID_Add:
			bResult =  TestStatusCondition( in_rFileStatus, 
											( FileStatus_LocalOnly ),		// Necessary
											( (FileStatus)0 )	// Unacceptable
											);
			break;
			
		case OperationID_Delete:
			bResult =  TestStatusCondition( in_rFileStatus, 
											( FileStatus_OnServer |		// Necessary
											  FileStatus_LocalOnly ),
											( FileStatus_Deleted |		// Unacceptable
											  FileStatus_Branch |
											  FileStatus_LocalEdit |
											  FileStatus_LocalLock |
											  FileStatus_Added |
											  FileStatus_MoveAdd |
											  FileStatus_MoveDelete )
											);
			break;

		case OperationID_Rename:
			bResult =  !in_bMultiSelection && TestStatusCondition( in_rFileStatus, 
																( FileStatus_OnServer |		// Necessary
																  FileStatus_LocalOnly |
																  FileStatus_Added |
																  FileStatus_MoveAdd ),
																( FileStatus_Deleted |		// Unacceptable
																  FileStatus_Branch |
																  FileStatus_LocalLock )
															   );
			break;

		case OperationID_Move:
			bResult =  TestStatusCondition( in_rFileStatus, 
											( FileStatus_OnServer |		// Necessary
											  FileStatus_LocalOnly |
											  FileStatus_Added |
											  FileStatus_MoveAdd ),
											( FileStatus_Deleted |		// Unacceptable
											  FileStatus_Branch |
											  FileStatus_LocalLock )
										   );
			break;

		case OperationID_Revert:
			bResult = bIsDirectory ||
						TestStatusCondition( in_rFileStatus, 
											( FileStatus_Added |		// Necessary
											  FileStatus_Deleted |
											  FileStatus_Branch |
											  FileStatus_LocalEdit |
											  FileStatus_LocalLock |
											  FileStatus_MoveAdd ),
											( (FileStatus)0 )			// Unacceptable
											);
			break;
			
		case OperationID_Resolve:
			bResult =  !in_bMultiSelection && TestStatusCondition( in_rFileStatus, 
																( FileStatus_Unresolved ),	// Necessary
																( (FileStatus)0 )			// Unacceptable
															   );
			break;
			
		case OperationID_Diff:
			bResult =  !in_bMultiSelection && TestStatusCondition( in_rFileStatus, 
																( FileStatus_OnServer ),	// Necessary
																( (FileStatus)0 )			// Unacceptable
															   );
			break;
			
		case OperationID_History:
			bResult =  !in_bMultiSelection && TestStatusCondition( in_rFileStatus, 
																( FileStatus_OnServer ),	// Necessary
																( (FileStatus)0 )			// Unacceptable
															   );

			break;
	}

	return bResult;
}

bool PerforceSourceControl::IsOperationNoUI( OperationID in_operationID )
{
	return in_operationID == OperationID_CheckOutNoUI ||
		in_operationID == OperationID_AddNoUI ||
		in_operationID == OperationID_DeleteNoUI ||
		in_operationID == OperationID_RenameNoUI ||
		in_operationID == OperationID_MoveNoUI ||
		in_operationID == OperationID_RevertNoUI;
}

bool PerforceSourceControl::TestStatusCondition( FileStatus in_fileStatus, unsigned int in_uiNecessaryStatusMask, unsigned int in_uiUnacceptableStatusMask)
{
	bool bResult = false;

	// The file status must satisfy the necessary conditions and, at the same time, not being in an unacceptable status
	// Note that unacceptable status are status that can exist at the same time of a necessary condition
	if ( ( in_fileStatus & in_uiNecessaryStatusMask ) && !( in_fileStatus & in_uiUnacceptableStatusMask ) )
	{
		bResult = true;
	}

	return bResult;
}

bool PerforceSourceControl::MoveToTemporaryFolder( const CString& in_rOriginalFilename, const CString& in_rNewName, CString& out_rMovedFilename )
{
	// Move file to a temporary folder with significant filename
	TCHAR acDrive[_MAX_DRIVE];
	TCHAR acDir[_MAX_DIR];
	TCHAR acFilename[_MAX_FNAME];
	TCHAR acExt[_MAX_EXT];

	_tsplitpath( in_rOriginalFilename, acDrive, acDir, acFilename, acExt );

	CString csFolder = CString( acDir );
	CString csDriveFolder = CString( acDrive ) + csFolder;
	CString csTempFolder;

	// Get the temporary folder
	bool bSucceed = true;
	TCHAR* acTempFolder = _tgetenv( _T( "TEMP" ) );
	if (! acTempFolder )
		acTempFolder = _tgetenv( _T( "TMP" ) );

	if ( acTempFolder )
	{
		csTempFolder = CString( acTempFolder );
	}
	else
	{
		bSucceed = false;
	}

	if ( bSucceed )
	{
		if ( csTempFolder.Right(1) == CString( L"\\" ) )
		{
			csTempFolder = csTempFolder.Left( csTempFolder.GetLength() - 1 );
		}

		csTempFolder += CString( L"\\Wwise Perforce plug-in\\" );

		if ( !::PathFileExists( csTempFolder ) )
		{
			if ( !::CreateDirectory( csTempFolder, NULL ) )
			{
				bSucceed = false;
			}
		}
	}

	if ( bSucceed )
	{
		// Construct new file name
		CString csMovedFilename = csTempFolder + in_rNewName + CString( acExt );

		// Make sure there's no file in the way
		::SetFileAttributes( csMovedFilename, FILE_ATTRIBUTE_NORMAL );
		::DeleteFile( csMovedFilename );

		// Move Base file
		if ( !::MoveFile( in_rOriginalFilename, csMovedFilename ) )
		{
			bSucceed = false;
		}
		else
		{
			out_rMovedFilename = csMovedFilename;
		}
	}

	return bSucceed;
}

CString PerforceSourceControl::GetFilename( const CString& in_rPath )
{
	char acDrive[_MAX_DRIVE];
	char acDir[_MAX_DIR];
	char acFilename[_MAX_FNAME];
	char acExt[_MAX_EXT];
	CW2A strOriginalFilename( in_rPath );
	
	::_splitpath( strOriginalFilename, acDrive, acDir, acFilename, acExt );

	return CString( acFilename );
}

CString PerforceSourceControl::FixFileMapKey( const CString& in_rPath )
{
	CString csKey( in_rPath );

	csKey.MakeLower();
	csKey.Replace( _T('/'), _T('\\') );

	return csKey;
}

CString PerforceSourceControl::EscapeWildcards( const CString& in_rPath )
{
	CString csEscapedPath = in_rPath;
			
	csEscapedPath.Replace( L"%", L"%25" );
	csEscapedPath.Replace( L"@", L"%40" );
	csEscapedPath.Replace( L"#", L"%23" );
	csEscapedPath.Replace( L"*", L"%2a" );

	return csEscapedPath;
}

