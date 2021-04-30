#include <string>
#include "Paint.h"

Paint::Paint() {}

Paint::Paint(HANDLE hProc, uintptr_t moduleBase, HWND overlayHWND, HWND targetHWND, int width, int height)
{
	offsets = Offsets(hProc, moduleBase, width, height);

	this->width1			= width;
	this->height1			= height;
	this->TargetHWND		= targetHWND;
	this->hProc				= hProc;
	this->moduleBase		= moduleBase;
	init(overlayHWND);

}

int Paint::init(HWND hWND)
{
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &object)))
	{
		exit(1);
	}

	ZeroMemory(&params, sizeof(params));
	params.BackBufferWidth = width1;
	params.BackBufferHeight = height1;
	params.Windowed = true;
	params.hDeviceWindow = hWND;
	params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	params.BackBufferFormat = D3DFMT_A8R8G8B8;
	params.EnableAutoDepthStencil = TRUE;
	params.AutoDepthStencilFormat = D3DFMT_D16;

	object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWND, D3DCREATE_HARDWARE_VERTEXPROCESSING, &params, 0, &device);
	D3DXCreateFont(device, 50, 0, FW_BOLD, 1, false, DEFAULT_CHARSET, OUT_DEVICE_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, "Comic Sans", &font);

	return 0;
}

void Paint::drawText(char* string, int x, int y, int a, int r, int g, int b)
{
	RECT rect;
	rect.top = y;
	rect.left = x;
	font->DrawTextA(0, string, (INT)(strlen(string)), &rect, DT_NOCLIP, D3DCOLOR_ARGB(a, r, g, b));
}

int Paint::render()
{
	offsets = Offsets(hProc, moduleBase, width1, height1);
	
	//if device don't exist, don't beginscene bruh. Don't print anything holmes. DEVICE is the foundation for drawing the hacks
	if (device == nullptr)
		return 1;
	device->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
	device->BeginScene();

/*
	first argument is a number 0-255. (255 is full opacity. 0 is full transparency)
	last three are RGB format:
		yellow	(255,255,0)
		red		(255, 0, 0)
*/
	D3DCOLOR boxColor = D3DCOLOR_ARGB(255, 255, 255, 0); //yellow

	//anything you want drawn on screen, put in here
	if (TargetHWND == GetForegroundWindow())
	{
		//anything you want drawn put in here
		drawText((char*)"U got hacked broo", width1 / 10, height1 / 10, 255, 171, 0, 182);
		//Create mod menu on top left corner
		//drawFilledRectangle(menuX, menuY, menuWidth, menuHeight, D3DCOLOR_ARGB(120, 54, 162, 255));
		//Write anything below if you want to write something on your menu! 
		// string[] = {"Welcome to my ESP Hack", "Press numpad 0 for ESP"}
		//for (size_t i = 0; i < lines.size(); ++i) {
		//	d3D9Helper.drawText(lines.at(i), menuX + padding, menuY + padding + i * lineHeight, D3DCOLOR_ARGB(255, 153, 255, 153));
		//}
		Draw(hProc);
	}

	device->EndScene();
	device->PresentEx(0, 0, 0, 0, 0);

	return 0;
}


