#include "TField.h"

// Задание статической переменной, которая хранит все стили тетринок
HBITMAP TField::bitmap_q[4] = { LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_Q1)),
LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_Q2)) ,
LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_Q3)) ,
LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_Q4)) };

// Копипаста в поле фигуры
void TField::Figure::ToField(TField & field)
{
	for (int i = 0; i < 4; i++)
	{
		field.field[pCoord[i][0]][pCoord[i][1]].empty = false;
		field.field[pCoord[i][0]][pCoord[i][1]].paint = type;
	}
}

// Проверка на доступность клетки
bool TField::Figure::Check(std::pair<int, int> Coord, TField & field)
{
	if (Coord.first >= field.sizeS[0] || Coord.second >= field.sizeS[1]
		|| Coord.first < 0 || Coord.second < 0)
		return false; // Сразу отсекаем то, что вне
	return field.field[Coord.first][Coord.second].empty;
}

// Проверка на падение
bool TField::Figure::IsFall(TField & field)
{
	std::vector<std::pair<int, int>> fCoord = GetFallCoord(field); // Получаем все коорднаты предположительного 
	//падения
	for (UINT i = 0; i < fCoord.size(); i++)
		if (!Check(fCoord[i], field))
			return false;
	return true;
}

// Падение
bool TField::Figure::Fall(TField & field)
{
	if (IsFall(field))
	{
		for (int i = 0; i < 4; i++)
			pCoord[i][0]++;
		return false; // Все прошло успешно
	}
	else
	{
		this->ToField(field);
		return true; // Придется удалить фигуру
	}
}

// Отрисовка
void TField::Figure::OnPaint(TField & field, HDC hdc)
{
	for (int i = 0; i < 4; i++)
	{
		if (pCoord[i][0] >= 4 && pCoord[i][0] < field.GetSize()[0] &&
			pCoord[i][1] >= 0 && pCoord[i][1] < field.GetSize()[1])
		{
			HDC hMem = CreateCompatibleDC(hdc);// Создание контекста
			SelectObject(hMem, bitmap_q[type]); // Выбор кубика по текущему стилю
			// Копирование из одного контекста в другой относительно координат
			BitBlt(hdc, field.GetCoord()[1] + pCoord[i][1] * OFFSET_TETR, field.GetCoord()[0] + (pCoord[i][0] - 4) * OFFSET_TETR, OFFSET_TETR, OFFSET_TETR, hMem, 0, 0,
				SRCCOPY);
			DeleteDC(hMem);
		}
	}
}

// Ход
void TField::Figure::Turn(bool direct, TField & field)
{
	if (direct) // Вправо
	{
		for (int i = 0; i < 4; i++)
			if (!Check(std::pair<int, int>(pCoord[i][0], pCoord[i][1] + 1), field))
				return; // Предпроверка возможности хода
		for (int i = 0; i < 4; i++)
			pCoord[i][1]++;
	}
	else // Влево
	{
		for (int i = 0; i < 4; i++)
			if (!Check(std::pair<int, int>(pCoord[i][0], pCoord[i][1] - 1), field))
				return;
		for (int i = 0; i < 4; i++)
			pCoord[i][1]--;
	}
}

void TField::Figure::Rotate(TField & field) // Поворот
{
	int new_coord[4][2]; // Для хранение координат
	for (int i = 0; i < 4; i++)
	{
		std::pair<int, int> n_Coord(pCoord[i][0] - pCoord[1][0], pCoord[i][1] - pCoord[1][1]);
		int x = n_Coord.first;
		int y = n_Coord.second;
		n_Coord.first = (int)round(x*cos(90 * 3.14 / 180) - y*sin(90 * 3.14 / 180));
		n_Coord.second = (int)round(x*sin(90 * 3.14 / 180) + y*cos(90 * 3.14 / 180));
		new_coord[i][0] = pCoord[1][0] + n_Coord.first;
		new_coord[i][1] = pCoord[1][1] + n_Coord.second; // Вычисляем новые координаты по матрице поворота
		// За точку центра принимаем точку в массиве хранение №1
		if (!Check(std::pair<int, int>(new_coord[i][0], new_coord[i][1]), field))
			return;// Если нет возможности повернуть
	}
	for (int i = 0; i < 4; i++)
	{// Поворот
		pCoord[i][0] = new_coord[i][0];
		pCoord[i][1] = new_coord[i][1];
	}
	field.rp = (RotatePos)((field.rp + 1) % 4); // Смещение текущего положения поворота
}


