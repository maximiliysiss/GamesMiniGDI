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
	enum MoveType { Ox, Oy, Oxy };
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
	MoveType moveType;
	Object(HBITMAP *hb, bool gr, Vector st, TYPE type, Vector normal = Vector(0.0f, 0.0f), MoveType moveType = Oxy); // Конструкторы
	Object(WORD hb_name, bool gr, Vector st, TYPE type, Vector normal = Vector(0.0f, 0.0f), MoveType moveType = Oxy);
protected:
	// Движение
	virtual void Movement();
	virtual void SetPosition(Vector vec);
	// Проверка работы гравитации
	void ForceGravity(std::vector<Object*>& objects, int index)
	{
	}
	void Paint(HDC hdc); // Отрисовка
	Vector Velocity; // Ускорение
	Vector StartPos; // Позиция левого верхнего угла
	Vector Center; // Позиция центра
	HBITMAP *hBitmap; // Битмап
	BITMAP bm_info; // Информация о битмап
	bool Gravity; // Начиличе гравитации
	bool hide; // Отрисовка?
	TYPE type; // Тип
};

class Linz : public Object // Класс линзы
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
	void ChangeDir(Object* obj, Vector& w, Vector& d, HDC& hdc); // Изменени направления
	void Paint(HDC hdc, std::vector<Object*> objects);
};

class Setup : public Object
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
	void Movement();
	void SetPosition(Vector vec);
	void Moving()
	{
		Center = StartPos + Vector(bm_info.bmWidth / 2, bm_info.bmHeight / 2);
		for (int i = 0; i < size; i++)
			lights[i]->StartPos = this->Center;
	}
	Setup(HBITMAP* hb, Vector st, int s) :Object(hb, false, st, TYPE::RECT, Vector(0.0f, 0.0f), Object::MoveType::Oy)
	{
		size = s;
		std::vector<Vector> direction = this->GetDirection(s);
		lights = new LineLight*[s];
		for (int i = 0; i < s; i++)
			lights[i] = new LineLight(this->Center, direction[i], RGB(rand() % 255, rand() % 255, rand() % 255));
	}
	Setup(WORD hb, Vector st, int s) :Object(hb, false, st, TYPE::RECT, Vector(0.0f, 0.0f), Object::MoveType::Oy)
	{
		size = s;
		std::vector<Vector> direction = this->GetDirection(s);
		lights = new LineLight*[s];
		for (int i = 0; i < s; i++)
			lights[i] = new LineLight(this->Center, direction[i], RGB(rand() % 255, rand() % 255, rand() % 255));
	}
	void ReCreate(int c_l);
};

