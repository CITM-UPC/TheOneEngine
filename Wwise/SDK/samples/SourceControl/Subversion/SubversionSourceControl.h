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

#include "SourceControlHelpers.h"

#include "SubversionPool.h"
#include "svn_client.h"

#include <vector>

class FileOperationResult;

class SubversionSourceControl
	: public AK::Wwise::ISourceControl
{
public:
	SubversionSourceControl();
	virtual ~SubversionSourceControl();

	static void GetPluginInfo( PluginInfo& out_rPluginInfo );

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

	enum OperationID
	{
		OperationID_Update = 0,
		OperationID_Commit,
		OperationID_GetLock,
		OperationID_ReleaseLock,
		OperationID_Add,
		OperationID_Delete,
		OperationID_Rename,
		OperationID_Move,
		OperationID_Revert,
		OperationID_Resolved,
		OperationID_Diff,
		OperationID_ShowLog,
		OperationID_AddNoUI,
		OperationID_DeleteNoUI,
		OperationID_RenameNoUI,
		OperationID_MoveNoUI,
		OperationID_RevertNoUI,

		OperationIDCount

	};

private:
	// Operation functions called from DoOperation
	IOperationResult* DoOperation(DWORD in_dwOperationID, const StringListView& in_rFilenameList);
	void Update(const StringListView& in_rFilenameList);
	void Commit(const StringListView& in_rFilenameList);
	void GetLock(const StringListView& in_rFilenameList);
	void ReleaseLock(const StringListView& in_rFilenameList);
	void Add(const StringListView& in_rFilenameList, bool in_bShowUI);
	AK::Wwise::ISourceControl::IOperationResult* Delete(const StringListView& in_rFilenameList, bool in_bShowUI);
	AK::Wwise::ISourceControl::IOperationResult* Rename(const StringListView& in_rFilenameList);
	AK::Wwise::ISourceControl::IOperationResult* Move(const StringListView& in_rFilenameList);
	void Revert(const StringListView& in_rFilenameList, bool in_bShowUI);
	void Resolved(const StringListView& in_rFilenameList);
	void Diff(const StringListView& in_rFilenameList);
	void ShowLog(const StringListView& in_rFilenameList);

	void Move( const CString& in_csFrom, const CString& in_csTo, FileOperationResult* io_pResult );
	IOperationResult* MoveNoUI( DWORD in_dwOperationID, const StringListView& in_rFilenameList, const StringListView& in_pTargetFilenameList );

	struct SvnStatus
	{
		svn_wc_status_kind m_kind;
		bool m_bLocked;
		bool m_bTreeConflict;
		CString m_csLockOwner;
	};

	using SvnStatusList = std::vector<SvnStatus>;

	enum SVNErrorDestination
	{
		SVNErrorDestination_Progress,
		SVNErrorDestination_MessageBox
	};

	virtual OperationResult _GetFileStatus( const StringList& in_rFilenameList, FilenameToStatusMap* out_pFileStatusMap, DWORD in_dwTimeoutMs, SvnStatusList* out_pStatuses );

	// Utility functions
	void ManageSVNError( svn_error_t* in_pError, SVNErrorDestination in_destination = SVNErrorDestination_Progress, bool in_bClearError = true );
	bool IsOperationValid( OperationID in_operationID, const SvnStatusList& in_rStatusList, bool in_bMultipleSelection );
	bool IsOperationValid( OperationID in_operationID, const SvnStatus& in_rStatus, bool in_bMultipleSelection );
	void RunCommand( const CString& in_rCommand );

	static CString GetSVNDate( const apr_time_t& in_rAprTime );
	static CString GetSVNStatusKindText( svn_wc_status_kind in_status );
	static CString GetPathFromSVN( const char* in_pSVNPath, apr_pool_t* in_pPool );
	static const char* GetSVNPath( const CString& in_rPath, apr_pool_t* in_pPool );
	static apr_array_header_t* GetSVNPathArray( const StringListView& in_rPathList, apr_pool_t* in_pPool );
	static svn_error_t* GetSVNFileStatus( const CString& in_csFilename, SvnStatus& out_rStatus, svn_client_ctx_t* in_pContext, apr_pool_t* in_pPool );

	// Subversion Context callbacks
	static svn_error_t* CancelCallback( AK::Wwise::ISourceControlUtilities* in_pUtilities );
	static svn_error_t* LogMessageCallback( const char** in_pLogMsg, const char** in_pTmpFile, const apr_array_header_t* in_pArrayHeader, const char* in_pMessage, apr_pool_t* in_pPool );
	static void NotifyCallback( SubversionSourceControl* in_pSourceControl, const svn_wc_notify_t* in_pNotifyInfo, apr_pool_t* in_pPool );

	// Operations callbacks
	static svn_error_t * GetFileStatusCallback(SvnStatus* io_pStatus, const char * path, const svn_client_status_t * status, apr_pool_t * /*pool*/);
	static svn_error_t * GetMissingFilesCallback(void * baton, const char * path, const svn_client_status_t * status, apr_pool_t * /*pool*/);
	static svn_error_t* ShowLogCallback( AK::Wwise::ISourceControlUtilities* in_pUtilities, apr_hash_t* in_paths, svn_revnum_t in_revision, const char* in_pAuthor, const char* in_pDate, const char* in_pMessage, apr_pool_t* in_pPool );
	static svn_error_t* SslServerPromptCallback(svn_auth_cred_ssl_server_trust_t **cred_p, void *baton, const char *realm, apr_uint32_t failures, const svn_auth_ssl_server_cert_info_t *cert_info, svn_boolean_t may_save, apr_pool_t *pool);

	// Members
	apr_pool_t* m_pGlobalPool;
	svn_client_ctx_t* m_pContext;
	bool m_bAutoAcceptMode;

	HICON* m_icons;
	bool m_bOnNewFilesAdd;
	StringVector m_filesToAdd;
	CString m_csDiffToolPath;
	bool m_bUseAKWavViewerToDiff;
	
	AK::Wwise::ISourceControlUtilities* m_pUtilities;
};