void Paint::Draw(HANDLE hProc)
{

	uintptr_t currEntPtr = 0;

	//starting from entlist offset check if entity exists then draw his/her rectangle
	for (int i = 0; i < offsets.numOfPlayersDeref; i++)
	{
		//start at the beginning of entity list, then loop till you get the next enemy/entity
		//ReadProcessMemory(hProc, (BYTE*)myEntlist, &(currEntPtr), sizeof(currEntPtr), 0);
		
		vec3 currEntPtrFeet = offsets.GetEntityHeadPos(i);
		vec3 currEntPtrTorso = currEntPtrFeet;

		//vec3 currEntSecondCoord = offsets.GetEntityGrouping(i);
		//vec3 currEntThirdCoord = offsets.GetEntityGrouping2(i);
		currEntPtrTorso.z = currEntPtrFeet.z - (PLAYER_HEIGHT - EYE_HEIGHT);

		if (IsValidEnt(currEntPtrFeet))
		{
			CameraEx cameraEx = CameraEx(hProc, moduleBase, width1, height1);
			Vector3 feetCoords = cameraEx.WorldToScreen(currEntPtrFeet);

			if (feetCoords.z < 100)
			{
				//get the screen coordinates of this enemy's torso (stored in x, y) and normalized distance (stored in torsoCoords.z)
				vec3 torsoCoords = cameraEx.WorldToScreen(currEntPtrTorso);
				/*
				vec3 secondCoord = cameraEx.WorldToScreen(currEntSecondCoord);
				vec3 thirdCoord = cameraEx.WorldToScreen(currEntThirdCoord);
				float x = torsoCoords.x;
				float y = torsoCoords.y;
				float x1 = secondCoord.x;
				float yy1 = secondCoord.y;
				float x2 = thirdCoord.x;
				float yy2 = thirdCoord.y;
				*/
				float scale  = (GAME_UNIT_MAGIC / torsoCoords.z) * (cameraEx.windowWidth / VIRTUAL_SCREEN_WIDTH);
				float x		 = torsoCoords.x - scale;
				float y		 = torsoCoords.y - scale * PLAYER_ASPECT_RATIO;
				float width	 = scale * 2;
				float height = scale * PLAYER_ASPECT_RATIO * 2;

				//float x1	 = secondCoord.x - scale;
				//float yy1	 = secondCoord.y - scale * PLAYER_ASPECT_RATIO;
				
				//GL::DrawOutline(x, y, width, height, 2.0f);
				/*
				Vector3 torsoCoords = cameraEx.WorldToScreen(currEntPtrTorso);
				float heightEntity = abs(feetCoords.y - torsoCoords.y) * 2;
				
				//you can print entity health or name here
				D3DCOLOR boxColor = D3DCOLOR_ARGB(255, 255, 255, 0); //yellow
				
				//d3D9Helper.drawRectangle(feetCoords.x - widthEntity / 2, torsoCoords.y - heightEntity / 2, widthEntity, heightEntity, boxColor);
				float widthEntity = heightEntity / 2;
				*/
				//you can print entity health or name here
				//print your menu with draw drawFilledRectangle() here

				//draw the rectangle at position so and so with origin on the screen at x, y pixels as origin
				
				D3DCOLOR boxColor = D3DCOLOR_ARGB(255, 255, 255, 0); //yellow
				drawRectangle(x, y, width, height, boxColor);

			}
		}
	}
}

bool Paint::IsValidEnt(vec3 ent)
{
	if (ent.x != 0.0f)
	{
		//if (ent.x == 0)
		//	//if ent has vtable of these addresses then it IS an entity
		//{
		//	return true;
		//}
		//ReadProcessMemory(hProc, (BYTE*)ent, &ent, sizeof(ent), 0);
		//if (ent == 0x4E4A98 ||
		//	ent == 0x4E4AC0)
			//if ent has vtable of these addresses then it IS an entity
		//{
		//	return true;
		//}
		return true;
	}
	return false;
}

void Paint::drawFilledRectangle(float x, float y, float width, float height, D3DCOLOR color) 
{
	D3DXVECTOR2 vertices[2] = { D3DXVECTOR2(x + width / 2, y),D3DXVECTOR2(x + width / 2, y + height) };
	if (!line)
		D3DXCreateLine(device, &line);
	line->SetWidth(width);
	line->Draw(vertices, 2, color);
}

void Paint::drawRectangle(float x, float y, float width, float height, D3DCOLOR color) 
{
	D3DXVECTOR2 rect[5];
	rect[0] = D3DXVECTOR2(x, y);
	rect[1] = D3DXVECTOR2(x + width, y);
	rect[2] = D3DXVECTOR2(x + width, y + height);
	rect[3] = D3DXVECTOR2(x, y + height);
	rect[4] = D3DXVECTOR2(x, y);
	if (!line) {
		D3DXCreateLine(device, &line);
	}
	line->SetWidth(1);
	line->Draw(rect, 5, color);
}

void Paint::drawLine(float x, float y, float x1, float yy1, D3DCOLOR color)
{
	D3DXVECTOR2 rect[2];
	rect[0] = D3DXVECTOR2(x, y);
	rect[1] = D3DXVECTOR2(x1, yy1);

	if (!line) {
		D3DXCreateLine(device, &line);
	}
	line->SetWidth(1);
	line->Draw(rect, 2, color);
}

void Paint::drawTriangle(float x, float y, float x1, float yy1, float x2, float yy2, D3DCOLOR color)
{
	D3DXVECTOR2 rect[4];
	rect[0] = D3DXVECTOR2(x, y);
	rect[1] = D3DXVECTOR2(x1, yy1);
	rect[2] = D3DXVECTOR2(x2, yy2);
	rect[3] = D3DXVECTOR2(x, y);

	if (!line) {
		D3DXCreateLine(device, &line);
	}
	line->SetWidth(1);
	line->Draw(rect, 4, color);
}
//void Paint::Rect(int x, int y, int l, int h, D3DCOLOR color)
//{
//	D3DRECT rect = { x, y, x + l, y + h };
//	pDevice->Clear(1, &rect, D3DCLEAR_TARGET, color, 0, 0);
//}
//void Paint::BorderBox(int x, int y, int l, int h, int thickness, D3DCOLOR color)
//{
//	this->Rect(x, y, l, thickness, color);
//	this->Rect(x, y, thickness, h, color);
//	this->Rect(x + l, y, thickness, h, color);
//	this->Rect(x, y + h, l + thickness, thickness, color);
//}