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
#include "DlgPerforceDescription.h"
#include "DlgRename.h"
#include "DlgPerforceConfig.h"
#include "DlgPerforceOnNewFiles.h"
#include "OperationResults.h"

#include <atlbase.h>
#include <string>
#include <vector>
#include <algorithm>
#include <set>

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

using namespace AK;
using namespace Wwise;

namespace
{
	constexpr UINT k_iconIDs[] = {
		IDI_PERFORCE_STATUS_NOT_CHECKED_OUT,
		IDI_PERFORCE_STATUS_CHECKED_OUT,
		IDI_PERFORCE_STATUS_ADDED,
		IDI_PERFORCE_STATUS_OTHER_CHECKOUT,
		IDI_PERFORCE_STATUS_BOTH_CHECKOUT,
		IDI_PERFORCE_STATUS_OUTDATED_NOT_CHECKED_OUT,
		IDI_PERFORCE_STATUS_OUTDATED_CHECKED_OUT,
		IDI_PERFORCE_STATUS_OUTDATED_OTHER_CHECKOUT,
		IDI_PERFORCE_STATUS_OUTDATED_BOTH_CHECKOUT
	};

	constexpr LPCTSTR k_tooltipText[] = {
		_T("Not checked out"),
		_T("Checked out"),
		_T("Added"),
		_T("Checked out by someone else"),	
		_T("Checked out by you and someone else"),
		_T("Outdated, Not checked out"),
		_T("Outdated, Checked out"),
		_T("Outdated, Checked out by someone else"),	
		_T("Outdated, Checked out by you and someone else")
	};

	constexpr LPCTSTR k_operationNames[PerforceSourceControl::OperationID_ContextMenuCount] = {
		0, // OperationID_None
		_T("Get Latest Version"), 
		_T("Submit Changes"),
		_T("Check Out"),
		_T("Lock"),
		_T("Unlock"),
		_T("Mark for Add"),
		_T("Mark for Delete"),
		_T("Rename"),
		_T("Move"),
		_T("Revert Changes"),
		_T("Resolve"),
		_T("Diff"),
		_T("File History")
	};

	constexpr LPCTSTR s_csPerforceInstallRegistry = _T("SOFTWARE\\PERFORCE\\");

	constexpr int AK_PERFORCE_MAX_SERVER_NAME_LENGTH = MAX_PATH;

	constexpr LPCTSTR k_csWorkspace = _T("workspace");
	constexpr LPCTSTR k_csServer = _T("server");
	constexpr LPCTSTR k_csPort = _T("port");
	constexpr LPCTSTR k_csHost = _T("host");
	constexpr LPCTSTR k_csUser = _T("user");
	constexpr LPCTSTR k_csPassword = _T("password");

	void FolderAddBackslash( CString& io_rPath )
	{
		if ( ! io_rPath.IsEmpty() && io_rPath[ io_rPath.GetLength() - 1 ] != L'\\' )
		{
			io_rPath += L'\\';
		}
	}

	void GetFilesUnder( const CString& in_csDirectory, StringVector& io_files )
	{
		CString csDirectory( in_csDirectory );
		FolderAddBackslash( csDirectory );

		CFileFind fileFind;
		BOOL bIsFileAvailable = fileFind.FindFile( csDirectory + L"*" ) ;

		while( bIsFileAvailable )
		{
			bIsFileAvailable = fileFind.FindNextFile();

			if ( fileFind.IsDots() || fileFind.IsHidden() )
				continue;

			if ( fileFind.IsDirectory() )
			{
				GetFilesUnder( fileFind.GetFilePath(), io_files );
			}
			else if( fileFind.GetFileName().Right(5) != L".akpk" )	// Skip akpk files
			{
				io_files.push_back(fileFind.GetFilePath());
			}
		}
	}

	void ExpandDirectories(const StringListView& in_files, StringVector& out_files)
	{
		out_files.reserve(in_files.size());
		for (const CString csFilename : in_files)
		{
			if( ::PathIsDirectory( csFilename ) )
			{
				// Need to get the files recursively
				GetFilesUnder( csFilename, out_files );
			}
			else
			{
				out_files.push_back(csFilename);
			}
		}
	}
}

PerforceSourceControl::PerforceSourceControl()
{
	// Load icons
	for ( unsigned int i=0 ; i<IconsIndexes_Count ; ++i )
	{
		m_icons[i] = ::LoadIcon( AfxGetStaticModuleState()->m_hCurrentResourceHandle, MAKEINTRESOURCE( k_iconIDs[i] ) );
	}
}

PerforceSourceControl::~PerforceSourceControl()
{
}

void PerforceSourceControl::GetPluginInfo( PluginInfo& out_rPluginInfo )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// Plug-in name and version
	CString csName = _T("Perforce");

	out_rPluginInfo.m_bstrName = csName.AllocSysString();
	out_rPluginInfo.m_uiVersion = k_uiVersion;

	// Functions availability
	out_rPluginInfo.m_bShowConfigDlgAvailable = true;
	out_rPluginInfo.m_dwUpdateCommandID = OperationID_GetLatestVersion;
	out_rPluginInfo.m_dwCommitCommandID = OperationID_Submit;
	out_rPluginInfo.m_dwRenameCommandID = OperationID_Rename;
	out_rPluginInfo.m_dwMoveCommandID = OperationID_Move;
	out_rPluginInfo.m_dwAddCommandID = OperationID_Add;
	out_rPluginInfo.m_dwDeleteCommandID = OperationID_Delete;
	out_rPluginInfo.m_dwRevertCommandID = OperationID_Revert;
	out_rPluginInfo.m_dwDiffCommandID = OperationID_Diff;
	out_rPluginInfo.m_dwCheckOutCommandID = OperationID_CheckOut;
	out_rPluginInfo.m_dwRenameNoUICommandID = OperationID_RenameNoUI;
	out_rPluginInfo.m_dwMoveNoUICommandID = OperationID_MoveNoUI;
	out_rPluginInfo.m_dwAddNoUICommandID = OperationID_AddNoUI;
	out_rPluginInfo.m_dwDeleteNoUICommandID = OperationID_DeleteNoUI;
	out_rPluginInfo.m_dwRevertNoUICommandID = OperationID_RevertNoUI;
	out_rPluginInfo.m_dwCheckOutNoUICommandID = OperationID_CheckOutNoUI;
	out_rPluginInfo.m_bStatusIconAvailable = true;
}

bool PerforceSourceControl::IsAvalaible()
{
	bool bAvalaible = true;

	CRegKey regKey;
	
	if ( regKey.Open( HKEY_LOCAL_MACHINE, s_csPerforceInstallRegistry, KEY_READ ) != ERROR_SUCCESS )
	{
		if ( regKey.Open( HKEY_CURRENT_USER, s_csPerforceInstallRegistry, KEY_READ ) != ERROR_SUCCESS )
			bAvalaible = false;
	}

	return bAvalaible;
}

