#include "Offsets.h"


Offsets::Offsets()
{

}
Offsets::Offsets(HANDLE hProc, uintptr_t moduleBase, int width, int height)
{
	this->hProc = hProc;
	this->width = width;
	this->height = height;
	this->moduleBase = moduleBase;
	this->entlist = FindDMAAddy(hProc, moduleBase + 0x05EF12F8, { 0x60 });
	this->localPlayerDeref = FindDMAAddy(hProc, moduleBase + 0x05EF12F8, { 0x918, 0xCC8, 0x0 });
	this->numOfPlayersDeref = 60;

}
vec3 Offsets::GetLocalPlayerPos()
{

	uintptr_t currHeadPosXPtr = localPlayerDeref + 0x120;
	uintptr_t currHeadPosYPtr = localPlayerDeref + 0x124;
	uintptr_t currHeadPosZPtr = localPlayerDeref + 0x128;

	vec3 LPHeadPos = vec3();

	ReadProcessMemory(hProc, (BYTE*)currHeadPosXPtr, &(LPHeadPos.x), sizeof(LPHeadPos.x), 0);
	ReadProcessMemory(hProc, (BYTE*)currHeadPosYPtr, &(LPHeadPos.y), sizeof(LPHeadPos.y), 0);
	ReadProcessMemory(hProc, (BYTE*)currHeadPosZPtr, &(LPHeadPos.z), sizeof(LPHeadPos.z), 0);

	return LPHeadPos;
}
/*
vec3 Offsets::GetBestEntity()
{
	vec3 myPos = GetLocalPlayerPos();
	vec3 entOfLeastDist;

	uintptr_t currEntPtr;
	float leastDist = 0;


	// loop through entity list 
	// find the one with the least distance (relative to crosshair)
	// return his XYZ head pos
	//problems with this: 2) does not distinguish between allies and enemies
	for (int i = 0; i < numOfPlayersDeref; i++)
	{
		CameraEx cameraEx = CameraEx(hProc, moduleBase, width, height);

		//currently this only gets the first entity in the entity list
		ReadProcessMemory(hProc, (BYTE*)entlist, &(currEntPtr), sizeof(currEntPtr), 0);
		//currEntPtr += 0x14;
		for (int j = 0; j < i; j++)
		{
			currEntPtr += 0x4;
		}

		ReadProcessMemory(hProc, (BYTE*)currEntPtr, &(currEntPtr), sizeof(currEntPtr), 0);

		uintptr_t XPosAddr = currEntPtr + 0x4;
		uintptr_t YPosAddr = currEntPtr + 0x8;
		uintptr_t ZPosAddr = currEntPtr + 0xC;

		Vector3 targetHead = Vector3();

		ReadProcessMemory(hProc, (BYTE*)XPosAddr, &(targetHead.x), sizeof(targetHead.x), 0);
		ReadProcessMemory(hProc, (BYTE*)YPosAddr, &(targetHead.y), sizeof(targetHead.y), 0);
		ReadProcessMemory(hProc, (BYTE*)ZPosAddr, &(targetHead.z), sizeof(targetHead.z), 0);

		//find the target's XY screen coordinates
		vec3 screenPos = cameraEx.WorldToScreen(targetHead);

		//... and my crosshair XY screen coordinates
		vec3 crossPos;

		crossPos.x = WINDOWWIDTH / 2;
		crossPos.y = WINDOWHEIGHT / 2;
		crossPos.z = 0.0f;

		//find the distance from my crosshair to the target
		float distRelCross = crossPos.Distance(screenPos);

		if (i == 0 || distRelCross < leastDist)
		{
			leastDist = distRelCross;
			entOfLeastDist = targetHead;
		}

	}

	return entOfLeastDist;
}
*/