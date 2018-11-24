#pragma once
#include "GravitySystem.h"
#include "PaintSystem.h"
#include "Object.h"

class Object;

class EventSystem // Система событий - Синглетон
{
private:
	friend Object;
	friend PaintSystem;
	friend GravitySystem;
	PaintSystem * SysPaint; // Система рисовки
	GravitySystem * SysGr; // Система гравитации
	static EventSystem * events; // Само событие
	EventSystem() // Конструктор
	{
		SysGr = &GravitySystem::Instance(); // Получение ссылок на объекты
		SysPaint = &PaintSystem::Instance();
	}
	~EventSystem() {}
	EventSystem& operator=(EventSystem&) = delete;
	void AddObject(Object * obj) // Добавить объект
	{
		SysGr->AddObject(obj);
		SysPaint->AddObject(obj);
	}
public:
	enum COND_MOUSE { DOWN, MOVE, UP };
	bool MouseButton(Vector c_pos, COND_MOUSE cond)
	{
		return PaintSystem::Instance().MouseRes(c_pos, (PaintSystem::COND_MOUSE)cond); // Нажатие клавиши мыши
	}
	static EventSystem * Instance() // Создание события
	{
		if (!events)
			events = new EventSystem();
		return events;
	}
	void TickEvent(PAINTSTRUCT & hdc) // Обработка тика
	{
		SysGr->Mathematic();
		SysPaint->OnPaint(hdc);
	}
};

