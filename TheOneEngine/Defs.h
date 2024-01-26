#ifndef __DEFS_H__
#define __DEFS_H__
#pragma once

#include <stdio.h>

#include "GL/glew.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"
#include "nlohmann/json.hpp"

#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

// NULL just in case ----------------------
#ifdef NULL
#undef NULL
#endif
#define NULL  0


// PREDEFINED & GENERAL VARIABLES ------------------------------------------------
/// Keep a value between 0.0f and 1.0f
#define CAP(n) ((n <= 0.0f) ? n=0.0f : (n >= 1.0f) ? n=1.0f : n=n)

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#define PI		 3.14159265359f
#define TO_BOOL( a )  ( (a != 0) ? true : false )

/// Useful helpers, InRange checker, min, max and clamp
#define MIN( a, b ) ( ((a) < (b)) ? (a) : (b) )
#define MAX( a, b ) ( ((a) > (b)) ? (a) : (b) )
#define CLAMP(x, upper, lower) (MIN(upper, MAX(x, lower)))
#define IN_RANGE( value, min, max ) ( ((value) >= (min) && (value) <= (max)) ? 1 : 0 )

/// Standard string size
#define SHORT_STR	32
#define MID_STR		255
#define HUGE_STR	8192

/// Camera Settings
#define MIN_FOV 15.0f
#define MAX_FOV 120.0f

/// Performance macros
//#define PERF_START(timer) timer.Start()
//#define PERF_PEEK(timer) LOG("%s took %f ms", __FUNCTION__, timer.ReadMs())

/// Releasing Memory
#define RELEASE( x )\
    {\
       if( x != nullptr )\
       {\
         delete x;\
	     x = nullptr;\
       }\
    }

// Deletes an array of buffers
#define RELEASE_ARRAY( x )\
	{\
	if( x != NULL )\
		{\
	  delete[] x;\
	  x = NULL;\
		}\
	}
// -------------------------------------------------------------------------------


// TYPEDEFS ----------------------------------------------------------------------
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned __int16 uint16;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;

using vec2 = glm::dvec2;
using vec3 = glm::dvec3;
using vec4 = glm::dvec4;
using mat4 = glm::dmat4;
using mat4f = glm::mat4;

using vec2f = glm::vec2;
using vec3f = glm::vec3;
using vec4f = glm::vec4;

using quat = glm::dquat;
using quatf = glm::quat;

using json = nlohmann::json;
// -------------------------------------------------------------------------------


// OTHERS ------------------------------------------------------------------------
/// Joins a path and file
inline const char* const PATH(const char* folder, const char* file)
{
	static char path[MID_STR];
	sprintf_s(path, MID_STR, "%s/%s", folder, file);
	return path;
}

/// Swaps 2 values
template <class VALUE_TYPE> void SWAP(VALUE_TYPE& a, VALUE_TYPE& b)
{
	VALUE_TYPE tmp = a;
	a = b;
	b = tmp;
}
// -------------------------------------------------------------------------------


// CONFIGURATION -----------------------------------------------------------------
#define TITLE "TheOneEngine"
#define VERSION "v0.3"
#define ORGANIZATION "[CITM - UPC]"

// Folder paths
#define ASSETS_PATH "Assets\\"
#define LIBRARY_PATH "Library\\"

//Logs
#define BUFFER_SIZE 4096
#define MAX_LOGS_CONSOLE 1000
// -------------------------------------------------------------------------------


#endif	// !__DEFS_H__