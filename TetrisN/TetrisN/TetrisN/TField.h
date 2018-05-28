#pragma once
#include <Windows.h>
#include "resource.h"
#include <vector>
#pragma comment(lib,"MSIMG32.lib")
#pragma comment(lib, "Gdi32.lib")
#define OFFSET_TETR 20 // Смещение относительно размеров
class TField
{
private:

	enum typeC { f, s, t, ff }; // Стиль кубика
	enum RotatePos { N, L, D, R }; // Поворот фигуры
	static HBITMAP bitmap_q[4]; // Все стили кубиков
	class Figure // Фигура
	{
	protected:
		void ToField(TField & field); // Перед уничтожение скопипастить фигуру на поле
		typeC type; // Тип кубиков
		bool Check(std::pair<int, int> Coord, TField & field); // Проверка на доступность клетки
		int pCoord[4][2]; // Все координаты тетринки
		bool IsFall(TField & field); // Проверка на падение на кадр
	public:
		Figure() // Конструктор
		{
			type = (typeC)(rand() % 4);
		}
		bool InFigure(std::pair<int, int> pos) // Является ли точка частью фигуры
		{
			for (int i = 0; i < 4; i++)
				if (pos.first == pCoord[i][0] && pos.second == pCoord[i][1])
					return true;
			return false;
		}
		// Получение точек падения
		std::vector<std::pair<int, int>> virtual GetFallCoord(TField & field)
		{
			std::vector<std::pair<int, int>> result;
			for (int i = 0; i < 4; i++)
				if (!InFigure(std::pair<int, int>(pCoord[i][0] + 1, pCoord[i][1])))
					result.push_back(std::pair<int, int>(pCoord[i][0] + 1, pCoord[i][1]));
			return result;
		}
		// Будет перегружена 
		// в наследниках - нужна для того, чтобы получать все клетки предположительного падения
		bool Fall(TField & field); // Падание фигуры
		void OnPaint(TField & field, HDC hdc); // Отрисовка
		virtual ~Figure() {} // Деструктор
		void Turn(bool direct, TField & field); // Движение влево/вправо
		virtual void Rotate(TField & field); // Поворот фигуры
	};

	class I_Figure :public Figure // Тип_Фигура
	{
	public:
		I_Figure(TField & field) :Figure() // Конструктор нужен для задания изначальных позиций тетринок
			// Точно также описываются и все оставшиеся 4 фигуры
		{
			for (int i = 0; i < 4; i++)
			{
				this->pCoord[i][0] = i;
				this->pCoord[i][1] = field.GetSize()[1] / 2;
			}
		}
	};
	class Z_Figure :public Figure // Смотри выше комментарий
	{
	public:
		Z_Figure(TField & field) :Figure()
		{
			pCoord[0][0] = 1;
			pCoord[0][1] = field.GetSize()[1] / 2 - 1;
			pCoord[1][0] = 2;
			pCoord[1][1] = field.GetSize()[1] / 2 - 1;
			pCoord[2][0] = 2;
			pCoord[2][1] = field.GetSize()[1] / 2;
			pCoord[3][0] = 3;
			pCoord[3][1] = field.GetSize()[1] / 2;
		}
	};
	class G_Figure :public Figure // Смотри выше
	{
	public:
		G_Figure(TField& field)
		{
			for (int i = 0; i < 3; i++)
			{
				pCoord[i][0] = i + 1;
				pCoord[i][1] = field.GetSize()[1] / 2 - 1;
			}
			pCoord[3][0] = 3;
			pCoord[3][1] = field.GetSize()[1] / 2;
		}
	};
	class Q_Figure :public Figure // Смотри выше
	{
	public:
		Q_Figure(TField & field)
		{
			for (int i = 0; i < 4; i++)
			{
				pCoord[i][0] = (i % 2) + 2;
				pCoord[i][1] = (i / 2 == 0 ? field.GetSize()[1] / 2 - 1 : field.GetSize()[1] / 2);
			}
		}
		void Rotate(TField & field) {}
		std::vector<std::pair<int, int>> GetFallCoord(TField & field)
		{
			std::vector<std::pair<int, int>> result; // Вектор для хранение результатов
			result.push_back(std::pair<int, int>(pCoord[1][0] + 1, pCoord[1][1])); // Добавление предположительного
			// результата
			result.push_back(std::pair<int, int>(pCoord[3][0] + 1, pCoord[3][1]));
			return result;
		}
	};
	class T_Figure :public Figure // Смотри выше
	{
	public:
		T_Figure(TField & field) :Figure()
		{
			for (int i = 0; i < 3; i++)
			{
				pCoord[i][0] = 3;
				pCoord[i][1] = field.GetSize()[1] / 2 - 1 + i;
			}
			pCoord[3][0] = 2;
			pCoord[3][1] = field.GetSize()[1] / 2;
		}
	};

	struct Cell // Клетка 
	{
		Cell & operator=(const Cell & c) // Оператор копирования
		{
			this->empty = c.empty;
			this->paint = c.paint;
			return *this;
		}
		Cell() { empty = true; paint = typeC::f; } // Дефолт конструктор
		bool empty; // Проверка на пустоту клетки
		typeC paint; // Стиль клетки
	};

private:
	Figure* CreateRndF(); // Сгенерировать случайную фигуру
	RotatePos rp; // Текущий поврот
	int Coord[2]; // Координаты верхнего левого угла поля
	int sizeS[2]; // Размеры поля
	Cell **field; // Поле
	Figure * CurrentFigure; // Текущая фигура
public:
	int * GetSize() { return sizeS; } // Получить размеры
	int * GetCoord() { return Coord; } // Получить Координаты
	TField(int sizeX, int sizeY, int st_x, int st_y) // Конструктор
	{
		rp = RotatePos::N;
		sizeX += 4; // Размер линий больше на 4, потому что там буду прятаться сами фигуры во время появления
		sizeS[0] = sizeX;
		sizeS[1] = sizeY;
		Coord[0] = st_x;
		Coord[1] = st_y;
		field = new Cell*[sizeS[0]];
		for (int i = 0; i < sizeS[0]; i++)
			field[i] = new Cell[sizeS[1]];
		CurrentFigure = this->CreateRndF();
	}
	void OnPaint(HDC hdc); // Отрисовка
	~TField()
	{
		if (CurrentFigure != NULL) // Удаление указателей
			delete CurrentFigure;
		for (int i = 0; i < sizeS[0]; i++)
			delete[] field[i];
		delete[] field;
	}
	void RotateFigure() // Поворот фигуры
	{
		if (CurrentFigure != NULL)
			CurrentFigure->Rotate(*this);
	}
	void Turns(bool Direct) // Ход влево/вправо
	{
		CurrentFigure->Turn(Direct, *this);
	}
	void CopyLineUp(int index); // От определенного индекса сместить все вниз, тип, упали
	void CheckLine(); // Проверка линий
	bool Finish() // Проврка на завершенность
	{
		for (int i = 0; i < sizeS[1]; i++)
			if (!field[3][i].empty)
				return true;
		return false;
	}
	void Clear(); // Очистка поля
};

