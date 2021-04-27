#include <iostream>
#include <Windows.h>
#include <string>

#define IDM_FILE_NEW 1
#define IDM_FILE_OPEN 2
#define IDM_FILE_QUIT 3
#define IDM_PEN 4
#define IDM_COLOUR 5
#define IDM_PEN_OPTIONS 6
#define IDM_CANVAS_SIZE 7
#define IDM_EDIT_X 8
#define IDM_EDIT_Y 9 
#define IDM_ENTER 10
#define IDM_ERASER 11
#define IDM_LINE 12
#define IDM_COLOUR_DROPPER 13
#define IDM_SQUARE 14
#define IDM_CIRCLE 15
#define IDM_FILE_SAVE 16
#define IDM_FILE_SAVE_AS 17

extern HWND canvasWin;

// canvas size test
extern int canvasWidth;
extern int canvasHeight;

// pen size
extern int penSizeVal;

// image from opening bitmap file
extern HBITMAP hBitmap;
extern BITMAP  bitmapInfo;
extern bool opened;

// declarations
void addMenus(HWND hwnd);
HWND CreateToolbar(HWND hwnd);
COLORREF ColourDialogue(HWND hwnd);
void penSize(HWND);
void createTrackbar(HWND);
void updateTrackbar();
void canvasSize(HWND);
BOOL checkTextIsNum(wchar_t* textInput, int len);

// class for saving and opening
class fileSaving {
private:
	std::wstring currentSave;
	void saveBmp(HWND hWnd, LPCWSTR fileName);
public:
	void fileDialogue(HWND hwnd);
	void saveFile(HWND hwnd);
	void openDialogue(HWND hwnd);
};
