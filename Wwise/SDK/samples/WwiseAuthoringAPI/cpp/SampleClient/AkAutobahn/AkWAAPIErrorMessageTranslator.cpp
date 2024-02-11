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
#include <AK/WwiseAuthoringAPI/AkAutobahn/AkWAAPIErrorMessageTranslator.h>
#include <AK/WwiseAuthoringAPI/waapi.h>
#ifndef AK_OPTIMIZED

#define TEMP_BUF_SIZE 2000

//Currently the queries are not used for the most part
//Keeping the currently unused one in case WAAPI support them one day (global tag)
namespace AK
{
	namespace WaapiTranslator
	{
		enum waqlQuery
		{
			AcousticTexture = 0,
			Action,
			ActionException,
			ActorMixer,
			Attenuation,
			AudioDevice,
			AudioSource,
			AuxBus,
			BlendContainer,
			BlendTrack,
			Bus,
			ControlSurfaceBinding,
			ControlSurfaceBindingGroup,
			ControlSurfaceSession,
			Conversion,
			Curve,
			CustomState,
			DialogueEvent,
			Effect,
			Event,
			ExternalSource,
			ExternalSourceFile,
			Folder,
			GameParameter,
			Language,
			Metadata,
			MidiParameter,
			MixingSession,
			Modifier,
			ModulatorEnvelope,
			ModulatorLfo,
			ModulatorTime,
			MultiSwitchEntry,
			MusicClip,
			MusicClipMidi,
			MusicCue,
			MusicEventCue,
			MusicFade,
			MusicPlaylistContainer,
			MusicPlaylistItem,
			MusicSegment,
			MusicStinger,
			MusicSwitchContainer,
			MusicTrack,
			MusicTrackSequence,
			MusicTransition,
			ObjectSettingAssoc,
			Panner,
			ParamControl,
			Path,
			Platform,
			PluginDataSource,
			Position,
			Project,
			Query,
			RandomSequenceContainer,
			SearchCriteria,
			Sound,
			SoundBank,
			SoundcasterSession,
			State,
			StateGroup,
			Switch,
			SwitchContainer,
			SwitchGroup,
			Trigger,
			UserProjectSettings,
			WorkUnit,
			Global,

			// ALWAYS ADD NEW CODES AT THE END !!!!!!!
			// Otherwise it may break comm compatibility in a patch

			Num_WAQL_Query // THIS STAYS AT END OF ENUM
		};
	}
}



