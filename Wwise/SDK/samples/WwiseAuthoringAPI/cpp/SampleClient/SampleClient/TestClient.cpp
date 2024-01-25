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

#include "TestClient.h"

// These AK header are not used at all. They are simply here to test that we can compile
// both the Wwise SDK and AkAutobahn in the same compilation unit without conflicts.
#include <AK/Tools/Common/AkAllocator.h>
#include <AK/Tools/Common/AkArray.h>
#include <AK/Tools/Common/AkAssert.h>
#include <AK/Tools/Common/AkAutoLock.h>
#include <AK/Tools/Common/AkBankReadHelpers.h>
#include <AK/Tools/Common/AkDynaBlkPool.h>
#include <AK/Tools/Common/AkFNVHash.h>
#include <AK/Tools/Common/AkHashList.h>
#include <AK/Tools/Common/AkKeyArray.h>
#include <AK/Tools/Common/AkKeyDef.h>
#include <AK/Tools/Common/AkListBare.h>
#include <AK/Tools/Common/AkListBareLight.h>
#include <AK/Tools/Common/AkLock.h>
#include <AK/Tools/Common/AkMonitorError.h>
#include <AK/Tools/Common/AkObject.h>
#include <AK/Tools/Common/AkPlatformFuncs.h>
#include <AK/Tools/Common/AkSet.h>
#include <AK/Tools/Common/AkSmartPtr.h>
#include <AK/Tools/Common/AkString.h>
#include <AK/Tools/Common/AkSyncCaller.h>

#include "AK/WwiseAuthoringAPI/AkAutobahn/civetweb.h"
#include <AK/WwiseAuthoringAPI/AkAutobahn/Client.h>
#include <AK/WwiseAuthoringAPI/AkAutobahn/FutureUtils.h>
#include <AK/WwiseAuthoringAPI/AkAutobahn/Logger.h>
#include "RapidJsonUtils.h"

#include <AK/WwiseAuthoringAPI/waapi.h>

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <cassert>
#include <codecvt>

#if defined( AK_ENABLE_ASSERTS )

void SampleAssertHook(const char * in_pszExpression, const char * in_pszFileName, int in_lineNumber)
{
    using namespace AK::WwiseAuthoringAPI;
    std::ostringstream os;

    os << "ASSERT: " << in_pszExpression << " " << in_pszFileName << " " << in_lineNumber;

    Logger::Get()->LogMessage("AkAutobahn", os.str().c_str());
    assert(false);
}

AkAssertHook g_pAssertHook = SampleAssertHook;

#endif

static void logCout(const char* log)
{
	std::cout << log << std::endl;
}

void HelloWorld()
{
	using namespace AK::WwiseAuthoringAPI;

	// Enable this for debugging purposes.
	//Logger::Get()->SetLoggerFunction(logCout);

	Client client;

	// Connect to Wwise Authoring on localhost.

	if (!client.Connect("127.0.0.1", 8080))
	{
		std::cout << "Could not connect to Wwise Authoring on localhost." << std::endl;
		return;
	}

	// Obtain the wwise info.
	AkJson wwiseInfoJson;
	if (!client.Call(ak::wwise::core::getInfo, AkJson(AkJson::Type::Map), AkJson(AkJson::Type::Map), wwiseInfoJson, 10))
	{
		std::cout << "Failed to obtain Wwise Info within 10ms: " << std::string(wwiseInfoJson["message"].GetVariant()) << std::endl;
		return;
	}
	
	// Output some information, including converting from UTF8 for proper display.
	std::ostringstream os;
	os << wwiseInfoJson["displayName"].GetVariant().GetString() << " " << wwiseInfoJson["version"]["displayName"].GetVariant().GetString() << "\n";
	os << wwiseInfoJson["copyright"].GetVariant().GetString() << "\n";
	os << wwiseInfoJson["branch"].GetVariant().GetString() << "\n\n";

#if defined (_WIN32) || defined (_WIN64)
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring wstr = converter.from_bytes(os.str());
	std::wcout << wstr;
#else
	std::cout << os.str();
#endif
}

