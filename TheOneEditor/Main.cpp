#include "App.h"
#include "Log.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <chrono>
#include <thread>
#include <stdlib.h>


enum MainState
{
	CREATE = 1,
	AWAKE,
	START,
	LOOP,
	CLEAN,
	FAIL,
	EXIT
};

App* app = NULL;

int main(int argc, char* args[])
{
	LOG(LogType::LOG_INFO, "Engine starting ...");

	MainState state = CREATE;
	int result = EXIT_FAILURE;

	while (state != EXIT)
	{
		//OPTICK_FRAME("MainLoop");
		switch (state)
		{
			// Allocate the engine ---------------------------------------------
		case CREATE:
			LOG(LogType::LOG_INFO, "CREATION PHASE ===============================");

			app = new App(argc, args);

			if (app != NULL)
				state = AWAKE;
			else
				state = FAIL;

			break;

			// Awake all modules -----------------------------------------------
		case AWAKE:
			LOG(LogType::LOG_INFO, "AWAKE PHASE ===============================");
			if (app->Awake() == true)
				state = START;
			else
			{
				LOG(LogType::LOG_ERROR, "Awake phase");
				state = FAIL;
			}

			break;

			// Call all modules before first frame  ----------------------------
		case START:
			LOG(LogType::LOG_INFO, "START PHASE ===============================");
			if (app->Start() == true)
			{
				state = LOOP;
				LOG(LogType::LOG_INFO, "UPDATE PHASE ===============================");
			}
			else
			{
				state = FAIL;
				LOG(LogType::LOG_ERROR, "Start phase");
			}
			break;

			// Loop all modules until we are asked to leave --------------------
		case LOOP:
		{
			//OPTICK_FRAME("Main Loop");
			if (app->Update() == false)
				state = CLEAN;
		}
		break;

		// Cleanup allocated memory --------------------------------------------
		case CLEAN:
			LOG(LogType::LOG_INFO, "CLEANUP PHASE ===============================");
			if (app->CleanUp() == true)
			{
				RELEASE(app);
				result = EXIT_SUCCESS;
				state = EXIT;
			}
			else
				state = FAIL;

			break;

			// Exit with errors and shame --------------------------------------
		case FAIL:
			LOG(LogType::LOG_ERROR, "Exiting with errors :(");
			result = EXIT_FAILURE;
			state = EXIT;
			break;
		}
	}

	// Dump memory leaks
	return result;
}