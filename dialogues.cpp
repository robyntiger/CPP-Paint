#include <iostream>
#include <vector>
#include <Windows.h>
#include <CommCtrl.h>
#include "resource.h"
#include "dialogues.h"
#include <stdio.h>
#include <cstdio>
#include "tools.h"
using namespace std;

// common controls
#pragma comment(lib, "comctl32.lib")

// trackbar for pen size
HWND hTrack;
HWND hlbl;

// height and width for canvas
int canvasWidth = 500;
int canvasHeight = 500;

// bitmaps for loadding images
HBITMAP hBitmap;
BITMAP  bitmapInfo;

// Adds menus to parent window
void addMenus(HWND hwnd) {
	HMENU hMenuBar;
	HMENU hMenu;

	hMenuBar = CreateMenu();
	hMenu = CreateMenu();

	// file options
	AppendMenuW(hMenu, MF_STRING, IDM_FILE_NEW, L"&New");
	AppendMenuW(hMenu, MF_STRING, IDM_FILE_OPEN, L"&Open");
	AppendMenuW(hMenu, MF_STRING, IDM_FILE_SAVE, L"&Save");
	AppendMenuW(hMenu, MF_STRING, IDM_FILE_SAVE_AS, L"&Save As");
	AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
	AppendMenuW(hMenu, MF_STRING, IDM_FILE_QUIT, L"&Quit");

	AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hMenu, L"&File");

	// options options
	HMENU optionsMenu = CreateMenu();
	AppendMenuW(optionsMenu, MF_STRING, IDM_CANVAS_SIZE, L"&Canvas Size");
	AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)optionsMenu, L"&Options");

	// tool options
	HMENU toolMenu = CreateMenu();
	AppendMenuW(toolMenu, MF_STRING, IDM_COLOUR, L"&Colour");
	AppendMenuW(toolMenu, MF_STRING, IDM_PEN_OPTIONS, L"&Pen Options");
	AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)toolMenu, L"&Tools");

	SetMenu(hwnd, hMenuBar);
}

// creates the vertical toolbar
HWND CreateToolbar(HWND hwnd) {
	// define buttons
	TBBUTTON tbb[6] = {
		{0, IDM_PEN ,TBSTATE_ENABLED | TBSTATE_WRAP, BTNS_BUTTON, {0}, 0L, 0},
		{1, IDM_ERASER ,TBSTATE_ENABLED | TBSTATE_WRAP, BTNS_BUTTON, {0}, 0L, 0},
		{2, IDM_LINE ,TBSTATE_ENABLED | TBSTATE_WRAP, BTNS_BUTTON, {0}, 0L, 0},
		{3, IDM_COLOUR_DROPPER ,TBSTATE_ENABLED | TBSTATE_WRAP, BTNS_BUTTON, {0}, 0L, 0},
		{4, IDM_CIRCLE ,TBSTATE_ENABLED | TBSTATE_WRAP, BTNS_BUTTON, {0}, 0L, 0},
		{5, IDM_SQUARE ,TBSTATE_ENABLED | TBSTATE_WRAP, BTNS_BUTTON, {0}, 0L, 0},
	};

	// create toolbar window
	HWND hwndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
		WS_CHILD | WS_VISIBLE | CCS_VERT | WS_BORDER, 0, 0, 0, 0,
		hwnd, 0, GetModuleHandle(NULL), NULL);

	// Create the image list
	HIMAGELIST hImageList = ImageList_LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP1), 24, 4, RGB(255, 255, 255));

	// set image list
	SendMessage(hwndToolbar, TB_SETIMAGELIST, (WPARAM)0, (LPARAM)hImageList);

	// Load the button images.
	SendMessage(hwndToolbar, TB_LOADIMAGES, (WPARAM)IDB_STD_LARGE_COLOR, (LPARAM)HINST_COMMCTRL);

	// Add them to the toolbar
	SendMessage(hwndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	SendMessage(hwndToolbar, TB_ADDBUTTONS, 6, (LPARAM)&tbb);

	return hwndToolbar;
}

// creates the colour dialogue box
COLORREF ColourDialogue(HWND hwnd) {
	CHOOSECOLOR cc;
	static COLORREF crCustClr[16];

	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = hwnd;
	cc.lpCustColors = (LPDWORD)crCustClr;
	cc.rgbResult = RGB(0, 255, 0);
	cc.Flags = CC_RGBINIT;
	ChooseColor(&cc);

	return cc.rgbResult;
}