// ISourceControl
void PerforceSourceControl::Init( ISourceControlUtilities* in_pUtilities, bool in_bAutoAccept )
{
	m_bAutoAcceptMode = in_bAutoAccept;

	m_pUtilities = in_pUtilities;

	{
		DWORD bEnabled = TRUE;
		m_pUtilities->GetUserPreferenceDword( k_csRegFolder + k_csRegKeyNewFilesAdd, bEnabled );
		m_bOnNewFilesAdd = bEnabled ? true : false;
	}

	{
		DWORD bEnabled = TRUE;
		m_pUtilities->GetUserPreferenceDword( k_csRegFolder + k_csRegKeyNewFilesCheckOut, bEnabled );
		m_bOnNewFilesCheckOut = bEnabled ? true : false;
	}

	// Log configuration used to initialize the client
	{
		ClientApi client;
		InitClientFromRegistry(client);

		std::string clientParameters;
		clientParameters.reserve(256);
		clientParameters += "Parameters {Host: '";
		clientParameters += client.GetHost().Length() > 0 ? client.GetHost().Text() : "(empty)";
		clientParameters += "', Client: '";
		clientParameters += client.GetClient().Length() > 0 ? client.GetClient().Text() : "(empty)";
		clientParameters += "', Port: '";
		clientParameters += client.GetPort().Length() > 0 ? client.GetPort().Text() : "(empty)";
		clientParameters += "', User: '";
		clientParameters += client.GetUser().Length() > 0 ? client.GetUser().Text() : "(empty)";
		clientParameters += "'}";
		CA2W clientParametersW(clientParameters.c_str());
		m_pUtilities->LogMessage(clientParametersW);
	}
}

void PerforceSourceControl::InitClientFromRegistry( ClientApi& io_client )
{
	const TCHAR chInvalid = (TCHAR)-1;
	TCHAR szValue[MAX_PATH] = {};
	ULONG size = MAX_PATH;

	m_pUtilities->GetUserPreferenceString( k_csRegFolder + k_csRegKeyClient, szValue, size );
	CW2A strClient( szValue );
	io_client.SetClient( strClient );

	szValue[0] = chInvalid;
	size = MAX_PATH;

	m_pUtilities->GetUserPreferenceString( k_csRegFolder + k_csRegKeyServer, szValue, size );
	if( szValue[0] != chInvalid )
	{
		CString csServer( szValue );
		szValue[0] = chInvalid;
		m_pUtilities->GetUserPreferenceString( k_csRegFolder + k_csRegKeyPort, szValue, size );
		if( szValue[0] != chInvalid )
		{
			// Format: "server:port"
			CW2A strServerAndPort( ServerAndPort( csServer, szValue ) );
			io_client.SetPort( strServerAndPort );
		}
	}
	else
	{
		// Backward compatibility: "Port" used to contain "server:port"...
		m_pUtilities->GetUserPreferenceString( k_csRegFolder + k_csRegKeyPort, szValue, size );
		if( szValue[0] != chInvalid )
		{
			// Format: "port"
			CW2A strServerAndPort( szValue );
			io_client.SetPort( strServerAndPort );
		}
	}

	szValue[0] = chInvalid;
	size = MAX_PATH;
	
	m_pUtilities->GetUserPreferenceString( k_csRegFolder + k_csRegKeyHost, szValue, size );
	if( szValue[0] != chInvalid )
	{
		CW2A strClient( szValue );
		io_client.SetHost( strClient );
	}

	szValue[0] = chInvalid;
	size = MAX_PATH;

	m_pUtilities->GetUserPreferenceString( k_csRegFolder + k_csRegKeyUser, szValue, size );
	if( szValue[0] != chInvalid )
	{
		CW2A strClient( szValue );
		io_client.SetUser( strClient );
	}

	if (m_bPasswordOverride)
	{
		io_client.SetIgnorePassword();
		io_client.SetPassword(CW2A(m_csPassword));
	}


	// Enable Unicode servers
	//const int utf8 = CharSetApi::UTF_8;
	//int charset = CharSetApi::Lookup(io_client.GetCharset().Text());
	//if (charset < 0) //charset env was not recognized
	//	charset = CharSetApi::NOCONV; 
	//io_client.SetTrans(utf8, charset, utf8, utf8);
}

void PerforceSourceControl::Term()
{
	// Terminate the client: close the connection
	Error error;
	TermClient( error );

	m_pUtilities->SetUserPreferenceDword( k_csRegFolder + k_csRegKeyNewFilesAdd, m_bOnNewFilesAdd ? TRUE : FALSE );
	m_pUtilities->SetUserPreferenceDword( k_csRegFolder + k_csRegKeyNewFilesCheckOut, m_bOnNewFilesCheckOut ? TRUE : FALSE );
}

void PerforceSourceControl::Destroy()
{
	delete this;
}

bool PerforceSourceControl::ShowConfigDlg()
{
	CDlgPerforceConfig configDlg( m_pUtilities );

	LRESULT uiResult = m_pUtilities->CreateModalCustomDialog( &configDlg );

	Error error;
	if (uiResult == IDOK)
	{
		ResetOverridePassword();
		if (!InitClient(&error))
		{
			m_bPrintTextToDialog = false;
			Message(&error);
			m_bPrintTextToDialog = true; // Reset parameter
			return false;
		}
	}

	return uiResult == IDOK;
}

ISourceControl::OperationResult PerforceSourceControl::GetOperationList( OperationMenuType in_menuType, const StringList& in_rFilenameList, OperationList& out_rOperationList )
{
	static bool s_bInitialized = false;
	static OperationListItem s_operationItems[OperationID_ContextMenuCount];

	if ( !s_bInitialized  )
	{
		for ( unsigned int i=OperationID_None+1 ; i<OperationID_ContextMenuCount ; ++i )
		{
			s_operationItems[i].m_dwOperationID = i;
			s_operationItems[i].m_bEnabled = false;
		}

		s_bInitialized = true;
	}

	// Operations Enabled/Disabled
	OperationResult operationResult = OperationResult_Succeed;

	if ( !in_rFilenameList.IsEmpty() )
	{
		operationResult = GetFileStatus( in_rFilenameList, nullptr, true );
	}

	for ( unsigned int i=OperationID_None+1 ; i<OperationID_ContextMenuCount ; ++i )
		s_operationItems[i].m_bEnabled = operationResult == OperationResult_Succeed ? IsOperationValid( (OperationID)i, m_fileStatusMap ) : false;

	// Operation list construction
	switch( in_menuType )
	{
		case OperationMenuType_WorkUnits:
			{
				out_rOperationList.AddTail( s_operationItems[OperationID_GetLatestVersion] );
				out_rOperationList.AddTail( s_operationItems[OperationID_Submit] );
				out_rOperationList.AddTail( s_operationItems[OperationID_CheckOut] );
				out_rOperationList.AddTail( s_operationItems[OperationID_Lock] );
				out_rOperationList.AddTail( s_operationItems[OperationID_Unlock] );
				out_rOperationList.AddTail( s_operationItems[OperationID_Add] );
				out_rOperationList.AddTail( s_operationItems[OperationID_Delete] );
				out_rOperationList.AddTail( s_operationItems[OperationID_Rename] );
				out_rOperationList.AddTail( s_operationItems[OperationID_Move] );
				out_rOperationList.AddTail( s_operationItems[OperationID_Revert] );
				out_rOperationList.AddTail( s_operationItems[OperationID_Resolve] );
				out_rOperationList.AddTail( s_operationItems[OperationID_Diff] );
				out_rOperationList.AddTail( s_operationItems[OperationID_History] );
				break;
			}
			
		case OperationMenuType_Sources:
		case OperationMenuType_Generated:
			{
				out_rOperationList.AddTail( s_operationItems[OperationID_GetLatestVersion] );
				out_rOperationList.AddTail( s_operationItems[OperationID_Submit] );
				out_rOperationList.AddTail( s_operationItems[OperationID_CheckOut] );
				out_rOperationList.AddTail( s_operationItems[OperationID_Lock] );
				out_rOperationList.AddTail( s_operationItems[OperationID_Unlock] );
				out_rOperationList.AddTail( s_operationItems[OperationID_Add] );
				out_rOperationList.AddTail( s_operationItems[OperationID_Delete] );
				out_rOperationList.AddTail( s_operationItems[OperationID_Rename] );
				out_rOperationList.AddTail( s_operationItems[OperationID_Move] );
				out_rOperationList.AddTail( s_operationItems[OperationID_Revert] );
				out_rOperationList.AddTail( s_operationItems[OperationID_Diff] );
				out_rOperationList.AddTail( s_operationItems[OperationID_History] );
				break;
			}
			
		case OperationMenuType_Explorer:
			{
				out_rOperationList.AddTail( s_operationItems[OperationID_Submit] );
				out_rOperationList.AddTail( s_operationItems[OperationID_CheckOut] );
				out_rOperationList.AddTail( s_operationItems[OperationID_Lock] );
				out_rOperationList.AddTail( s_operationItems[OperationID_Unlock] );
				out_rOperationList.AddTail( s_operationItems[OperationID_Add] );
				out_rOperationList.AddTail( s_operationItems[OperationID_Revert] );
				out_rOperationList.AddTail( s_operationItems[OperationID_Diff] );
				out_rOperationList.AddTail( s_operationItems[OperationID_History] );
				break;
			}
	}

	return operationResult;
}

