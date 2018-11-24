#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <iostream>
#pragma comment(lib,"MSIMG32.lib")
#pragma comment(lib, "Gdi32.lib")
#include "resource.h"


class GravitySystem;
class PaintSystem;
class LineLight;
class EventSystem;
class Setup;

struct Vector // Класс вектора 2д
{
	float x, y;
	Vector() { x = y = 0; }
	Vector(Vector vec1, Vector vec2)
	{
		x = vec2.x - vec1.x;
		y = vec2.y - vec1.y;
	}
	void Rotate(Vector center, float angle)
	{
		angle = angle * 3.14f / 180.0f;
		Vector t(center, *this);
		float xt = t.x;
		t.x = xt * cos(angle) - t.y * sin(angle);
		t.y = xt * sin(angle) + t.y*cos(angle);
		this->x = center.x + t.x;
		this->y = center.y + t.y;
	}
	Vector(float x1, float y1, float x2, float y2)
	{
		x = x2 - x1;
		y = y2 - y1;
	}
	Vector(float _x, float _y) :x(_x), y(_y) {}
	Vector(LONG _x, LONG _y) :x((float)_x), y((float)_y) {}
	Vector& operator=(const Vector& vec)
	{
		x = vec.x;
		y = vec.y;
		return *this;
	}
	Vector(POINT& p)
	{
		x = (float)p.x;
		y = (float)p.y;
	}
	Vector operator-(const Vector& vec)
	{
		return Vector(x - vec.x, y - vec.y);
	}
	Vector operator+(const Vector& vec)
	{
		return Vector(x + vec.x, y + vec.y);
	}
	Vector operator*(int i)
	{
		return Vector(x*i, y*i);
	}
	Vector operator*(float i)
	{
		return Vector(x*i, y*i);
	}
	void operator+=(const Vector& vec)
	{
		x += vec.x;
		y += vec.y;
	}
	void Normalize()
	{
		x /= Length();
		y /= Length();
	}
	float Scalar(const Vector vec)
	{
		return x * vec.x + y * vec.y;
	}
	void operator-=(const Vector& vec)
	{
		x -= vec.x;
		y -= vec.y;
	}
	float Length()
	{
		return sqrt(x*x + y * y);
	}
	bool operator==(const Vector & vec)
	{
		return x == vec.x && y == vec.y;
	}
};

class Object // Класс объекта
{
public:
	int count_normal; // количество нормалей
	Vector * Normal; // Нормали
	enum TYPE { RECT, SPHERE }; // Тип объекта
	friend GravitySystem;
	friend PaintSystem;
	friend LineLight;
	void Hide() // Не отрисовывать
	{
		hide = true;
	}
	void Unhide() // Отрисовывать
	{
		hide = false;
	}
	void AddForce(Vector force) // Добавить силу в какую-то сторону
	{
		Velocity += force;
		Velocity.Normalize();
		Velocity = Velocity * 2;
	}
	Object(HBITMAP *hb, bool gr, Vector st, TYPE type, Vector normal = Vector(0.0f, 0.0f)); // Конструкторы
	Object(WORD hb_name, bool gr, Vector st, TYPE type, Vector normal = Vector(0.0f, 0.0f));
protected:
	bool Contact(Object * obj) // Проверка на удар
	{
		if (obj->type == TYPE::SPHERE && this->type == TYPE::SPHERE)
			return Vector(obj->Center, Center).Length() - this->bm_info.bmWidth / 2 - obj->bm_info.bmWidth / 2 < 0;
		if (obj->type == TYPE::RECT && this->type == TYPE::SPHERE)
			return isCircleToRect(this->Center.x, this->Center.y, (float)this->bm_info.bmWidth / 2, obj->StartPos.x,
				obj->StartPos.y, (int)obj->bm_info.bmWidth, (int)obj->bm_info.bmHeight);
		return false;
	}
	// Проверка на пересечение прямоугольника и окружности
	bool isCircleToRect(float cx, float cy, float radius, float rx, float ry, int width, int height) {
		float x = cx;
		float y = cy;

		if (cx < rx)
			x = rx;
		else if (cx > (rx + width))
			x = rx + width;

		if (cy < ry)
			y = ry;
		else if (cy > (ry + height))
			y = ry + height;

		return (((cx - x)*(cx - x) + (cy - y)*(cy - y)) <= (radius * radius));
	}

