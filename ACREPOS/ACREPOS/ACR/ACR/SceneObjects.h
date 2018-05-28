#pragma once
#include <string>
#include <sstream>
#include <Windows.h>
#include <vector>
#include <chrono>
#include "Settings.h"
#include "resource.h"
#pragma comment(lib,"MSIMG32.lib")
#pragma comment(lib, "Gdi32.lib")

inline WORD GETNAMERESOURSE(std::string name) // Получение ресурса
{
	// Относительно имени получаем битмап
	if (name == "Aircraft") return IDB_AIRCRAFT;
	if (name == "AntiAirCraft") return IDB_AAC;
	if (name == "Bomb") return IDB_BOMB;
	if (name == "Patron") return IDB_PATRON;
	return NULL;
}

const float PI = 3.14159265f;

class Object // Объект, основной, для всех
{
	std::string name; // Имя
	bool LoadBMP() // Загрузка bitmap объекта
	{
		// Загрузка из ресурсов
		hBitmap = (HBITMAP)LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(GETNAMERESOURSE(name)));
		if (hBitmap == NULL)
			return false;
		GetObject(hBitmap, sizeof(BITMAP), &bitmap); // Получение информации в битмап 
		return true;
	}
protected:
	HINSTANCE hInst; // Идентиификатор процесса
	HBITMAP hBitmap;
	BITMAP bitmap; // Битман для получения информации об изображении
	std::pair<float, float> position; // Позиция (левый верхний угол)
	int sizeX, sizeY; // Размеры поля
	void virtual SetPos() {} // Для установки ползиции, объекты создаются еще до события WM_SIZE
	bool Wait(std::chrono::steady_clock::time_point t_p, float sek) // Функция ожидания
	{
		return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - t_p).count() >= sek;
	}
	void RotateHBITMAPFlip() // Поворот изображения на 180 градусов
	{
		POINT point[3]; // Указатель на 3 точки, формирующие полигон с битмапом
		point[0].x = bitmap.bmWidth; point[0].y = 0; // Новый верхний левый угол
		point[1].x = point[1].y = 0; // Новый верхний правый угол
		point[2].x = bitmap.bmWidth; point[2].y = bitmap.bmHeight; // Новый нижний левый угол
		HDC hdc = CreateCompatibleDC(NULL); // Создание простого контекста для хранения
		SelectObject(hdc, hBitmap);
		PlgBlt(hdc, point, hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, NULL, 0, 0); // Поворот к контексте
		hBitmap = (HBITMAP)GetCurrentObject(hdc, OBJ_BITMAP);
		DeleteDC(hdc);
	}
public:
	void virtual DrawObject(HDC hdc) final // Отрисовка объекта, общая для всех
	{
		HDC hMem = CreateCompatibleDC(hdc); // Соотнесение с полученным контекстом
		HBITMAP hOld = (HBITMAP)SelectObject(hMem, hBitmap); // Двойная буферизация
		TransparentBlt(hdc, (int)position.first, (int)position.second, bitmap.bmWidth, bitmap.bmHeight, hMem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, RGB(255, 255, 255)); // Копирование с пропуском белого
		SelectObject(hMem, hOld);
		DeleteDC(hMem); // Удаление контекста
		DeleteObject(hOld);
	}
public:
	inline void SetSize(int x, int y) { sizeX = x; sizeY = y; SetPos(); } // Установка контекста
	inline std::pair<float, float> GetSizeAll() { return std::pair<float, float>((float)bitmap.bmWidth, (float)bitmap.bmHeight); } // Получение всех размеров
	inline std::string GetName() { return name; } // Получение имени
	inline float GetSizeH() { return (float)(bitmap.bmHeight + bitmap.bmWidth) / 2; } // Получение некого усреднения размера по высоте
	inline std::pair<float, float> GetPos() { return position; } // Получение позиции
	Object(std::string _name, HINSTANCE hInstance) :name(_name), hInst(hInstance)
	{
		if (!LoadBMP())
		{
			MessageBox(NULL, std::string("Error load bitmap for " + name).c_str(), "Error BMP", NULL);
			this->~Object();
		}
	}
	virtual ~Object() {} // Для пресечения утечек памяти
};

