#pragma once

#include <string>
#include "geom.h"
#include "proc.h"
#include <d3dx9math.h>
#include "Offsets.h"


class CameraEx
{
public:
	HANDLE		hProc			=	0;
	int			windowWidth		=	0, 
				windowHeight	=	0;
	float		matrix			 [16];
	Offsets		offsets;

	uintptr_t	moduleBase		=	0;
	uintptr_t	matrixStart		=	0;

	CameraEx(HANDLE hProc, uintptr_t moduleBase, int windowWidth, int windowHeight);

	vec3 WorldToScreen(vec3 pos);
};