	// Установка нормалей
	void SetNormals(Vector * vecs, int count)
	{
		delete[] Normal;
		Normal = vecs;
		count_normal = count;
	}
	void ContactRect(Object * obj, int i = 0) // Обработка удара о прямоугольник
	{
		Vector vec = Velocity * -0.1f;
		StartPos += vec;
		//this->Velocity.Normalize();
		float k = obj->Normal[i].x*this->Velocity.x + obj->Normal[i].y*this->Velocity.y;
		Vector b(Velocity.x + obj->Normal[i].x*k*-2.0f, Velocity.y + obj->Normal[i].y*k*-2.0f);
		b.Normalize();
		Velocity.x = b.x * 2;
		Velocity.y = b.y * 2;
	}
	// Провека на удар о грань прямоугольника
	void MultiContact(Object * obj)
	{
		if (obj->StartPos.y + obj->bm_info.bmHeight <= StartPos.y + bm_info.bmHeight + obj->bm_info.bmHeight)
			ContactRect(obj, 0);
		if (obj->StartPos.x + obj->bm_info.bmWidth <= StartPos.x + bm_info.bmWidth + obj->bm_info.bmWidth)
			ContactRect(obj, 1);
	}
	// Обработка столкновения сфер
	void ContactSphere(Object * obj)
	{
		Vector n = this->Center - obj->Center;
		n.Normalize();
		float a1 = Velocity.Scalar(n), a2 = obj->Velocity.Scalar(n);
		float oprimizeP = 1;
		Velocity = Velocity + n * oprimizeP;
		Velocity.Normalize();
		Velocity = Velocity * 2;
	}
	// Движение
	void Movement()
	{
		StartPos.x += Velocity.x;
		StartPos.y += Velocity.y;
		Center.x = StartPos.x + bm_info.bmWidth / 2;
		Center.y = StartPos.y + bm_info.bmHeight / 2;
	}
	// Проверка работы гравитации
	void ForceGravity(std::vector<Object*>& objects, int index)
	{
	}
	void Paint(HDC hdc) // Отрисовка
	{
		if (hide)
			return;
		HDC hMem = CreateCompatibleDC(hdc); // Доп контекст
		HGDIOBJ hOld = SelectObject(hMem, *hBitmap); // Выбор в него битмапа
		TransparentBlt(hdc, (int)StartPos.x, (int)StartPos.y, bm_info.bmWidth, bm_info.bmHeight,
			hMem, 0, 0, bm_info.bmWidth, bm_info.bmHeight, RGB(255, 255, 255)); // Отрисовка с исключением белого
		SelectObject(hMem, hOld);
		DeleteObject(hOld);
		DeleteDC(hMem);
	}
	Vector Velocity; // Ускорение
	Vector StartPos; // Позиция левого верхнего угла
	Vector Center; // Позиция центра
	HBITMAP *hBitmap; // Битмап
	BITMAP bm_info; // Информация о битмап
	bool Gravity; // Начиличе гравитации
	bool hide; // Отрисовка?
	TYPE type; // Тип
};

class Linz : Object // Класс линзы
{
	friend LineLight;
	friend PaintSystem;
	float k; // Преломляющая способность
public:
	Linz(HBITMAP* hb, bool gr, Vector st, TYPE _type, float _k) :Object(hb, false, st, _type)
	{
		k = _k;
	}
	Linz(WORD hb, bool gr, Vector st, TYPE _type, float _k) :Object(hb, false, st, _type)
	{
		k = _k;
	}
};

