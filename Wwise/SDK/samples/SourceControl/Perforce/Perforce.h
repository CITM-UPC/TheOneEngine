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

#pragma once

#include "Resource.h"

#include "SourceControlHelpers.h"
#include <afxtempl.h>

#include <array>
#include <map>
#include <optional>
#include <stack>
#include <vector>

#pragma warning( push )
#pragma warning ( disable : 4244 4267 )
#include "p4/ClientApi.h"
#include "p4/i18napi.h"
#pragma warning( pop )

namespace
{
	const static unsigned int k_uiVersion = 1;
	const static CString k_csRegFolder = L"Version 1\\";
	const static CString k_csRegKeyClient = L"P4Client";
	const static CString k_csRegKeyServer = L"P4Server";
	const static CString k_csRegKeyPort = L"P4Port";
	const static CString k_csRegKeyHost = L"P4Host";
	const static CString k_csRegKeyUser = L"P4User";
	const static CString k_csRegKeyNewFilesCheckOut = L"NewFilesCheckOut";
	const static CString k_csRegKeyNewFilesAdd = L"NewFilesAdd";
	const static CString k_csRegKeyDiffCommand = L"DiffCommand";
	const static CString k_csUseAKWaveViewerForDiff = L"UseAKWaveViewerForDiff";
};

class OperationDialog;
class FileOperationResult;

