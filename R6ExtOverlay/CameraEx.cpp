#include <algorithm>
#include "CameraEx.h"

//these offsets change with each game. (See CameraEx.h as well)
CameraEx::CameraEx(HANDLE hProc, uintptr_t moduleBase, int windowWidth, int windowHeight)
{
	offsets = Offsets(hProc, moduleBase, windowWidth, windowHeight);

	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;
	//windowWidth = 1920;
	//windowHeight = 1080;
	//adjust your resolution of game here
	//windowHeight = *(__int32*)pResolution;
	//windowWidth = *(__int32*)(pResolution + 0x4);
	this->hProc = hProc;
	this->moduleBase = moduleBase;
	this->matrixStart = FindDMAAddy(hProc, moduleBase + 0x05EF12F8, { 0x918, 0xCC8, 0x140 });            // also try 0x13C or 0x140
	//this->localPlayerDeref = FindDMAAddy(hProc, moduleBase + 0x05EF12F8, { 0x918, 0xCC8, 0x0 });

	float value;
	uintptr_t floatAddr;
	uintptr_t currMatrixIndex = 0;
	//uintptr_t myMatrixDeref;
	//ReadProcessMemory(hProc, (BYTE*)matrix, &(myMatrixDeref), sizeof(myMatrixDeref), 0);

	//initialize the view matrix in this class... usually 16 float values all right next to each other in memory;
	for (int m = 0; m < 16; m++)
	{
		value = 0;
		floatAddr = 0;
		floatAddr = matrixStart + currMatrixIndex;
		ReadProcessMemory(hProc, (BYTE*)floatAddr, &(value), sizeof(value), 0);

		matrix[m] = value;
		currMatrixIndex += 0x4;
	}

	//uintptr_t fovxAddr = FindDMAAddy(hProc, moduleBase + 0x05EE8418, { 0x918, 0xCD0, 0x180 });
	//ReadProcessMemory(hProc, (BYTE*)fovxAddr, &fovx, sizeof(fovx), 0);
	
	//fovy = fovx / windowWidth * windowHeight;
}

vec3 CameraEx::WorldToScreen(vec3 pos)
{
	//calculations that require view matrix, our position, and an enemy xyz position
	vec3 screen;

	Vec4 clipCoords;

	//directx version
	//clipCoords.x = pos.x * matrix[0] + pos.y * matrix[1] + pos.z * matrix[2] + matrix[3];
	//clipCoords.y = pos.x * matrix[4] + pos.y * matrix[5] + pos.z * matrix[6] + matrix[7];
	//clipCoords.z = pos.x * matrix[8] + pos.y * matrix[9] + pos.z * matrix[10] + matrix[11];
	//clipCoords.w = pos.x * matrix[12] + pos.y * matrix[13] + pos.z * matrix[14] + matrix[15];

	//reversed directx
	//clipCoords.x = pos.x * matrix[15] + pos.y * matrix[14] + pos.z * matrix[13] + matrix[12];
	//clipCoords.y = pos.x * matrix[11] + pos.y * matrix[10] + pos.z * matrix[9] + matrix[8];
	//clipCoords.z = pos.x * matrix[7] + pos.y * matrix[6] + pos.z * matrix[5] + matrix[4];
	//clipCoords.w = pos.x * matrix[3] + pos.y * matrix[2] + pos.z * matrix[1] + matrix[0];

	//reversed opengl
	//clipCoords.x = pos.x * matrix[15] + pos.y * matrix[11] + pos.z * matrix[7] + matrix[3];
	//clipCoords.y = pos.x * matrix[14] + pos.y * matrix[10] + pos.z * matrix[6] + matrix[2];
	//clipCoords.z = pos.x * matrix[13] + pos.y * matrix[9] + pos.z * matrix[5] + matrix[1];
	//clipCoords.w = pos.x * matrix[12] + pos.y * matrix[8] + pos.z * matrix[4] + matrix[0];

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
	//ReadProcessMemory(hProc, (BYTE*)localPlayer, &(localPlayerDeref), sizeof(localPlayerDeref), 0);

	//uintptr_t currHeadPosXPtr = localPlayerDeref + 0x120;
	//uintptr_t currHeadPosYPtr = localPlayerDeref + 0x124;
	//uintptr_t currHeadPosZPtr = localPlayerDeref + 0x128;
	// 
	//vec3 locPlayerPos = vec3();
	// 
	//ReadProcessMemory(hProc, (BYTE*)currHeadPosXPtr, &(locPlayerPos.x), sizeof(locPlayerPos.x), 0);
	//ReadProcessMemory(hProc, (BYTE*)currHeadPosYPtr, &(locPlayerPos.y), sizeof(locPlayerPos.y), 0);
	//ReadProcessMemory(hProc, (BYTE*)currHeadPosZPtr, &(locPlayerPos.z), sizeof(locPlayerPos.z), 0);

	vec3 locPlayerPos = offsets.GetLocalPlayerPos();

	//then by getting the enemy entity's position which is passed as vec3 already
	//Then calculate distance from us and enemy and store in our vec3
	screen.z = locPlayerPos.Distance(pos);

	//returns the screenCoords
	return screen;
}