const char* s_waqlQuery[AK::WaapiTranslator::waqlQuery::Num_WAQL_Query] =
{
	"$ from object \"AcousticTexture:%u\"",
	"$ from object \"Action:%u\"",
	"$ from object \"ActionException:%u\"",
	"$ from object \"ActorMixer:%u\"",
	"$ from object \"Attenuation:%u\"",
	"$ from object \"AudioDevice:%u\"",
	"$ from object \"AudioSource:%u\"",
	"$ from object \"AuxBus:%u\"",
	"$ from object \"BlendContainer:%u\"",
	"$ from object \"BlendTrack:%u\"",
	"$ from object \"Bus:%u\"",
	"$ from object \"ControlSurfaceBinding:%u\"",
	"$ from object \"ControlSurfaceBindingGroup:%u\"",
	"$ from object \"ControlSurfaceSession:%u\"",
	"$ from object \"Conversion:%u\"",
	"$ from object \"Curve:%u\"",
	"$ from object \"CustomState:%u\"",
	"$ from object \"DialogueEvent:%u\"",
	"$ from object \"Effect:%u\"",
	"$ from object \"Event:%u\"",
	"$ from object \"ExternalSource:%u\"",
	"$ from object \"ExternalSourceFile:%u\"",
	"$ from object \"Folder:%u\"",
	"$ from object \"GameParameter:%u\"",
	"$ from object \"Language:%u\"",
	"$ from object \"Metadata:%u\"",
	"$ from object \"MidiParameter:%u\"",
	"$ from object \"MixingSession:%u\"",
	"$ from object \"Modifier:%u\"",
	"$ from object \"ModulatorEnvelope:%u\"",
	"$ from object \"ModulatorLfo:%u\"",
	"$ from object \"ModulatorTime:%u\"",
	"$ from object \"MultiSwitchEntry:%u\"",
	"$ from object \"MusicClip:%u\"",
	"$ from object \"MusicClipMidi:%u\"",
	"$ from object \"MusicCue:%u\"",
	"$ from object \"MusicEventCue:%u\"",
	"$ from object \"MusicFade:%u\"",
	"$ from object \"MusicPlaylistContainer:%u\"",
	"$ from object \"MusicPlaylistItem:%u\"",
	"$ from object \"MusicSegment:%u\"",
	"$ from object \"MusicStinger:%u\"",
	"$ from object \"MusicSwitchContainer:%u\"",
	"$ from object \"MusicTrack:%u\"",
	"$ from object \"MusicTrackSequence:%u\"",
	"$ from object \"MusicTransition:%u\"",
	"$ from object \"ObjectSettingAssoc:%u\"",
	"$ from object \"Panner:%u\"",
	"$ from object \"ParamControl:%u\"",
	"$ from object \"Path:%u\"",
	"$ from object \"Platform:%u\"",
	"$ from object \"PluginDataSource:%u\"",
	"$ from object \"Position:%u\"",
	"$ from object \"Project:%u\"",
	"$ from object \"Query:%u\"",
	"$ from object \"RandomSequenceContainer:%u\"",
	"$ from object \"SearchCriteria:%u\"",
	"$ from object \"Sound:%u\"",
	"$ from object \"SoundBank:%u\"",
	"$ from object \"SoundcasterSession:%u\"",
	"$ from object \"State:%u\"",
	"$ from object \"StateGroup:%u\"",
	"$ from object \"Switch:%u\"",
	"$ from object \"SwitchContainer:%u\"",
	"$ from object \"SwitchGroup:%u\"",
	"$ from object \"Trigger:%u\"",
	"$ from object \"UserProjectSettings:%u\"",
	"$ from object \"WorkUnit:%u\"",
	"$ from object \"Global:%u\"",

};


AkWAAPIErrorMessageTranslator::AkWAAPIErrorMessageTranslator(const char* connectionIP, const AkUInt32 in_port, AkUInt32 in_msTimeout) : AkErrorMessageTranslator()
{
	SetConnectionIP(connectionIP, in_port, in_msTimeout);
	m_bConnectFailed = false;
}

AkWAAPIErrorMessageTranslator::~AkWAAPIErrorMessageTranslator()
{
	Term();
}

void AkWAAPIErrorMessageTranslator::Term()
{
	if (m_isReadyForTranslation && !m_bConnectFailed)
		m_Client.Disconnect();

	m_isReadyForTranslation = false;
}

AKRESULT AkWAAPIErrorMessageTranslator::Setup()
{
	if (!m_isReadyForTranslation && m_connectionSettingChanged)
	{
		m_bConnectFailed = !m_Client.Connect(m_connectionIP, m_port);
		m_isReadyForTranslation = !m_bConnectFailed;
		m_connectionSettingChanged = false;
	}
	return m_isReadyForTranslation ? AK_Success : AK_Fail;
}