LPCWSTR PerforceSourceControl::GetOperationName( DWORD in_dwOperationID )
{
	if( in_dwOperationID > OperationID_None && in_dwOperationID < OperationID_ContextMenuCount )
		return k_operationNames[in_dwOperationID];
	return L"";
}

DWORD PerforceSourceControl::GetOperationEffect( DWORD in_dwOperationID )
{
	DWORD dwRetVal = 0;

	switch( in_dwOperationID )
	{
	case OperationID_GetLatestVersion:
	case OperationID_Rename:
	case OperationID_Move:
	case OperationID_Revert:
	case OperationID_Delete:
		dwRetVal |= OperationEffect_LocalContentModification;
		break;

	case OperationID_Submit:
		dwRetVal |= OperationEffect_ServerContentModification;
		break;
	}

	return dwRetVal;
}

ISourceControl::OperationResult PerforceSourceControl::GetFileStatus( const StringList& in_rFilenameList, FilenameToStatusMap& out_rFileStatusMap, DWORD in_dwTimeoutMs )
{
	return GetFileStatus( in_rFilenameList, &out_rFileStatusMap, true, in_dwTimeoutMs );
}

ISourceControl::OperationResult PerforceSourceControl::GetFileStatus( 
	const StringListView& in_rFilenameList, 
	FilenameToStatusMap* out_pFileStatusMap,
	bool in_bSkipDirectories,
	DWORD in_dwTimeoutMs,
	bool in_bShowErrorMsgs,
	bool in_skipDirectoryExpansion)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// Init parameters
	Operation operation( this, OperationID_GetFileStatus );
	m_pFilenameToStatusMap = out_pFileStatusMap;
	m_bPrintTextToDialog = false;

	// Convert filenames
	StringVector escapedFilenameList;
	GetValidPathList(in_rFilenameList, escapedFilenameList, in_bSkipDirectories, in_skipDirectoryExpansion);

	// Get the status of the files
	m_fileStatusMap.RemoveAll();
	OperationResult result = OperationResult_Succeed;
	
	if (!escapedFilenameList.empty())
		result = RunCommand("fstat", escapedFilenameList, {}, in_dwTimeoutMs, in_bShowErrorMsgs );

	if ( result == OperationResult_Succeed )
	{
		// Unsetted values must be 'local only'
		CString csStatus = _T( "local only" );

		for (const auto& filename : in_rFilenameList)
		{
			const CString csFileKey = FixFileMapKey(filename);

			FileStatus fileStatus;
			bool bFound = m_fileStatusMap.Lookup( csFileKey, fileStatus ) == TRUE;

			if ( !bFound || ( fileStatus == FileStatus_NotAvailable ) )
			{
				if ( out_pFileStatusMap )
				{
					FilenameToStatusMapItem statusItem;
					if ( out_pFileStatusMap->Lookup( csFileKey, statusItem ) )
					{
						::SysFreeString( statusItem.m_bstrStatus );
						::SysFreeString( statusItem.m_bstrOwner );
					}
				
					FilenameToStatusMapItem defaultStatusItem = {0};
					if( ::PathIsDirectory( csFileKey ) )
						defaultStatusItem.m_bstrStatus = nullptr;
					else
						defaultStatusItem.m_bstrStatus = csStatus.AllocSysString();

					out_pFileStatusMap->SetAt( filename, defaultStatusItem );
				}
				
				m_fileStatusMap.SetAt( csFileKey, FileStatus_LocalOnly );
			}
		}
	}

	// Reset parameters
	m_pFilenameToStatusMap = nullptr;
	m_bPrintTextToDialog = true;

	return result;
}

AK::Wwise::ISourceControl::OperationResult PerforceSourceControl::GetFilesForOperation( DWORD in_dwOperationID, const StringList& in_rFilenameList, StringList& out_rFilenameList, FilenameToStatusMap& out_rFileStatusMap )
{
	ISourceControl::OperationResult result = UpdateWorkspaces();
	if (result == OperationResult_Succeed)
	{
		Operation operation(this, OperationID_Opened);

		m_pOpenedFiles = &out_rFilenameList;

		// Argument used to get missing files from the 'have' list
		result = RunCommand("opened", {}, {});

		m_pOpenedFiles = nullptr;
	}

	return result;
}

AK::Wwise::ISourceControl::OperationResult PerforceSourceControl::CheckFilesForOperation( DWORD in_dwOperationID, const StringList& in_rFilenameList, BooleanList& out_rFileStatusList )
{
	// First get the file statuses
	ISourceControl::OperationResult eResult = GetFileStatus( in_rFilenameList, nullptr, true, INFINITE );
	if ( eResult == OperationResult_Succeed )
	{
		// Get files that are not checked out
		SourceControlContainers::AkPos filePos = in_rFilenameList.GetHeadPosition();
		for ( ;filePos; in_rFilenameList.GetNext( filePos ) )
		{
			LPCWSTR czFilename = in_rFilenameList.GetAt( filePos );

			FileStatus fileStatus;
			VERIFY( m_fileStatusMap.Lookup( FixFileMapKey( czFilename ), fileStatus ) );

			bool bOpOk = IsOperationValid( (OperationID)in_dwOperationID, fileStatus, czFilename, false );

			out_rFileStatusList.AddTail( bOpOk );
		}
	}
	return eResult;
}

AK::Wwise::ISourceControl::OperationResult PerforceSourceControl::FilesUnderSourceControl( const StringList& in_rFilenameList, BooleanList& out_rFileStatusList )
{
	ISourceControl::OperationResult eResult = GetFileStatus( in_rFilenameList, nullptr, true, INFINITE, false );
	if ( eResult == OperationResult_Succeed )
	{
		// Get files that are not checked out
		SourceControlContainers::AkPos filePos = in_rFilenameList.GetHeadPosition();
		for ( ;filePos; in_rFilenameList.GetNext( filePos ) )
		{
			LPCWSTR czFilename = in_rFilenameList.GetAt( filePos );

			FileStatus fileStatus;
			VERIFY( m_fileStatusMap.Lookup( FixFileMapKey( czFilename ), fileStatus ) );

			bool bOnServer = fileStatus & FileStatus_OnServer;
			out_rFileStatusList.AddTail( bOnServer );
		}
	}
	return eResult;
}

ISourceControl::OperationResult PerforceSourceControl::GetFileStatusIcons( const StringList& in_rFilenameList, FilenameToIconMap& out_rFileIconsMap, DWORD in_dwTimeoutMs )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	OperationResult result = OperationResult_Succeed;

	// Get the status of the files
	result = GetFileStatus(
		in_rFilenameList,
		nullptr,				// Filling our own map
		false,				// Ignored due to skip directories
		in_dwTimeoutMs,		// Respect given timeout
		false,				// No error message
		true				// Don't check for directories, Perforce will ignore them
	);

	if ( result == OperationResult_Succeed )
	{
		PopulateFilenameToIconMap(in_rFilenameList, out_rFileIconsMap);
	}

	return result;
}