void TestWampClient()
{
	using namespace AK::WwiseAuthoringAPI;

	// Enable this for debugging purposes.
	//Logger::Get()->SetLoggerFunction(logCout);

	{
		Client client;

		if (!client.Connect("127.0.0.1", 8080))
		{
			std::cout << "Please make sure Wwise Authoring is running and try again." << std::endl;
			return;
		}
		
		uint64_t idObjectCreatedSubscription = 0;
		std::future<bool> objectCreatedFuture;
		std::promise<bool> objectCreatedPromise;
		std::atomic_bool objectCreatedCalled;

		{
			objectCreatedCalled = false;
			objectCreatedFuture = objectCreatedPromise.get_future();

			// This assumes that we block the thread until events have been received, and then unsubscribe before going out of scope.
			auto objectCreated = [&objectCreatedCalled, &objectCreatedPromise](uint64_t, const JsonProvider& in_rJson)
			{
				// We can't assume exclusive usage of the server, therefore we can't assume events will be fired only once.
				// Let's make sure we don't execute this more than once.
				if (!objectCreatedCalled)
				{
					const char* szName = in_rJson.GetAkJson()["object"]["id"].GetVariant().GetString().c_str();
					std::cout << "Received object notification. Object id is " << szName << std::endl;
					objectCreatedCalled = true;
					objectCreatedPromise.set_value(true);
				}
			};

			// Subscribe to object created notification
			AkJson subscribeResult;
			client.Subscribe(ak::wwise::core::object::created, AkJson(AkJson::Type::Map), objectCreated, idObjectCreatedSubscription, subscribeResult);
		}

		AkJson queryResult;

		{
			// Get the default work unit for actor-mixer
			AkJson args(AkJson::Map{
				{ "from", AkJson::Map{
					{ "path", AkJson::Array{ AkVariant("\\Actor-Mixer Hierarchy\\Default Work Unit") } } } }
				});

			AkJson options(AkJson::Map{ 
				{ "return", AkJson::Array{ AkVariant("id"), AkVariant("name"), AkVariant("category") } }
			});

			if (!client.Call(ak::wwise::core::object::get, args, options, queryResult))
			{
				std::cout << "Failed to get the default actor mixer work unit" << std::endl;
				return;
			}
		}

		AkJson createResult;

		{
			// Get the query results
			const auto& firstObject = queryResult["return"].GetArray()[0];
			const auto& parentId = firstObject["id"];

			// create an object
			AkJson args(AkJson::Map{
				{ "parent", parentId },
				{ "type", AkVariant("Sound") },
				{ "name", AkVariant("Boom") },
				{ "onNameConflict", AkVariant("rename") },
			});

			if (!client.Call(ak::wwise::core::object::create, args, AkJson(AkJson::Type::Map), createResult))
			{
				std::cout << "Failed to create an object" << std::endl;
				return;
			}
		}

		// Retreive the object id created
		const auto& createId = createResult["id"];
		uint64_t idPropertyChangedSubscription = 0;
		std::promise<bool> propertyChangedPromise;
		std::future<bool> propertyChangedFuture;
		std::atomic_bool propertyChangedCalled;

		{
			// Subscribe to property changed.
			AkJson options(AkJson::Map
			{
				{ "object", createId.GetVariant() },
				{ "property", AkVariant("Volume") },
				{ "return", AkJson::Array{
					AkVariant("id")
				} }
			});
			
			propertyChangedCalled = false;
			propertyChangedFuture = propertyChangedPromise.get_future();

			// This assumes that we block the thread until events have been received, and then unsubscribe before going out of scope.
			auto onPropertyChanged = [&propertyChangedCalled, &propertyChangedPromise](uint64_t subscriptionId, const JsonProvider&)
			{
				// We can't assume exclusive usage of the server, therefore we can't assume events will be fired only once.
				// Let's make sure we don't execute this more than once.
				if (!propertyChangedCalled)
				{
					std::cout << "Received property changed event. Subscription id is " << subscriptionId << std::endl;
					propertyChangedCalled = true;
					propertyChangedPromise.set_value(true);
				}
			};

			AkJson subscribeResult;
			if (!client.Subscribe(ak::wwise::core::object::propertyChanged, options, onPropertyChanged, idPropertyChangedSubscription, subscribeResult))
			{
				std::cout << "Failed to subscribe to propertyChanged." << std::endl;
				return;
			}
			else
			{
				std::cout << "Subscribed to propertyChanged. Subscription id is " << idPropertyChangedSubscription << std::endl;
			}
		}

		{
			// Set the property.
			AkJson args(AkJson::Map{
				{ "object", createId.GetVariant() },
				{ "platform", AkVariant("{00000000-0000-0000-0000-000000000000}") },
				{ "property", AkVariant("Volume") },
				{ "value", AkVariant(0.1) },
			});

			AkJson result;
			if (!client.Call(ak::wwise::core::object::setProperty, args, AkJson(AkJson::Type::Map), result))
			{
				std::cout << "Failed to create an object" << std::endl;
				return;
			}
		}
		
		// Block here until both events have been received.
		propertyChangedFuture.get();
		objectCreatedFuture.get();

		AkJson unsubscribeResult;
		client.Unsubscribe(idPropertyChangedSubscription, unsubscribeResult);
		client.Unsubscribe(idObjectCreatedSubscription, unsubscribeResult);
	}

	// Connection closes when the client goes out of scope.

	std::cout << "Done!" << std::endl;
}

