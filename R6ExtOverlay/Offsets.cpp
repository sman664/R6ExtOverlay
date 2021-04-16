#include "Offsets.h"


Offsets::Offsets()
{

}
Offsets::Offsets(HANDLE hProc, uintptr_t moduleBase, int width, int height)
{
	this->hProc				=	hProc;
	this->width				=	width;
	this->height			=	height;
	this->moduleBase		=	moduleBase;

	//NOTE: findDMAAddy already derefences the module base. Enter these as though it is cheat engine
	//this->entlist = FindDMAAddy(hProc, moduleBase + 0x05EDEE60, { 0x0, 0xA28, 0x0 });                           
	this->entlist			=	FindDMAAddy(hProc, moduleBase + 0x05EDEE60, { 0x0, 0xA28, 0x68, 0x0 });
	this->localPlayerDeref	=	FindDMAAddy(hProc, moduleBase + 0x05ED29A8, { 0x918, 0xCD0, 0x0 });				
	this->numOfPlayersDeref =	80; 

	this->matrixStart		=	FindDMAAddy(hProc, moduleBase + 0x05ED29A8, { 0x918, 0xCD0, 0x140 });
	
	this->firstEntityAddr	=	entlist + 0x20;
	this->spaceBetweenAddys	=	0x40;
	this->XposAddr			=	0x30;
}
vec3 Offsets::GetLocalPlayerPos()
{

	uintptr_t currHeadPosXPtr	=	localPlayerDeref + 0x120;
	uintptr_t currHeadPosYPtr	=	localPlayerDeref + 0x124;
	uintptr_t currHeadPosZPtr	=	localPlayerDeref + 0x128;

	vec3 LPHeadPos = vec3();

	ReadProcessMemory(hProc, (BYTE*)currHeadPosXPtr, &(LPHeadPos.x), sizeof(LPHeadPos.x), 0);
	ReadProcessMemory(hProc, (BYTE*)currHeadPosYPtr, &(LPHeadPos.y), sizeof(LPHeadPos.y), 0);
	ReadProcessMemory(hProc, (BYTE*)currHeadPosZPtr, &(LPHeadPos.z), sizeof(LPHeadPos.z), 0);

	return LPHeadPos;
}
vec3 Offsets::GetEntityHeadPos(int index)
{

	uintptr_t currEntPtr = firstEntityAddr;

	for (int j = 0; j < index; j++)
	{
		currEntPtr += spaceBetweenAddys;															//offset between each entity
	}
	ReadProcessMemory(hProc, (BYTE*)currEntPtr, &(currEntPtr), sizeof(currEntPtr), 0);

	//get the entity's X,Y, and Z positions
	uintptr_t thisEntPtrXPos = currEntPtr +	XposAddr;
	uintptr_t thisEntPtrYPos = currEntPtr +	XposAddr + 0x4;
	uintptr_t thisEntPtrZPos = currEntPtr +	XposAddr + 0x8;

	Vector3 headPos;

	ReadProcessMemory(hProc, (BYTE*)thisEntPtrXPos, &(headPos.x), sizeof(headPos.x), 0);
	ReadProcessMemory(hProc, (BYTE*)thisEntPtrYPos, &(headPos.y), sizeof(headPos.y), 0);
	ReadProcessMemory(hProc, (BYTE*)thisEntPtrZPos, &(headPos.z), sizeof(headPos.z), 0);
	
	return headPos;
}

