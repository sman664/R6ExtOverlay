#pragma once

#include <string>						//save error
#include <Windows.h>
#include "Offsets.h"

#include "geom.h"
#include "CameraEx.h"

#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include <DxErr.h>						//get error from error code
#pragma comment(lib, "dxerr.lib")
#pragma comment(lib, "legacy_stdio_definitions.lib")

const float PLAYER_HEIGHT = 5.25f;
const float PLAYER_WIDTH = 2.0f;			
const float EYE_HEIGHT = 4.6f;
//4.25f;
//1.75f;
//4.0f;

const float VIRTUAL_SCREEN_WIDTH = 1280; //this is the screen width you develop/test your hack with, I use same for every game
const float GAME_UNIT_MAGIC = 175; //magic scaling number you adjust so the boxes fit the players in your game
const float PLAYER_ASPECT_RATIO = PLAYER_HEIGHT / PLAYER_WIDTH;


class Paint
{
private:
	//variables we automatically grab from game
	HANDLE					hProc			= 0;
	HWND					TargetHWND;
	int						width1, 
							height1;
	uintptr_t				moduleBase		= 0;
	Offsets					offsets;

	//variables for creating d3d device to draw to
	IDirect3D9Ex*			object			= NULL;
	IDirect3DDevice9Ex*		device			= NULL;
	D3DPRESENT_PARAMETERS	params;
	ID3DXFont*				font			= 0;
	ID3DXLine*				line			= nullptr;

	float menuX = 35, menuWidth = 220, menuY = 100, menuHeight = 70;
	float padding = 10;
	float lineHeight = 20;

	int init(HWND hWND);
	void drawText(char* string, int x, int y, int a, int r, int g, int b);
	void drawFilledRectangle(float x, float y, float width, float height, D3DCOLOR color);
	void drawRectangle(float x, float y, float width, float height, D3DCOLOR color);
	void drawLine(float x, float y, float x1, float yy1, D3DCOLOR color);
	void drawTriangle(float x, float y, float x1, float yy1, float x2, float yy2, D3DCOLOR color);
	
public:
	Paint();
	Paint(HANDLE hProc, uintptr_t moduleBase ,HWND overlayHWND, HWND targetHWND, int width, int height);

	void Draw(HANDLE hProc);
	bool IsValidEnt(vec3 ent);

	int render();
};