void TestWampClientJsonString()
{
	using namespace AK::WwiseAuthoringAPI;

	// Enable this for debugging purposes.
	//Logger::Get()->SetLoggerFunction(logCout);

	{
		Client client;

		if (!client.Connect("127.0.0.1", 8080))
		{
			std::cout << "Please make sure Wwise Authoring is running and try again." << std::endl;
			return;
		}

		uint64_t idObjectCreatedSubscription = 0;
		std::future<bool> objectCreatedFuture;
		std::promise<bool> objectCreatedPromise;
		std::atomic_bool objectCreatedCalled;

		{
			objectCreatedCalled = false;
			objectCreatedFuture = objectCreatedPromise.get_future();

			// This assumes that we block the thread until events have been received, and then unsubscribe before going out of scope.
			auto objectCreated = [&objectCreatedCalled, &objectCreatedPromise](uint64_t, const JsonProvider& in_rJson)
			{
				// We can't assume exclusive usage of the server, therefore we can't assume events will be fired only once.
				// Let's make sure we don't execute this more than once.
				if (!objectCreatedCalled)
				{
					std::cout << "Received object notification. JSON string is " << in_rJson.GetJsonString() << std::endl;
					objectCreatedCalled = true;
					objectCreatedPromise.set_value(true);
				}
			};

			// Subscribe to object created notification
			AkJson subscribeResult;
			client.Subscribe(ak::wwise::core::object::created, AkJson(AkJson::Type::Map), objectCreated, idObjectCreatedSubscription, subscribeResult);
		}

		AkJson queryResult;

		{
			std::string queryResultString;

			// Get the default work unit for actor-mixer
			std::string args = "\n{\"from\":\n\n{\"path\":\n[\t\"\\\\Actor-Mixer Hierarchy\\\\Default Work Unit\"\r]\r\n}}\t\r\n";

			std::string options = "{\n\"return\"\n:\n\n[\t\"id\",\"name\",\"category\"]\t}\r";

			if (!client.Call(ak::wwise::core::object::get, args.c_str(), options.c_str(), queryResultString))
			{
				std::cout << "Failed to get the default actor mixer work unit" << std::endl;
				return;
			}

			rapidjson::Document doc;
			if (doc.Parse(queryResultString.c_str()).HasParseError() || !RapidJsonUtils::FromRapidJson(doc, queryResult))
			{
				std::cout << "Failed to parse result JSON string." << std::endl;
				return;
			}
		}
		
		AkJson createResult;

		{
			std::string createResultString;

			// Get the query results
			const auto& firstObject = queryResult["return"].GetArray()[0];
			const auto& parentId = firstObject["id"];

			std::string args = "{\"name\":\"Boom\",\"onNameConflict\":\"rename\",\"parent\":\"" + parentId.GetVariant().GetString() + "\",\"type\":\"Sound\"}";

			if (!client.Call(ak::wwise::core::object::create, args.c_str(), "{}", createResultString))
			{
				std::cout << "Failed to create an object" << std::endl;
				return;
			}

			rapidjson::Document doc;
			if (doc.Parse(createResultString.c_str()).HasParseError() || !RapidJsonUtils::FromRapidJson(doc, createResult))
			{
				std::cout << "Failed to parse result JSON string." << std::endl;
				return;
			}
		}
		
		// Retreive the object id created
		const auto& createId = createResult["id"];
		uint64_t idPropertyChangedSubscription = 0;
		std::promise<bool> propertyChangedPromise;
		std::future<bool> propertyChangedFuture;
		std::atomic_bool propertyChangedCalled;
		
		{
			// Subscribe to property changed.
			std::string options = "{\"object\":\"" + std::string(createId.GetVariant()) + "\",\"property\":\"Volume\",\"return\":[\"id\"]}";

			propertyChangedCalled = false;
			propertyChangedFuture = propertyChangedPromise.get_future();
			
			// This assumes that we block the thread until events have been received, and then unsubscribe before going out of scope.
			auto onPropertyChanged = [&propertyChangedCalled, &propertyChangedPromise](uint64_t subscriptionId, const JsonProvider&)
			{
				// We can't assume exclusive usage of the server, therefore we can't assume events will be fired only once.
				// Let's make sure we don't execute this more than once.
				if (!propertyChangedCalled)
				{
					std::cout << "Received property changed event. Subscription id is " << subscriptionId << std::endl;
					propertyChangedCalled = true;
					propertyChangedPromise.set_value(true);
				}
			};

			std::string subscribeResult;
			if (!client.Subscribe(ak::wwise::core::object::propertyChanged, options.c_str(), onPropertyChanged, idPropertyChangedSubscription, subscribeResult))
			{
				std::cout << "Failed to subscribe to propertyChanged" << std::endl;
				return;
			}
			else
			{
				std::cout << "Subscribed to propertyChanged. Subscription id is " << idPropertyChangedSubscription << std::endl;
			}
		}

		{
			// Set the property.
			std::string args = "{\"object\":\"" + std::string(createId.GetVariant()) + "\",\"platform\":\"{00000000-0000-0000-0000-000000000000}\",\"property\":\"Volume\",\"value\":0.1}";

			std::string result;
			if (!client.Call(ak::wwise::core::object::setProperty, args.c_str(), "{}", result))
			{
				std::cout << "Failed to create an object" << std::endl;
				return;
			}
		}

		// Block here until both events have been received.
		propertyChangedFuture.get();
		objectCreatedFuture.get();

		AkJson unsubscribeResult;
		client.Unsubscribe(idPropertyChangedSubscription, unsubscribeResult);
		client.Unsubscribe(idObjectCreatedSubscription, unsubscribeResult);

		std::cout << "Disconnecting." << std::endl;
		client.Disconnect();

		std::cout << "Attempting to use a disconnected client." << std::endl;

		std::string result;
		if (!client.Call(ak::wwise::core::object::setProperty, "{}", "{}", result))
		{
			std::cout << "Failed as expected." << std::endl;
		}
		else
		{
			std::cout << "Call should not have succeeded." << std::endl;
		}
	}

	// Connection closes when the client goes out of scope.

	std::cout << "Done!" << std::endl;
}


