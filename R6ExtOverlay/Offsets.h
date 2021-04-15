#pragma once
#include "geom.h"
#include "Windows.h"
#include "proc.h"
#include <d3dx9math.h>

class Offsets
{
public:
	//these are grabbed before and get passed to this class. No need to change them
	HANDLE hProc;
	int width, height;
	uintptr_t moduleBase;

	//variables that we grab from the game...
	uintptr_t entlist = 0;
	uintptr_t localPlayerDeref = 0;
	int numOfPlayersDeref = 0;
	uintptr_t matrixStart = 0;

	Offsets();
	Offsets(HANDLE hProc, uintptr_t moduleBase, int width, int height);
	vec3 GetLocalPlayerPos();
	//vec3 GetEntityHeadPos();
	//vec3 GetBestEntity();
};