ISourceControl::OperationResult PerforceSourceControl::GetFileStatusAndIcons(
	const StringList& in_rFilenameList,
	FilenameToStatusMap& out_rFileStatusMap,
	FilenameToIconMap& out_rFileIconsMap,
	DWORD in_dwTimeoutMs)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	OperationResult result = OperationResult_Succeed;

	// Get the status of the files
	result = GetFileStatus(
		in_rFilenameList,
		&out_rFileStatusMap,// Filling our own map
		false,				// Ignored due to skip directories
		in_dwTimeoutMs,		// Respect given timeout
		false,				// No error message
		true				// Don't check for directories, Perforce will ignore them
	);

	if ( result == OperationResult_Succeed )
	{
		PopulateFilenameToIconMap(in_rFilenameList, out_rFileIconsMap);
	}

	return result;
}

void PerforceSourceControl::PopulateFilenameToIconMap(const StringList& in_rFilenameList, FilenameToIconMap& out_rFileIconsMap)
{
	SourceControlContainers::AkPos filePos = in_rFilenameList.GetHeadPosition();

	// Fill the "FileToIcon" map
	while ( filePos )
	{
		auto filenameRequested = in_rFilenameList.GetAt( filePos );
		const CString csFileKey = FixFileMapKey( filenameRequested );

		FileStatus fileStatus = FileStatus_NotAvailable;

		if ( m_fileStatusMap.Lookup( csFileKey, fileStatus ) )
		{
			HICON hIcon = nullptr;
			CString csTooltipText;

			if ( (fileStatus & FileStatus_OtherCheckout) && (fileStatus & FileStatus_LocalEdit) && (fileStatus & FileStatus_Outdated) )
			{
				hIcon = m_icons[IconsIndexes_Outdated_BothCheckout];
				csTooltipText = k_tooltipText[IconsIndexes_Outdated_BothCheckout];
			}
			else if ( (fileStatus & FileStatus_OtherCheckout) && (fileStatus & FileStatus_LocalEdit) )
			{
				hIcon = m_icons[IconsIndexes_BothCheckout];
				csTooltipText = k_tooltipText[IconsIndexes_BothCheckout];
			}
			else if ( (fileStatus & FileStatus_OtherCheckout) && (fileStatus & FileStatus_Outdated) )
			{
				hIcon = m_icons[IconsIndexes_Outdated_OtherCheckout];
				csTooltipText = k_tooltipText[IconsIndexes_Outdated_OtherCheckout];
			}
			else if ( fileStatus & FileStatus_OtherCheckout )
			{
				hIcon = m_icons[IconsIndexes_OtherCheckout];
				csTooltipText = k_tooltipText[IconsIndexes_OtherCheckout];
			}
			else if ( (fileStatus & FileStatus_LocalEdit) && (fileStatus & FileStatus_Outdated) )
			{
				hIcon = m_icons[IconsIndexes_Outdated_CheckOut];
				csTooltipText = k_tooltipText[IconsIndexes_Outdated_CheckOut];
			}
			else if ( fileStatus & FileStatus_LocalEdit )
			{
				hIcon = m_icons[IconsIndexes_CheckOut];
				csTooltipText = k_tooltipText[IconsIndexes_CheckOut];
			}
			else if ( (fileStatus & FileStatus_OnServer) && (fileStatus & FileStatus_Outdated) )
			{
				hIcon = m_icons[IconsIndexes_Outdated_NotCheckOut];
				csTooltipText = k_tooltipText[IconsIndexes_Outdated_NotCheckOut];
			}
			else if ( fileStatus & FileStatus_Added || fileStatus & FileStatus_MoveAdd )
			{
				hIcon = m_icons[IconsIndexes_Added];
				csTooltipText = k_tooltipText[IconsIndexes_Added];
			}
			else if ( fileStatus & FileStatus_OnServer )
			{
				hIcon = m_icons[IconsIndexes_NotCheckOut];
				csTooltipText = k_tooltipText[IconsIndexes_NotCheckOut];
			}

			FilenameToIconMapItem iconItem = { hIcon, csTooltipText.AllocSysString() };
			out_rFileIconsMap.SetAt( filenameRequested, iconItem );
		}

		in_rFilenameList.GetNext( filePos );
	}
}

ISourceControl::OperationResult PerforceSourceControl::UpdateWorkspaces()
{
	// Obtain the workspace root
	Operation operation( this, OperationID_Workspaces );
	
	m_workspaces.clear();
	return RunCommand("workspaces", {}, {});
}

ISourceControl::OperationResult PerforceSourceControl::GetMissingFilesInDirectories( const StringList& in_rDirectoryList, StringList& out_rFilenameList )
{
	// Init parameters
	m_bPrintTextToDialog = false;

	StringVector localFilenameList;
	GetValidPathList(in_rDirectoryList, localFilenameList, false);

	OperationResult result = UpdateWorkspaces();

	if( result == OperationResult_Succeed )
	{
		// Obtain the deleted files
		Operation operation( this, OperationID_Opened );
		
		m_pMissingFiles = &out_rFilenameList;

		// Argument used to get missing files from the 'have' list
		RunCommand("opened", localFilenameList, {});

		m_pMissingFiles = nullptr;
	}

	// Reset parameters
	m_bPrintTextToDialog = true;

	return result;
}

AK::Wwise::ISourceControl::IOperationResult* PerforceSourceControl::DoOperation( DWORD in_dwOperationID, const StringList& in_rFilenameList, const StringList* in_pTargetFilenameList )
{
	if (in_dwOperationID == OperationID_RenameNoUI || in_dwOperationID == OperationID_MoveNoUI)
	{
		ASSERT(in_pTargetFilenameList);
		return in_pTargetFilenameList ? MoveNoUI(in_dwOperationID, in_rFilenameList, *in_pTargetFilenameList) : nullptr;
	}

	return DoOperation( in_dwOperationID, in_rFilenameList );
}

