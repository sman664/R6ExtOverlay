#pragma once
#include "geom.h"
#include "Windows.h"
#include "proc.h"
#include <d3dx9math.h>


class Offsets
{
public:
	//variables that we grab AUTOMATICALLY from the game
	HANDLE		hProc				=	0;
	int			width, height		=	0;
	uintptr_t	moduleBase			=	0;

	//variables that we find MANUALLY from the game...
	uintptr_t	entlist				=	0;
	uintptr_t	localPlayerDeref	=	0;
	int			numOfPlayersDeref	=	0;
	uintptr_t	matrixStart			=	0;
	
	uintptr_t	firstEntityAddr		=	0;
	uintptr_t	spaceBetweenAddys	=	0;
	uintptr_t	XposAddr			=	0;
	uintptr_t	XposAddr2			=   0;
	uintptr_t	XposAddr3			=	0;


	//constructors for initializing all the goodies above
	Offsets();
	Offsets(HANDLE hProc, uintptr_t moduleBase, int width, int height);
	
	//functions for modulization
	vec3 GetLocalPlayerPos();
	vec3 GetEntityHeadPos(int index);
	vec3 GetEntityGrouping(int index);
	vec3 GetEntityGrouping2(int index);
	void initEntList();

};

//struct Vec3WBool
//{
//	vec3 vec;
//	bool flag = true;
//};
//static struct EntityList
//{
//	static const int arrSize = 1000;
//	Vec3WBool entListArr[arrSize];
//};