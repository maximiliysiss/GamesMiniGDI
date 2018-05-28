#pragma once
#include <Windows.h>
class Button_IMG
{
	HBITMAP hBitmap; // bitmap
	int st_x; // coords
	int st_y;
	BITMAP bmp; // info about bitmap
	void(*pf)(HWND); // point to function
public:
	Button_IMG(WORD id, int x, int y, void(*_pf)(HWND)) :st_x(x), st_y(y), // construct
		hBitmap(LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(id))), pf(_pf)
	{
		GetObject(hBitmap, sizeof(BITMAP), &bmp); // get info about bitmap
	}
	void OnPaint(HDC hdc) // paint
	{
		HDC hMem = CreateCompatibleDC(hdc); // temp context
		SelectObject(hMem, hBitmap); // select bitmap
		// copy with erasing white
		TransparentBlt(hdc, st_x, st_y, bmp.bmWidth, bmp.bmHeight, hMem, 0, 0, bmp.bmWidth, bmp.bmHeight, RGB(255, 255, 255));
		DeleteDC(hMem); // delete temp context
	}
	void OnClick(RECT wndRect, POINT Cursor, HWND myHWND) // processing click
	{
		// if click in area of button
		if (abs(wndRect.left - Cursor.x) >= st_x + 9 && abs(wndRect.left - Cursor.x) <= st_x + bmp.bmWidth + 9
			&& abs(wndRect.top - Cursor.y) >= st_y + 30 && abs(wndRect.top - Cursor.y) <= st_y + bmp.bmHeight + 30)
		{
			// process delegate(point to fucntion)
			pf(myHWND);
		}
	}
};