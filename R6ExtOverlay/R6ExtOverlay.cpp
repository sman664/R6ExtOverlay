// externOverlayDirectX.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <vector>
#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include "framework.h"
#include "Paint.h"
#include "R6ExtOverlay.h"
#include "proc.h"
#include "mem.h"
#include "CameraEx.h"
#include "geom.h"
#include "Aimbot.h"
#include "Offsets.h"

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR overlayTitle[50] = L"Overlay";                  // The title bar text (does not show since window is transparent in hack)
LPCSTR targetTitle = "Rainbow Six";                 //target window's name goes here
int width, height;
HWND overlayHWND, targetHWND;
Paint paint;

//instance of helper class
//D3D9Helper d3D9Helper;

HANDLE hProcess = 0;
uintptr_t moduleBase = 0;
//uintptr_t* entlist = (uintptr_t*)(0x50f4f8);

//const float PLAYER_HEIGHT = 5.25f;
//const float PLAYER_WIDTH = 2.0f;
//const float EYE_HEIGHT = 4.5f;

//Create an instance of Camera every loop (passing all your final dereferenced value) so your values being read in are always up to date
//Also remember to make a Draw() function. This gets called in the main loop and checks if the entity exists, entity list exists and prints them on the screen using WorldToScreen function
//Pass the entlist offset with handle to the process and you can iterate through entity list



// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Get Handle to game process and store base address
    DWORD procId = GetProcId(L"RainbowSix.exe");                      //name of process file or whatever client its called goes here

    if (procId)
    {
        //Open process returning 0. Looks like it's not getting the processes handle correctly
        //Get Handle to Process
        hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

        //Getmodulebaseaddress
        moduleBase = GetModuleBaseAddress(procId, L"RainbowSix.exe");
        //hProcess = hProcess;
    }

    MyRegisterClass(hInstance);

    //Get window of game and attach
    targetHWND = FindWindowA(0, targetTitle);  //Find the target game's window
    if (targetHWND)
    {
        RECT rect;
        GetWindowRect(targetHWND, &rect);
        width = rect.right - rect.left;
        height = rect.bottom - rect.top;
    }
    else
        return FALSE;

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }
    //Offsets offsets = Offsets(hProcess, moduleBase, width, height);
    //offsets.initEntList();
    paint = Paint(hProcess, moduleBase, overlayHWND, targetHWND, width, height);
    bool aimbotBool = false;
    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (GetAsyncKeyState(VK_MBUTTON) & 1)
        {
            aimbotBool = !aimbotBool;

        }
        if (aimbotBool)
        {
            Aimbot aimbot = Aimbot(hProcess, moduleBase, width, height);
            aimbot.AimAt();
        }


        RECT rect;
        GetWindowRect(targetHWND, &rect);
        width = rect.right - rect.left;
        height = rect.bottom - rect.top;

        MoveWindow(overlayHWND, rect.left, rect.top, width, height, true);    //keeps our overlay matching the target window location and size

    }

    return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = 0;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));   //set background as black
    wcex.lpszMenuName = overlayTitle;
    wcex.lpszClassName = overlayTitle;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    overlayHWND = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED, overlayTitle, overlayTitle, WS_POPUP,
        1, 1, width, height, nullptr, nullptr, hInstance, nullptr);

    if (!overlayHWND)
    {
        return FALSE;
    }
    SetLayeredWindowAttributes(overlayHWND, RGB(0, 0, 0), 0, LWA_COLORKEY);

    ShowWindow(overlayHWND, nCmdShow);

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
// external window for hacking does not need to take any commands
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
    {
        //this is where we call the function to render the boxes... 
        paint.render();
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
