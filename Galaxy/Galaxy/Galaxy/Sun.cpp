#include "ObjectsGalaxy.h"

void Sun::Draw(HDC hdc) // Отрисовка солнца
{
	this->DrawObject(hdc); // Отрисовка солнца
	this->DrawPower(hdc); // Отрисовка силы гравитаиции
}

Sun::Sun(std::string name, HINSTANCE hInst) :ObjectGalaxy(name, hInst)
{
	position.first = position.second = 0; // Установка позиции
	gravity_rad = size + 200; // Радиус грав
	gravity_f = 0.5; // Сила грав
}