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
	
	this->firstEntityAddr	=	entlist - 0xA0; //0x20;
	this->spaceBetweenAddys	=	0x40;
	this->XposAddr			=	0xA0;

	this->XposAddr2			=	0x40;
	this->XposAddr3			=	0x50;
}
vec3 Offsets::GetLocalPlayerPos()
{

	uintptr_t currHeadPosXPtr	=	localPlayerDeref + 0x120;
	uintptr_t currHeadPosYPtr	=	localPlayerDeref + 0x124;
	uintptr_t currHeadPosZPtr	=	localPlayerDeref + 0x128;

	vec3 LPHeadPos;

	ReadProcessMemory(hProc, (BYTE*)currHeadPosXPtr, &(LPHeadPos.x), sizeof(LPHeadPos.x), 0);
	ReadProcessMemory(hProc, (BYTE*)currHeadPosYPtr, &(LPHeadPos.y), sizeof(LPHeadPos.y), 0);
	ReadProcessMemory(hProc, (BYTE*)currHeadPosZPtr, &(LPHeadPos.z), sizeof(LPHeadPos.z), 0);

	return LPHeadPos;
}
vec3 Offsets::GetEntityHeadPos(int index)
{

	uintptr_t currEntPtr = firstEntityAddr;

	//loop to find the current entity
	for (int j = 0; j < index; j++)
	{
		currEntPtr += spaceBetweenAddys;															//offset between each entity
	}
	//ReadProcessMemory(hProc, (BYTE*)currEntPtr, &(currEntPtr), sizeof(currEntPtr), 0);

	currEntPtr = FindDMAAddy(hProc, currEntPtr, { 0x60, 0xD8, 0x68, 0x0 });

	//get the entity's X,Y, and Z positions
	uintptr_t currHeadPosXPtr = currEntPtr	+	XposAddr;
	uintptr_t currHeadPosYPtr = currEntPtr	+	XposAddr + 0x4;
	uintptr_t currHeadPosZPtr = currEntPtr	+	XposAddr + 0x8;

	vec3 headPos;

	ReadProcessMemory(hProc, (BYTE*)currHeadPosXPtr, &(headPos.x), sizeof(headPos.x), 0);
	ReadProcessMemory(hProc, (BYTE*)currHeadPosYPtr, &(headPos.y), sizeof(headPos.y), 0);
	ReadProcessMemory(hProc, (BYTE*)currHeadPosZPtr, &(headPos.z), sizeof(headPos.z), 0);

	//if (headPos.x == 0 || headPos.y == 0)
	//{
	//	headPos = GetEntityGrouping2(index);
	//}

	//vec3 LPHeadPos = GetLocalPlayerPos();
	//if (headPos.x == LPHeadPos.x && headPos.y == LPHeadPos.y && headPos.z == LPHeadPos.z)
	//{
	//	return headPos = vec3(0,0,0);
	//}

	return headPos;
}

vec3 Offsets::GetEntityGrouping(int index)
{

	uintptr_t currEntPtr = firstEntityAddr;
	
	//loop to find the current entity
	for (int j = 0; j < index; j++)
	{
		currEntPtr += spaceBetweenAddys;															//offset between each entity
	}

	ReadProcessMemory(hProc, (BYTE*)currEntPtr, &(currEntPtr), sizeof(currEntPtr), 0);
	
	//get the entity's X,Y, and Z positions
	uintptr_t currHeadPosXPtr = currEntPtr + XposAddr2;
	uintptr_t currHeadPosYPtr = currEntPtr + XposAddr2 + 0x4;
	uintptr_t currHeadPosZPtr = currEntPtr + XposAddr2 + 0x8;

	Vector3 headPos;

	ReadProcessMemory(hProc, (BYTE*)currHeadPosXPtr, &(headPos.x), sizeof(headPos.x), 0);
	ReadProcessMemory(hProc, (BYTE*)currHeadPosYPtr, &(headPos.y), sizeof(headPos.y), 0);
	ReadProcessMemory(hProc, (BYTE*)currHeadPosZPtr, &(headPos.z), sizeof(headPos.z), 0);

	return headPos;
}

vec3 Offsets::GetEntityGrouping2(int index)
{

	uintptr_t currEntPtr = firstEntityAddr;

	//loop to find the current entity
	for (int j = 0; j < index; j++)
	{
		currEntPtr += spaceBetweenAddys;															//offset between each entity
	}

	ReadProcessMemory(hProc, (BYTE*)currEntPtr, &(currEntPtr), sizeof(currEntPtr), 0);

	//get the entity's X,Y, and Z positions
	uintptr_t currHeadPosXPtr = currEntPtr + XposAddr3;
	uintptr_t currHeadPosYPtr = currEntPtr + XposAddr3 + 0x4;
	uintptr_t currHeadPosZPtr = currEntPtr + XposAddr3 + 0x8;

	Vector3 headPos;

	ReadProcessMemory(hProc, (BYTE*)currHeadPosXPtr, &(headPos.x), sizeof(headPos.x), 0);
	ReadProcessMemory(hProc, (BYTE*)currHeadPosYPtr, &(headPos.y), sizeof(headPos.y), 0);
	ReadProcessMemory(hProc, (BYTE*)currHeadPosZPtr, &(headPos.z), sizeof(headPos.z), 0);

	return headPos;
}

/*void Offsets::initEntList()
{
	
	//1) create an array of vec3 pos for every entity's head position with a flag labeling dead or alive.
	//2) if dead, set flag to false, else keep as true
	
	EntityList
	for (int i = 0; i < numOfPlayersDeref; i++)
	{
		uintptr_t currEntPtr = firstEntityAddr;

		for (int j = 0; j < i; j++)
		{
			currEntPtr += spaceBetweenAddys;															//offset between each entity
		}
		ReadProcessMemory(hProc, (BYTE*)currEntPtr, &(currEntPtr), sizeof(currEntPtr), 0);

		//get the entity's X,Y, and Z positions
		uintptr_t currHeadPosXPtr = currEntPtr + XposAddr;
		uintptr_t currHeadPosYPtr = currEntPtr + XposAddr + 0x4;
		uintptr_t currHeadPosZPtr = currEntPtr + XposAddr + 0x8;

		vec3 headPos;

		ReadProcessMemory(hProc, (BYTE*)currHeadPosXPtr, &(headPos.x), sizeof(headPos.x), 0);
		ReadProcessMemory(hProc, (BYTE*)currHeadPosYPtr, &(headPos.y), sizeof(headPos.y), 0);
		ReadProcessMemory(hProc, (BYTE*)currHeadPosZPtr, &(headPos.z), sizeof(headPos.z), 0);

		//entListArr[i].vec = headPos;

	}
	
}*/