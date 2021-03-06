// Tetris.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Tetris.h"
#include "GameManager.h"

bool clearRows = false;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
	srand((unsigned int)time(nullptr));

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TETRIS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TETRIS));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
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

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TETRIS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TETRIS);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      200, 50, 750, 900, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

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
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int dropTime = 1000 - 100 * GameManager::getLevel();
	if (dropTime <= 250)
	{
		dropTime = 250;
	}

	switch (message)
	{
	case WM_CREATE:
		{
			GameManager::initialize();
			SetTimer(hWnd, 1, 1000, nullptr);
		}
		break;
	case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			// Parse the menu selections:
			switch (wmId)
			{
			case IDM_ABOUT:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
			case IDM_EXIT:
				DestroyWindow(hWnd);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
        break;
	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			GameManager::move(down);
			SetTimer(hWnd, 1, dropTime, nullptr);
			InvalidateRgn(hWnd, nullptr, true);
			break;
		}
		break;
	case WM_KEYDOWN:
		
		switch (wParam)
		{
		case VK_LEFT:
			GameManager::move(left);
			break;
		case VK_RIGHT:
			GameManager::move(right);
			break;
		case VK_DOWN:
			clearRows = GameManager::move(down);
			break;
		case VK_SPACE:	
			clearRows = GameManager::fallToBottom();
			break;
		case VK_UP:
			GameManager::rotate(right);
			break;
		case 'Z':
			GameManager::rotate(left);
			break;
		case 'C':
			GameManager::holdShape();
			break;
		}

		InvalidateRgn(hWnd, nullptr, true);
		break;
    case WM_PAINT:
        {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);

			RECT window;
			GetClientRect(hWnd, &window);
			int win_width = window.right - window.left;
			int win_height = window.bottom - window.top;

			HDC hdcCopy = CreateCompatibleDC(hdc);

			HBITMAP bitmap = CreateCompatibleBitmap(hdc, win_width, win_height);
			SelectObject(hdcCopy, bitmap);

			HBRUSH bckgrnd = CreateSolidBrush(RGB(255, 255, 255));
			FillRect(hdcCopy, &window, bckgrnd);
			DeleteObject(bckgrnd);

			DrawBlockDropPreview(hdcCopy);
			DrawBlocks(hdcCopy);
			DrawGrid(hdcCopy);
			DrawNextAndHeldShape(hdcCopy);
			DrawScore(hdcCopy);
			//DrawRowClear(hdcCopy);

			BitBlt(hdc, 0, 0, win_width, win_height, hdcCopy, 0, 0, SRCCOPY);
			DeleteObject(bitmap);
			DeleteDC(hdcCopy);
			DeleteDC(hdc);			

			EndPaint(hWnd, &ps);
        }
        break;
	case WM_ERASEBKGND:
		return true;
		break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void DrawGrid(HDC hdc)
{
	HPEN temp;

	COLORREF gridColor = RGB(120, 120, 120);
	HPEN gridPen = CreatePen(PS_SOLID, 1, gridColor);
	temp = (HPEN)SelectObject(hdc, gridPen);

	for (int i = 0; i < Grid::WIDTH + 1; i++)
	{
		MoveToEx(hdc, GRID_STARTING_X + GRID_TILE_SIZE * i, GRID_STARTING_Y, nullptr);
		LineTo(hdc, GRID_STARTING_X + GRID_TILE_SIZE * i, GRID_STARTING_Y + Grid::HEIGHT * GRID_TILE_SIZE);
	}

	for (int i = 0; i < Grid::HEIGHT + 1; i++)
	{
		MoveToEx(hdc, GRID_STARTING_X, GRID_STARTING_Y + GRID_TILE_SIZE * i, nullptr);
		LineTo(hdc, GRID_STARTING_X + Grid::WIDTH * GRID_TILE_SIZE, GRID_STARTING_Y + GRID_TILE_SIZE * i);
	}

	SelectObject(hdc, gridPen);
	DeleteObject(gridPen);
}