// window procedure dialogue for pen size slider
LRESULT CALLBACK DialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_HSCROLL: {
		updateTrackbar();
		break;
	}
	case WM_DESTROY:
	{
		break;
	}
	default:
		return DefWindowProcW(hwnd, msg, wParam, lParam);
	}
	return 0;
}

// creates dialogue window to let user choose pen size
void penSize(HWND hwnd) {
	// register the win class
	WNDCLASSEXW pw = { 0 };
	pw.cbSize = sizeof(WNDCLASSEXW);
	pw.lpfnWndProc = (WNDPROC)DialogProc;
	pw.hInstance = GetModuleHandle(NULL);
	pw.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	pw.lpszClassName = L"DialogClass";
	RegisterClassExW(&pw);

	// display dialogue box
	HWND penWin = CreateWindowExW(WS_EX_DLGMODALFRAME | WS_EX_TOPMOST, L"DialogClass", L"Pen Options",
		WS_VISIBLE | WS_SYSMENU | WS_CAPTION, 100, 100, 340, 100,
		NULL, NULL, GetModuleHandle(NULL), NULL);

	createTrackbar(penWin);
}

// creates the trackbar
void createTrackbar(HWND hwnd) {
	// win class
	WNDCLASSW wc = { 0 };
	wc.lpszClassName = L"Trackbar";
	wc.hInstance = GetModuleHandle(NULL);
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpfnWndProc = DialogProc;
	wc.hCursor = LoadCursor(0, IDC_ARROW);

	RegisterClassW(&wc);

	// actual trackbar
	HWND trackbar = CreateWindowW(wc.lpszClassName, L"Trackbar",
		WS_CHILD | WS_VISIBLE, 20, 0, 350, 180, hwnd, 0, GetModuleHandle(NULL), 0);

	// labels
	HWND hLeftLabel = CreateWindowW(L"Static", L"0",
		WS_CHILD | WS_VISIBLE, 0, 0, 10, 30, trackbar, (HMENU)1, NULL, NULL);

	HWND hRightLabel = CreateWindowW(L"Static", L"100",
		WS_CHILD | WS_VISIBLE, 0, 0, 30, 30, trackbar, (HMENU)2, NULL, NULL);

	hlbl = CreateWindowW(L"Static", L"0", WS_CHILD | WS_VISIBLE,
		270, 20, 30, 30, trackbar, (HMENU)3, NULL, NULL);

	// allows controls to be used
	INITCOMMONCONTROLSEX icex;

	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&icex);

	// displays trackbar
	hTrack = CreateWindowW(TRACKBAR_CLASSW, L"Trackbar Control",
		WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
		20, 20, 170, 30, trackbar, (HMENU)3, NULL, NULL);

	SendMessageW(hTrack, TBM_SETRANGE, TRUE, MAKELONG(0, 100));
	SendMessageW(hTrack, TBM_SETPAGESIZE, 0, 10);
	SendMessageW(hTrack, TBM_SETTICFREQ, 10, 0);
	SendMessageW(hTrack, TBM_SETPOS, FALSE, 0);
	SendMessageW(hTrack, TBM_SETBUDDY, TRUE, (LPARAM)hLeftLabel);
	SendMessageW(hTrack, TBM_SETBUDDY, FALSE, (LPARAM)hRightLabel);
}

// updates trackbar
void updateTrackbar() {
	LRESULT pos = SendMessageW(hTrack, TBM_GETPOS, 0, 0);
	wchar_t buf[4];
	wsprintfW(buf, L"%ld", pos);
	penSizeVal = wcstol(buf, NULL, 10);

	SetWindowTextW(hlbl, buf);
}

