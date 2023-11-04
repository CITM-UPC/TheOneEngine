#include "Log.h"
#include "App.h"
#include "..\TheOneEngine\Defs.h"

#include <windows.h>
#include <stdio.h>

void Log(const char file[], int line, const char* format, ...)
{
	static char tmpString1[BUFFER_SIZE];
	static char tmpString2[BUFFER_SIZE];
	static va_list ap;

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(tmpString1, BUFFER_SIZE, format, ap);
	va_end(ap);
	sprintf_s(tmpString2, BUFFER_SIZE, "\n%s(%d) : %s", file, line, tmpString1);
	OutputDebugString(tmpString2);

	if (app)
	{
		sprintf_s(tmpString2, BUFFER_SIZE, "\n%s", tmpString1);
		app->LogConsole(tmpString2);
	}
}