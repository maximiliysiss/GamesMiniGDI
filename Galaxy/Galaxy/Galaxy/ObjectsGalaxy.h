#pragma once
#include <string>
#include <Windows.h>
#include <vector>
#include <ctime>
#include <chrono>
#include "Constant.h"
#include "resource.h"
#pragma comment(lib,"MSIMG32.lib")
#pragma comment(lib, "Gdi32.lib")


class ObjectGalaxy // Объекты галактики
{
private:
	std::string name; // Имя
	bool LoadObject(); // Подгрузка объекта
	std::pair<int, int> sizeWnd; // Размеры окна
protected:
	HBITMAP hBitmap; // Битмап, который выводится на экран
	std::pair<float, float> position; // Позиция
	HBITMAP hOrigin; // Битмап-оригинал
	float gravity_rad; // Радиус гравитации
	float size; // Размеры для зумирования
	float gravity_f; // Сила грав
	BITMAP bitmap; // Информация о битмапе
	HINSTANCE hInst; // Дескриптор/айди окна
	void DrawPower(HDC hdc); // Отрисовка грав поля
	inline std::pair<float, float> GetXYMatrix(float posSX, float posSY, float x, float y, float angle) // Поворот вектора на угол - матрица поворота в основе
	{
		/*float x2 = posSX + (x - posSX)*cos(angle*PI / 180) - (y - posSY)*sin(angle*PI / 180); //Поворот на angle градусов
		float y2 = posSY + (x - posSX)*sin(angle*PI / 180) + (y - posSY)*cos(angle*PI / 180); //Поворот на angle градусов*/
		return std::pair<float, float>(posSX + (x - posSX)*cos(angle*PI / 180) - (y - posSY)*sin(angle*PI / 180), posSY + (x - posSX)*sin(angle*PI / 180) + (y - posSY)*cos(angle*PI / 180));
	}
	void RotateHBITMAP(float angle); // Поворот битмапа
	void DrawObject(HDC hdc); // Отрисовка объекта
public:
	inline void SetWndSize(int x, int y) { sizeWnd.first = x; sizeWnd.second = y; } // Установка размеров окна
	inline bool GetDrawChance() // Проверка на требуется ли рисовать или выходит за пределы
	{
		return position.first - bitmap.bmWidth / 2 > sizeWnd.first / 2 || position.first + bitmap.bmWidth / 2 < -sizeWnd.first / 2 || position.second - bitmap.bmHeight / 2 > sizeWnd.second / 2 ||
			position.second + bitmap.bmHeight / 2 < -sizeWnd.second / 2;
	}
	inline bool GetDrawChanceGravity() // Проверка на требуется ли рисовать поле гравитации
	{
		return position.first - gravity_rad / 2 > sizeWnd.first / 2 || position.first + gravity_rad / 2 < -sizeWnd.first / 2 || position.second - gravity_rad / 2 > sizeWnd.second / 2 ||
			position.second + gravity_rad / 2 < -sizeWnd.second / 2;
	}
	inline std::pair<float, float> GetPos() // Получение позиции объекта
	{
		return std::pair<float, float>(position.first, position.second);
	}
	inline int GetSize() // Получение его размеров, чтобы брать и их в расчет
	{
		return (int)((bitmap.bmWidth + bitmap.bmHeight) / 2);
	}
	inline std::string GetName() // Получение имени
	{
		return name.c_str();
	}
	ObjectGalaxy(std::string _name, HINSTANCE hInstance); 
	void virtual Draw(HDC hdc) = 0; // Перегрузить для каждого объекта
	void Resize(bool direction); // Изменить размеры
	inline HBITMAP ReturnHBMP() { return hBitmap; } // Вернуть битмап
	inline std::pair<float, float> GetGravity() { return std::pair<float, float>(gravity_rad, gravity_f); } // Вернуть информацию о гравитации
};


class Planet : public ObjectGalaxy // Планеты
{
private:
	bool isDraw; // Стоит ли отрисовывать
	float radius; // Радиус
	float angle;
	float speed;
	void DrawRadius(HDC hdc);
	inline std::pair<float, float> MovementPlanet() // Двжиение планеты по окружности
	{
		return std::pair<float, float>(radius*cos(speed*angle*PI / 180), radius*sin(speed*angle*PI / 180));
	}
public:
	inline bool isThere() // Существует ли
	{
		return isDraw;
	}
	void Block(); // Заблокировать, то есть уничтожена
	void Reload(); // Перезагрузить
	void Movement(); // Движение
	Planet(std::string name, float _rad, float _speed, HINSTANCE hInst);
	inline void AnglePlus() { angle++; } // Движение
	void Draw(HDC hdc); // Отрисовка
	void ResizeOrbit(bool direction); // Ресайз орбиты
};

class Sun : public ObjectGalaxy // Солнце
{
public:
	Sun(std::string name, HINSTANCE hInst);
	void Draw(HDC hdc);
};

class Kometa : public ObjectGalaxy // Комета
{
private:
	float KOMETA_OUT; // Точка невозврата - меняется относительно зума
	std::pair<float, float> velocity; // Ускорение
	inline std::pair<float, float> StartPos(float angle, float radius) // Установка изначальной позиции
	{
		return std::pair<float, float>(radius*cos(speed*angle*PI / 180), radius*sin(speed*angle*PI / 180));
	}
	float speed;
public:
	void Movement();
	void RotateToVelocity(); // Поворот к ускорению
	void Draw(HDC hdc);
	Kometa(std::string name, HINSTANCE hInst);
	inline bool isConnetc(std::pair<float, float> to, int rad_to) // Пересекается ли с чем-то
	{
		return (sqrt((GetPos().first - to.first)*(GetPos().first - to.first) + (GetPos().second - to.second)*(GetPos().second - to.second)) - bitmap.bmWidth / 2 - rad_to / 2) <= 0;
	}
	inline bool isGravity(std::pair<float, float> to, float gravity) // Пересекается ли с полем гравитации
	{
		return (sqrt((GetPos().first - to.first)*(GetPos().first - to.first) + (GetPos().second - to.second)*(GetPos().second - to.second)) - bitmap.bmWidth / 2 - gravity) <= 0;
	}
	void ResizeCoord(bool direction); // Ресайз кординат
	void ChangeMovement(std::pair<float, float> target, std::pair<float, float> powers); // Изменение движения кометы
	void ReloadKometa(); // Перезагрузка кометы
	inline bool Delete() // Проверка на выход за пределы и перезагрузка
	{
		if (position.first < -KOMETA_OUT || position.first>KOMETA_OUT || position.second > KOMETA_OUT || position.second < -KOMETA_OUT)
			return true;
		return false;
	}
};

extern HBITMAP Kometa_Static_Origin_bmp;