class PerforceSourceControl final
	: public AK::Wwise::ISourceControl
	, public ClientUser
	, public KeepAlive
{
public:
	PerforceSourceControl();
	virtual ~PerforceSourceControl();

	static void GetPluginInfo( PluginInfo& out_rPluginInfo );
	static bool IsAvalaible();

	// ISourceControl
	virtual void Init( AK::Wwise::ISourceControlUtilities* in_pUtilities, bool in_bAutoAccept ) override;
	virtual void Term() override;
	virtual void Destroy() override;

	virtual bool ShowConfigDlg() override;
	virtual OperationResult GetOperationList( OperationMenuType in_menuType, const StringList& in_rFilenameList, OperationList& out_rOperationList ) override;
	virtual LPCWSTR GetOperationName( DWORD in_dwOperationID ) override;
	virtual DWORD GetOperationEffect( DWORD in_dwOperationID ) override;
	virtual OperationResult GetFileStatus( const StringList& in_rFilenameList, FilenameToStatusMap& out_rFileStatusMap, DWORD in_dwTimeoutMs ) override;
	virtual OperationResult GetFileStatusIcons( const StringList& in_rFilenameList, FilenameToIconMap& out_rFileIconsMap, DWORD in_dwTimeoutMs ) override;
	virtual OperationResult GetFileStatusAndIcons( const StringList& in_rFilenameList, FilenameToStatusMap& out_rFileStatusMap, FilenameToIconMap& out_rFileIconsMap, DWORD in_dwTimeoutMs ) override;
	virtual OperationResult GetMissingFilesInDirectories( const StringList& in_rDirectoryList, StringList& out_rFilenameList ) override;
	virtual IOperationResult* DoOperation( DWORD in_dwOperationID, const StringList& in_rFilenameList, const StringList* in_pTargetFilenameList ) override;
	virtual OperationResult PreCreateOrModify( const StringList& in_rFilenameList, CreateOrModifyOperation in_eOperation, bool& out_rContinue ) override;
	virtual OperationResult PostCreateOrModify( const StringList& in_rFilenameList, CreateOrModifyOperation in_eOperation, bool& out_rContinue ) override;
	virtual AK::Wwise::ISourceControl::OperationResult GetFilesForOperation( DWORD in_dwOperationID, const StringList& in_rFilenameList, StringList& out_rFilenameList, FilenameToStatusMap& out_rFileStatusMap ) override;
	virtual AK::Wwise::ISourceControl::OperationResult CheckFilesForOperation( DWORD in_dwOperationID, const StringList& in_rFilenameList, BooleanList& out_rFileStatusList ) override;
	virtual AK::Wwise::ISourceControl::OperationResult FilesUnderSourceControl( const StringList& in_rFilenameList, BooleanList& out_rFileStatusList ) override;
	virtual OperationResult Connect(const IConnectParameterMap& parameterMap) override;
	virtual OperationResult Disconnect() override;

	// Operations ID
	enum OperationID
	{
		OperationID_None = 0,

		OperationID_GetLatestVersion,
		OperationID_Submit,
		OperationID_CheckOut,
		OperationID_Lock,
		OperationID_Unlock,
		OperationID_Add,
		OperationID_Delete,
		OperationID_Rename,
		OperationID_Move,
		OperationID_Revert,
		OperationID_Resolve,
		OperationID_Diff,
		OperationID_History,
		OperationID_CheckOutNoUI,
		OperationID_AddNoUI,
		OperationID_DeleteNoUI,
		OperationID_RenameNoUI,
		OperationID_MoveNoUI,
		OperationID_RevertNoUI,

		OperationID_ContextMenuCount, // End of the operation IDs shown in the contextual menu

		// Extra operations
		OperationID_GetFileStatus,
		OperationID_GetMissingFiles,
		OperationID_GetMovedFiles,
        OperationID_GetSubmitFiles,    
		OperationID_Login,
		OperationID_LoginCheckExpired,
		OperationID_Reopen,
		OperationID_Opened,
		OperationID_Workspaces,
		OperationID_Where,
	};

	static bool ExtractServerAndPort( LPCTSTR in_szString, CString& out_csServer, CString& out_csPort );
	static CString ServerAndPort( LPCTSTR in_szServer, LPCTSTR in_szPort );

private:
	// Operation functions called from DoOperation
	void GetLatestVersion(const StringListView& in_rFilenameList);
	void Submit(const StringListView& in_rFilenameList);
	void CheckOut(const StringListView& in_rFilenameList, bool in_bShowUI);
	void Lock(const StringListView& in_rFilenameList);
	void Unlock(const StringListView& in_rFilenameList);
	void Add(const StringListView& in_rOriginalFilenameList, bool in_bShowUI);
	AK::Wwise::ISourceControl::IOperationResult* Delete(const StringListView& in_rOriginalFilenameList, bool in_bShowUI);
	AK::Wwise::ISourceControl::IOperationResult* Rename(const StringListView& in_rOriginalFilenameList);
	AK::Wwise::ISourceControl::IOperationResult* Move(const StringListView& in_rOriginalFilenameList);
	void Revert(const StringListView& in_rFilenameList, bool in_bShowUI);
	void Resolve(const StringListView& in_rFilenameList);
	void Diff(const StringListView& in_rFilenameList);
	void History(const StringListView& in_rFilenameList);

	void Move( const CString& in_csFrom, const CString& in_csTo, FileOperationResult* io_pResult );

	// Overloaded method for GetFileStatus and DoOperation
	IOperationResult* DoOperation(DWORD in_dwOperationID, const StringListView& in_rFilenameList);
	IOperationResult* MoveNoUI(DWORD in_dwOperationID, const StringListView& in_rFilenameList, const StringListView& in_pTargetFilenameList);

	OperationResult GetFileStatus(const StringListView& in_rFilenameList, FilenameToStatusMap* out_rFileStatusMap, bool in_bSkipDirectories = false, DWORD in_dwTimeoutMs = INFINITE, bool in_bShowErrorMsgs = true,  bool in_skipDirectoryExpansion = false);

	inline void PopulateFilenameToIconMap(const StringList& in_rFilenameList, FilenameToIconMap& out_rFileIconsMap);

	// Perforce ClientUser implementation
	virtual int IsAlive() override;
	virtual void Message( Error* in_pError ) override;
	virtual void OutputError( const char* in_pErrorBuffer ) override;
	virtual void OutputInfo( char in_cLevel, const char* in_pInfoBuffer ) override;
	virtual void OutputStat( StrDict* in_pStrDict ) override;
	virtual void InputData( StrBuf* in_pBuffer, Error* in_pError ) override;
	virtual void Prompt( const StrPtr& in_rMessage, StrBuf& out_rUserResponse, int in_iNoEcho, Error* in_pError ) override;
	virtual void Diff( FileSys* in_pTheirsFile, FileSys* in_pYoursFile, int in_iDoPage, char* in_acDiffFlags, Error* in_pError ) override;
	virtual void Merge( FileSys* in_pBaseFile, FileSys* in_pTheirsFile, FileSys* in_pYoursFile, FileSys* in_pResultFile, Error* in_pError ) override;
	void RunCmd( const char* in_pCommand, const char* in_pArg1, const char* in_pArg2, const char* in_pArg3, const char* in_pArg4, 
		         const char* in_pArg5, const char* in_pPager, Error* in_pError );

	// File Status masks
	enum FileStatus
	{
		FileStatus_NotAvailable		= 0,
		FileStatus_LocalMissing		= 1 << 0,
		FileStatus_LocalOnly		= 1 << 1,
		FileStatus_OnServer			= 1 << 2,
		FileStatus_Added			= 1 << 3,
		FileStatus_Deleted			= 1 << 4,
		FileStatus_Branch			= 1 << 5,	// Renamed or Integrated
		FileStatus_Outdated			= 1 << 6,
		FileStatus_Unresolved		= 1 << 7,
		FileStatus_LocalEdit		= 1 << 8,
		FileStatus_LocalLock		= 1 << 9,
		FileStatus_OtherLock		= 1 << 10,
		FileStatus_OtherCheckout	= 1 << 11,
		FileStatus_MoveAdd			= 1 << 12,
		FileStatus_MoveDelete		= 1 << 13,
	};

	// List type
	typedef CMap<CString, LPCTSTR, FileStatus, FileStatus> FileToPerforceStatusMap;

	// Utility functions
	bool InitClient(Error* out_error);
	bool ReInitClient(Error& out_error);
	void PrepareClient(ClientApi* in_pClient);
	OperationResult RunCommand(
		const char* in_acOperation,
		std::optional<StringListView> in_pFilenameList,
		std::optional<StringListView> in_pArgumentList,
		DWORD in_dwTimeoutMs = INFINITE,
		bool in_bShowErrorMsgs = true
	);

	void ExtractWhereFiles( char* in_pInfoText );
	void PrintText( char in_cLevel, const char* in_pInfoBuffer );
	void DisplayErrorMessageBox( Error* in_pError );
	static bool IsOperationValid( OperationID in_operationID, const FileToPerforceStatusMap& in_rFileStatusMap );
	static bool IsOperationValid( OperationID in_operationID, const FileStatus& in_rFileStatus, LPCWSTR in_pszFilename, bool in_bMultiSelection );
	static bool IsOperationNoUI( OperationID in_operationID );
	static bool TestStatusCondition( FileStatus in_fileStatus, unsigned int in_uiNecessaryStatusMask, unsigned int in_uiUnacceptableStatusMask );
	bool MoveToTemporaryFolder( const CString& in_rOriginalFilename, const CString& in_rNewName, CString& out_rMovedFilename );
	ISourceControl::OperationResult UpdateWorkspaces();

	void TermClient( Error& io_error );

	static void AddEnumerationString( CString& out_rEnumeration, const CString& in_rStringToAdd );
	static void GetValidPathList( const StringListView& in_pFilenameList, StringVector& out_pFilenameList, bool in_bSkipDirectories = false, bool in_skipDirectoryExpansion = false );
	static CString EscapeWildcards( const CString& in_rPath );
	static CString GetFilename( const CString& in_rPath );
	static CString FixFileMapKey( const CString& in_rPath );

	void ResetOverridePassword();
	void SetOverridePassword(const CString& in_csOverridePassword);
	void InitClientFromRegistry(ClientApi& io_client);
	void LogError(const Error& error, const TCHAR * szContext);

	class Operation
	{
	public:
		Operation( PerforceSourceControl* in_pPerforce, OperationID in_eOperation )
			: m_pPerforce( in_pPerforce ) 
		{
			m_pPerforce->m_currentOperation.push( in_eOperation );
		}
		~Operation()
		{
			m_pPerforce->m_currentOperation.pop();
		}
	private:
		PerforceSourceControl* m_pPerforce;
	};

	template<bool fixPaths, class OperationT>
	auto DoOperation(DWORD in_operationID, const StringListView& in_filenameList, OperationT&& in_operation)
	{
		// Init parameters
		Operation operation(this, static_cast<OperationID>(in_operationID));

		if constexpr (fixPaths)
		{
			// Get a valid filename list (correct directory path)
			StringVector escapedFilenameList;
			GetValidPathList(in_filenameList, escapedFilenameList);
			return in_operation(escapedFilenameList);
		}
		else
		{
			return in_operation(in_filenameList);
		}
	}

	// Saved parameters
	AK::Wwise::ISourceControlUtilities* m_pUtilities = nullptr;
	FilenameToStatusMap* m_pFilenameToStatusMap = nullptr;
	StringList* m_pMissingFiles = nullptr;
	StringList* m_pOpenedFiles = nullptr;

	// Local information
    StringVector m_submitFileList;
	FileToPerforceStatusMap m_fileStatusMap;
	OperationResult m_operationResult = OperationResult_Succeed;
	std::stack<OperationID> m_currentOperation;
	CString m_csDescription;
	bool m_bPasswordOverride = false;
	CString m_csPassword; // override password
	bool m_bPrintTextToDialog = true;
	bool m_bDismissNextErrorMessageBox = false;
	bool m_bCancelOperation = false;
	bool m_bFileDiff = false;
	bool m_bFirstResolve = false;
	bool m_bMergeFileChanged = false;
	bool m_bOnNewFilesDoOperation = false;
	bool m_bOnNewFilesAdd = false;
	bool m_bOnNewFilesCheckOut = false;
	bool m_bUserCancelledLogin = false;
	bool m_bNeedLogin = false;
	bool m_bRetryUnicodeServer = false;

	bool m_bAutoAcceptMode = false;

	enum IconsIndexes
	{
		IconsIndexes_NotCheckOut = 0,
		IconsIndexes_CheckOut,
		IconsIndexes_Added,
		IconsIndexes_OtherCheckout,
		IconsIndexes_BothCheckout,

		IconsIndexes_Outdated_NotCheckOut,
		IconsIndexes_Outdated_CheckOut,
		IconsIndexes_Outdated_OtherCheckout,
		IconsIndexes_Outdated_BothCheckout,

		IconsIndexes_Count
	};

	std::array<HICON, IconsIndexes_Count> m_icons;

	ClientApi* m_pClient = nullptr;
	ClientApi* m_pClientTag = nullptr;

	typedef std::map<CString, CString> ClientToRootPath;
	ClientToRootPath m_workspaces;

	friend OperationDialog;
	OperationDialog* m_pOperationDialog = nullptr;
	DWORD m_dwOperationBeginTime = 0;
	DWORD m_dwOperationTimeout = 0;
};

class OperationDialog : public CDialog 
{
public:
	//virtual BOOL OnInitDialog();
	OperationDialog( PerforceSourceControl* in_pPerforce )
		: m_pPerforce( in_pPerforce )
	{
		CWnd* pWnd = ::AfxGetMainWnd();

		BOOL bCreated = __super::Create( IDD_LONG_OPERATION_DIALOG, pWnd );

		if( bCreated )
		{
			CRect rcWindow(0,0,0,0);
			if( pWnd )
				pWnd->GetWindowRect( rcWindow );

			SetWindowPos(NULL, rcWindow.left + 50, rcWindow.top + 50, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOACTIVATE );
		}
	}
	~OperationDialog()
	{
		DestroyWindow();
	}
protected:
	virtual void OnCancel()
	{
		m_pPerforce->m_bCancelOperation = true;
		__super::OnCancel();
	}
private:
	PerforceSourceControl* m_pPerforce;
};
