#include <string>
#include "Paint.h"

Paint::Paint() {}

Paint::Paint(HANDLE hProc, uintptr_t moduleBase, HWND overlayHWND, HWND targetHWND, int width, int height)
{
	this->width1 = width;
	this->height1 = height;
	this->TargetHWND = targetHWND;
	this->hProc = hProc;
	this->moduleBase = moduleBase;
	init(overlayHWND);

	this->entlist = FindDMAAddy(hProc, moduleBase + 0x05EF12F8, { 0x60 });
	//05EF12F8
	//this->entlist = moduleBase + 0x6ACEAB0;
	//uintptr_t entityBasePtr = moduleBase + 0x6ACEAB0;
	//ReadProcessMemory(hProc, (BYTE*)entityBasePtr, &(entlist), sizeof(entlist), 0);
	//ReadProcessMemory(hProc, (BYTE*)numOfPlayers, &(numOfPlayersDeref), sizeof(numOfPlayersDeref), 0);


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
	font->DrawTextA(0, string, strlen(string), &rect, DT_NOCLIP, D3DCOLOR_ARGB(a, r, g, b));
}

int Paint::render()
{
	//if device don't exist, don't beginscene bruh. Don't print anything holmes. DEVICE is the foundation for drawing the hacks
	if (device == nullptr)
		return 1;
	device->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
	device->BeginScene();

	//yellow (255,255,0)
	//red (255, 0, 0)
	//first argument is a number 0-255. (255 is full opacity. 0 is full transparency)
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

	//uintptr_t currEntlist;
	//ReadProcessMemory(hProc, (BYTE*)entlist, &currEntlist, sizeof(currEntlist), 0);
	//uintptr_t currEntPoints;
	//ReadProcessMemory(hProc, (BYTE*)currEntlist, &currEntPoints, sizeof(currEntPoints), 0);
	//uintptr_t myEntlist = entlist;
	uintptr_t currEntPtr = 0;

	//starting from entlist offset 0x50f4f8 (or moduleBase + 10f4f8) check if entity exists then draw his/her rectangle
	//check out ESP.cpp file in assaultcube external application I made.
	for (int i = 0; i < numOfPlayersDeref; i++)
	{
		//start at the beginning of entity list, then loop till you get the next enemy/entity
		//ReadProcessMemory(hProc, (BYTE*)myEntlist, &(currEntPtr), sizeof(currEntPtr), 0);
		currEntPtr = entlist;
		for (int j = 0; j < i; j++)
		{
			currEntPtr += 0x8;																	//offset between each entity
		}
		ReadProcessMemory(hProc, (BYTE*)currEntPtr, &(currEntPtr), sizeof(currEntPtr), 0);

		//get the entity's X,Y, and Z positions
		uintptr_t thisEntPtrXPos = currEntPtr + 0x30;
		uintptr_t thisEntPtrYPos = currEntPtr + 0x34;
		uintptr_t thisEntPtrZPos = currEntPtr + 0x38;

		Vector3 currEntPtrFeet = Vector3();
		Vector3 currEntPtrTorso = Vector3();

		ReadProcessMemory(hProc, (BYTE*)thisEntPtrXPos, &(currEntPtrFeet.x), sizeof(currEntPtrFeet.x), 0);
		ReadProcessMemory(hProc, (BYTE*)thisEntPtrYPos, &(currEntPtrFeet.y), sizeof(currEntPtrFeet.y), 0);
		ReadProcessMemory(hProc, (BYTE*)thisEntPtrZPos, &(currEntPtrFeet.z), sizeof(currEntPtrFeet.z), 0);

		currEntPtrTorso = currEntPtrFeet;
		//currEntPtrTorso.z = currEntPtrFeet.z - EYE_HEIGHT + PLAYER_HEIGHT / 2;
		currEntPtrTorso.z = currEntPtrFeet.z - EYE_HEIGHT + PLAYER_HEIGHT;

		if (IsValidEnt(hProc, currEntPtrFeet))
		{
			CameraEx cameraEx = CameraEx(hProc, moduleBase, width1, height1);
			Vector3 feetCoords = cameraEx.WorldToScreen(currEntPtrFeet);

			if (feetCoords.z < 100)
			{
				//get the screen coordinates of this enemy's torso (stored in x, y) and normalized distance (stored in torsoCoords.z)
				Vector3 torsoCoords = cameraEx.WorldToScreen(currEntPtrTorso);

				float scale = (GAME_UNIT_MAGIC / torsoCoords.z) * (cameraEx.windowWidth / VIRTUAL_SCREEN_WIDTH);
				float x = torsoCoords.x - scale;
				float y = torsoCoords.y - scale * PLAYER_ASPECT_RATIO;
				float width = scale * 2;
				float height = scale * PLAYER_ASPECT_RATIO * 2;

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
				
				//drawRectangle(feetCoords.x - widthEntity - 120, torsoCoords.y - heightEntity - 150, widthEntity, heightEntity, boxColor);
				//drawRectangle(feetCoords.x - widthEntity / 2, torsoCoords.y - heightEntity / 2, widthEntity, heightEntity, boxColor);
			}
		}
	}
}

bool Paint::IsValidEnt(HANDLE hProc, vec3 ent)
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

void Paint::drawFilledRectangle(float x, float y, float width, float height, D3DCOLOR color) {
	D3DXVECTOR2 vertices[2] = { D3DXVECTOR2(x + width / 2, y),D3DXVECTOR2(x + width / 2, y + height) };
	if (!line)
		D3DXCreateLine(device, &line);
	line->SetWidth(width);
	line->Draw(vertices, 2, color);
}

void Paint::drawRectangle(float x, float y, float width, float height, D3DCOLOR color) {
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