// win procedure for canvas size dialogue
LRESULT CALLBACK CanvasProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static HWND xInput;
	static HWND yInput;
	switch (msg) {
	case WM_CREATE: {
		// text titles
		CreateWindowW(L"Static", L"Width: ",
			WS_CHILD | WS_VISIBLE | SS_LEFT,
			10, 15, 50, 50,
			hwnd, (HMENU)1, NULL, NULL);

		CreateWindowW(L"Static", L"Height: ",
			WS_CHILD | WS_VISIBLE | SS_LEFT,
			10, 45, 50, 50,
			hwnd, (HMENU)1, NULL, NULL);

		// inputs
		xInput = CreateWindowW(L"Edit", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			65, 15, 50, 20, hwnd, (HMENU)IDM_EDIT_X,
			NULL, NULL);

		yInput = CreateWindowW(L"Edit", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			65, 45, 50, 20, hwnd, (HMENU)IDM_EDIT_Y,
			NULL, NULL);

		// enter button
		CreateWindowW(L"Button", L"Enter",
			WS_VISIBLE | WS_CHILD,
			140, 70, 60, 25, hwnd, (HMENU)IDM_ENTER, NULL, NULL);

		break;
	}
	case WM_COMMAND: {
		if (LOWORD(wParam) == IDM_ENTER) {
			// get text entries
			int lenX = GetWindowTextLengthW(xInput) + 1;
			int lenY = GetWindowTextLengthW(yInput) + 1;

			wchar_t* textX = new wchar_t[lenX];
			wchar_t* textY = new wchar_t[lenY];

			GetWindowTextW(xInput, textX, lenX);
			GetWindowTextW(yInput, textY, lenY);

			// check input is numerical
			if (checkTextIsNum(textX, lenX) == true && checkTextIsNum(textY, lenY) == true) {
				// set global canvas size variables
				canvasWidth = _wtoi(textX);
				canvasHeight = _wtoi(textY);
				SetWindowPos(canvasWin, HWND_TOP, 31, 0, canvasWidth, canvasHeight, SWP_NOZORDER);
			}
			else {
				// otherwise produce error
				MessageBox(NULL, L"Only numbers [greater than 0] can be entered.", L"Error", MB_ICONEXCLAMATION | MB_OK);
			}

			// close dialogue
			PostMessage(hwnd, WM_CLOSE, 0, 0);
		}
		break;
	}
	case WM_DESTROY:
	{
		//PostQuitMessage(0);
		break;
	}
	default:
		return DefWindowProcW(hwnd, msg, wParam, lParam);
	}
	return 0;
}

// displays dialogue allowing user to change canvas size
void canvasSize(HWND hwnd) {
	// win class
	WNDCLASSW cs = { 0 };
	cs.lpszClassName = L"Change Canvas Size";
	cs.hInstance = GetModuleHandle(NULL);
	cs.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	cs.lpfnWndProc = CanvasProc;
	cs.hCursor = LoadCursor(0, IDC_ARROW);


	// display dialogue box
	HWND penWin = CreateWindowExW(WS_EX_DLGMODALFRAME | WS_EX_TOPMOST, L"Change Canvas Size", L"Canvas Size",
		WS_VISIBLE | WS_SYSMENU | WS_CAPTION, 100, 100, 225, 140,
		NULL, NULL, GetModuleHandle(NULL), NULL);

	RegisterClassW(&cs);
}

// check if wchar_t is a number
BOOL checkTextIsNum(wchar_t* textInput, int len) {
	// just assume it is a number for now
	BOOL isDigit = true;

	if (len - 1 == 0) {
		isDigit = false;
	}
	else if (textInput[0] == '0') {
		isDigit = false;
	}
	else {
		for (int i = 0; i < len - 1; i++) {
			if (iswdigit(textInput[i]) == false) {
				isDigit = false;
				break;
			}
			else {
				continue;
			}
		}
	}

	return isDigit;
}