//
//	This code registers an empty ak.sampleclientcommand command, that you can find in the Extra menu of Authoring.
//	Then, it subscribes to the commands being executed, and executes the command.
//	Once it gets the mention the command has been executed, it unregisters the command.
//
//	If everything goes correctly, you should not (or briefly) see the Extra menu appear. You can also put a breakpoint before
//	calling the command and see the menu appearing.
//
void TestCustomCommands()
{
	using namespace AK::WwiseAuthoringAPI;

	// Enable this for debugging purposes.
	// Logger::Get()->SetLoggerFunction(logCout);

	{
		Client client;

		if (!client.Connect("127.0.0.1", 8080))
		{
			std::cout << "Please make sure Wwise Authoring is running and try again." << std::endl;
			return;
		}

		{
			std::string callResultString;

			// Register a sample command
			std::string args = "{\n\"commands\":[\n{\"id\":\"ak.sampleclientcommand\",\n\"displayName\" : \"Sample Client Command\",\n\"mainMenu\" : {\n\"basePath\":\"Extra\"\n}\n}\n]\n}";
			std::string options = "{}";

			if (!client.Call(ak::wwise::ui::commands::register_, args.c_str(), options.c_str(), callResultString))
			{
				std::cout << "Failed to register the sample command" << std::endl;
				return;
			}
		}

		// Wait for our command execution
		std::promise<bool> executedPromise;
		std::future<bool> executedFuture;
		std::atomic_bool executedCalled;

		uint64_t commandExecutedSubscriptionId = 0;
		{
			AkJson options(AkJson::Map{});

			executedCalled = false;
			executedFuture = executedPromise.get_future();

			// This assumes that we block the thread until events have been received, and then unsubscribe before going out of scope.
			auto onCommandExecuted = [&executedCalled, &executedPromise](uint64_t subscriptionId, const JsonProvider& json)
			{
				auto commandName = json.GetAkJson()["command"].GetVariant().GetString();
				if (commandName == "ak.sampleclientcommand" && !executedCalled)
				{
					std::cout << "Received command executed. Subscription id is " << subscriptionId << std::endl;
					executedCalled = true;
					executedPromise.set_value(true);
				}
			};

			AkJson subscribeResult;
			if (!client.Subscribe(ak::wwise::ui::commands::executed, options, onCommandExecuted, commandExecutedSubscriptionId, subscribeResult))
			{
				std::cout << "Failed to subscribe to " << ak::wwise::ui::commands::executed << "." << std::endl;
				return;
			}
			else
			{
				std::cout << "Subscribed to " << ak::wwise::ui::commands::executed << ". Subscription id is " << commandExecutedSubscriptionId << std::endl;
			}
		}

		{
			std::string callResultString;

			// Execute the sample command
			std::string args = "{\n\"command\":\"ak.sampleclientcommand\"\n}";
			std::string options = "{}";

			if (!client.Call(ak::wwise::ui::commands::execute, args.c_str(), options.c_str(), callResultString))
			{
				std::cout << "Failed to execute the stress-test command" << std::endl;
				return;
			}
		}

		// Block here until we receive our event
		executedFuture.get();

		// Unsubscribe from the executed subscription
		AkJson unsubscribeResult;
		client.Unsubscribe(commandExecutedSubscriptionId, unsubscribeResult);

		// And remove our sample command
		{
			std::string callResultString;

			std::string args = "{\n\"commands\":[\"ak.sampleclientcommand\"]\n}";
			std::string options = "{}";

			if (!client.Call(ak::wwise::ui::commands::unregister, args.c_str(), options.c_str(), callResultString))
			{
				std::cout << "Failed to unregister the sample command" << std::endl;
				return;
			}
		}

		std::cout << "Disconnecting." << std::endl;
		client.Disconnect();
	}

	// Connection closes when the client goes out of scope.

	std::cout << "Done!" << std::endl;
}


