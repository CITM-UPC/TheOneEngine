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

#include <iostream>

#include "TestClient.h"
#include "StressTest.h"
#include "StressTestConnections.h"
#include "TestErrors.h"
#include "TestMonitoring.h"

int main(int argc, char** argv)
{
	std::string testToRun;

	if (argc < 2)
	{
		testToRun = "HelloWorld";
	}
	else
	{
		testToRun = argv[1];
	}

	if (testToRun == "HelloWorld")
	{
		HelloWorld();
	}
	else if (testToRun == "TestMonitoring")
	{
		TestMonitoring();
	}
	else if (testToRun == "TestWampClient")
	{
		TestWampClient();
	}
	else if (testToRun == "TestWampClientJsonString")
	{
		TestWampClientJsonString();
	}
	else if (testToRun == "TestCustomCommands")
	{
		TestCustomCommands();
	}
	else if (testToRun == "TestErrors")
	{
		TestErrors();
	}
	else if (testToRun == "PerfTest")
	{
		PerfTest();
	}
	else if (testToRun == "StressTest")
	{
		StressTest(argc, argv);
	}
	else if (testToRun == "StressTestConnections")
	{
		StressTestConnections(argc, argv);
	}
	else if (testToRun == "ScreenCapture")
	{
		ScreenCapture();
	}
	else if (testToRun == "TestAll")
	{
		HelloWorld();
		TestWampClient();
		TestWampClientJsonString();
		TestCustomCommands();
		PerfTest();
		TestErrors();
		ScreenCapture();
	}
	else
	{
		std::cout << "Usage: SampleClient [HelloWorld|TestWampClient|TestWampClientJsonString|TestMonitoring|TestCustomCommands|TestErrors|PerfTest|ScreenCapture|TestAll]" << std::endl;
		return 1;
	}

	return 0;
}
