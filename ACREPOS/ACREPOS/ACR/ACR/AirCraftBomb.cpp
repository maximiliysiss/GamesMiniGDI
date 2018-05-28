#include "SceneObjects.h"

// Файл бомб

void AirCraftBomb::Movement()
{
	position.first += velocity.first*force;
	position.second += velocity.second*force;
	force += 0.001f; // Увеличение скорости падения бомб
	velocity.first += (direct ? -1 : 1)*0.01f*force;
	velocity.second += 0.1f*(force);
}

bool AirCraftBomb::Delete() // Проверка на выход вне
{
	return position.second > BOMB_OUT;
}