AK::Wwise::ISourceControl::IOperationResult* PerforceSourceControl::DoOperation(DWORD in_dwOperationID, const StringListView& in_rFilenameList)
{
	// Do the operation
	switch ( in_dwOperationID )
	{
		case OperationID_GetLatestVersion:
			DoOperation<true>(in_dwOperationID, in_rFilenameList, [this](const auto& in_filenameList) {
				GetLatestVersion(in_filenameList);
			});
			break;
			
		case OperationID_Submit:
			DoOperation<true>(in_dwOperationID, in_rFilenameList, [this](const auto& in_filenameList) {
				Submit(in_filenameList);
			});
			break;
			
		case OperationID_CheckOut:
		case OperationID_CheckOutNoUI:
			DoOperation<true>(in_dwOperationID, in_rFilenameList, [=](const auto& in_filenameList) {
				CheckOut(in_filenameList, in_dwOperationID == OperationID_CheckOut);
			});
			break;
			
		case OperationID_Lock:
			DoOperation<true>(in_dwOperationID, in_rFilenameList, [this](const auto& in_filenameList) {
				Lock(in_filenameList);
			});
			break;
			
		case OperationID_Unlock:
			DoOperation<true>(in_dwOperationID, in_rFilenameList, [this](const auto& in_filenameList) {
				Unlock(in_filenameList);
			});
			break;
			
		case OperationID_Add:
		case OperationID_AddNoUI:
			// The Add operation must use the original filename list to force the use of wildcards
			DoOperation<false>(in_dwOperationID, in_rFilenameList, [=](const auto& in_filenameList) {
				Add(in_filenameList, in_dwOperationID == OperationID_Add);
			});
			break;
			
		case OperationID_Delete:
		case OperationID_DeleteNoUI:
			// The Delete operation must use the original filename list to manage local file name (will be converted later)
			return DoOperation<false>(in_dwOperationID, in_rFilenameList, [=](const auto& in_filenameList) {
				return Delete(in_filenameList, in_dwOperationID == OperationID_Delete);
			});
			break;
			
		case OperationID_Rename:
			// The Rename operation must use the original filename list to manage local file name (will be converted later)
			return DoOperation<false>(in_dwOperationID, in_rFilenameList, [this](const auto& in_filenameList) {
				return Rename(in_filenameList);
			});
			break;

		case OperationID_Move:
			// The Move operation must use the original filename list to manage local file name (will be converted later)
			return DoOperation<false>(in_dwOperationID, in_rFilenameList, [this](const auto& in_filenameList) {
				return Move(in_filenameList);
			});
			break;

		case OperationID_Revert:
		case OperationID_RevertNoUI:
			DoOperation<true>(in_dwOperationID, in_rFilenameList, [=](const auto& in_filenameList) {
				Revert(in_filenameList, in_dwOperationID == OperationID_Revert);
			});
			break;
			
		case OperationID_Resolve:
			DoOperation<true>(in_dwOperationID, in_rFilenameList, [this](const auto& in_filenameList) {
				Resolve(in_filenameList);
			});
			break;
			
		case OperationID_Diff:
			DoOperation<true>(in_dwOperationID, in_rFilenameList, [this](const auto& in_filenameList) {
				Diff(in_filenameList);
			});
			break;
			
		case OperationID_History:
			DoOperation<true>(in_dwOperationID, in_rFilenameList, [this](const auto& in_filenameList) {
				History(in_filenameList);
			});
			break;
	}

	return nullptr;
}

AK::Wwise::ISourceControl::IOperationResult* PerforceSourceControl::MoveNoUI(DWORD in_dwOperationID, const StringListView& in_rFilenameList, const StringListView& in_pTargetFilenameList)
{
	ASSERT(in_rFilenameList.size() == in_pTargetFilenameList.size());
	if (in_rFilenameList.size() != in_pTargetFilenameList.size())
		return nullptr;

	// Init parameters
	Operation operation( this, static_cast<OperationID>( in_dwOperationID ) );

	// Do the operation
	m_pUtilities->GetProgress()->BeginOperation();

	FileOperationResult* pResult = new FileOperationResult();

	// Get file status once
	StringVector statusFileList;
	statusFileList.reserve(in_rFilenameList.size() * 2);
	for (size_t i = 0; i < in_rFilenameList.size(); ++i)
	{
		statusFileList.push_back(in_rFilenameList[i]);
		statusFileList.push_back(in_pTargetFilenameList[i]);
	}

	bool bCanProceed = (GetFileStatus( statusFileList, nullptr) == OperationResult_Succeed);

	if( bCanProceed )
	{
		for (size_t i = 0; i < in_rFilenameList.size(); ++i)
		{
			Move(in_rFilenameList[i], in_pTargetFilenameList[i], pResult);
		}
	}

	m_pUtilities->GetProgress()->EndOperation();

	return pResult;
}

void PerforceSourceControl::GetLatestVersion(const StringListView& in_rFilenameList)
{
	m_pUtilities->GetProgress()->BeginOperation();

	RunCommand("sync", in_rFilenameList, {});

	m_pUtilities->GetProgress()->EndOperation();
}

void PerforceSourceControl::Submit(const StringListView& in_rFilenameList)
{
	// Convert file name from client to depot format
	m_submitFileList.clear();
	m_submitFileList.reserve(in_rFilenameList.size());
	Operation operation( this, OperationID_GetSubmitFiles );

	OperationResult result = RunCommand("where", in_rFilenameList, {});

	if ( result == OperationResult_Succeed )
	{
		bool bCheckOutAfterDialog = false;
		if (m_pUtilities->GetProgress()->IsSilent())
		{
			m_csDescription = m_pUtilities->GetProgress()->GetCommitMessage();
		}
		else
		{
			CDlgPerforceDescription descriptionDlg(m_pUtilities);
			INT_PTR dialogResult = m_pUtilities->CreateModalCustomDialog(&descriptionDlg);

			if (dialogResult == IDOK)
			{
				{
					Operation operation(this, OperationID_Reopen);

					// Ensure all items are in the default change list, unless the submit will fail
					StringVector escapedFilenameList;
					GetValidPathList(in_rFilenameList, escapedFilenameList);

					const StringVector argList = { L"-c", L"default" };
					result = RunCommand("reopen", escapedFilenameList, argList);
				}

				m_csDescription = descriptionDlg.GetDescription();
			}
			bCheckOutAfterDialog = descriptionDlg.CheckOutAfterSubmit();
		}

		// Submit changes
		StringVector argList;
		Operation operation( this, OperationID_Submit );
			
		m_pUtilities->GetProgress()->BeginOperation();

		// Create argument list
		argList.push_back(L"-i");

		if (bCheckOutAfterDialog)
		{
			argList.push_back(L"-r");
		}

		ASSERT(in_rFilenameList.size() == m_submitFileList.size());

		result = RunCommand("submit", {}, argList);
			
		m_pUtilities->GetProgress()->EndOperation();
	}
}

void PerforceSourceControl::CheckOut(const StringListView& in_rFilenameList, bool in_bShowUI)
{
	if (in_bShowUI)
		m_pUtilities->GetProgress()->BeginOperation();

	RunCommand("edit", in_rFilenameList, {});

	if (in_bShowUI)
		m_pUtilities->GetProgress()->EndOperation(m_operationResult != OperationResult_Succeed);
}

void PerforceSourceControl::Lock(const StringListView& in_rFilenameList)
{
	m_pUtilities->GetProgress()->BeginOperation();

	RunCommand("lock", in_rFilenameList, {});

	m_pUtilities->GetProgress()->EndOperation();
}

void PerforceSourceControl::Unlock(const StringListView& in_rFilenameList)
{
	m_pUtilities->GetProgress()->BeginOperation();

	RunCommand("unlock", in_rFilenameList, {});

	m_pUtilities->GetProgress()->EndOperation();
}

void PerforceSourceControl::Add(const StringListView& in_rOriginalFilenameList, bool in_bShowUI)
{
	// Expand directories to files under it
	StringVector files;
	ExpandDirectories(in_rOriginalFilenameList, files);

	FileStatus fileStatus = FileStatus::FileStatus_NotAvailable;

	if (!files.empty())
	{
		// Get the status of the files
		m_fileStatusMap.RemoveAll();
		m_operationResult = GetFileStatus(in_rOriginalFilenameList, nullptr, true);

		bool bReconcile = false;

		// If a single file is marked for delete, we reconcile - WG-39390
		for (const auto& csFilename : files)
		{
			m_fileStatusMap.Lookup(FixFileMapKey(csFilename), fileStatus);

			if (fileStatus & FileStatus_Deleted)
				bReconcile = true;
		}

		if (in_bShowUI)
			m_pUtilities->GetProgress()->BeginOperation();

		if (bReconcile) 
		{
			RunCommand("reconcile", files, {}); // reconcile will open file for edit instead of letting it marked for delete
		}
		else
		{
			const StringVector argList = { L"-f" }; // Force wildcards characters
			RunCommand("add", files, argList);
		}

		if (in_bShowUI)
			m_pUtilities->GetProgress()->EndOperation(m_operationResult != OperationResult_Succeed);
	}
}

