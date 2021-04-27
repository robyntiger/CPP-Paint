#include <iostream>
#include <vector>
#include <Windows.h>
#include <CommCtrl.h>
#include "tools.h"
#include "resource.h"
#include "dialogues.h"
using namespace std;

// common controls
#pragma comment(lib, "comctl32.lib")

// variable declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK canvasProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK CanvasProc(HWND, UINT, WPARAM, LPARAM);

bool penselected = FALSE;
bool opened = FALSE;
bool eraserselected = FALSE;
bool lineselected = FALSE;
bool pickerselected = FALSE;
bool circleselected = FALSE;
bool squareselected = FALSE;
int penSizeVal = 1;
HBITMAP hBmp = NULL;
COLORREF gColour = RGB(255, 255, 255);

// HWND's for the windows
HWND hwnd1;
HWND canvasWin;

// win32 version of main
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR szCmdLine, _In_ int nCmdShow) {
	// register window class
	MSG msg;
	WNDCLASSW wc;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.lpszClassName = L"Cpp Paint";
	wc.hInstance = GetModuleHandle(NULL);
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpszMenuName = NULL;
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);

	// canvas window class
	WNDCLASSW childWc;

	childWc.style = CS_HREDRAW | CS_VREDRAW | CS_NOCLOSE;
	childWc.cbClsExtra = 0;
	childWc.cbWndExtra = 0;
	childWc.lpszClassName = L"Chile";
	childWc.hInstance = GetModuleHandle(NULL);
	childWc.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
	childWc.lpszMenuName = NULL;
	childWc.lpfnWndProc = canvasProc;
	childWc.hCursor = LoadCursor(NULL, IDC_ARROW);
	childWc.hIcon = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassW(&wc);
	RegisterClassW(&childWc);

	// create window
	hwnd1 = CreateWindowW(wc.lpszClassName, L"Window", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL, NULL, hInstance, NULL);
	ShowWindow(hwnd1, nCmdShow);
	UpdateWindow(hwnd1);

	// create child window inside for the canvas
	canvasWin = CreateWindowW(childWc.lpszClassName, NULL, WS_OVERLAPPEDWINDOW | WS_CHILD | WS_VISIBLE, 31, 0, canvasWidth, canvasWidth, hwnd1, NULL, hInstance, NULL);
	SetWindowLong(canvasWin, GWL_STYLE, 0); // removes window style (to remove bar)
	SetWindowPos(canvasWin, hwnd1, 31, 0, canvasWidth, canvasHeight, NULL);// change window style
	ShowWindow(canvasWin, nCmdShow);

	// message loop
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

fileSaving winDialogue;

// window procedure main
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CREATE:
	{
		addMenus(hwnd);
		CreateToolbar(hwnd);
		break;
	}
	case WM_COMMAND:
	{
		// if user selects any tools
		// pen
		if (LOWORD(wParam) == IDM_PEN) {
			eraserselected = FALSE;
			lineselected = FALSE;
			penselected = TRUE;
			pickerselected = FALSE;
			circleselected = FALSE;
			squareselected = FALSE;
		}
		else if (LOWORD(wParam) == IDM_ERASER) {
			eraserselected = TRUE;
			lineselected = FALSE;
			penselected = FALSE;
			pickerselected = FALSE;
			circleselected = FALSE;
			squareselected = FALSE;
		}
		else if (LOWORD(wParam) ==  IDM_LINE) {
			eraserselected = FALSE;
			lineselected = TRUE;
			penselected = FALSE;
			pickerselected = FALSE;
			circleselected = FALSE;
			squareselected = FALSE;
		}
		else if (LOWORD(wParam) == IDM_COLOUR_DROPPER) {
			pickerselected = TRUE;
			eraserselected = FALSE;
			lineselected = FALSE;
			penselected = FALSE;
			circleselected = FALSE;
			squareselected = FALSE;
		}
		else if (LOWORD(wParam) == IDM_CIRCLE) {
			circleselected = TRUE;
			pickerselected = FALSE;
			eraserselected = FALSE;
			lineselected = FALSE;
			penselected = FALSE;
			squareselected = FALSE;
		}
		else if (LOWORD(wParam) == IDM_SQUARE) {
			squareselected = TRUE;
			circleselected = FALSE;
			pickerselected = FALSE;
			eraserselected = FALSE;
			lineselected = FALSE;
			penselected = FALSE;
		}

		// if user selects anything from top menu
		else {
			switch (LOWORD(wParam)) {
			case IDM_FILE_NEW:
			{
				// clears screen
				// makes sure to ask the user if they want to do that first though
				int cont = MessageBox(hwnd, L"Are you sure you want to create a new drawing? This will delete your previous work if not saved!", L"Continue?", MB_YESNO);
				if (cont == 6) {
					// clear all the variables
					clearCanvas(hwnd);
				}
				else {
					// otherwise do nothing
				}
				break;
			}
			case IDM_FILE_OPEN:
			{
				// opens existing drawing
				winDialogue.openDialogue(hwnd);
				break;
			}
			case IDM_FILE_SAVE:
			{
				winDialogue.saveFile(hwnd);
				break;
			}
			case IDM_FILE_SAVE_AS:
			{
				winDialogue.fileDialogue(hwnd);
				break;
			}
			case IDM_FILE_QUIT:
			{
				SendMessage(hwnd, WM_CLOSE, 0, 0);
				break;
			}
			// selecting colour from tools
			case IDM_COLOUR:
			{
				gColour = ColourDialogue(hwnd);
				break;

			}
			case IDM_PEN_OPTIONS: {
				penSize(hwnd);
				break;
			}
			case IDM_CANVAS_SIZE: {
				canvasSize(hwnd);
				break;
			}
			}
		}

		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	default:
		return DefWindowProcW(hwnd, msg, wParam, lParam);
	}
	return 0;
}

// window procedure canvas
// also allows user to draw
LRESULT CALLBACK canvasProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	//tool test;
	tool winTools;
	if (penselected) {
		winTools.pen(msg, hwnd);
	}
	else if (eraserselected) {
		winTools.eraser(msg, hwnd);
	}
	else if (lineselected) {
		winTools.line(msg, hwnd);
	}
	else if (pickerselected) {
		winTools.colourPicker(msg, hwnd);
	}
	else if (circleselected) {
		winTools.circle(msg, hwnd);
	}
	else if (squareselected) {
		winTools.square(msg, hwnd);
	}
	else {
	}

	if (opened) {
		// makes image display instantly when opened
		PAINTSTRUCT ps;
		winTools.draw(hwnd, ps);
		opened = FALSE;
	}

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}