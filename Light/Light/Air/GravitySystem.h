#pragma once
#include "Object.h"

class EventSystem;

class GravitySystem // Система гравитации
{
public:
	friend EventSystem;
	static GravitySystem& Instance() // Создание
	{
		if (!it)
			it = new GravitySystem();
		return *it;
	}
private:
	void Mathematic(); // Математика
	void AddObject(Object* obj) // Добавить объект
	{
		objects_gr.push_back(obj);
	}
	std::vector<Object*> objects_gr; // Объекты
	static GravitySystem* it;
	GravitySystem() {}
	~GravitySystem() {}
	GravitySystem& operator=(const GravitySystem&) = delete;
};
