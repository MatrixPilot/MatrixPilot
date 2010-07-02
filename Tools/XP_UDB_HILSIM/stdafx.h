// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

#pragma once

#define PI (float)3.141592653589793238462643
#define NUMLOOPS 6
#define R_EARTH 6371000


#ifdef WIN32

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <Windows.h>
#include <GL/gl.h>		// Win32

#else

#include <OpenGL/OpenGL.h>	// Mac

#endif


#include "XPLMDataAccess.h"
#include "XPLMDisplay.h"
#include "XPLMProcessing.h"
#include "XPLMGraphics.h"
#include "XPLMUtilities.h"
#include "XPLMCamera.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "utility.h"
#include "Setup.h"






