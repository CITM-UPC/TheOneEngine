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

#include "StressTest.h"

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
static const int nbThreads = 5;

// Number of test repeat (once all clients and threads have properly terminated).
static int nbOuterLoops = 200000;

// Number of test repeat for each client inside its own thread (not reconnecting).
static const int nbInnerLoops = 15;

// Number of time to call unsubscribe. This is to repro some edge-case bug when terminating in the
// sender thread because of a dead connection while the client threads has already called something
// else (since requests to send are queued in the client thread and handled in the server thread).
static const int nbUnsubscribeAbuse = 5;

bool TestWampClientForStressTest(AK::WwiseAuthoringAPI::Client* pClient)
{
	using namespace AK::WwiseAuthoringAPI;

	const int futureGetTimeout = 5000;

	bool passedError = false;

	{
		if (!pClient->Connect("127.0.0.1", 8080, nullptr, futureGetTimeout))
		{
			return false;
		}
		
		for (int i = 0; i < nbInnerLoops; ++i)
		{
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
						objectCreatedCalled = true;
						objectCreatedPromise.set_value(true);
					}
				};
				
				// Subscribe to object created notification
				AkJson subscribeResult;
				if (!pClient->Subscribe(ak::wwise::core::object::created, AkJson(AkJson::Type::Map), objectCreated, idObjectCreatedSubscription, subscribeResult, futureGetTimeout))
				{
					break;
				}
			}
			
			AkJson createResult;

			{
				// create an object
				AkJson args(AkJson::Map{
					{ "parent", AkVariant("\\Actor-Mixer Hierarchy\\Default Work Unit") },
					{ "type", AkVariant("Sound") },
					{ "name", AkVariant("Boom") },
					{ "onNameConflict", AkVariant("rename") },
				});

				if (!pClient->Call(ak::wwise::core::object::create, args, AkJson(AkJson::Type::Map), createResult, futureGetTimeout))
				{
					AkJson unsubscribeResult;
					pClient->Unsubscribe(idObjectCreatedSubscription, unsubscribeResult, futureGetTimeout);
					break;
				}
			}
			
			bool callbackResult;
			bool successFuture = GetFutureWithTimeout<bool>(futureGetTimeout, objectCreatedFuture, callbackResult);

#ifdef VALIDATE_ECHO
			if (!successFuture)
			{
				std::cout << "TIMEOUT! It's a trap!" << std::endl;
			}
#endif

			AkJson unsubscribeResult;
			pClient->Unsubscribe(idObjectCreatedSubscription, unsubscribeResult, futureGetTimeout);

			// Unsubscribe a few times more, to be sure it doesn't cause any problem.
			{
				for (int i = 0; i < nbUnsubscribeAbuse; ++i)
				{
					AkJson unsubscribeResult;
					pClient->Unsubscribe(idObjectCreatedSubscription, unsubscribeResult, futureGetTimeout);
				}
			}
			
			{
				// Delete the new object.
				AkJson args(AkJson::Map{
					{ "object", createResult["id"] },
				});

				pClient->Call(ak::wwise::core::object::delete_, args, AkJson(AkJson::Type::Map), createResult, futureGetTimeout);
			}
			
			// Do a call that will fail, testing the wrong schema code path.
			{
				AkJson args(AkJson::Map
				{
				});

				AkJson result;
				pClient->Call(ak::wwise::core::object::setProperty, args, AkJson(AkJson::Type::Map), result, futureGetTimeout);
			}

			// Do a call that will fail, testing the wrong URI code path.
			{
				AkJson args(AkJson::Map
				{
				});

				AkJson result;
				pClient->Call("ak.wronguri", args, AkJson(AkJson::Type::Map), result, futureGetTimeout);
			}
		}
	}

	return true;
}

#if 0
static void logCout(const char* log)
{
	std::cout << log << std::endl;
}
#endif

void StressTest(int in_argc, char** in_argv)
{
	using namespace AK::WwiseAuthoringAPI;

	if (in_argc > 2)
	{
		nbOuterLoops = atoi(in_argv[2]);
	}

	// Enable this for debugging purposes.
#if 0
	Logger::Get()->SetLoggerFunction(logCout);
#endif

	for (int j = 0; j < nbOuterLoops; ++j)
	{
		std::cout << "Loop #" << j << std::endl;

		std::vector<std::thread> threads;
		std::vector<Client*> clients(nbThreads);

		// Create the clients.
		for (int i = 0; i < nbThreads; ++i)
		{
			clients[i] = new Client();
		}

		auto begin = std::chrono::high_resolution_clock::now();

		// Start all threads.
		for (int i = 0; i < nbThreads; ++i)
		{
			int startDelayMs = (rand() % startDelayIncrementMs);
			Client* pClient = clients[i];

			threads.push_back(std::thread([startDelayMs, pClient] {
				std::this_thread::sleep_for(std::chrono::milliseconds(startDelayMs));
				TestWampClientForStressTest(pClient);
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

		// Delete and disconnect when everything is done. Don't disconnect in the thread, it's just too slow.
		for (auto it = clients.begin(); it != clients.end(); ++it)
		{
			delete *it;
		}
	}

	std::cout << "Job's done!" << std::endl;
}