void PerfTest()
{
	using namespace AK::WwiseAuthoringAPI;

	Client client;

	if (!client.Connect("127.0.0.1", 8080))
	{
		std::cout << "Please make sure Wwise Authoring is running and try again." << std::endl;
		return;
	}

	const unsigned int k_uiNumberLoop = 10000;

	// Get the default work unit for actor-mixer
	const AkJson args(AkJson::Map{
		{ "from", AkJson::Map{
			{ "ofType", AkJson::Array{ AkVariant("Project") } } } }
	});

	const AkJson options(AkJson::Map{
		{ "return", AkJson::Array{ AkVariant("id"), AkVariant("name") } }
	});

	AkJson queryResult;

	auto begin = std::chrono::high_resolution_clock::now();

	for (unsigned int i = 0; i < k_uiNumberLoop; ++i)
	{
		if (!client.Call(ak::wwise::core::object::get, args, options, queryResult))
		{
			std::cout << "Failed to get the default actor mixer work unit" << std::endl;
			return;
		}
	}

	auto end = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double, std::milli> duration = end - begin;
	std::cout << "total:" << duration.count() << " ms, average per call:" << duration.count() / (double)k_uiNumberLoop << "ms\n";
}

void ScreenCapture()
{
	using namespace AK::WwiseAuthoringAPI;

	// Enable this for debugging purposes
	Logger::Get()->SetLoggerFunction(logCout);

	Client client;

	if (!client.Connect("127.0.0.1", 8080))
	{
		std::cout << "Please make sure Wwise Authoring is running and try again." << std::endl;
		return;
	}

	// Set the arguments
	AkJson args(AkJson::Map{
		{ "viewName", AkVariant("Property Editor") },
		// Uncomment the following line to generate an error
		// { "viewSyncGroup", AkVariant(5) },
		{ "rect", AkJson::Map{
			{ "x", AkVariant(5) },
			{ "y", AkVariant(10) },
			{ "width", AkVariant(100) },
			{ "height", AkVariant(50) }
		}}
	});

	const AkJson options(AkJson::Map{});

	AkJson queryResult;
	if (!client.Call(ak::wwise::ui::captureScreen, args, options, queryResult))
	{
		std::cout << "Failed to capture the requested view" << std::endl;
		return;
	}

	const auto& imageData = queryResult["contentBase64"].GetVariant().GetString();
	std::vector<char> data(imageData.size());
	size_t bytesRead = 0;
	base64_decode(reinterpret_cast<const unsigned char*>(imageData.c_str()), (int)imageData.size(), data.data(), &bytesRead);

	const auto filePath = "ScreenCaptureOutput.png";
	std::ofstream fileStream(filePath, std::ios::out | std::ios::binary);
	if (fileStream && fileStream.write(data.data(), bytesRead))
	{
		std::cout << "Done!" << std::endl;
	}
	else
	{
		std::cout << "Failed to write image data to file" << std::endl;
	}
}