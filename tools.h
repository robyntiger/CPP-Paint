#include <iostream>
#include <vector>
#include <string>
using namespace std;

void clearCanvas(HWND hwnd);

// variables
extern COLORREF gColour;
extern int canvasWidth;
extern int canvasHeight;
extern int penSizeVal;
extern bool drawing;
extern int lineNum;
extern bool penselected;

// image from opening bitmap file
extern HBITMAP hBmp;

// declaring point class
class Point {
	int x;
	int y;
public:
	Point(int x_coord, int y_coord);
	int getX();
	int getY();
};


//declaring style class
class Style {
	COLORREF lineColour;
	int lineSize;
	string shape;
public:
	Style(COLORREF col, int size, string type);
	COLORREF getlineColour();
	int getlineSize();
	string getType();
};

// declaring tool class
class tool {
public:
	void draw(HWND hwnd, PAINTSTRUCT& ps);
	void pen(int msg, HWND hwnd);
	void eraser(int msg, HWND hwnd);
	void line(int msg, HWND hwnd);
	void colourPicker(int msg, HWND hwnd);
	void circle(int msg, HWND hwnd);
	void square(int msg, HWND hwnd);
};