bool AkWAAPIErrorMessageTranslator::GetInfo(TagInformation* in_pTagList, AkUInt32 in_uCount, AkUInt32& io_uTranslated)
{
	if (m_msTimeout == 0)
		return in_uCount == io_uTranslated;

	//Check if we can translate anything, before connecting.
	//Of all the tags, only the 'm' tag isn't supported by WAAPI
	AkUInt32 i = 0;
	for (i = 0; i < in_uCount; ++i)
	{
		if (!in_pTagList[i].m_infoIsParsed && *in_pTagList[i].m_pTag != 'm'  && *in_pTagList[i].m_pTag != 'g')
			break;
	}

	if (i == in_uCount)	//No translatable tags
		return in_uCount == io_uTranslated;

	if (!m_isReadyForTranslation && !m_bConnectFailed)
	{
		//Only try to connect once. If it failed, don't try anymore.
		Setup();
	}

	if (!m_isReadyForTranslation)
		return in_uCount == io_uTranslated;

	AkInt64 timeStart, timeStop;
	AKPLATFORM::PerformanceCounter(&timeStart);
	timeStop = timeStart;

	for (AkUInt32 i = 0; i < in_uCount && AKPLATFORM::Elapsed(timeStop, timeStart) < m_msTimeout; ++i)
	{
		if (!in_pTagList[i].m_infoIsParsed)
		{
			switch (*in_pTagList[i].m_pTag)
			{
			case 'b': //Bank ID		
			{
				AkUInt32 arg = *(AkUInt32*)(in_pTagList[i].m_args);
				if (WAAPIGet(s_waqlQuery[AK::WaapiTranslator::waqlQuery::SoundBank], in_pTagList[i], arg))
					++io_uTranslated;
				break;
			}
			case 'p': //Plugin ID
			{
				AkUInt32 pluginID = *(AkUInt32*)(in_pTagList[i].m_args);
				if (WAAPIGetFromTypes(in_pTagList[i], pluginID))
					++io_uTranslated;
				break;
			}
			case 's': //SwitchStates ID		
			{
				AkUInt32 arg = *(AkUInt32*)(in_pTagList[i].m_args);
				if (WAAPIGet(s_waqlQuery[AK::WaapiTranslator::waqlQuery::SwitchGroup], in_pTagList[i], arg))
					++io_uTranslated;
				else if (WAAPIGet(s_waqlQuery[AK::WaapiTranslator::waqlQuery::StateGroup], in_pTagList[i], arg))
					++io_uTranslated;
				break;
			}
			/*case 'g': //GameObject ID	(Not supported yet)
			{
				AkUInt64 arg = AK::ReadUnaligned<AkUInt64>((AkUInt8*)in_varArgPtr);
				if (WAAPIGetFromGameObject(in_pTagList[i], arg))
					++io_uTranslated;
				break;
			}*/
			case 'w': //WwiseObject ID		
			{
				AkUInt32 arg = *(AkUInt32*)(in_pTagList[i].m_args);
				if (WAAPIGet(s_waqlQuery[AK::WaapiTranslator::waqlQuery::Global], in_pTagList[i], arg))
					++io_uTranslated;
				break;
			}
			default:
				AKASSERT("Parameter type not supported\n");
			case 'g':	//GameObject, not supported yet in the WAAPI handler in Authoring.
			case 'm': //Media ID					
				//Nothing for now
				break;
			}
		}

		AKPLATFORM::PerformanceCounter(&timeStop);
	}
	
	return (io_uTranslated == in_uCount);
}

void AkWAAPIErrorMessageTranslator::SetConnectionIP(const char* in_ConnectionIP, AkUInt32 in_port, AkUInt32 in_msTimeout)
{	
	m_port = in_port;
	size_t filePathLength = strlen(in_ConnectionIP) + 1;	//+1 for the '/0'
	AKPLATFORM::SafeStrCpy(m_connectionIP, in_ConnectionIP, filePathLength);
	m_connectionSettingChanged = true;
	m_msTimeout = in_msTimeout;
}


