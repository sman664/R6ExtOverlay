#pragma once

#include <string>
#include "geom.h"
#include "proc.h"
#include <d3dx9math.h>


//find the camera and the resolution ptr path and store here before compile time
//const uintptr_t pCamera = 0x501AE8;
//const uintptr_t pResolution = 0x510C94;
//const uintptr_t matrixStart = 0;
//0x501AE8;
//const uintptr_t localPlayer = 0;
//(uintptr_t)0x50F4F4;
//const uintptr_t numOfPlayers = 0x50f500;
//int numOfPlayers = 6;

class CameraEx
{
public:
	float windowWidth = 0, windowHeight = 0;
	float matrix[29];
	float fovx = 0;
	float fovy = 0;

	uintptr_t moduleBase = 0;
	uintptr_t matrixStart = 0;
	uintptr_t localPlayerDeref = 0;
	HANDLE hProc = 0;

	CameraEx(HANDLE hProc, uintptr_t moduleBase, float windowWidth, float windowHeight);

	vec3 WorldToScreen(vec3 pos);
};