AK::Wwise::ISourceControl::IOperationResult* PerforceSourceControl::Delete(const StringListView& in_rOriginalFilenameList, bool in_bShowUI)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( in_bShowUI )
	{
		constexpr auto csCaption = _T( "Delete confirmation" );
		constexpr auto csMessage = _T( "Are you sure you want to delete the selected file(s)?" );

		if(!m_pUtilities->GetProgress()->IsSilent() && m_pUtilities->MessageBox( nullptr, csMessage, csCaption, MB_YESNO ) != IDYES )
			return NULL;
	}

	FileOperationResult* pResult = nullptr;

	if ( in_bShowUI )
		m_pUtilities->GetProgress()->BeginOperation();

	// Get the latest status of the files
	OperationResult result = GetFileStatus( in_rOriginalFilenameList, nullptr);

	if ( result == OperationResult_Succeed )
	{
		StringVector localOnlyFilenameList;
		StringVector serverFilenameList;

		// Split local files and server files
		for (CString csFilename : in_rOriginalFilenameList)
		{
			FileStatus fileStatus;
			VERIFY( m_fileStatusMap.Lookup( FixFileMapKey( csFilename ), fileStatus ) );

			// Do not continue if any of the files is opened on Perforce
			if( (fileStatus & FileStatus_Added) || 
				(fileStatus & FileStatus_Deleted) ||
				(fileStatus & FileStatus_LocalEdit) ||
				(fileStatus & FileStatus_MoveAdd) ||
				(fileStatus & FileStatus_MoveDelete))
			{
				if ( in_bShowUI )
				{
					CString csMessage;
					csMessage.FormatMessage( _T("File '%1' is opened.  Can not delete a file that is currently opened."), csFilename );
					m_pUtilities->GetProgress()->AddLogMessage( AK::Wwise::ISourceControlProgress::Severity_Error, csMessage );
				}
				result = OperationResult_Failed;
			}

			if( ::PathIsDirectory( csFilename ) )
			{
                localOnlyFilenameList.push_back(csFilename);

				FolderAddBackslash( csFilename );
				csFilename += L"...";
				serverFilenameList.push_back(EscapeWildcards(csFilename));
			}
			else if ( fileStatus & FileStatus_LocalOnly )
			{
				localOnlyFilenameList.push_back(csFilename);
			}
			else
			{
				serverFilenameList.push_back(EscapeWildcards(csFilename));
			}
		}

		if ( result == OperationResult_Succeed )
		{
			pResult = new FileOperationResult();

			// Delete files on server
			if (!serverFilenameList.empty())
			{
				OperationResult result = RunCommand("delete", serverFilenameList, {});
				if( result == OperationResult_Succeed )
				{
					// Save results
					for (const auto& filename : serverFilenameList)
					{
						// Verify if file exist
						if (!::PathFileExists(filename))
							pResult->AddFile(filename);
					}
				}
			}

			// Delete local files
			if (!localOnlyFilenameList.empty())
			{
				CString csMessage;

				for (const auto& csFilename : localOnlyFilenameList)
				{
					bool bSuccess = false;
					if( ::PathIsDirectory( csFilename ) )
					{
						bSuccess = ::RemoveDirectory( csFilename ) != FALSE;
					}
					else
					{
						bSuccess = ::DeleteFile( csFilename ) != FALSE;
					}

					if ( bSuccess )
					{
						csMessage.Format( _T("Successfully deleted local file %s"), csFilename );
						pResult->AddFile( csFilename );
					}
					else
					{
						csMessage.Format( _T("Error deleting local file %s (%s)"), csFilename, SourceControlHelpers::GetLastErrorString() );
					}

					if ( in_bShowUI )
						m_pUtilities->GetProgress()->AddLogMessage(
							bSuccess ? AK::Wwise::ISourceControlProgress::Severity_Info : AK::Wwise::ISourceControlProgress::Severity_Error,
							csMessage 
						);
				}
			}
		}

		if ( in_bShowUI )
			m_pUtilities->GetProgress()->EndOperation();
	}

	return pResult;
}

AK::Wwise::ISourceControl::IOperationResult* PerforceSourceControl::Rename(const StringListView& in_rOriginalFilenameList)
{
	FileOperationResult* pResult = nullptr;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ASSERT( in_rOriginalFilenameList.size() == 1 );

	const auto& csOldFilename = in_rOriginalFilenameList[0];

	// Get the folder and the initial name
	CDlgRename renameDialog( csOldFilename, m_pUtilities );
	INT_PTR dialogResult = m_pUtilities->CreateModalCustomDialog( &renameDialog );

	if ( dialogResult == IDOK )
	{
		m_pUtilities->GetProgress()->BeginOperation();

		pResult = new FileOperationResult();

		// Get the status of the files
		const StringVector filenameList = { csOldFilename, renameDialog.GetNewFilename() };
		OperationResult result = GetFileStatus( filenameList, nullptr, false );

		if ( result == OperationResult_Succeed )
		{
			Move( csOldFilename, renameDialog.GetNewFilename(), pResult );
		}
		
		m_pUtilities->GetProgress()->EndOperation();
	}

	return pResult;
}

AK::Wwise::ISourceControl::IOperationResult* PerforceSourceControl::Move(const StringListView& in_rFilenameList)
{
	FileOperationResult* pResult = nullptr;

	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	
	// Wwise makes sure the move operation is done only on sources of the same language
	// Find the language root
	wchar_t szRootPath[MAX_PATH] = {};
	m_pUtilities->GetMoveRootPath( in_rFilenameList[0], szRootPath, ARRAYSIZE(szRootPath) );

	constexpr auto csPrompt = _T("Choose the destination folder for the Move operation.");

	wchar_t szDestinationDir[MAX_PATH] = {};
	if ( m_pUtilities->ShowBrowseForFolderDialog(
			csPrompt, 
			szDestinationDir,
			ARRAYSIZE(szDestinationDir),
			szRootPath ) )
	{
		m_pUtilities->GetProgress()->BeginOperation();

		std::vector<CString> newPaths;
		bool bCanProceed = SourceControlHelpers::CanProceedWithMove( in_rFilenameList, szDestinationDir, newPaths, m_pUtilities,
			_T("Error: The following file already exist in the destination directory: %1"),
			_T("Error: Multiple files with the same name can't be moved at the same destination (%1).") );

		if( bCanProceed )
		{
			// Get file status once
			StringVector statusFileList;
			statusFileList.reserve(in_rFilenameList.size() * 2);
			for (size_t i = 0; i < in_rFilenameList.size(); ++i)
			{
				statusFileList.push_back(in_rFilenameList[i]);
				statusFileList.push_back(newPaths[i]);
			}

			bCanProceed = (GetFileStatus( statusFileList, NULL ) == OperationResult_Succeed);
		}

		if( bCanProceed )
		{
			ASSERT( newPaths.size() == in_rFilenameList.size() );

			// Create the move results
			pResult = new FileOperationResult();

			for (size_t i = 0; i < in_rFilenameList.size(); ++i)
			{
				Move(in_rFilenameList[i], newPaths[i], pResult);
			}
		}

		m_pUtilities->GetProgress()->EndOperation();
	}

	return pResult;
}

