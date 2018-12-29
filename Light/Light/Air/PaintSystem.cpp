#include "PaintSystem.h"

void PaintSystem::SetBackGround(WORD hb_name)
{
	BackGround = new HBITMAP();
	*BackGround = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(hb_name));
}

void PaintSystem::OnPaint(PAINTSTRUCT & ps)
{
	HDC hMemDC, hTempDC; // Теневой буффер
	HGDIOBJ hMemBmp, hSysBmp;
	HBRUSH hBrush = (HBRUSH)WHITE_BRUSH;
	hMemDC = CreateCompatibleDC(ps.hdc); // Создать конекст схожии с основным
	hMemBmp = CreateCompatibleBitmap(ps.hdc, WndSize.first, WndSize.second); // Создание битмап для фона
	hSysBmp = SelectObject(hMemDC, hMemBmp);
	SelectObject(hMemDC, hBrush);
	hTempDC = CreateCompatibleDC(hMemDC);
	SelectObject(hTempDC, *BackGround);
	BitBlt(hMemDC, 0, 0, WndSize.first, WndSize.second, hTempDC, 0, 0, SRCCOPY); // Копироание одного контекста в другой контекст
	DeleteDC(hTempDC); // Удаление временного контекста
	for (auto i = ++objects_pt.begin(); i != objects_pt.end(); i++)
		(*i)->Paint(hMemDC); // Отрисовка всех объектов
	for (auto i = objects_line.begin(); i != objects_line.end(); i++)
		(*i)->Paint(hMemDC, objects_pt); // Отрисовка линии
	objects_pt[0]->Paint(hMemDC);
	for (auto i = ++objects_pt.begin(); i != objects_pt.end(); i++)
		(*i)->Paint(hMemDC); // Отрисовка всех объектов
	BitBlt(ps.hdc, 0, 0, WndSize.first, WndSize.second, hMemDC, 0, 0, SRCCOPY); // Отправка в основной контекст
	SelectObject(hMemDC, hSysBmp);
	DeleteObject(hMemBmp); // Удаление Битмапа
	DeleteObject(hSysBmp);
	DeleteDC(hMemDC); // Удаление контекстов
	DeleteObject(hBrush);
}

bool PaintSystem::MouseRes(Vector c_pos, COND_MOUSE cond)
{
	switch (cond)
	{
	case COND_MOUSE::DOWN:
		for (auto i = objects_pt.begin(); i != objects_pt.end(); i++)
			if (LineLight::havePoint(*i, c_pos - Vector(9.0f, 30.0f))) // Если есть пересечение мыши с объектом
			{
				d_vec = (*i)->StartPos - c_pos; // Запомнить смещение
				movement = *i; // Запомнить объект
				return true;
			}
		break;
	case COND_MOUSE::MOVE: // Перемещение - восстановление позиции через смещение
		movement->SetPosition(d_vec + Vector(c_pos.x, c_pos.y));
		break;
	}
	return false;
}