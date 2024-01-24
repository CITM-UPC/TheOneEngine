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

#include "StressTestConnections.h"

#include "AK/WwiseAuthoringAPI/AkAutobahn/Client.h"
#include "AK/WwiseAuthoringAPI/AkAutobahn/FutureUtils.h"
#include "AK/WwiseAuthoringAPI/AkAutobahn/Logger.h"

#include <AK/WwiseAuthoringAPI/waapi.h>

#include <string>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <future>

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

// Modify this to change the test configuration.

// Each new thread will wait randomly between 0 and startDelayIncrementMs before starting.
static const int startDelayIncrementMs = 30;

// Number of threads to run simultanously.
static const int nbThreads = 4;

// Number of test repeat (once all clients and threads have properly terminated).
static int nbOuterLoops = 200000;

void TestWampClientForStressTestConnections(AK::WwiseAuthoringAPI::Client* pClient)
{
	using namespace AK::WwiseAuthoringAPI;

	const int futureGetTimeout = 5000;

	if (!pClient->Connect("127.0.0.1", 8080, nullptr, futureGetTimeout))
		return;

	// create an object
	AkJson args(AkJson::Map{
		{ "waql", AkVariant("from type project") }
		});

	AkJson result;

	pClient->Call(ak::wwise::core::object::get, args, AkJson(AkJson::Type::Map), result, futureGetTimeout);
	std::this_thread::sleep_for(std::chrono::milliseconds(startDelayIncrementMs));
	pClient->Disconnect();
}

void RestartWaapiServer(AK::WwiseAuthoringAPI::Client* pClient)
{
	using namespace AK::WwiseAuthoringAPI;

	const int futureGetTimeout = 5000;

	if (!pClient->Connect("127.0.0.1", 8080, nullptr, futureGetTimeout))
	{
		return;
	}

	AkJson args(AkJson::Map{});
	AkJson result;

	pClient->Call(ak::wwise::debug::restartWaapiServers, args, AkJson(AkJson::Type::Map), result, futureGetTimeout);
	pClient->Disconnect();
}

void StressTestConnections(int in_argc, char** in_argv)
{
	using namespace AK::WwiseAuthoringAPI;

	if (in_argc > 2)
	{
		nbOuterLoops = atoi(in_argv[2]);
	}

	std::cout << "WAAPI Connect/Disconnect stress test" << std::endl;

	for (int j = 0; j < nbOuterLoops; ++j)
	{
		std::cout << "Loop #" << j << std::endl;

		std::vector<std::thread> threads;
		std::vector<std::unique_ptr<Client>> clients(nbThreads);

		// Create the clients.
		for (int i = 0; i < nbThreads; ++i)
		{
			clients[i] = std::make_unique<Client>();
		}

		auto begin = std::chrono::high_resolution_clock::now();

		// Start all threads.
		for (int i = 0; i < nbThreads - 1; ++i)
		{
			int startDelayMs = (rand() % startDelayIncrementMs);
			Client* pClient = clients[i].get();

			threads.push_back(std::thread([startDelayMs, pClient] {
				std::this_thread::sleep_for(std::chrono::milliseconds(startDelayMs));
				TestWampClientForStressTestConnections(pClient);
			}));
		}

		{
			int startDelayMs = (rand() % startDelayIncrementMs);
			Client* pClient = clients[nbThreads - 1].get();

			threads.push_back(std::thread([startDelayMs, pClient] {
				std::this_thread::sleep_for(std::chrono::milliseconds(startDelayMs));
				RestartWaapiServer(pClient);
			}));
		}

		// Wait for all threads.
		for (auto it = threads.begin(); it != threads.end(); ++it)
		{
			it->join();
		}

		auto end = std::chrono::high_resolution_clock::now();

		std::chrono::duration<double, std::milli> duration = end - begin;
		std::cout << "Stress duration: " << duration.count() << std::endl;
	}

	std::cout << "Job's done!" << std::endl;
}