// saves screenshot of canvas to bitmap
void fileSaving::saveBmp(HWND hWnd, LPCWSTR fileName) {
	BITMAP bmpScreen;
	DWORD bytesWritten = 0;
	char* lpbitmap = NULL;

	// save image to hbitmap
	HDC hdcScreen = GetDC(NULL);
	HDC hdcWindow = GetDC(canvasWin);
	HDC hdcMem = CreateCompatibleDC(hdcWindow);

	RECT client;
	GetClientRect(canvasWin, &client);

	HBITMAP hbmScreen = CreateCompatibleBitmap(hdcWindow, client.right - client.left, client.bottom - client.top);
	SelectObject(hdcMem, hbmScreen);
	BitBlt(hdcMem, 0, 0, client.right - client.left, client.bottom - client.top, hdcWindow, 0, 0, SRCCOPY);
	GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);

	// headers for the actual bitmap file
	BITMAPFILEHEADER bmpHeader;
	BITMAPINFOHEADER infoHeader;

	infoHeader.biSize = sizeof(BITMAPINFOHEADER);
	infoHeader.biWidth = bmpScreen.bmWidth;
	infoHeader.biHeight = bmpScreen.bmHeight;
	infoHeader.biPlanes = 1;
	infoHeader.biBitCount = 32;
	infoHeader.biCompression = BI_RGB;
	infoHeader.biSizeImage = 0;
	infoHeader.biXPelsPerMeter = 0;
	infoHeader.biYPelsPerMeter = 0;
	infoHeader.biClrUsed = 0;
	infoHeader.biClrImportant = 0;

	DWORD dwBmpSize = ((bmpScreen.bmWidth * infoHeader.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;

	HANDLE DIB = GlobalAlloc(GHND, dwBmpSize);

	if (DIB != 0) {
		lpbitmap = (char*)GlobalLock(DIB);
	}
	else {

	}

	GetDIBits(hdcWindow, hbmScreen, 0, (UINT)bmpScreen.bmHeight, lpbitmap, (BITMAPINFO*)&infoHeader, DIB_RGB_COLORS);

	// Save drawing to file
	HANDLE fileHandle = CreateFile(fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	DWORD fileSize = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmpHeader.bfSize = fileSize;
	bmpHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);
	bmpHeader.bfType = 0x4D42;

	WriteFile(fileHandle, (LPSTR)&bmpHeader, sizeof(BITMAPFILEHEADER), &bytesWritten, NULL);
	WriteFile(fileHandle, (LPSTR)&infoHeader, sizeof(BITMAPINFOHEADER), &bytesWritten, NULL);
	WriteFile(fileHandle, (LPSTR)lpbitmap, dwBmpSize, &bytesWritten, NULL);

	// free DIB from heap
	if (DIB != 0) {
		GlobalUnlock(DIB);
	}
	else {
	}
	GlobalFree(DIB);

	// Clean up
	CloseHandle(fileHandle);

	DeleteObject(hbmScreen);
	DeleteObject(hdcMem);
	ReleaseDC(NULL, hdcScreen);
	ReleaseDC(canvasWin, hdcWindow);
}

// saves to current location if already saved
void fileSaving::saveFile(HWND hwnd) {
	if (currentSave != L"") {
		saveBmp(hwnd, currentSave.c_str());
	}
	else {
		fileDialogue(hwnd);
	}
}

// save as
void fileSaving::fileDialogue(HWND hwnd) {
	OPENFILENAMEW ofn;

	char szFileName[MAX_PATH] = "";
	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = (LPCWSTR)L"Bitmap Files (*.bmp)\0*.bmp\0";
	ofn.lpstrFile = (LPWSTR)szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_NOREADONLYRETURN | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
	ofn.lpstrDefExt = (LPCWSTR)L"txt";

	// create file using name
	if (GetSaveFileName(&ofn)) {
		currentSave = ofn.lpstrFile;
		saveBmp(hwnd, ofn.lpstrFile);
	};
}

// opens file to be drawn on later
void fileSaving::openDialogue(HWND hwnd) {
	// ask if they wanna open forst
	int cont = MessageBox(hwnd, L"Are you sure you want to open an image? This will delete your previous work if not saved!", L"Continue?", MB_YESNO);
	if (cont == 6) {
		// open image
		OPENFILENAMEW ofn;

		char szFileName[MAX_PATH] = "";
		ZeroMemory(&ofn, sizeof(ofn));

		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = hwnd;
		ofn.lpstrFilter = (LPCWSTR)L"Bitmap Files (*.bmp)\0*.bmp\0";
		ofn.lpstrFile = (LPWSTR)szFileName;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
		ofn.lpstrDefExt = (LPCWSTR)L"txt";

		if (GetOpenFileName(&ofn))
		{
			// clear all the variables
			clearCanvas(hwnd);

			ZeroMemory(&bitmapInfo, sizeof(bitmapInfo));
			hBitmap = (HBITMAP)LoadImage(NULL, ofn.lpstrFile,
				IMAGE_BITMAP, 0, 0,
				LR_LOADFROMFILE);

			GetObject(hBitmap, sizeof(BITMAP), &bitmapInfo);

			canvasHeight = bitmapInfo.bmHeight;
			canvasWidth = bitmapInfo.bmWidth;

			SetWindowPos(canvasWin, HWND_TOP, 31, 0, canvasWidth, canvasHeight, SWP_NOZORDER);

			// redraws so user doesnt have to click 
			opened = TRUE;
			InvalidateRect(canvasWin, NULL, NULL);
		}
	}
	else {
		// otherwise break
		return;
	}
}