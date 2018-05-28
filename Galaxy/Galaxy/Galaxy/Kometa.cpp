#include "ObjectsGalaxy.h"

void Kometa::Draw(HDC hdc) // Отрисовка
{
	this->DrawObject(hdc); // Отрисовка кометы
}

void Kometa::RotateToVelocity() // Поворот к вектору движения
{
	const std::pair<float, float> from(0.0f, -1.0f); // Изначальное направление на исходнике
	float scal = velocity.first*from.first + velocity.second*from.second; // Скалярное
	float size1 = sqrt(velocity.first*velocity.first + velocity.second*velocity.second), // Длины векторов
		size2 = sqrt(from.first*from.first + from.second*from.second);
	float cos = scal / (size1*size2); // Косинус между ними
	float angle = acos(cos) * 180 / PI; // Итоговый угол
	if (velocity.first < 0)angle *= -1; // Относительно поворота
	this->RotateHBITMAP(angle); // Поворот
}

void Kometa::ReloadKometa() // Перезагрузка кометы (смотри конструктор, те же поля)
{
	float angle = (float)(1 + rand() % 360);
	position.first = StartPos(angle, KOMETA_OUT - 1).first;
	position.second = StartPos(angle, KOMETA_OUT - 1).second;
	velocity.first = (float)(position.first <= 0 ? 1 : -1)*(1 + rand() % MAX_ANGLE_OUT);
	velocity.second = (float)(position.first <= 0 ? 1 : -1)*(1 + rand() % MAX_ANGLE_OUT);
	this->RotateToVelocity();
}

Kometa::Kometa(std::string name, HINSTANCE hInst) : ObjectGalaxy(name, hInst)
{
	speed = SPEED_KOMETA; // Скосроть
	KOMETA_OUT = OUT_KOMETA; // Выход за пределы
	float angle = (float)(1 + rand() % 360); // Угол откуда вылетает
	position.first = StartPos(angle, KOMETA_OUT - 10).first; // Позиции
	position.second = StartPos(angle, KOMETA_OUT - 10).second;
	velocity.first = (float)(position.first <= 0 ? 1 : -1)*(1 + rand() % MAX_ANGLE_OUT); // Ускорение
	velocity.second = (float)(position.first <= 0 ? 1 : -1)*(1 + rand() % MAX_ANGLE_OUT);
	this->RotateToVelocity(); // Поворот относительно ускорения
}

void Kometa::ResizeCoord(bool direction)
{
	position.first = (direction ? position.first*sZoom : position.first / sZoom);
	position.second = (direction ? position.second*sZoom : position.second / sZoom);
	speed = direction ? speed * sZoom : speed / sZoom;
	KOMETA_OUT = (direction ? KOMETA_OUT*sZoom : KOMETA_OUT / sZoom);
	this->RotateToVelocity();
}

void Kometa::Movement()
{
	position.first += velocity.first *speed;
	position.second += velocity.second *speed;
}

void Kometa::ChangeMovement(std::pair<float, float> target, std::pair<float, float> powers)
{
	std::pair<float, float> wayVec(target.first - GetPos().first, target.second - GetPos().second);
	float sizeWay = sqrt(wayVec.first*wayVec.first + wayVec.second*wayVec.second);
	wayVec.first = wayVec.first / sizeWay * powers.second * (ADDICTIVE_TOFORCE * 100 / sizeWay);
	wayVec.second = wayVec.second / sizeWay * powers.second * (ADDICTIVE_TOFORCE * 100 / sizeWay);
	velocity.first += wayVec.first;
	velocity.second += wayVec.second;
	this->RotateToVelocity();
}