//this was casualGamer's approach of WorldToScreen function for Halo d3dx game. But his view matrix was different and wierd...
/*
Vector3 CameraEx::WorldToScreen(Vector3 pos)
{
	uintptr_t currHeadPosXPtr = localPlayerDeref + 0x120;
	uintptr_t currHeadPosYPtr = localPlayerDeref + 0x124;
	uintptr_t currHeadPosZPtr = localPlayerDeref + 0x128;

	vec3 locPlayerPos = vec3();

	ReadProcessMemory(hProc, (BYTE*)currHeadPosXPtr, &(locPlayerPos.x), sizeof(locPlayerPos.x), 0);
	ReadProcessMemory(hProc, (BYTE*)currHeadPosYPtr, &(locPlayerPos.y), sizeof(locPlayerPos.y), 0);
	ReadProcessMemory(hProc, (BYTE*)currHeadPosZPtr, &(locPlayerPos.z), sizeof(locPlayerPos.z), 0);

	//find distance length from me to enemy and normalize it
	float yFOV = fovx / windowWidth * windowHeight;
	//float yFOV = camera->fovx / windowWidth * windowHeight;
	D3DXVECTOR3 camToObj = { pos.x - locPlayerPos.x, pos.y - locPlayerPos.y, pos.z - locPlayerPos.z };
	float distToObj = sqrtf(camToObj.x * camToObj.x + camToObj.y * camToObj.y + camToObj.z * camToObj.z);
	D3DXVec3Normalize(&camToObj, &camToObj);

	float camYaw = atan2f(camera->lookAt.y, camera->lookAt.x);
	float objYaw = atan2f(camToObj.y, camToObj.x);
	float relYaw = camYaw - objYaw;

	if (relYaw > D3DX_PI)
		relYaw -= 2 * D3DX_PI;
	if (relYaw < D3DX_PI)
		relYaw += 2 * D3DX_PI;

	float objPitch = asin(camToObj.z);
	float camPitch = asin(camera->lookAt.z);
	float relPitch = camPitch - objPitch;

	float x = relYaw / (0.5 * fovx);
	float y = relPitch / (0.5 * yFOV);

	x = (x + 1) / 2;
	y = (y + 1) / 2;

	return { x * windowWidth, y * windowHeight, distToObj };
}
*/
/*
Vector3 CameraEx::WorldToScreen(Vector3 worldPosition)
{
	float yFOV = camera->fovx / windowWidth * windowHeight;
	D3DXVECTOR3 camToObj = { worldPosition.x - camera->position.x, worldPosition.y - camera->position.y, worldPosition.z - camera->position.z };
	float distToObj = sqrtf(camToObj.x * camToObj.x + camToObj.y * camToObj.y + camToObj.z * camToObj.z);
	D3DXVec3Normalize(&camToObj, &camToObj);

	float camYaw = atan2f(camera->lookAt.y, camera->lookAt.x);
	float objYaw = atan2f(camToObj.y, camToObj.x);
	float relYaw = camYaw - objYaw;

	if (relYaw > D3DX_PI)
		relYaw -= 2 * D3DX_PI;
	if (relYaw < D3DX_PI)
		relYaw += 2 * D3DX_PI;

	float objPitch = asin(camToObj.z);
	float camPitch = asin(camera->lookAt.z);
	float relPitch = camPitch - objPitch;

	float x = relYaw / (0.5 * camera->fovx);
	float y = relPitch / (0.5 * yFOV);

	x = (x + 1) / 2;
	y = (y + 1) / 2;

	return { x * windowWidth, y * windowHeight, distToObj };
}
*/