class LineLight // Класс луча
{
	friend Setup;
	friend PaintSystem;
	Vector StartPos; // Начало
	Vector Dir; // Начальное направление
	COLORREF color; // Цвет
public:
	LineLight(Vector sp, Vector dir, COLORREF c); // Конструктор
	bool static havePoint(Object* obj, Vector& vec) // Проверка на нахождении точки в объекте
	{
		return vec.x >= obj->StartPos.x && vec.x <= obj->StartPos.x + obj->bm_info.bmWidth &&
			vec.y >= obj->StartPos.y && vec.y <= obj->StartPos.y + obj->bm_info.bmHeight;
	}
	void ChangeDir(Object* obj, Vector& w, Vector& d, HDC& hdc) // Изменени направления
	{
		while ((GetPixel(hdc, (int)w.x, (int)w.y) != RGB(0, 0, 0) ||
			GetPixel(hdc, (int)w.x, (int)w.y - 1) != RGB(0, 0, 0) || GetPixel(hdc, (int)w.x, (int)w.y - 2)) &&
			havePoint(obj, w)) // Пока не достигнем края линзы
			w = w + d;
		if (!havePoint(obj, w)) // Если вдруг мимо пролетели
			return;
		LineTo(hdc, (int)w.x, (int)w.y);
		while (GetPixel(hdc, (int)w.x, (int)w.y) != RGB(0, 0, 0) && havePoint(obj, w)) // Пока не достигли противоположного края
			w = w + Vector(d.x > 0 ? 1.0f : -1.0f, 0);
		LineTo(hdc, (int)w.x, (int)w.y);
		d.Rotate(w, 1 * ((Linz*)obj)->k); // Повернули относительно прелом способности
		while (havePoint(obj, w)) // Пока не вышли за пределы объекта
			w = w + d;
	}
	void Paint(HDC hdc, std::vector<Object*> objects)
	{
		Vector way = StartPos;
		Vector t_dir = Dir;
		HPEN pen = CreatePen(PS_SOLID, 2, color); // Ручка
		HGDIOBJ obj = SelectObject(hdc, pen);
		MoveToEx(hdc, (int)way.x, (int)way.y, NULL); // Начальная позиция луча
		while (way.x >= -10 && way.x <= 610 && way.y >= -10 && way.y <= 610) // Пока не вышли за пределы 
		{
			for (auto i = objects.begin(); i != objects.end(); i++) // Проверка на пересечение со всеми объектами
				if (havePoint(*i, way))
					ChangeDir(*i, way, t_dir, hdc);
			way = way + t_dir;
		}
		LineTo(hdc, (int)way.x, (int)way.y);
		SelectObject(hdc, obj);
		DeleteObject(pen); // Удаление ручки
	}
};

class Setup :Object
{
	LineLight ** lights;
	int size;
	std::vector<Vector> GetDirection(int count)
	{
		std::vector<Vector> res;
		float step = 90.0f / count;
		for (int i = 0; i < count; i++)
		{
			Vector temp(1.0f, 0.0f);
			temp.Rotate(Vector(0.0f, 0.0f), step*i);
			res.push_back(temp);
		}
		return res;
	}
public:
	void Moving()
	{
		Center = StartPos + Vector(bm_info.bmWidth / 2, bm_info.bmHeight / 2);
		for (int i = 0; i < size; i++)
			lights[i]->StartPos = this->Center;
	}
	Setup(HBITMAP* hb, Vector st, int s) :Object(hb, false, st, TYPE::RECT)
	{
		size = s;
		std::vector<Vector> direction = this->GetDirection(s);
		lights = new LineLight*[s];
		for (int i = 0; i < s; i++)
			lights[i] = new LineLight(this->Center, direction[i], RGB(rand() % 255, rand() % 255, rand() % 255));
	}
	Setup(WORD hb, Vector st, int s) :Object(hb, false, st, TYPE::RECT)
	{
		size = s;
		std::vector<Vector> direction = this->GetDirection(s);
		lights = new LineLight*[s];
		for (int i = 0; i < s; i++)
			lights[i] = new LineLight(this->Center, direction[i], RGB(rand() % 255, rand() % 255, rand() % 255));
	}
	void ReCreate(int c_l);
};

