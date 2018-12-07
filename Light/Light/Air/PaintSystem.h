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
	void OnPaint(PAINTSTRUCT & ps);
	static PaintSystem * SysPaint;
	std::vector<Object*> objects_pt; // Структура указателей на объекты
	std::vector<LineLight*> objects_line; // Линии
	Vector d_vec; // Вектор смещения
	Object * movement; // Объект, который движется
	enum COND_MOUSE { DOWN, MOVE, UP }; // Состояние мыши
	bool MouseRes(Vector c_pos, COND_MOUSE cond);
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

