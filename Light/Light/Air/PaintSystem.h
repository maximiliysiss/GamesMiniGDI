#pragma once
#include "Object.h"

class EventSystem;
class LineLight;
class Setup;

class PaintSystem // Система рисования
{
public:
	friend Setup;
	friend EventSystem;
	friend LineLight;
	static PaintSystem & Instance() // Создание системы
	{
		if (!SysPaint)
			SysPaint = new PaintSystem();
		return *SysPaint;
	}
	void SetWndSize(int x, int y) // Устанвока размеров
	{
		WndSize.first = x;
		WndSize.second = y;
	}
	void SetBackGround(HBITMAP * hb)
	{
		BackGround = hb;
	}
	void SetBackGround(WORD hb_name);
private:
	HBITMAP * BackGround; // Изображение бэкграунд
	std::pair<int, int> WndSize; // Размеры
	void OnPaint(PAINTSTRUCT & ps)
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
		BitBlt(ps.hdc, 0, 0, WndSize.first, WndSize.second, hMemDC, 0, 0, SRCCOPY); // Отправка в основной контекст
		SelectObject(hMemDC, hSysBmp);
		DeleteObject(hMemBmp); // Удаление Битмапа
		DeleteObject(hSysBmp);
		DeleteDC(hMemDC); // Удаление контекстов
		DeleteObject(hBrush);
	}
	static PaintSystem * SysPaint;
	std::vector<Object*> objects_pt; // Структура указателей на объекты
	std::vector<LineLight*> objects_line; // Линии
	Vector d_vec; // Вектор смещения
	Object * movement; // Объект, который движется
	enum COND_MOUSE { DOWN, MOVE, UP }; // Состояние мыши
	bool MouseRes(Vector c_pos, COND_MOUSE cond)
	{
		switch (cond)
		{
		case COND_MOUSE::DOWN:
			for (auto i = objects_pt.begin(); i != objects_pt.end(); i++)
				if (LineLight::havePoint(*i, c_pos - Vector(9.0f, 30.0f))) // Если есть пересечение мыши с объектом
				{
					if (*i != objects_pt[0])
						d_vec = (*i)->StartPos - c_pos; // Запомнить смещение
					else
						d_vec = Vector(0, (*i)->StartPos.y - c_pos.y);
					movement = *i; // Запомнить объект
					return true;
				}
			break;
		case COND_MOUSE::MOVE: // Перемещение - восстановление позиции через смещение
			movement->StartPos = d_vec + Vector(0, c_pos.y);
			if (movement == objects_pt[0])
				((Setup*)movement)->Moving();
			break;
		}
		return false;
	}
	PaintSystem()
	{
	}
	void AddObject(LineLight * adding)
	{
		objects_line.push_back(adding); // Добавление линий света
	}
	void AddObject(Object * adding) // Добавление объектов
	{
		objects_pt.push_back(adding);
	}
};