// Генерация случайной фигуры
TField::Figure* TField::CreateRndF()
{
	switch (rand() % 5)
	{
	case 0:
		return new I_Figure(*this);
	case 1:
		return new G_Figure(*this);
	case 2:
		return new Q_Figure(*this);
	case 3:
		return new Z_Figure(*this);
	case 4:
		return new T_Figure(*this);
	default:
		return NULL;
	}
	return NULL;
}

// Отрисовка
void TField::OnPaint(HDC hdc)
{
	HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0)); // Создание ручки
	SelectObject(hdc, pen);
	for (int i = 4; i < sizeS[0] + 1; i++) // Рисуем горизонтальные линии
	{
		MoveToEx(hdc, this->Coord[1], this->Coord[0] + (i - 4) * OFFSET_TETR, NULL);
		LineTo(hdc, this->Coord[1] + this->sizeS[1] * OFFSET_TETR, this->Coord[0] + (i - 4) * OFFSET_TETR);
	}
	for (int i = 0; i < sizeS[1] + 1; i++) // Рисуем вертикальные линии
	{
		MoveToEx(hdc, this->Coord[1] + i * OFFSET_TETR, this->Coord[0], NULL);
		LineTo(hdc, this->Coord[1] + i * OFFSET_TETR, this->Coord[0] + (sizeS[0] - 4) * OFFSET_TETR);
	}
	DeleteObject(pen); // Удаление ручки
	for (int i = 4; i < sizeS[0]; i++)
		for (int j = 0; j < sizeS[1]; j++)
			if (!field[i][j].empty) // Отрисовка заполненных клеток поля
			{
				HDC hMem = CreateCompatibleDC(hdc);
				SelectObject(hMem, TField::bitmap_q[field[i][j].paint]);
				BitBlt(hdc, Coord[1] + j * OFFSET_TETR, Coord[0] + (i - 4) * OFFSET_TETR, OFFSET_TETR, OFFSET_TETR, hMem, 0, 0, SRCCOPY);
				DeleteDC(hMem);
			}
	if (CurrentFigure != NULL) // Отрисовка фигуры
	{
		CurrentFigure->OnPaint(*this, hdc);
		if (CurrentFigure->Fall(*this)) // Падение фигуры
		{
			rp = RotatePos::N; // Изначальное положение
			CheckLine(); // Проверка линий на удаление
			delete CurrentFigure; // Удаление фигуры
			CurrentFigure = this->CreateRndF(); // Создание новой фигуры
		}
	}
}

void TField::CopyLineUp(int index) // Смещение относительно индекса
{
	for (int i = index - 1; i > 4; i--)
		for (int j = 0; j < sizeS[1]; j++)
		{
			field[i][j].empty = field[i - 1][j].empty;
			field[i][j].paint = field[i - 1][j].paint;
		}
}


void TField::CheckLine() // Проверка линий на удаление
{
	for (int i = 5; i < sizeS[0]; i++)
	{
		int k = 0;
		for (int j = 0; j < sizeS[1]; j++)
			if (!field[i][j].empty)
				k++;
		if (k == sizeS[1])
			CopyLineUp(i + 1);
	}
}

void TField::Clear() // Очистка поля
{
	for (int i = 0; i<sizeS[0]; i++)
		for (int j = 0; j < sizeS[1]; j++)
		{
			field[i][j].empty = true;
			field[i][j].paint = typeC::f;
		}
}