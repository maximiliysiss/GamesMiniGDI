#pragma once
#include <Windows.h>
// Класс кнопки
class Button_IMG
{
	HBITMAP hBitmap; // Изображение маска
	int st_x; // Позиция верзнего левого угла
	int st_y;
	BITMAP bmp; // Информация о битмап
	void(*pf)(); // Указатель на функцию 
public:
	// Констурктор
	Button_IMG(WORD id, int x, int y, void(*_pf)()) :st_x(x), st_y(y),
		hBitmap(LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(id))), pf(_pf)
	{
		GetObject(hBitmap, sizeof(BITMAP), &bmp);
	}
	//Отрисовка
	void OnPaint(HDC hdc)
	{
		HDC hMem = CreateCompatibleDC(hdc); // Контекст схожий с аргументом
		SelectObject(hMem, hBitmap); // Выбор рисунка
		// Отрисовка
		TransparentBlt(hdc, st_x, st_y, bmp.bmWidth, bmp.bmHeight, hMem, 0, 0, bmp.bmWidth, bmp.bmHeight, RGB(255, 255, 255));
		DeleteDC(hMem); // Удаление контекста
	}
	// Нажатие мыши
	void OnClick(RECT wndRect, POINT Cursor)
	{
		// Выполнение в случае нажатия на кнопку
		if (abs(wndRect.left - Cursor.x) >= st_x + 9 && abs(wndRect.left - Cursor.x) <= st_x + bmp.bmWidth + 9
			&& abs(wndRect.top - Cursor.y) >= st_y + 30 && abs(wndRect.top - Cursor.y) <= st_y + bmp.bmHeight + 30)
		{
			pf(); // Выполнение функции
		}
	}
};