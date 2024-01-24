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

#ifndef _SAMPLE_PLUGIN_H
#define _SAMPLE_PLUGIN_H

#include "AK/Wwise/SourceControl/ISourceControl.h"

class SamplePlugin
	: public AK::Wwise::ISourceControl
{
public:
	SamplePlugin();
	virtual ~SamplePlugin();

	static void GetPluginInfo( PluginInfo& out_rPluginInfo );

	// ISourceControl implementation
	virtual void Init( AK::Wwise::ISourceControlUtilities* in_pUtilities, bool in_bAutoAccept );
	virtual void Term();
	virtual void Destroy();

	virtual bool ShowConfigDlg();
	virtual OperationResult GetOperationList( OperationMenuType in_menuType, const StringList& in_rFilenameList, OperationList& out_rOperationList );
	virtual LPCWSTR GetOperationName( DWORD in_dwOperationID );
	virtual DWORD GetOperationEffect( DWORD in_dwOperationID );
	virtual OperationResult GetFileStatus( const StringList& in_rFilenameList, FilenameToStatusMap& out_rFileStatusMap, DWORD in_dwTimeoutMs );
	virtual OperationResult GetFileStatusIcons( const StringList& in_rFilenameList, FilenameToIconMap& out_rFileIconsMap, DWORD in_dwTimeoutMs );
	virtual OperationResult GetFileStatusAndIcons( const StringList& in_rFilenameList, FilenameToStatusMap& out_rFileStatusMap, FilenameToIconMap& out_rFileIconsMap, DWORD in_dwTimeoutMs );
	virtual OperationResult GetMissingFilesInDirectories( const StringList& in_rDirectoryList, StringList& out_rFilenameList );
	virtual IOperationResult* DoOperation( DWORD in_dwOperationID, const StringList& in_rFilenameList, const StringList* in_pTargetFilenameList );
	virtual OperationResult PreCreateOrModify( const StringList& in_rFilenameList, CreateOrModifyOperation in_eOperation, bool& out_rContinue );
	virtual OperationResult PostCreateOrModify( const StringList& in_rFilenameList, CreateOrModifyOperation in_eOperation, bool& out_rContinue );
	virtual AK::Wwise::ISourceControl::OperationResult GetFilesForOperation( DWORD in_dwOperationID, const StringList& in_rFilenameList, StringList& out_rFilenameList, FilenameToStatusMap& out_rFileStatusMap );
	virtual AK::Wwise::ISourceControl::OperationResult CheckFilesForOperation( DWORD in_dwOperationID, const StringList& in_rFilenameList, BooleanList& out_rFileStatusList );
	virtual AK::Wwise::ISourceControl::OperationResult FilesUnderSourceControl( const StringList& in_rFilenameList, BooleanList& out_rFileStatusList );
	virtual OperationResult Connect(const IConnectParameterMap& parameterMap) override;
	virtual OperationResult Disconnect() override;

	// Operation ID enumeration
	enum OperationID
	{
		OperationID_Operation1,
		OperationID_Operation2
	};

private:

	// Local operations
	void DoOperation1( const StringList& in_rFilenameList );
	void DoOperation2( const StringList& in_rFilenameList );

	// Local members
	AK::Wwise::ISourceControlUtilities* m_pSourceControlUtilities;
	HICON* m_icons;
};

#endif // _SAMPLE_PLUGIN_H