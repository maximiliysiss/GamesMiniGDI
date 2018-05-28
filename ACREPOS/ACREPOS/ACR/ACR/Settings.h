#pragma once
#define HEIGHT_OF_ACFLY 200.0f // Высота полета самолетов от верха
#define RELOAD_SPEED 0.5f // Перезарядка пушки
#define FORCE 0.6f // Изначальная сила притяжения
#define AIRCRAFT_SPEED 4.0f // Скорость самолета минимальная
#define MAX_AIRCRAFT 3 // Максимальное количество самолетов


// Настройки, которые не стоит изменять


#define PATRON_OUT -50 // Максимальный выход ха пределы для патрона
#define SPOSITION_AAC 500 // Позиция Y для зенитки
#define OFFSET_AIRCRAFT 100 // Макс выход для самолета
#define BOMB_OUT 610 // высота уничтожения для бомб
#define SPEED_AAC 4 // Скороть зенитки
#define OFFSET_OF_LINES 22.0f // Ширина линнии пролета самолета
#define OFFSET_PATRON 30 // Выход для патрона
#define TIMER_BOMB 2.0f // Время выпуска бомб

// Но иногда можно

namespace
{
	int COUNT_AIRCRAFT = 4; // Количество самолетов
	const char* AppName = "AirCraft Detroyer"; 
	const char* ClassName = "AirCraftDestr";
	float PATRON_SPEED = 10.0f; // Скорость патрона
}