void PerforceSourceControl::Move( 
	const CString& in_csFrom, 
	const CString& in_csTo, 
	FileOperationResult* io_pResult )
{
	// Get the fixed version of the file name
	const CString csFixedFrom = FixFileMapKey( in_csFrom );
	const CString csFixedTo = FixFileMapKey( in_csTo );

	// Local or server file renaming
	FileStatus fileStatusFrom;
	VERIFY( m_fileStatusMap.Lookup( csFixedFrom, fileStatusFrom ) );

	bool bIsDirectory = ::PathIsDirectory(in_csFrom) != 0;

	// When m_fileStatusMap.GetCount() == 2, it means only the source and destination were found in the status map
	// When we move/rename a directory, it means there are no files in the depot.
	if ( (fileStatusFrom & FileStatus_LocalOnly) && (!bIsDirectory || m_fileStatusMap.GetCount() == 2) )
	{
		// File is not in repository yet, move it
		CString csMessage;

		if ( ::MoveFile( in_csFrom, in_csTo ) )
		{
			csMessage.Format( _T("Successfully moved local file %s to %s"), (LPCTSTR)in_csFrom, (LPCTSTR)in_csTo );
			io_pResult->AddMovedFile( in_csFrom, in_csTo );
			m_pUtilities->GetProgress()->AddLogMessage(AK::Wwise::ISourceControlProgress::Severity_Info, csMessage);
		}
		else
		{
			csMessage.Format( _T("Error moving local file %s (%s)"), (LPCTSTR)in_csFrom, SourceControlHelpers::GetLastErrorString() );
			m_pUtilities->GetProgress()->AddLogMessage(AK::Wwise::ISourceControlProgress::Severity_Error, csMessage);
		}

	}
	else
	{			
		// We must reinit parameters since GetFileStatus modify them
		Operation operation( this, OperationID_Move );

		FileStatus fileStatusTo;
		VERIFY( m_fileStatusMap.Lookup( csFixedTo, fileStatusTo ) );

		if ( !( fileStatusTo & FileStatus_LocalOnly ) || ::PathFileExists( in_csTo ) )
		{
			// Failed, name conflict
			CString csMsg;
			csMsg.Format( _T("Can't rename %s to %s, file already exists"), in_csFrom, in_csTo );

			m_pUtilities->GetProgress()->AddLogMessage( AK::Wwise::ISourceControlProgress::Severity_Error, csMsg );
		}
		else
		{
			const StringVector filenameList = { in_csFrom, in_csTo };

			StringVector escapedFilenameList;
			GetValidPathList(filenameList, escapedFilenameList);

			// Run "edit" first on the source
			const StringVector escapedEditFilenameList = { escapedFilenameList.front() };
			RunCommand("edit", escapedEditFilenameList, {});

			OperationResult result = RunCommand("move", escapedFilenameList, {});

			if ( result == OperationResult_Succeed && ::PathFileExists( in_csTo ) )
			{
				io_pResult->AddMovedFile( in_csFrom, in_csTo );
			}
		}
	}
}

void PerforceSourceControl::Revert(const StringListView& in_rFilenameList, bool in_bShowUI)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( in_bShowUI )
	{
		constexpr auto csCaption = _T("Perforce plug-in");
		constexpr auto csMessage = _T("Are you sure you want to revert the selected item(s)? You will lose all changes since the last update.");

		if(m_pUtilities->GetProgress()->IsSilent() || m_pUtilities->MessageBox( NULL, csMessage, csCaption, MB_YESNO ) == IDYES )
		{
			m_pUtilities->GetProgress()->BeginOperation();

			RunCommand("revert", in_rFilenameList, {});

			m_pUtilities->GetProgress()->EndOperation();
		}
	}
	else
	{
		RunCommand("revert", in_rFilenameList, {});
	}
}

void PerforceSourceControl::Resolve(const StringListView& in_rFilenameList)
{
	m_pUtilities->GetProgress()->BeginOperation();

	m_bFirstResolve = true;

	const StringVector argList = { L"-o", L"-t" };
	RunCommand("resolve", in_rFilenameList, argList);

	m_pUtilities->GetProgress()->EndOperation();
}

void PerforceSourceControl::Diff(const StringListView& in_rFilenameList)
{
	m_bFileDiff = false;

	const StringVector argList;
	RunCommand("diff", in_rFilenameList, argList);

	// If the ClientUser diff function has not been called, files are identical
	if ( !m_bFileDiff )
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		constexpr auto csCaption = _T("Perforce plug-in");
		constexpr auto csMessage = _T("Files are identical");

		m_pUtilities->MessageBox( NULL, csMessage, csCaption, MB_OK | MB_ICONINFORMATION );
	}
}

void PerforceSourceControl::History(const StringListView& in_rFilenameList)
{
	ASSERT(in_rFilenameList.size() == 1); // This has never been tested with multiple files, no guarantee it will work!

	m_pUtilities->GetProgress()->BeginOperation();

	RunCommand("filelog", in_rFilenameList, {});

	m_pUtilities->GetProgress()->EndOperation();
}

ISourceControl::OperationResult PerforceSourceControl::PreCreateOrModify( const StringList& in_rFilenameList, CreateOrModifyOperation in_eOperation, bool& out_rContinue )
{
	StringVector notCheckedOutFiles;
	std::vector< LPCWSTR > notCheckedOutFilesList;
	OperationResult result = OperationResult_Succeed;

	ASSERT( in_eOperation );
	
	// Get the status of the files to know if we need to check out files
	if ( in_eOperation & CreateOrModifyOperation_Modify )
	{
		result = GetFileStatus( in_rFilenameList, nullptr);

		if ( result == OperationResult_Succeed )
		{
			// Get files that are not checked out
			SourceControlContainers::AkPos filePos = in_rFilenameList.GetHeadPosition();

			while ( filePos )
			{
				LPCWSTR czFilename = in_rFilenameList.GetAt( filePos );

				FileStatus fileStatus;
				VERIFY( m_fileStatusMap.Lookup( FixFileMapKey( czFilename ), fileStatus ) );

				if ( !( fileStatus & FileStatus_LocalEdit ) && 
					( fileStatus & FileStatus_OnServer ) )
				{
					notCheckedOutFiles.push_back( czFilename );
					notCheckedOutFilesList.push_back( czFilename );
				}

				in_rFilenameList.GetNext( filePos );
			}
		}
	}

	bool bHasFilesToCheckOut = !notCheckedOutFiles.empty();
	bool bHasFilesToAdd = ( in_eOperation & CreateOrModifyOperation_Create ) ? true : false;
	
	// Show the CheckOut/Add dialog only if operations can be done on files
	if ( bHasFilesToCheckOut || bHasFilesToAdd )
	{
		auto pProgress = m_pUtilities->GetProgress();
		if( m_bAutoAcceptMode || (pProgress && pProgress->IsSilent()))
		{
			m_bOnNewFilesDoOperation = false;
			out_rContinue = true;
			m_bOnNewFilesCheckOut = false;
			m_bOnNewFilesAdd = false;
			if (pProgress->IsAutoCheckout())
			{
				m_bOnNewFilesDoOperation = true;
				m_bOnNewFilesCheckOut = bHasFilesToCheckOut;
			}
			if (pProgress->IsAutoAdd())
			{
				m_bOnNewFilesDoOperation = true;
				m_bOnNewFilesAdd = bHasFilesToAdd;
			}
		}
		else
		{
			CDlgPerforceOnNewFiles newFiles( 
				bHasFilesToAdd, 
				bHasFilesToCheckOut, 
				m_bOnNewFilesAdd, 
				m_bOnNewFilesCheckOut, 
				m_pUtilities, 
				notCheckedOutFilesList.empty() ? nullptr : &(notCheckedOutFilesList[0]),
				(unsigned int)notCheckedOutFilesList.size() );

			m_pUtilities->CreateModalCustomDialog( &newFiles );
			INT_PTR uiResult = newFiles.GetResult();

			m_bOnNewFilesDoOperation = uiResult == ID_OK;
			out_rContinue = uiResult != ID_CANCEL;

			if( out_rContinue )
			{
				m_bOnNewFilesAdd = newFiles.NeedToAddFiles();
				m_bOnNewFilesCheckOut = newFiles.NeedToCheckOutFiles();
			}
		}
	}
	else
	{
		m_bOnNewFilesDoOperation = false;
		out_rContinue = true;
	}

	// We need to check out files
	if( m_bOnNewFilesDoOperation && m_bOnNewFilesCheckOut && bHasFilesToCheckOut )
	{
		DoOperation<true>(OperationID_CheckOut, notCheckedOutFiles, [this](const auto& in_filenameList) {
			CheckOut(in_filenameList, true);
		});
	}

	return result;
}