//static float fovx = 1.7777f; //0.426f glaz zoom
//static float fovy = 1.0f; //0.24f glaz zoom
/*
vec3 CameraEx::WorldToScreen(vec3 pos) 
{
	//if (!matrix[])
	//	return false;
	vec3 screen;

	uintptr_t currHeadPosXPtr = localPlayerDeref + 0x120;
	uintptr_t currHeadPosYPtr = localPlayerDeref + 0x124;
	uintptr_t currHeadPosZPtr = localPlayerDeref + 0x128;

	vec3 locPlayerPos = vec3();

	ReadProcessMemory(hProc, (BYTE*)currHeadPosXPtr, &(locPlayerPos.x), sizeof(locPlayerPos.x), 0);
	ReadProcessMemory(hProc, (BYTE*)currHeadPosYPtr, &(locPlayerPos.y), sizeof(locPlayerPos.y), 0);
	ReadProcessMemory(hProc, (BYTE*)currHeadPosZPtr, &(locPlayerPos.z), sizeof(locPlayerPos.z), 0);

	//point.x -= worldToScreen.m[3][0];
	//point.y -= worldToScreen.m[3][1];
	//point.z -= worldToScreen.m[3][2];
	//myMatrix[] = FindDMAAddy(hProc, moduleBase + 0x05EE8418, { 0x918, 0xCD0, 0x110 });

	//pos.x -= matrix[12];
	//pos.y -= matrix[13];
	//pos.z -= matrix[14];
	/*
	vec3 lookAt;
	lookAt.x = matrix[20];
	lookAt.y = matrix[24];
	lookAt.z = matrix[28];

	D3DXVECTOR3 camToObj = { pos.x - locPlayerPos.x, pos.y - locPlayerPos.y, pos.z - locPlayerPos.z };
	float distToObj = sqrtf(camToObj.x * camToObj.x + camToObj.y * camToObj.y + camToObj.z * camToObj.z);
	D3DXVec3Normalize(&camToObj, &camToObj);

	float camYaw = atan2f(lookAt.y, lookAt.x);
	float objYaw = atan2f(camToObj.y, camToObj.x);
	float relYaw = camYaw - objYaw;

	if (relYaw > D3DX_PI)
		relYaw -= 2 * D3DX_PI;
	if (relYaw < D3DX_PI)
		relYaw += 2 * D3DX_PI;

	float objPitch = asinf(camToObj.z);
	float camPitch = asinf(lookAt.z);
	float relPitch = camPitch - objPitch;

	float x = relYaw / (0.5f * fovx);
	float y = relPitch / (0.5f * fovy);

	x = (x + 1) / 2;
	y = (y + 1) / 2;

	return { x * windowWidth, y * windowHeight, distToObj };
	*/
	
	//need viewRight, viewUp, and viewForward offsets?
	//screen.x = worldToScreen.m[0][0] * point.x + worldToScreen.m[0][1] * point.y + worldToScreen.m[0][2] * point.z;
	//screen.y = worldToScreen.m[2][0] * point.x + worldToScreen.m[2][1] * point.y + worldToScreen.m[2][2] * point.z;
	//screen.z = worldToScreen.m[1][0] * point.x + worldToScreen.m[1][1] * point.y + worldToScreen.m[1][2] * point.z;
/*
	screen.x = matrix[15] * pos.x + matrix[16] * pos.y + matrix[17] * pos.z;
	screen.y = matrix[21] * pos.x + matrix[22] * pos.y + matrix[23] * pos.z;
	screen.z = matrix[18] * pos.x + matrix[19] * pos.y + matrix[20] * pos.z;

	screen.x /= screen.z * fovx;
	screen.y /= screen.z * fovy;

	if (screen.z < 0.001f)
	{
		return screen;
	}
	else
	{
		screen.x = (windowWidth / 2) + 0.5f * (screen.x * windowWidth) + 0.5f;
		screen.y = (windowHeight / 2) - 0.5f * (screen.y * windowHeight) + 0.5f;

		return screen;
	}
	
}
*/
//vec3 CameraEx::WorldToScreen(vec3 vOrigin) {
//	vec3 screen;
//	if (ScreenTransform(vOrigin, screen)) {
//		screen.x = (windowWidth / 2) + 0.5 * (screen.x * windowWidth) + 0.5;
//		screen.y = (windowHeight / 2) - 0.5 * (screen.y * windowHeight) + 0.5;
//		return true;
//	}
//	return screen;
//}
