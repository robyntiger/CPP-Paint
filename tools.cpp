#include <iostream>
#include <vector>
#include <Windows.h>
#include <CommCtrl.h>
#include "resource.h"
#include "tools.h"
#include "dialogues.h"
using namespace std;

// vars for drawing with pen
vector<vector<pair<Style, Point>>> lines = { {} }; // contains all lines currently on screen
int lineNum = 0;
HDC hdc;

// creating Point class
Point::Point(int x_coord, int y_coord) : x(x_coord), y(y_coord) {}

int Point::getX() {
	return x;
}

int Point::getY() {
	return y;
}

// line style class
Style::Style(COLORREF col, int size, string type) : lineColour(col), lineSize(size), shape(type) {}

COLORREF Style::getlineColour() {
	return lineColour;
}

int Style::getlineSize() {
	return lineSize;
}

string Style::getType() {
	return shape;
}

bool drawing = FALSE;

// draws all the lines to screen
void tool::draw(HWND hwnd, PAINTSTRUCT& ps) {
	hdc = BeginPaint(hwnd, &ps);

	// keeps track of current row and column
	int currentRow = 0;
	int currentCol = 0;

	HPEN hPen;
	HGDIOBJ hOldPen;

	// double buffering
	// create off screen dc
	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP hbmMem = CreateCompatibleBitmap(hdc, canvasWidth, canvasHeight);

	HANDLE hOld = SelectObject(hdcMem, hbmMem);

	// set background colour otherwise its black
	RECT bg = { 0, 0, canvasWidth, canvasHeight };
	HBRUSH brush = CreateSolidBrush(RGB(255, 255, 255));
	FillRect(hdcMem, &bg, brush);

	// opens image if open has been selected
	HDC hDCBits = CreateCompatibleDC(hdcMem);
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bitmapInfo);
	SelectObject(hDCBits, hBitmap);
	BOOL bResult = BitBlt(hdcMem, 0, 0, bitmapInfo.bmWidth, bitmapInfo.bmHeight, hDCBits, 0, 0, SRCCOPY);
	DeleteDC(hDCBits);

	// can only draw if there is at least 1 line
	if (lineNum > 0) {
		for (auto row = lines.begin(); row != lines.end(); row++) {
			currentCol = 0;
			for (auto col = row->begin(); col != row->end(); col++) {
				// draws lines
				if (currentCol != 0) {
					// draw into hdcmem
					hPen = CreatePen(PS_SOLID, lines[currentRow][currentCol - 1].first.getlineSize(), lines[currentRow][currentCol - 1].first.getlineColour());
					hOldPen = SelectObject(hdcMem, hPen);
					if (lines[currentRow][currentCol - 1].first.getType() == "line") {

						MoveToEx(hdcMem, lines[currentRow][currentCol - 1].second.getX(), lines[currentRow][currentCol - 1].second.getY(), NULL);
						LineTo(hdcMem, lines[currentRow][currentCol].second.getX(), lines[currentRow][currentCol].second.getY());
					}
					else if (lines[currentRow][currentCol - 1].first.getType() == "circle") {
						// transparent
						SelectObject(hdcMem, GetStockObject(HOLLOW_BRUSH));

						int left = lines[currentRow][currentCol - 1].second.getX();
						int top = lines[currentRow][currentCol - 1].second.getY();
						int right = lines[currentRow][currentCol].second.getX();
						int bottom = lines[currentRow][currentCol].second.getY();

						Ellipse(hdcMem, left, top, right, bottom);


					}

					SelectObject(hdcMem, hOldPen);
					DeleteObject(hPen);

				}

				currentCol = currentCol + 1;
			}

			currentRow = currentRow + 1;
		}
	}

	// Transfer the off-screen DC to the screen
	BitBlt(hdc, 0, 0, canvasWidth, canvasHeight, hdcMem, 0, 0, SRCCOPY);

	// Free-up the off-screen DC
	SelectObject(hdcMem, hOld);

	DeleteObject(hbmMem);
	DeleteDC(hdcMem);
	EndPaint(hwnd, &ps);

	EndPaint(hwnd, &ps);
}