ISourceControl::OperationResult PerforceSourceControl::PostCreateOrModify( const StringList& in_rFilenameList, CreateOrModifyOperation in_eOperation, bool& out_rContinue )
{
	out_rContinue = true;

	if ( m_bOnNewFilesDoOperation && ( in_eOperation & CreateOrModifyOperation_Create )  )
	{
		// Add files
		if ( m_bOnNewFilesAdd )
		{
			OperationResult result = GetFileStatus( in_rFilenameList, nullptr);

			if ( result == OperationResult_Succeed )
			{
				// Get files that need to be added
				StringVector toAddFiles;
				SourceControlContainers::AkPos filePos = in_rFilenameList.GetHeadPosition();

				while ( filePos )
				{
					const auto csFilename = in_rFilenameList.GetAt( filePos );

					FileStatus fileStatus;

					VERIFY( m_fileStatusMap.Lookup( FixFileMapKey( csFilename ), fileStatus ) );

					if ( fileStatus & FileStatus_LocalOnly )
					{
						toAddFiles.push_back(csFilename);
					}

					in_rFilenameList.GetNext( filePos );
				}

				if ( !toAddFiles.empty() )
				{
					DoOperation<false>(OperationID_Add, toAddFiles, [this](const auto& in_filenameList) {
						Add(in_filenameList, true);
					});
				}
			}
		}
	}

	return OperationResult_Succeed;
}

bool PerforceSourceControl::ExtractServerAndPort( LPCTSTR in_szString, CString& out_csServer, CString& out_csPort )
{
	bool bResult = false;

	UINT unPort = 0;

	// Note: in_szString might be in the "protocol:server:port", "server:port" format, or just "port" (for local server)

	CString protocol;
	CString server;
	const int nProtocolScanCount = _stscanf_s(in_szString, _T("%[^:]:%[^:]:%d"), protocol.GetBuffer(AK_PERFORCE_MAX_SERVER_NAME_LENGTH), AK_PERFORCE_MAX_SERVER_NAME_LENGTH, server.GetBuffer(AK_PERFORCE_MAX_SERVER_NAME_LENGTH), AK_PERFORCE_MAX_SERVER_NAME_LENGTH, &unPort);
	protocol.ReleaseBuffer();
	server.ReleaseBuffer();

	if (nProtocolScanCount == 3)
	{
		out_csServer.Format(_T("%s:%s"), protocol, server);
		out_csPort.Format(_T("%d"), unPort);
		bResult = true;
	}
	else
	{
		const int nScanCount = _stscanf_s(in_szString, _T("%[^:]:%d"), out_csServer.GetBuffer(AK_PERFORCE_MAX_SERVER_NAME_LENGTH), AK_PERFORCE_MAX_SERVER_NAME_LENGTH, &unPort);
		out_csServer.ReleaseBuffer();

		if (nScanCount == 2)
		{
			// Format: "server:port"
			out_csPort.Format(_T("%d"), unPort);
			bResult = true;
		}
		else if (_stscanf_s(in_szString, _T("%d"), &unPort) == 1)
		{
			// Format: "port" (no server)
			out_csPort.Format(_T("%d"), unPort);
			out_csServer.Empty();
			bResult = true;
		}
	}

	return bResult;
}

CString PerforceSourceControl::ServerAndPort( LPCTSTR in_szServer, LPCTSTR in_szPort )
{
	ASSERT( in_szServer && in_szServer );

	CString csServerAndPort;

	if ( _tcslen( in_szServer ) > 0 )
	{
		// Format: "server:port"
		csServerAndPort.Format( _T("%s:%s"), in_szServer, in_szPort );
	}
	else
	{
		// Format: "port" (no server)
		csServerAndPort = in_szPort;
	}

	return csServerAndPort;
}

static LPCWSTR GetParameterValue(const PerforceSourceControl::IConnectParameterMap& in_parameterMap, LPCWSTR in_name)
{
	LPCWSTR value;
	if (in_parameterMap.Lookup(in_name, value)) return value;

	return _T("");
}

void PerforceSourceControl::LogError(const Error & error, const TCHAR* szContext)
{
	m_pUtilities->GetProgress()->AddLogMessage(
		AK::Wwise::ISourceControlProgress::Severity_Error,
		szContext
	);

	for (auto errorNum = 0; errorNum < error.GetErrorCount(); errorNum++)
	{
		auto pErrorId = error.GetId(errorNum);
		if (pErrorId)
		{
			switch (pErrorId->Severity())
			{
			case E_INFO:
				m_pUtilities->GetProgress()->AddLogMessage(
					AK::Wwise::ISourceControlProgress::Severity_Info,
					CA2W(pErrorId->fmt)
				);
				break;

			case E_WARN:
				m_pUtilities->GetProgress()->AddLogMessage(
					AK::Wwise::ISourceControlProgress::Severity_Warning,
					CA2W(pErrorId->fmt)
				);
				break;

			case E_FAILED:
			case E_FATAL:
				m_pUtilities->GetProgress()->AddLogMessage(
					AK::Wwise::ISourceControlProgress::Severity_Error,
					CA2W(pErrorId->fmt)
				);
				break;

			default:
				break;
			}
		}
	}
}

ISourceControl::OperationResult PerforceSourceControl::Connect(const IConnectParameterMap& parameterMap)
{
	m_pUtilities->SetUserPreferenceString(
		k_csRegFolder + k_csRegKeyClient,
		GetParameterValue(parameterMap, k_csWorkspace)
	);

	m_pUtilities->SetUserPreferenceString(
		k_csRegFolder + k_csRegKeyServer, 
		GetParameterValue(parameterMap, k_csServer)
	);

	m_pUtilities->SetUserPreferenceString(
		k_csRegFolder + k_csRegKeyPort, 
		GetParameterValue(parameterMap, k_csPort)
	);

	m_pUtilities->SetUserPreferenceString(
		k_csRegFolder + k_csRegKeyHost, 
		GetParameterValue(parameterMap, k_csHost)
	);

	m_pUtilities->SetUserPreferenceString(
		k_csRegFolder + k_csRegKeyUser, 
		GetParameterValue(parameterMap, k_csUser)
	);

	LPCWSTR szPassword;
	if (parameterMap.Lookup(k_csPassword, szPassword))
	{
		SetOverridePassword(szPassword);
	}
	else
	{
		ResetOverridePassword();
	}

	Error error;
	if (!ReInitClient(error))
	{
		LogError( error, _T("Cannot not initialize Perforce client") );
		return OperationResult_Failed;
	}

	return OperationResult_Succeed;
}

ISourceControl::OperationResult PerforceSourceControl::Disconnect()
{
	ResetOverridePassword();
	return OperationResult_Succeed;
}

void PerforceSourceControl::ResetOverridePassword()
{
	m_bPasswordOverride = false;
	m_csPassword.Empty();
}

void PerforceSourceControl::SetOverridePassword(const CString& in_csOverridePassword)
{
	m_bPasswordOverride = true;
	m_csPassword = in_csOverridePassword;
}

