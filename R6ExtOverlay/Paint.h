#pragma once

#include <string> //save error
#include <Windows.h>

//#include "D3D9Helper.h"
#include "geom.h"
#include "CameraEx.h"

#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include <DxErr.h> //get error from error code
#pragma comment(lib, "dxerr.lib")
#pragma comment(lib, "legacy_stdio_definitions.lib")

const float PLAYER_HEIGHT = 5.25f;
//const float PLAYER_HEIGHT = 4.25f;
const float PLAYER_WIDTH = 2.0f;			
//const float PLAYER_WIDTH = 1.75f;
const float EYE_HEIGHT = 4.5f;
//const float EYE_HEIGHT = 4.0f;

const float VIRTUAL_SCREEN_WIDTH = 1280; //this is the screen width you develop/test your hack with, I use same for every game
const float GAME_UNIT_MAGIC = 350; //magic scaling number you adjust so the boxes fit the players in your game
const float PLAYER_ASPECT_RATIO = PLAYER_HEIGHT / PLAYER_WIDTH;


//const uintptr_t numOfPlayers = 0x50f500;
//const int numOfPlayers = 17;

class Paint
{
private:
	IDirect3D9Ex* object = NULL;
	IDirect3DDevice9Ex* device = NULL;
	D3DPRESENT_PARAMETERS params;
	ID3DXFont* font = 0;
	ID3DXLine* line = nullptr;
	HWND TargetHWND;
	int width1, height1;
	uintptr_t entlist = 0;
	int numOfPlayersDeref = 30;
	uintptr_t moduleBase = 0;
	

	HANDLE hProc = 0;

	float menuX = 35, menuWidth = 220, menuY = 100, menuHeight = 70;
	float padding = 10;
	float lineHeight = 20;

	int init(HWND hWND);
	void drawText(char* string, int x, int y, int a, int r, int g, int b);
	void drawFilledRectangle(float x, float y, float width, float height, D3DCOLOR color);
	void drawRectangle(float x, float y, float width, float height, D3DCOLOR color);

public:
	Paint();
	Paint(HANDLE hProc, uintptr_t moduleBase ,HWND overlayHWND, HWND targetHWND, int width, int height);
	void Draw(HANDLE hProc);
	bool IsValidEnt(HANDLE hProc, vec3 ent);

	int render();
};


