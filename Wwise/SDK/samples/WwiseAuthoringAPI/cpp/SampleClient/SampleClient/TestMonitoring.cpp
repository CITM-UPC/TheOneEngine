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

#include "TestMonitoring.h"

#include <AK/WwiseAuthoringAPI/waapi.h>
#include <AK/WwiseAuthoringAPI/AkAutobahn/Client.h>
#include <AK/WwiseAuthoringAPI/AkAutobahn/FutureUtils.h>

#include <string>
#include <iostream>
#include <cstdio>
#include <algorithm>
#include <csignal>

namespace
{
	const int pollingSleepTimeMs = 100;

	// Using a value of 0 (no sleep in retry loop) is a valid
	// stress test of the monitoring (it still should work fine) but it's usually
	// a good idea to reduce the rate of the WAAPI queries to a minimum to avoid
	// unnecessary processing time in Wwise.
	const int retrySleepTimeMs = 100;

	bool GetCurrentProject(AK::WwiseAuthoringAPI::Client& client, std::string& result)
	{
		using namespace AK::WwiseAuthoringAPI;

		AkJson args(AkJson::Map{
			{ "from", AkJson::Map{
				{ "ofType", AkJson::Array{ AkVariant("Project") } } } }
		});

		AkJson options(AkJson::Map{
			{ "return", AkJson::Array{ AkVariant("filePath") } }
		});

		AkJson queryResult;
		bool success = client.Call(ak::wwise::core::object::get, args, options, queryResult);

		if (success)
		{
			auto& returnResult = queryResult["return"];

			if (!returnResult.GetArray().empty())
			{
				result = returnResult[0]["filePath"].GetVariant().GetString();
			}
		}

		return success;
	}

	// WAAPI can become temporarily unavailable from time to time, this should be taken into account in the design.
	// Please note that you shouldn't use this kind of design unless you know your call is accurate. If you do this
	// and the error is caused by an argument problem, you might end up with an infinite retry loop.
	// For a more rebust design, a retry delay which be required, but keep in mind that WAAPI can remain
	// unavailable for extended periods of time (for example, as long as the user is using a popup dialog in Wwise).
	bool ExecuteAutoRetry(AK::WwiseAuthoringAPI::Client& client, std::function<bool()> function)
	{
		bool result = false;

		while (client.IsConnected() && !(result = function()))
		{
			// Avoid flooding with requests while WAAPI is unavailable.
			if (retrySleepTimeMs > 0)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(retrySleepTimeMs));
			}
		}

		return result;
	}

	std::string GetAndWaitForCurrentProject(AK::WwiseAuthoringAPI::Client& client)
	{
		std::string result = "No project.";

		ExecuteAutoRetry(client, [&client, &result]() {
			return GetCurrentProject(client, result);
		});

		return result;
	}

	volatile bool kbHit = false;

	void signalHandler(int)
	{
		kbHit = true;
	}
}

void TestMonitoring()
{
	using namespace AK::WwiseAuthoringAPI;

	std::signal(SIGINT, signalHandler);

	const int minRetryMs = 1000;
	const int maxRetryMs = 10000;
	int retryDelayMs = minRetryMs;

	std::cout << "Press Ctrl-C to exit Wwise monitoring..." << std::endl << std::endl;

	Client client;
	std::atomic_bool wasDisconnected;
	std::atomic_bool projectStatusChanged;

	// vc_120 compatibility.
	wasDisconnected = false;
	projectStatusChanged = false;

	auto onProjectLoaded = [&projectStatusChanged](uint64_t, const JsonProvider&)
	{
		// Setting a flag here that will be handled from the main thread (rudimentary thread communication).
		// It is important to never call the client from inside a callback since the client doesn't support reentry. Once
		// a callback is received, it is important to schedule the response from another thread (usually the one that connects).
		// Using an atomic bool is a simple implementation in which the main loop will pool the status of project reload,
		// but this pattern can be implemented in many different ways depending on the
		//architecture (sending an event, invoking a method on another thread, etc.)
		projectStatusChanged = true;
	};

	auto onProjectUnloaded = [&projectStatusChanged](uint64_t, const JsonProvider&)
	{
		std::cout << "Project was closed." << std::endl;
	};

	// Exit monitoring once the user presses any key.
	while (!kbHit)
	{
		wasDisconnected = false;

		// Reconnect loop.
		while (!client.IsConnected() && !kbHit)
		{
			std::cout << "Client is not connected, trying to connect now..." << std::endl;

			if (!client.Connect("127.0.0.1", 8080, [&wasDisconnected] {
				wasDisconnected = true;
			}))
			{
				std::cout << "Failed to connect, will retry in " << retryDelayMs << " miliseconds." << std::endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(retryDelayMs));
				retryDelayMs = std::min(retryDelayMs * 2, maxRetryMs);
			}
			else
			{
				retryDelayMs = minRetryMs;

				std::cout << "Client successfully connected to Wwise." << std::endl;

				std::cout << "Current project name: " << GetAndWaitForCurrentProject(client) << std::endl;

				AkJson subscribeResult;
				uint64_t idProjectLoadedSubscription = 0;

				if (!ExecuteAutoRetry(client, [&client, onProjectLoaded, &idProjectLoadedSubscription, &subscribeResult]() {
					return client.Subscribe(ak::wwise::core::project::loaded, AkJson::Map{}, onProjectLoaded, idProjectLoadedSubscription, subscribeResult);
				}))
				{
					std::cout << "FAILED: Could not subscribe to project.loaded." << std::endl;
				}

				uint64_t idProjectUnloadedSubscription = 0;

				if (!ExecuteAutoRetry(client, [&client, onProjectUnloaded, &idProjectUnloadedSubscription, &subscribeResult]() {
					return client.Subscribe(ak::wwise::core::project::postClosed, AkJson::Map{}, onProjectUnloaded, idProjectUnloadedSubscription, subscribeResult);
				}))
				{
					std::cout << "FAILED: Could not subscribe to project.postClosed." << std::endl;
				}

				// No need to keep the ids to unsubscribe since we keep the subscriptions alive for as long as we are connected to Wwise.
				// They will be cleaned up automatically once the connection is lost.
			}
		}
		
		// Wait here until either the user exits or the client gets disconnected. Monitor the state of the project.
		while (!kbHit && !wasDisconnected)
		{
			if (projectStatusChanged)
			{
				projectStatusChanged = false;
				std::cout << "New current project: " << GetAndWaitForCurrentProject(client) << std::endl;
			}

			// Monitor the current status every pollingSleepTimeMs. Another way to implement this would be to use an event-based architecture.
			if (pollingSleepTimeMs > 0)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(pollingSleepTimeMs));
			}
		}
	}
}