class AntiAirCraft : public Object // Зенитка
{
private:
	class Patron : public Object // Патрон зенитки
	{
	public:
		Patron(std::string _name, HINSTANCE _hInst, std::pair<float, float> AACoord, int size) :Object(_name, _hInst)
		{
			position.first = AACoord.first + size - bitmap.bmWidth / 2; // Смещение относительно дула
			position.second = AACoord.second + OFFSET_PATRON;
		}
		void Movemenet(); // Передвижение
		bool Delete() // Удалении в случае выхода вне
		{
			return position.second < PATRON_OUT;
		}
		bool HaveShoot(std::pair<float, float> pos, float size) // Проверка пересечения 
		{
			return sqrt((pos.first - position.first)*(pos.first - position.first) + (pos.second - position.second)*(pos.second - position.second)) - size / 2 - bitmap.bmWidth / 2 <= 0;
		}
	};
	std::vector<AntiAirCraft::Patron> Patrons; // Вектор патронов (массив)
	float speed; // Скорость
	void SetPos() // Устновка ползиции
	{
		position.first = (float)sizeX / 2 - bitmap.bmWidth / 2; // Перемещение по оси X
		position.second = SPOSITION_AAC; // Перемещение по оси Y
	}
	std::chrono::steady_clock::time_point time_reload; // Запоминание времени выстрела
	float reload; // Время перезарядки
	float * AntiAirCraft::GetModelSizes(); // Получение размеров зенитки - разделена на 7 частей (7 квадратов)
	float * PosS; // Позиции для квадратов
public:
	inline void ClearPatron()
	{
		Patrons.clear();
	}
	inline float* GetSizesP() { return PosS; } //Получения указателя на размеры
	std::pair<float, float> * GetModelPosP(); // Получения позиций
	bool isConnect(std::pair<float, float> target, float size) // Проверка пересечения объекта с патронами для бомб
	{
		for (UINT i = 0; i < Patrons.size(); i++)
			if (Patrons[i].HaveShoot(target, size)) // Успех  - удаляем патрон
			{
				Patrons.erase(Patrons.begin() + i);
				return true;
			}
		return false;
	}
	bool isConnect(std::pair<float, float>* target, float* size) // Провека пересечения объекта, представленного массивом квадратов с патроном
	{
		for (UINT i = 0; i < Patrons.size(); i++)
			for (UINT j = 0; j < 4; j++)
				if (Patrons[i].HaveShoot(target[j], size[j]))
				{
					Patrons.erase(Patrons.begin() + i);
					delete[] target;
					return true;
				}
		delete[] target; // Удаление позиций цели
		return false;
	}
	void Moving(bool); // Движение
	void CreatePatron(); // Создание патрона
	void Draw(HDC hdc); // Отрисовка
	AntiAirCraft(std::string _name, HINSTANCE _hInst, float _speed) :Object(_name, _hInst), speed(_speed)
	{
		PosS = GetModelSizes();
		reload = RELOAD_SPEED;
	}
	~AntiAirCraft()
	{
		delete[] PosS;
	}
};

class AirCraft : public Object
{
private:
	float SetLine(); // Получение линии, доступной для пролета
	bool direction; // Направление
	int indexLine; // Текщий индекс линии
	float WaitTimeSek; // Ожидание перед вылетом
	float speed; // Скорость
	std::chrono::steady_clock::time_point time_reload, time_wait; // Время ожидание и время перезагрузки перед сбросом бомбы
	float reload; // Время перезагрузки
	inline float* GetModelSizes(); // Модули размеров
	float * SizeS; // Модули размеров
	void SetPos()
	{
		WaitTimeSek = 1.0f + 1 / (float)(1 + rand() % 3) * (rand() % 2 == 0 ? 1 : -1) + 1;
		time_wait = std::chrono::high_resolution_clock::now();
		position.first = (float)(direction ? 0 - OFFSET_AIRCRAFT : sizeX + OFFSET_AIRCRAFT);
	}
public:
	inline float* GetSizes() { return SizeS; }
	inline float GetLine() { return position.second; }
	inline bool Reload() // Перемещение самолета за пределы видимой области и его повторный запуск
	{
		return (direction ? position.first >= sizeX + OFFSET_AIRCRAFT : position.first < 0 - OFFSET_AIRCRAFT);
	}
	void Draw(HDC hdc);
	void Movement();
	void CreateBomb();
	void ReloadBMP(); // Перезагрузка изображения
	std::pair<float, float> * AirCraft::GetModelPos(); // Возврат позиций для проверки столкновения
	AirCraft(std::string _name, HINSTANCE _hInst);
};


class AirCraftBomb : public Object // Бомбы
{
	inline bool HaveInsp(std::pair<float, float> pos, float size) // Проверка пересечения
	{
		return sqrt((pos.first - position.first)*(pos.first - position.first) + (pos.second - position.second)*(pos.second - position.second)) - size / 2 - bitmap.bmWidth / 2 <= 0;
	}
	std::pair<float, float> velocity; // Ускорение
	float force; // Сила гравитации
	float size; // Размер
	bool direct; // Направление
public:
	bool isConnect(std::pair<float, float>* target, float* size) // Проверка пересечения
	{
		for (int i = 0; i < 7; i++)
			if (this->HaveInsp(target[i], size[i]))
			{
				delete[] target;
				return true;
			}
		delete[] target;
		return false;
	}
	inline std::pair<float, float>* GetPosP() // Получение позиции
	{
		return &position;
	}
	inline float * GetSizeP() // Получение размеров
	{
		return &size;
	}
	AirCraftBomb(std::string _name, HINSTANCE _hInst, bool direction, float offsetX, float offsetY, std::pair<float, float> ACCoord, float ac_speed) :Object(_name, _hInst)
	{
		if (direction) // изменение в случае направления в другую сторону
			this->RotateHBITMAPFlip();
		position.first = ACCoord.first + (direction ? offsetX / 2 : 0); // позиция Х
		position.second = ACCoord.second; // Позиция Y
		force = FORCE;
		velocity.first = (float)(direction ? 1 : -1) * 3 * ac_speed/10;
		velocity.second = 1;
		direct = direction;
		size = (float)bitmap.bmWidth;
	}
	bool Delete(); // Проверка на удаление
	void Movement();
};

//Мегаглобальные переменные

extern std::vector<AirCraftBomb> Bombs; // Вектор бомб
extern std::vector<AirCraft> AllAirCrafts; // Вектор самолетов
extern bool * AccessLine; // Вектор линиий