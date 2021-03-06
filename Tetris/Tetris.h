#pragma once

#include "resource.h"

#define MAX_LOADSTRING 100

#define GRID_STARTING_X 200
#define GRID_STARTING_Y 50
#define GRID_TILE_SIZE 30

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void DrawGrid(HDC hdc);
void DrawBlocks(HDC hdc);
void DrawBlockDropPreview(HDC hdc);
void DrawNextAndHeldShape(HDC hdc);
void DrawScore(HDC hdc);
void DrawRowClear(HDC hdc);