// tool class - pen tool
void tool::pen(int msg, HWND hwnd) {
	PAINTSTRUCT ps;
	POINT prevPoint;

	switch (msg) {
	case WM_LBUTTONDOWN:
	{
		drawing = TRUE;
		lines.push_back({});
		lineNum = lineNum + 1;

		break;
	}
	case WM_LBUTTONUP:
	{
		drawing = FALSE;
		break;
	}
	case WM_MOUSEMOVE:
	{
		// if mouse down, gets cursor position
		if (drawing) {
			GetCursorPos(&prevPoint);
			ScreenToClient(hwnd, &prevPoint);

			// adding previous and current point to the current line buffer
			lines[lineNum].push_back(make_pair(Style(gColour, penSizeVal, "line"), Point(prevPoint.x, prevPoint.y)));

			// Updates screen and stops flickering
			InvalidateRect(hwnd, NULL, FALSE);
		}
		break;
	}
	case WM_PAINT:
	{
		draw(hwnd, ps);
		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	}
}

// tool class - eraser tool
void tool::eraser(int msg, HWND hwnd) {
	PAINTSTRUCT ps;
	POINT prevPoint;

	switch (msg) {
	case WM_LBUTTONDOWN:
	{
		drawing = TRUE;
		lines.push_back({});
		lineNum = lineNum + 1;

		break;
	}
	case WM_LBUTTONUP:
	{
		drawing = FALSE;
		break;
	}
	case WM_MOUSEMOVE:
	{
		// if mouse down, gets cursor position
		if (drawing) {
			GetCursorPos(&prevPoint);
			ScreenToClient(hwnd, &prevPoint);

			// adding previous and current point to the current line buffer
			lines[lineNum].push_back(make_pair(Style(RGB(255, 255, 255), penSizeVal, "line"), Point(prevPoint.x, prevPoint.y)));

			// Updates screen and stops flickering
			InvalidateRect(hwnd, NULL, FALSE);
		}
		break;
	}
	case WM_PAINT:
	{
		draw(hwnd, ps);
		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	}
}

// line buffer for line/circle/square tool
vector<pair<Style, Point>> lineBuffer(2, make_pair(Style(RGB(255, 255, 255), 0, "line"), Point(0, 0)));

// tool class - line tool
void tool::line(int msg, HWND hwnd) {
	PAINTSTRUCT ps;
	POINT prevPoint;

	switch (msg) {
	case WM_LBUTTONDOWN:
	{
		drawing = TRUE;

		GetCursorPos(&prevPoint);
		ScreenToClient(hwnd, &prevPoint);

		lineBuffer[0] = make_pair(Style(gColour, penSizeVal, "line"), Point(prevPoint.x, prevPoint.y));

		break;
	}
	case WM_LBUTTONUP:
	{
		if (drawing) {
			lines.push_back({});
			lineNum = lineNum + 1;
			lines[lineNum].push_back(lineBuffer[0]);
			lines[lineNum].push_back(lineBuffer[1]);

			lineBuffer = { make_pair(Style(RGB(255, 255, 255), 0, "line"), Point(0, 0)) ,make_pair(Style(RGB(255, 255, 255), 0, "line"), Point(0, 0)) };

		}
		drawing = FALSE;
		break;
	}
	case WM_MOUSEMOVE:
	{
		// if mouse down, gets cursor position
		if (drawing) {
			GetCursorPos(&prevPoint);
			ScreenToClient(hwnd, &prevPoint);

			// adding previous and current point to the current line buffer
			lineBuffer[1] = make_pair(Style(gColour, penSizeVal, "line"), Point(prevPoint.x, prevPoint.y));

			// Updates screen and stops flickering
			InvalidateRect(hwnd, NULL, FALSE);
		}
		break;
	}
	case WM_PAINT:
	{
		// draws with buffer

		hdc = BeginPaint(hwnd, &ps);

		// track current row and column
		int currentRow = 0;
		int currentCol = 0;

		HPEN hPen;
		HGDIOBJ hOldPen;

		// double buffering
		// create off screen dc
		HDC hdcMem = CreateCompatibleDC(hdc);
		HBITMAP hbmMem = CreateCompatibleBitmap(hdc, canvasWidth, canvasHeight);

		HANDLE hOld = SelectObject(hdcMem, hbmMem);

		// set background colour otherwise its black
		RECT bg = { 0, 0, canvasWidth, canvasHeight };
		HBRUSH brush = CreateSolidBrush(RGB(255, 255, 255));
		FillRect(hdcMem, &bg, brush);

		// opens image if open has been selected
		HDC hDCBits = CreateCompatibleDC(hdcMem);
		GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bitmapInfo);
		SelectObject(hDCBits, hBitmap);
		BOOL bResult = BitBlt(hdcMem, 0, 0, bitmapInfo.bmWidth, bitmapInfo.bmHeight, hDCBits, 0, 0, SRCCOPY);
		DeleteDC(hDCBits);

		// obvs can only draw if there is at least 1 line
		if (lineNum > 0) {
			for (auto row = lines.begin(); row != lines.end(); row++) {
				currentCol = 0;
				for (auto col = row->begin(); col != row->end(); col++) {
					// draws lines
					if (currentCol != 0) {
						// draw into hdcmem
						hPen = CreatePen(PS_SOLID, lines[currentRow][currentCol - 1].first.getlineSize(), lines[currentRow][currentCol - 1].first.getlineColour());
						hOldPen = SelectObject(hdcMem, hPen);
						if (lines[currentRow][currentCol - 1].first.getType() == "line") {

							MoveToEx(hdcMem, lines[currentRow][currentCol - 1].second.getX(), lines[currentRow][currentCol - 1].second.getY(), NULL);
							LineTo(hdcMem, lines[currentRow][currentCol].second.getX(), lines[currentRow][currentCol].second.getY());
						}
						else if (lines[currentRow][currentCol - 1].first.getType() == "circle") {
							// transparent
							SelectObject(hdcMem, GetStockObject(HOLLOW_BRUSH));

							int left = lines[currentRow][currentCol - 1].second.getX();
							int top = lines[currentRow][currentCol - 1].second.getY();
							int right = lines[currentRow][currentCol].second.getX();
							int bottom = lines[currentRow][currentCol].second.getY();

							Ellipse(hdcMem, left, top, right, bottom);


						}

						SelectObject(hdcMem, hOldPen);
						DeleteObject(hPen);
					}

					currentCol = currentCol + 1;
				}

				currentRow = currentRow + 1;
			}
		}

		// draw line on top
		hPen = CreatePen(PS_SOLID, lineBuffer[0].first.getlineSize(), lineBuffer[0].first.getlineColour());
		hOldPen = SelectObject(hdcMem, hPen);

		MoveToEx(hdcMem, lineBuffer[0].second.getX(), lineBuffer[0].second.getY(), NULL);
		LineTo(hdcMem, lineBuffer[1].second.getX(), lineBuffer[1].second.getY());

		SelectObject(hdcMem, hOldPen);
		DeleteObject(hPen);


		// Transfer the off-screen DC to the screen
		BitBlt(hdc, 0, 0, canvasWidth, canvasHeight, hdcMem, 0, 0, SRCCOPY);

		// Free-up the off-screen DC
		SelectObject(hdcMem, hOld);

		DeleteObject(hbmMem);
		DeleteDC(hdcMem);
		EndPaint(hwnd, &ps);

		EndPaint(hwnd, &ps);

		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	}
}

// circle tool
void tool::circle(int msg, HWND hwnd) {
	PAINTSTRUCT ps;
	POINT prevPoint;

	switch (msg) {
	case WM_LBUTTONDOWN:
	{
		drawing = TRUE;

		GetCursorPos(&prevPoint);
		ScreenToClient(hwnd, &prevPoint);

		lineBuffer[0] = make_pair(Style(gColour, penSizeVal, "circle"), Point(prevPoint.x, prevPoint.y));

		break;
	}
	case WM_LBUTTONUP:
	{
		if (drawing) {
			lines.push_back({});
			lineNum = lineNum + 1;
			lines[lineNum].push_back(lineBuffer[0]);
			lines[lineNum].push_back(lineBuffer[1]);

			lineBuffer = { make_pair(Style(RGB(255, 255, 255), 0, "circle"), Point(0, 0)) ,make_pair(Style(RGB(255, 255, 255), 0, "circle"), Point(0, 0)) };

		}
		drawing = FALSE;
		break;
	}
	case WM_MOUSEMOVE:
	{
		// if mouse down, gets cursor position
		if (drawing) {
			GetCursorPos(&prevPoint);
			ScreenToClient(hwnd, &prevPoint);

			// adding previous and current point to the current line buffer
			lineBuffer[1] = make_pair(Style(gColour, penSizeVal, "circle"), Point(prevPoint.x, prevPoint.y));

			// Updates screen and stops flickering
			InvalidateRect(hwnd, NULL, FALSE);
		}
		break;
	}
	case WM_PAINT:
	{
		// draw buffwe

		hdc = BeginPaint(hwnd, &ps);

		// keep track of current row and column
		int currentRow = 0;
		int currentCol = 0;

		HPEN hPen;
		HGDIOBJ hOldPen;

		// double buffering
		// create off screen dc
		HDC hdcMem = CreateCompatibleDC(hdc);
		HBITMAP hbmMem = CreateCompatibleBitmap(hdc, canvasWidth, canvasHeight);

		HANDLE hOld = SelectObject(hdcMem, hbmMem);

		// set background colour otherwise its black
		RECT bg = { 0, 0, canvasWidth, canvasHeight };
		HBRUSH brush = CreateSolidBrush(RGB(255, 255, 255));
		FillRect(hdcMem, &bg, brush);

		// opens image if open has been selected
		HDC hDCBits = CreateCompatibleDC(hdcMem);
		GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bitmapInfo);
		SelectObject(hDCBits, hBitmap);
		BOOL bResult = BitBlt(hdcMem, 0, 0, bitmapInfo.bmWidth, bitmapInfo.bmHeight, hDCBits, 0, 0, SRCCOPY);
		DeleteDC(hDCBits);

		// can only draw if there is at least 1 line
		if (lineNum > 0) {
			for (auto row = lines.begin(); row != lines.end(); row++) {
				currentCol = 0;
				for (auto col = row->begin(); col != row->end(); col++) {
					// draws lines
					if (currentCol != 0) {
						// draw into hdcmem
						hPen = CreatePen(PS_SOLID, lines[currentRow][currentCol - 1].first.getlineSize(), lines[currentRow][currentCol - 1].first.getlineColour());
						hOldPen = SelectObject(hdcMem, hPen);
						if (lines[currentRow][currentCol - 1].first.getType() == "line") {

							MoveToEx(hdcMem, lines[currentRow][currentCol - 1].second.getX(), lines[currentRow][currentCol - 1].second.getY(), NULL);
							LineTo(hdcMem, lines[currentRow][currentCol].second.getX(), lines[currentRow][currentCol].second.getY());
						}
						else if (lines[currentRow][currentCol - 1].first.getType() == "circle") {

							SelectObject(hdcMem, GetStockObject(HOLLOW_BRUSH));

							int left = lines[currentRow][currentCol - 1].second.getX();
							int top = lines[currentRow][currentCol - 1].second.getY();
							int right = lines[currentRow][currentCol].second.getX();
							int bottom = lines[currentRow][currentCol].second.getY();

							Ellipse(hdcMem, left, top, right, bottom);


						}

						SelectObject(hdcMem, hOldPen);
						DeleteObject(hPen);
					}

					currentCol = currentCol + 1;
				}

				currentRow = currentRow + 1;
			}
		}

		// transparent
		SelectObject(hdcMem, GetStockObject(HOLLOW_BRUSH));

		// draw line on top
		hPen = CreatePen(PS_SOLID, lineBuffer[0].first.getlineSize(), lineBuffer[0].first.getlineColour());
		hOldPen = SelectObject(hdcMem, hPen);

		// quick maffs
		int left = lineBuffer[0].second.getX();
		int top = lineBuffer[0].second.getY();
		int right = lineBuffer[1].second.getX();
		int bottom = lineBuffer[1].second.getY();

		Ellipse(hdcMem, left, top, right, bottom);

		SelectObject(hdcMem, hOldPen);
		DeleteObject(hPen);


		// Transfer the off-screen DC to the screen
		BitBlt(hdc, 0, 0, canvasWidth, canvasHeight, hdcMem, 0, 0, SRCCOPY);

		// Free-up the off-screen DC
		SelectObject(hdcMem, hOld);

		DeleteObject(hbmMem);
		DeleteDC(hdcMem);
		EndPaint(hwnd, &ps);

		EndPaint(hwnd, &ps);

		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	}
}

// square tool
void tool::square(int msg, HWND hwnd) {
	//HDC hdc;
	PAINTSTRUCT ps;
	POINT prevPoint;

	switch (msg) {
	case WM_LBUTTONDOWN:
	{
		drawing = TRUE;

		GetCursorPos(&prevPoint);
		ScreenToClient(hwnd, &prevPoint);

		lineBuffer[0] = make_pair(Style(gColour, penSizeVal, "line"), Point(prevPoint.x, prevPoint.y));

		break;
	}
	case WM_LBUTTONUP:
	{
		if (drawing) {
			lines.push_back({});
			lineNum = lineNum + 1;

			lines[lineNum].push_back(make_pair(Style(gColour, penSizeVal, "line"), Point(lineBuffer[0].second.getX(), lineBuffer[0].second.getY())));
			lines[lineNum].push_back(make_pair(Style(gColour, penSizeVal, "line"), Point(lineBuffer[1].second.getX(), lineBuffer[0].second.getY())));
			lines[lineNum].push_back(make_pair(Style(gColour, penSizeVal, "line"), Point(lineBuffer[1].second.getX(), lineBuffer[1].second.getY())));
			lines[lineNum].push_back(make_pair(Style(gColour, penSizeVal, "line"), Point(lineBuffer[0].second.getX(), lineBuffer[1].second.getY())));
			lines[lineNum].push_back(make_pair(Style(gColour, penSizeVal, "line"), Point(lineBuffer[0].second.getX(), lineBuffer[0].second.getY())));

			lineBuffer = { make_pair(Style(RGB(255, 255, 255), 0, "line"), Point(0, 0)) ,make_pair(Style(RGB(255, 255, 255), 0, "line"), Point(0, 0)) };

		}
		drawing = FALSE;
		break;
	}
	case WM_MOUSEMOVE:
	{
		// if mouse down, gets cursor position
		if (drawing) {
			GetCursorPos(&prevPoint);
			ScreenToClient(hwnd, &prevPoint);

			// adding previous and current point to the current line buffer
			lineBuffer[1] = make_pair(Style(gColour, penSizeVal, "line"), Point(prevPoint.x, prevPoint.y));

			// Updates screen and stops flickering
			InvalidateRect(hwnd, NULL, FALSE);
		}
		break;
	}
	case WM_PAINT:
	{
		// draw buffer

		hdc = BeginPaint(hwnd, &ps);

		// keep track of current row and column
		int currentRow = 0;
		int currentCol = 0;

		HPEN hPen;
		HGDIOBJ hOldPen;

		// double buffering
		// create off screen dc
		HDC hdcMem = CreateCompatibleDC(hdc);
		HBITMAP hbmMem = CreateCompatibleBitmap(hdc, canvasWidth, canvasHeight);

		HANDLE hOld = SelectObject(hdcMem, hbmMem);

		// set background colour otherwise its black
		RECT bg = { 0, 0, canvasWidth, canvasHeight };
		HBRUSH brush = CreateSolidBrush(RGB(255, 255, 255));
		FillRect(hdcMem, &bg, brush);

		// opens image if open has been selected
		HDC hDCBits = CreateCompatibleDC(hdcMem);
		GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bitmapInfo);
		SelectObject(hDCBits, hBitmap);
		BOOL bResult = BitBlt(hdcMem, 0, 0, bitmapInfo.bmWidth, bitmapInfo.bmHeight, hDCBits, 0, 0, SRCCOPY);
		DeleteDC(hDCBits);

		// can only draw if there is at least 1 line
		if (lineNum > 0) {
			for (auto row = lines.begin(); row != lines.end(); row++) {
				currentCol = 0;
				for (auto col = row->begin(); col != row->end(); col++) {
					// draws lines
					if (currentCol != 0) {
						// draw into hdcmem
						hPen = CreatePen(PS_SOLID, lines[currentRow][currentCol - 1].first.getlineSize(), lines[currentRow][currentCol - 1].first.getlineColour());
						hOldPen = SelectObject(hdcMem, hPen);
						if (lines[currentRow][currentCol - 1].first.getType() == "line") {

							MoveToEx(hdcMem, lines[currentRow][currentCol - 1].second.getX(), lines[currentRow][currentCol - 1].second.getY(), NULL);
							LineTo(hdcMem, lines[currentRow][currentCol].second.getX(), lines[currentRow][currentCol].second.getY());
						}
						else if (lines[currentRow][currentCol - 1].first.getType() == "circle") {
							// transparent
							SelectObject(hdcMem, GetStockObject(HOLLOW_BRUSH));

							int left = lines[currentRow][currentCol - 1].second.getX();
							int top = lines[currentRow][currentCol - 1].second.getY();
							int right = lines[currentRow][currentCol].second.getX();
							int bottom = lines[currentRow][currentCol].second.getY();

							Ellipse(hdcMem, left, top, right, bottom);


						}

						SelectObject(hdcMem, hOldPen);
						DeleteObject(hPen);
					}

					currentCol = currentCol + 1;
				}

				currentRow = currentRow + 1;
			}
		}

		// draw line on top
		hPen = CreatePen(PS_SOLID, lineBuffer[0].first.getlineSize(), lineBuffer[0].first.getlineColour());
		hOldPen = SelectObject(hdcMem, hPen);

		MoveToEx(hdcMem, lineBuffer[0].second.getX(), lineBuffer[0].second.getY(), NULL);
		LineTo(hdcMem, lineBuffer[1].second.getX(), lineBuffer[0].second.getY());

		MoveToEx(hdcMem, lineBuffer[1].second.getX(), lineBuffer[0].second.getY(), NULL);
		LineTo(hdcMem, lineBuffer[1].second.getX(), lineBuffer[1].second.getY());

		MoveToEx(hdcMem, lineBuffer[1].second.getX(), lineBuffer[1].second.getY(), NULL);
		LineTo(hdcMem, lineBuffer[0].second.getX(), lineBuffer[1].second.getY());

		MoveToEx(hdcMem, lineBuffer[0].second.getX(), lineBuffer[1].second.getY(), NULL);
		LineTo(hdcMem, lineBuffer[0].second.getX(), lineBuffer[0].second.getY());

		SelectObject(hdcMem, hOldPen);
		DeleteObject(hPen);

		// Transfer the off-screen DC to the screen
		BitBlt(hdc, 0, 0, canvasWidth, canvasHeight, hdcMem, 0, 0, SRCCOPY);

		// Free-up the off-screen DC
		SelectObject(hdcMem, hOld);

		DeleteObject(hbmMem);
		DeleteDC(hdcMem);
		EndPaint(hwnd, &ps);

		//DeleteObject(hPen);

		EndPaint(hwnd, &ps);

		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	}
}

// colour picker
void tool::colourPicker(int msg, HWND hwnd) {
	POINT currPoint;

	switch (msg) {
	case WM_LBUTTONDOWN:
	{
		GetCursorPos(&currPoint);
		ScreenToClient(hwnd, &currPoint);

		hdc = GetDC(hwnd);

		COLORREF gColourref = GetPixel(hdc, currPoint.x, currPoint.y);

		RGBTRIPLE rgb;

		rgb.rgbtRed = GetRValue(gColourref);
		rgb.rgbtGreen = GetGValue(gColourref);
		rgb.rgbtBlue = GetBValue(gColourref);

		gColour = RGB(rgb.rgbtRed, rgb.rgbtGreen, rgb.rgbtBlue);

		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	}
}

// clears canvas
// used 
void clearCanvas(HWND hwnd) {
	// clears lines on screen
	lines = { {} };
	lineNum = 0;
	hBitmap = NULL;
	// redraws so user doesnt have to click (:
	InvalidateRect(hwnd, NULL, NULL);
}