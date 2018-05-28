#pragma once
#include <Windows.h>
#include "resource.h"
#pragma comment(lib,"MSIMG32.lib")
#pragma comment(lib, "Gdi32.lib")

class TField
{
private:
	struct Record // Запись поля
	{
		enum Color { BLACK, RED, BLUE };
		struct Cell
		{
			Cell() { side = ' '; color = Color::BLACK; }
			char side;
			Color color;
		};
		Record(int _size); // Конструктор по размеру - пустой
		Record() { size = 0; field = NULL; } // Дефолт конструктор
		Record(Cell** f, int _size); // Конструктор по полю
		Cell** field; // Поле
		int size; // Размер
		Record& operator=(const Record & rec); // Оператор копирования
		~Record() // Деструктор
		{
			for (int i = 0; i < size; i++)
				delete[] field[i];
		}
	};
	static HBITMAP FIGURE[6]; // Фигкры крестика и нолика
	Record *database; // БД
	int size; // размер
	int start_sizeX; // координата начала отрисовки поля X
	int start_sizeY; // координата начала отрисовки поля Y
	int offset; // отступ для клетки
	int turn; // текущий ход
	int reload; // ход отрисовки
	bool Turn(int i, int j, char side); // Ход
	TField() {} // Дефолт конструктор
	bool FindSubstr(const char * Line, const char* sbt, int len_sbt)
	{
		for (int i = 0; i < size - len_sbt + 1; i++)
			if (Line[i] == sbt[0])
			{
				int j = 0;
				for (; j < len_sbt && sbt[j] == Line[i]; i++, j++) {}
				if (j == len_sbt)
					return true;
				else
					i--;
			}
		return false;
	}
	char WinLine(const char * Line)
	{
		int k = 0;
		for (int i = 0; i < size - 1; i++)
			if (Line[i] == Line[i + 1])
				k++;
		if (k == size - 1 && Line[0] != ' ')
			return Line[0];
		return ' ';
	}
	char PrevWin(const char * Line)
	{
		char * space = new char[1]{ ' ' };
		char * s_X = new char[size - 1];
		char * s_O = new char[size - 1];
		for (int i = 0; i < size - 1; i++)
		{
			s_X[i] = 'x';
			s_O[i] = 'o';
		}
		if (this->FindSubstr(Line, s_X, size - 1) && this->FindSubstr(Line, space, 1))
			return 'x';
		if (this->FindSubstr(Line, s_O, size - 1) && this->FindSubstr(Line, space, 1))
			return 'o';
		delete[] s_X;
		delete[] s_O;
		return ' ';
	}
public:
	TField(int size_s, int st_x, int st_y, int size_wnd); // Конструктор по размеру, начальным коордами и размером поля
	void OnPaint(HDC hdc); // Отрисовка
	bool OnClick(RECT wndRect, POINT CursorPos, char side); // Обработка нажатия
	char Finish(); // Проверка на завершение
	void Clear() // Очитска
	{
		turn = reload = 0;
		Record rec(size);
		database = new Record[size*size + 1];
		database[turn] = rec;
	}
	void MoveR(bool direct) // Перемещение при промотке партии
	{
		if (direct)
		{
			if (reload != 0)
				reload--;
		}
		else
			if (reload != turn)
				reload++;
	}
};

