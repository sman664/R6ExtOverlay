#include "Aimbot.h"

Aimbot::Aimbot(HANDLE hProc, uintptr_t moduleBase, int width, int height)
{
	offsets = Offsets(hProc, moduleBase, width, height);

	this->hProc				=	hProc;
	this->moduleBase		=	moduleBase;
	this->WINDOWWIDTH		=	width;
	this->WINDOWHEIGHT		=	height;

}

vec3 Aimbot::GetBestEntity()
{
	vec3 myPos = offsets.GetLocalPlayerPos();
	vec3 bestEntPos;

	uintptr_t currEntPtr = 0;
	float leastDist = 0;
	int count = 0;

/* Steps:
	 1) loop through entity list 
	 2) find the one with the least distance (relative to crosshair or center of screen really)
	 3) return his XYZ head pos
	*** problems with this:  it does not distinguish between allies/enemies and alive/dead
*/
	for (int i = 0; i < offsets.numOfPlayersDeref; i++)
	{
		CameraEx cameraEx = CameraEx(hProc, moduleBase, WINDOWWIDTH, WINDOWHEIGHT);

		//start at the beginning of entity list, then loop till you get the next enemy/entity
		vec3 currEntHeadPos = offsets.GetEntityHeadPos(i);
		
		//find the target's XY screen coordinates...
		vec3 screenPos = cameraEx.WorldToScreen(currEntHeadPos);

		//... and my crosshair XY screen coordinates (a.k.a. center of window)...
		vec3 crossPos;
		crossPos.x = (float)	WINDOWWIDTH	 / 2;
		crossPos.y = (float)	WINDOWHEIGHT / 2;
		crossPos.z = 0.0f;

		//...find the distance (relative to crosshair or CoW) to the target
		float distRelCross = crossPos.Distance(screenPos);
		
		//here we set the global target w/ least distance from localplayer. (We will omit any NaNs i.e. null values to avoid breaking our aimbot)
		if (distRelCross > 0)
		{
			count++;
			bool firstEntity = false;
			
			if (count == 1)
			{
				firstEntity = true;
			}
			if (firstEntity == true || distRelCross < leastDist)
			{
				if (currEntHeadPos.x != 0 && currEntHeadPos.x != myPos.x)
				{
					leastDist = distRelCross;
					bestEntPos = currEntHeadPos;
				}

			}
		}


	}

	return bestEntPos;

}

void Aimbot::AimAt()
{
	CameraEx cameraEx = CameraEx(hProc, moduleBase, WINDOWWIDTH, WINDOWHEIGHT);

	vec3 target = GetBestEntity();

	//get center mass of player/entity (relative to feet position)
	//target.z = target.z + 0.80f;
	//target.z = target.z + 1.60f;

	vec3 point = cameraEx.WorldToScreen(target);

	INPUT Input = { 0 };
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_MOVE;
	Input.mi.dx = LONG	(point.x - (float)	WINDOWWIDTH  / 2);
	Input.mi.dy = LONG	(point.y - (float)	WINDOWHEIGHT / 2);
	SendInput(1, &Input, sizeof(INPUT));
}
/*
void Aimbot::AimAt()
{
	vec3 target = GetEntity();

	CameraEx cameraEx = CameraEx(hProc);

	float aX, aY;

	vec3 screen = cameraEx.WorldToScreen(target);
	//if (screen == {0, 0, 0})
	//	return;

	//float centerX = pRefdef->width / 2.f;
	//float centerY = pRefdef->height / 2.f;

	float enemyX = screen.x;
	float enemyY = screen.y;

	//aX = enemyX - centerX;
	//aY = enemyY - centerY;

	//if (bSmoothAim)
	//{
	//	aX /= fSmoothAmount;
	//	aY /= fSmoothAmount;
	//}

	mouse_event(MOUSEEVENTF_MOVE, -enemyX, enemyY, 0, 0);
}*/
/*void Aimbot::AimAt()
{
	vec3 myPos = GetLocalPlayerPos();
	vec3 target = GetEntity();
	//get the pitch and yaw

	vec3 deltaVec = { target.x - myPos.x, target.y - myPos.y, target.z - myPos.z };
	float deltaVecLength = sqrtf(deltaVec.x * deltaVec.x + deltaVec.y * deltaVec.y + deltaVec.z * deltaVec.z);


	if (deltaVecLength < 300 && deltaVec.x != 0)
	{
		float newPitch = asinf(deltaVec.z / deltaVecLength) * (180.0f / PI);
		float newYaw = atan2f(-deltaVec.x, deltaVec.y) * (180.0f / PI) + 180.0f;

		if (newPitch >= -89.0f && newPitch <= 89.0f)
		{
			//&& newYaw > 0.0f && newYaw < 360.0f
			mem::PatchEx((BYTE*)pitchAddr, (BYTE*)&newPitch, sizeof(newPitch), hProc);
			mem::PatchEx((BYTE*)yawAddr, (BYTE*)&newYaw, sizeof(newYaw), hProc);
		}
		//mem::PatchEx((BYTE*)pitchAddr, (BYTE*)&newPitch, sizeof(newPitch), hProc);
		//WriteProcessMemory(hProc, (BYTE*)pitchAddr, (BYTE*)&newPitch, sizeof(newPitch), nullptr);
	}

	//float newYaw = atan2f(deltaVec.y, deltaVec.x) * (180 / PI);
	//if (newYaw < 0)
	//{}

	//set the new pitch and yaw values in game memory
	//uintptr_t newPitchPtr = newPitch;
	//uintptr_t newYawPtr = newYaw;

	//WriteProcessMemory(hProc, (BYTE*)pitchAddr, (BYTE*)&newPitch, sizeof(newPitch), nullptr);
	//WriteProcessMemory(hProc, (BYTE*)yawAddr, (BYTE*)&newYaw, sizeof(newYaw), nullptr);
}*/