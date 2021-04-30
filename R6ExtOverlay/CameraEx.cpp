#include <algorithm>
#include "CameraEx.h"

//these offsets change with each game. (See CameraEx.h as well)
CameraEx::CameraEx(HANDLE hProc, uintptr_t moduleBase, int windowWidth, int windowHeight)
{
	offsets = Offsets(hProc, moduleBase, windowWidth, windowHeight);

	this->windowWidth		=	windowWidth;
	this->windowHeight		=	windowHeight;
	this->hProc				=	hProc;
	this->moduleBase		=	moduleBase;
	this->matrixStart		=	offsets.matrixStart;

	//stuff to help build view matrix array
	float value;
	uintptr_t floatAddr;
	uintptr_t spaceFromStart = 0;

	//initialize the view matrix array... usually 16 float values all right next to each other in memory;
	for (int m = 0; m < 16; m++)
	{
		value = 0;
		floatAddr = 0;

		floatAddr = matrixStart + spaceFromStart;
		ReadProcessMemory(hProc, (BYTE*)floatAddr, &(value), sizeof(value), 0);

		matrix[m] = value;
		spaceFromStart += 0x4;
	}

}

vec3 CameraEx::WorldToScreen(vec3 pos)
{
	//calculations that require view matrix, our position, and an enemy xyz position
	vec3 screen;
	Vec4 clipCoords;

	/*
	directx version
	clipCoords.x = pos.x * matrix[0] + pos.y * matrix[1] + pos.z * matrix[2] + matrix[3];
	clipCoords.y = pos.x * matrix[4] + pos.y * matrix[5] + pos.z * matrix[6] + matrix[7];
	clipCoords.z = pos.x * matrix[8] + pos.y * matrix[9] + pos.z * matrix[10] + matrix[11];
	clipCoords.w = pos.x * matrix[12] + pos.y * matrix[13] + pos.z * matrix[14] + matrix[15];

	reversed directx
	clipCoords.x = pos.x * matrix[15] + pos.y * matrix[14] + pos.z * matrix[13] + matrix[12];
	clipCoords.y = pos.x * matrix[11] + pos.y * matrix[10] + pos.z * matrix[9] + matrix[8];
	clipCoords.z = pos.x * matrix[7] + pos.y * matrix[6] + pos.z * matrix[5] + matrix[4];
	clipCoords.w = pos.x * matrix[3] + pos.y * matrix[2] + pos.z * matrix[1] + matrix[0];

	reversed opengl
	clipCoords.x = pos.x * matrix[15] + pos.y * matrix[11] + pos.z * matrix[7] + matrix[3];
	clipCoords.y = pos.x * matrix[14] + pos.y * matrix[10] + pos.z * matrix[6] + matrix[2];
	clipCoords.z = pos.x * matrix[13] + pos.y * matrix[9] + pos.z * matrix[5] + matrix[1];
	clipCoords.w = pos.x * matrix[12] + pos.y * matrix[8] + pos.z * matrix[4] + matrix[0];
	*/

	//opengl version
	clipCoords.x = pos.x * matrix[0] + pos.y * matrix[4] + pos.z * matrix[8] + matrix[12];
	clipCoords.y = pos.x * matrix[1] + pos.y * matrix[5] + pos.z * matrix[9] + matrix[13];
	clipCoords.z = pos.x * matrix[2] + pos.y * matrix[6] + pos.z * matrix[10] + matrix[14];
	clipCoords.w = pos.x * matrix[3] + pos.y * matrix[7] + pos.z * matrix[11] + matrix[15];

	if (clipCoords.w < 0.1f)
	{
		screen = { 0, 0, 0 };
		return screen;
	}

	vec3 NDC;
	NDC.x = clipCoords.x / clipCoords.w;
	NDC.y = clipCoords.y / clipCoords.w;
	NDC.z = clipCoords.z / clipCoords.w;

	screen.x = (windowWidth / 2 * NDC.x) + (NDC.x + windowWidth / 2);
	screen.y = -(windowHeight / 2 * NDC.y) + (NDC.y + windowHeight / 2);

	//Now we calculate the distance from us to object (vector functions localPlayer.Distance(object))
	//start by finding local player's address and getting our position...
	vec3 locPlayerPos = offsets.GetLocalPlayerPos();

	//Then calculate distance from us and enemy and store in our vec3
	screen.z = locPlayerPos.Distance(pos);

	//returns the 2D screen coordinates
	return screen;
}

//check out casualGamer's approach of WorldToScreen function for Halo d3dx game. His view matrix was different and wierd but still might be worth a look!
	

