#include "BuilderApp.h"
#include "../TheOneEngine/Log.h"

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

BuilderApp* bApp = NULL;

int main(int argc, char* args[])
{
	LOG(LogType::LOG_INFO, "Builder starting ...");
	MainState state = CREATE;
	int result = EXIT_FAILURE;

	while (state != EXIT)
	{
		switch (state)
		{
			// Allocate the engine ---------------------------------------------
		case CREATE:
			LOG(LogType::LOG_INFO, "CREATION PHASE ===============================");

			bApp = new BuilderApp(argc, args);

			if (bApp != NULL)
				state = AWAKE;
			else
				state = FAIL;

			break;

			// Awake all modules -----------------------------------------------
		case AWAKE:
			LOG(LogType::LOG_INFO, "AWAKE PHASE ===============================");
			if (bApp->Awake() == true)
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
			if (bApp->Start() == true)
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
			if (bApp->Update() == false)
				state = CLEAN;
		}
		break;

		// Cleanup allocated memory --------------------------------------------
		case CLEAN:
			LOG(LogType::LOG_INFO, "CLEANUP PHASE ===============================");
			if (bApp->CleanUp() == true)
			{
				RELEASE(bApp);
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