void DrawBlocks(HDC hdc)
{
	for (int i = 0; i < Grid::HEIGHT; i++)
	{
		for (int j = 0; j < Grid::WIDTH; j++)
		{
			if (Grid::grid[i][j].occupied)
			{
				RECT block;
				block.left = GRID_STARTING_X + GRID_TILE_SIZE * j;
				block.top = GRID_STARTING_Y + GRID_TILE_SIZE * (Grid::HEIGHT - 1 - i);
				block.right = block.left + GRID_TILE_SIZE;
				block.bottom = block.top + GRID_TILE_SIZE;

				HBRUSH brush = CreateSolidBrush(Grid::grid[i][j].color);
				FillRect(hdc, &block, brush);
				DeleteObject(brush);
			}
		}
	}
}

void DrawBlockDropPreview(HDC hdc)
{
	Block* preview = GameManager::getDropPreview();
	HBRUSH brush = CreateSolidBrush(preview->color);
	HBRUSH white = CreateSolidBrush(RGB(255, 255, 255));

	for (int i = 0; i < Shape::NUM_BLOCKS; i++)
	{
		RECT outer;
		outer.left = GRID_STARTING_X + GRID_TILE_SIZE * preview[i].xPos;
		outer.top = GRID_STARTING_Y + GRID_TILE_SIZE * (Grid::HEIGHT - 1 - preview[i].yPos);
		outer.right = outer.left + GRID_TILE_SIZE;
		outer.bottom = outer.top + GRID_TILE_SIZE;

		RECT inner;
		inner.left = GRID_STARTING_X + 3 + GRID_TILE_SIZE * preview[i].xPos;
		inner.top = GRID_STARTING_Y + 3 + GRID_TILE_SIZE * (Grid::HEIGHT - 1 - preview[i].yPos);
		inner.right = inner.left + 25;
		inner.bottom = inner.top + 25;

		FillRect(hdc, &outer, brush);
		FillRect(hdc, &inner, white);
	}

	delete[] preview;

	DeleteObject(brush);
	DeleteObject(white);
}

