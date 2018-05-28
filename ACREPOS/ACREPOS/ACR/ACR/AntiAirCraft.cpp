#include "SceneObjects.h"

// Файл зенитки

void AntiAirCraft::Moving(bool direction) // Передвижение относительно выбранного направления
{
	if (direction)
	{
		if (position.first + 1 * speed + bitmap.bmWidth <= sizeX) // Проверка на выход за пределы
			position.first += 1 * speed;
	}
	else
	{
		if (position.first - 1 * speed >= 0)
			position.first -= 1 * speed;
	}
}

std::pair<float, float> * AntiAirCraft::GetModelPosP() // Получение позиций
{
	std::pair<float, float> * result = new std::pair<float, float>[7];
	for (UINT i = 0; i < 6; i++)
		result[i] = std::pair<float, float>(position.first + bitmap.bmWidth / 2 - 6, position.second + 12 * i);
	result[6] = std::pair<float, float>(position.first + bitmap.bmWidth / 2, position.second + 72 + 37);
	return result;
}

float * AntiAirCraft::GetModelSizes() // Получение размеров
{
	float * result = new float[7];
	for (UINT i = 0; i < 6; i++)
		result[i] = 12;
	result[6] = 77;
	return result;
}

void AntiAirCraft::Draw(HDC hdc) // Отрисовка
{
	for (UINT i = 0; i < Patrons.size(); i++) // Отрисовка патронов и их передвижение
	{
		Patrons[i].DrawObject(hdc);
		Patrons[i].Movemenet();
		if (Patrons[i].Delete())
		{
			Patrons.erase(Patrons.begin() + i);
			i--;
		}
	}
	this->DrawObject(hdc); // Отрисовка зенитки
}

void AntiAirCraft::Patron::Movemenet() // Движение патрона
{
	position.second -= PATRON_SPEED;
}

void AntiAirCraft::CreatePatron() // Создание патрона с учетом перезарядки
{
	if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - time_reload).count() / 1000 >= reload)
	{
		time_reload = std::chrono::high_resolution_clock::now();
		Patrons.push_back(Patron("Patron", hInst, position, bitmap.bmWidth / 2));
	}
}