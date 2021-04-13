#include <Windows.h>
#include <Math.h>
#include "mem.h"
#include "geom.h"
#include "CameraEx.h"
#include "Offsets.h"

class Aimbot
{
public:
	int WINDOWWIDTH = 0;
	int WINDOWHEIGHT = 0;
	float PI = 3.141592f;

	HANDLE hProc = 0;
	//uintptr_t entlist;
	//uintptr_t localPlayer = (uintptr_t)0x50F4F4;
	//uintptr_t localPlayerDeref = 0;
	//uintptr_t numOfPlayers = 0x50f500;
	uintptr_t moduleBase = 0;
	Offsets offsets;

	//uintptr_t pitchAddr;
	//uintptr_t yawAddr;

	//int numOfPlayersDeref;

	Aimbot(HANDLE hProc, uintptr_t moduleBase, int width, int height);
	//vec3 GetLocalPlayerPos();
	vec3 GetBestEntity();
	void AimAt();
};