void DrawNextAndHeldShape(HDC hdc)
{
	Shape* nextShape = GameManager::getNextShape();
	Shape* heldShape = GameManager::getHeldShape();

	Block* nextShapeBlocks = nullptr;
	Block* heldShapeBlocks = nullptr;

	TextOut(hdc, 70, 10, L"Held Shape", 10);
	TextOut(hdc, GRID_STARTING_X + Grid::WIDTH * GRID_TILE_SIZE + 90, 10, L"Next Shape", 10);

	RECT blocks[Shape::NUM_BLOCKS];

	if (nextShape != nullptr)
	{
		nextShapeBlocks = nextShape->getBlocks();

		for (int i = 0; i < Shape::NUM_BLOCKS; i++)
		{
			switch (nextShapeBlocks->color)
			{
			case LIGHT_BLUE:
				blocks[0].left = GRID_STARTING_X + Grid::WIDTH * GRID_TILE_SIZE + 70;
				blocks[0].top = GRID_STARTING_Y;

				blocks[1].left = blocks[0].left + GRID_TILE_SIZE;
				blocks[1].top = blocks[0].top;

				blocks[2].left = blocks[1].left + GRID_TILE_SIZE;
				blocks[2].top = blocks[1].top;

				blocks[3].left = blocks[2].left + GRID_TILE_SIZE;
				blocks[3].top = blocks[2].top;
				break;
			case YELLOW:
				blocks[0].left = GRID_STARTING_X + Grid::WIDTH * GRID_TILE_SIZE + 70 + GRID_TILE_SIZE;
				blocks[0].top = GRID_STARTING_Y;

				blocks[1].left = blocks[0].left + GRID_TILE_SIZE;
				blocks[1].top = blocks[0].top;

				blocks[2].left = blocks[0].left;
				blocks[2].top = blocks[0].top + GRID_TILE_SIZE;

				blocks[3].left = blocks[1].left;
				blocks[3].top = blocks[2].top;
				break;
			case ORANGE:
				blocks[0].left = GRID_STARTING_X + Grid::WIDTH * GRID_TILE_SIZE + 55 + GRID_TILE_SIZE;
				blocks[0].top = GRID_STARTING_Y + GRID_TILE_SIZE;

				blocks[1].left = blocks[0].left + GRID_TILE_SIZE;
				blocks[1].top = blocks[0].top;

				blocks[2].left = blocks[1].left + GRID_TILE_SIZE;
				blocks[2].top = blocks[1].top;

				blocks[3].left = blocks[2].left;
				blocks[3].top = blocks[2].top - GRID_TILE_SIZE;
				break;
			case BLUE:
				blocks[0].left = GRID_STARTING_X + Grid::WIDTH * GRID_TILE_SIZE + 55 + GRID_TILE_SIZE;
				blocks[0].top = GRID_STARTING_Y + GRID_TILE_SIZE;

				blocks[1].left = blocks[0].left + GRID_TILE_SIZE;
				blocks[1].top = blocks[0].top;

				blocks[2].left = blocks[1].left + GRID_TILE_SIZE;
				blocks[2].top = blocks[1].top;

				blocks[3].left = blocks[0].left;
				blocks[3].top = blocks[0].top - GRID_TILE_SIZE;
				break;
			case GREEN:
				blocks[0].left = GRID_STARTING_X + Grid::WIDTH * GRID_TILE_SIZE + 55 + GRID_TILE_SIZE;
				blocks[0].top = GRID_STARTING_Y + GRID_TILE_SIZE;

				blocks[1].left = blocks[0].left + GRID_TILE_SIZE;
				blocks[1].top = blocks[0].top;

				blocks[2].left = blocks[1].left;
				blocks[2].top = blocks[1].top - GRID_TILE_SIZE;

				blocks[3].left = blocks[2].left + GRID_TILE_SIZE;
				blocks[3].top = blocks[2].top;
				break;
			case RED:
				blocks[0].left = GRID_STARTING_X + Grid::WIDTH * GRID_TILE_SIZE + 55 + GRID_TILE_SIZE;
				blocks[0].top = GRID_STARTING_Y;

				blocks[1].left = blocks[0].left + GRID_TILE_SIZE;
				blocks[1].top = blocks[0].top;

				blocks[2].left = blocks[1].left;
				blocks[2].top = blocks[1].top + GRID_TILE_SIZE;

				blocks[3].left = blocks[2].left + GRID_TILE_SIZE;
				blocks[3].top = blocks[2].top;
				break;
			case PURPLE:
				blocks[0].left = GRID_STARTING_X + Grid::WIDTH * GRID_TILE_SIZE + 55 + GRID_TILE_SIZE;
				blocks[0].top = GRID_STARTING_Y + GRID_TILE_SIZE;

				blocks[1].left = blocks[0].left + GRID_TILE_SIZE;
				blocks[1].top = blocks[0].top;

				blocks[2].left = blocks[1].left + GRID_TILE_SIZE;
				blocks[2].top = blocks[1].top;

				blocks[3].left = blocks[1].left;
				blocks[3].top = blocks[1].top - GRID_TILE_SIZE;
				break;
			}
		}

		HBRUSH brush = CreateSolidBrush(nextShapeBlocks->color);

		for (int i = 0; i < Shape::NUM_BLOCKS; i++)
		{
			blocks[i].right = blocks[i].left + GRID_TILE_SIZE;
			blocks[i].bottom = blocks[i].top + GRID_TILE_SIZE;
			FillRect(hdc, &blocks[i], brush);
		}

		DeleteObject(brush);
		delete nextShapeBlocks;
	}

	if (heldShape != nullptr)
	{
		heldShapeBlocks = heldShape->getBlocks();

		for (int i = 0; i < Shape::NUM_BLOCKS; i++)
		{
			switch (heldShapeBlocks->color)
			{
			case LIGHT_BLUE:
				blocks[0].left = 50;
				blocks[0].top = GRID_STARTING_Y;

				blocks[1].left = blocks[0].left + GRID_TILE_SIZE;
				blocks[1].top = blocks[0].top;

				blocks[2].left = blocks[1].left + GRID_TILE_SIZE;
				blocks[2].top = blocks[1].top;

				blocks[3].left = blocks[2].left + GRID_TILE_SIZE;
				blocks[3].top = blocks[2].top;
				break;
			case YELLOW:
				blocks[0].left = 50 + GRID_TILE_SIZE;
				blocks[0].top = GRID_STARTING_Y;

				blocks[1].left = blocks[0].left + GRID_TILE_SIZE;
				blocks[1].top = blocks[0].top;

				blocks[2].left = blocks[0].left;
				blocks[2].top = blocks[0].top + GRID_TILE_SIZE;

				blocks[3].left = blocks[1].left;
				blocks[3].top = blocks[2].top;
				break;
			case ORANGE:
				blocks[0].left = 35 + GRID_TILE_SIZE;
				blocks[0].top = GRID_STARTING_Y + GRID_TILE_SIZE;

				blocks[1].left = blocks[0].left + GRID_TILE_SIZE;
				blocks[1].top = blocks[0].top;

				blocks[2].left = blocks[1].left + GRID_TILE_SIZE;
				blocks[2].top = blocks[1].top;

				blocks[3].left = blocks[2].left;
				blocks[3].top = blocks[2].top - GRID_TILE_SIZE;
				break;
			case BLUE:
				blocks[0].left = 35 + GRID_TILE_SIZE;
				blocks[0].top = GRID_STARTING_Y + GRID_TILE_SIZE;

				blocks[1].left = blocks[0].left + GRID_TILE_SIZE;
				blocks[1].top = blocks[0].top;

				blocks[2].left = blocks[1].left + GRID_TILE_SIZE;
				blocks[2].top = blocks[1].top;

				blocks[3].left = blocks[0].left;
				blocks[3].top = blocks[0].top - GRID_TILE_SIZE;
				break;
			case GREEN:
				blocks[0].left = 35 + GRID_TILE_SIZE;
				blocks[0].top = GRID_STARTING_Y + GRID_TILE_SIZE;

				blocks[1].left = blocks[0].left + GRID_TILE_SIZE;
				blocks[1].top = blocks[0].top;

				blocks[2].left = blocks[1].left;
				blocks[2].top = blocks[1].top - GRID_TILE_SIZE;

				blocks[3].left = blocks[2].left + GRID_TILE_SIZE;
				blocks[3].top = blocks[2].top;
				break;
			case RED:
				blocks[0].left = 35 + GRID_TILE_SIZE;
				blocks[0].top = GRID_STARTING_Y;

				blocks[1].left = blocks[0].left + GRID_TILE_SIZE;
				blocks[1].top = blocks[0].top;

				blocks[2].left = blocks[1].left;
				blocks[2].top = blocks[1].top + GRID_TILE_SIZE;

				blocks[3].left = blocks[2].left + GRID_TILE_SIZE;
				blocks[3].top = blocks[2].top;
				break;
			case PURPLE:
				blocks[0].left = 35 + GRID_TILE_SIZE;
				blocks[0].top = GRID_STARTING_Y + GRID_TILE_SIZE;

				blocks[1].left = blocks[0].left + GRID_TILE_SIZE;
				blocks[1].top = blocks[0].top;

				blocks[2].left = blocks[1].left + GRID_TILE_SIZE;
				blocks[2].top = blocks[1].top;

				blocks[3].left = blocks[1].left;
				blocks[3].top = blocks[1].top - GRID_TILE_SIZE;
				break;
			}
		}

		HBRUSH brush = CreateSolidBrush(heldShapeBlocks->color);

		for (int i = 0; i < Shape::NUM_BLOCKS; i++)
		{
			blocks[i].right = blocks[i].left + GRID_TILE_SIZE;
			blocks[i].bottom = blocks[i].top + GRID_TILE_SIZE;
			FillRect(hdc, &blocks[i], brush);
		}

		DeleteObject(brush);
		delete heldShapeBlocks;
	}
}

void DrawScore(HDC hdc)
{
	std::wstring scoreString = std::to_wstring(GameManager::getScore());
	const wchar_t* score = scoreString.c_str();

	int sc = GameManager::getScore();
	int scoreStrLength = 1;
	while (sc >= 10)
	{
		sc /= 10;
		scoreStrLength++;
	}

	TextOut(hdc, 70, 150, L"Score:", 6);
	TextOut(hdc, 130, 150, score, scoreStrLength);
}

void DrawRowClear(HDC hdc)
{
	if (clearRows)
	{
		Sleep(500);
		GameManager::clearRows();
		clearRows = false;
	}
	if (GameManager::atBottom)
	{
		delete GameManager::getCurrentShape();
		
		GameManager::placeShape();
		GameManager::atBottom = false;
	}
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
