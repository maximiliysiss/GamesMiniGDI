#include "ObjectsGalaxy.h"

Planet::Planet(std::string name, float _rad, float _speed, HINSTANCE hInst) :ObjectGalaxy(name, hInst), radius(_rad), speed(_speed)
{
	isDraw = true; // Отрисовать ли?
	angle = (float)(rand() % 360);
	gravity_rad = size + 10 + rand() % 60;
	gravity_f = 1.0f / (1 + rand() % 10);
	position.first = (MovementPlanet().first);
	position.first = (MovementPlanet().second);
}

void Planet::Block() // Блокировка
{
	isDraw = false;
}

void Planet::Reload() // Перезагрузка
{
	isDraw = true;
}

void Planet::Movement() // Движение
{
	if (!isDraw) // Не двигать, когда заблокирована
		return;
	AnglePlus();
}

void Planet::Draw(HDC hdc)
{
	if (!isDraw) // Не рисовать, когда заблокирована
		return;
	this->DrawPower(hdc); // Отрисовка гравитац поля
	this->DrawRadius(hdc); // Отрисовка радиуса движения
	this->DrawObject(hdc); // Отрисовка объекта
	position.first = (MovementPlanet().first); // Получение позиции
	position.second = (MovementPlanet().second);
}

void Planet::DrawRadius(HDC hdc) // Отрисовка движения (смотреть отрисовку у грав поля)
{
	HPEN pen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	HBRUSH brush = (HBRUSH)GetStockObject(NULL_BRUSH);
	SelectObject(hdc, pen);
	SelectObject(hdc, brush);
	Ellipse(hdc, -(int)radius,
		-(int)radius,
		(int)radius,
		(int)radius);
	DeleteObject(pen);
	DeleteObject(brush);
}


void Planet::ResizeOrbit(bool direction) // Ресайз радиуса
{
	radius = direction ? radius * sZoom : radius / sZoom;
}