bool AkWAAPIErrorMessageTranslator::WAAPIGet(const char* in_waqlQuery, TagInformation& io_Tag, AkUInt32 in_arg)
{
	AK::WwiseAuthoringAPI::AkJson queryResult;
	char parsedQuery[TEMP_BUF_SIZE];
	snprintf(parsedQuery, TEMP_BUF_SIZE, in_waqlQuery, in_arg);

	AK::WwiseAuthoringAPI::AkJson args(AK::WwiseAuthoringAPI::AkJson::Map{
		{ "waql", AK::WwiseAuthoringAPI::AkVariant(parsedQuery)}
		});

	AK::WwiseAuthoringAPI::AkJson options(AK::WwiseAuthoringAPI::AkJson::Map{
		{ "return", AK::WwiseAuthoringAPI::AkJson::Array{ AK::WwiseAuthoringAPI::AkVariant("id"), AK::WwiseAuthoringAPI::AkVariant("name"), AK::WwiseAuthoringAPI::AkVariant("category"), AK::WwiseAuthoringAPI::AkVariant("shortId") , AK::WwiseAuthoringAPI::AkVariant("type")} }
		});

	if (!m_Client.Call(ak::wwise::core::object::get, args, options, queryResult))
		return false;

	CharPrintResult(io_Tag.m_parsedInfo, AK_TRANSLATOR_MAX_NAME_SIZE, ((AkInt32)queryResult["return"][0]["name"].GetVariant().GetString().size()) + 1, queryResult["return"][0]["name"].GetVariant().GetString().c_str());
	io_Tag.m_infoIsParsed = true;
	return true;
}

bool AkWAAPIErrorMessageTranslator::WAAPIGetFromTypes(TagInformation& io_Tag, AkUInt32 in_typeId)
{
	AK::WwiseAuthoringAPI::AkJson queryResult;
	AK::WwiseAuthoringAPI::AkJson args(AK::WwiseAuthoringAPI::AkJson::Map{});
	AK::WwiseAuthoringAPI::AkJson options(AK::WwiseAuthoringAPI::AkJson::Map{});

	m_Client.Call(ak::wwise::core::object::getTypes, args, options, queryResult);

	AkInt32 arraySize = (AkInt32) queryResult["return"].GetArray().size();
	for (AkInt32 i = 0; i < arraySize; ++i)
	{
		AkUInt32 classID = (AkUInt32)queryResult["return"][i]["classId"].GetVariant().GetInt32();
		if (classID == in_typeId)
		{
			//Plugin type name
			CharPrintResult(io_Tag.m_parsedInfo, AK_TRANSLATOR_MAX_NAME_SIZE, ((AkInt32)queryResult["return"][i]["name"].GetVariant().GetString().size())+1, queryResult["return"][i]["name"].GetVariant().GetString().c_str());
			io_Tag.m_infoIsParsed = true;
			return true;
		}
	}
	return false;
}
bool AkWAAPIErrorMessageTranslator::WAAPIGetFromGameObject(TagInformation& io_Tag, AkUInt64 in_gId)
{
	AK::WwiseAuthoringAPI::AkJson queryResult;

	AK::WwiseAuthoringAPI::AkJson args(AK::WwiseAuthoringAPI::AkJson::Map{
	{ "time", AK::WwiseAuthoringAPI::AkVariant("capture")}	//Capture is the only call available atm that doesn't require to send a specific time. A jira was created for this : WG-58412
		});
	AK::WwiseAuthoringAPI::AkJson options(AK::WwiseAuthoringAPI::AkJson::Map{
		});

	if(!m_Client.Call(ak::wwise::core::profiler::getGameObjects, args, options, queryResult))
		return false;

	AkInt32 arraySize = (AkInt32)queryResult["return"].GetArray().size();
	for (AkInt32 i = 0; i < arraySize; ++i)
	{
		AkUInt64 objId = queryResult["return"][i]["id"].GetVariant().GetUInt64();
		if (objId == in_gId)
		{
			CharPrintResult(io_Tag.m_parsedInfo, AK_TRANSLATOR_MAX_NAME_SIZE, ((AkInt32)queryResult["return"][i]["name"].GetVariant().GetString().size()) + 1, queryResult["return"][i]["name"].GetVariant().GetString().c_str());
			io_Tag.m_infoIsParsed = true;
			return true;
		}
	}
	return false;
}
#endif