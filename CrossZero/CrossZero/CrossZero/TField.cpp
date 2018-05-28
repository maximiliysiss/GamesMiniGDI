#include "TField.h"

HBITMAP TField::FIGURE[6] = { LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_X_Figure)),
LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_W_X)),
LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_Pre_X)),
LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_O_Figure)),
LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_W_O)),
LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_Pre_O)) }; // Загрузка фигур


TField::Record::Record(int _size) :size(_size) // Пустой конструктор
{
	field = new Cell*[size]; // Выделение памяти
	for (int i = 0; i < size; i++)
		field[i] = new Cell[size];
	for (int i = 0; i < size; i++) // Заполнение пустотой
		for (int j = 0; j < size; j++)
		{
			field[i][j].side = ' ';
			field[i][j].color = Color::BLACK;
		}
}

TField::Record& TField::Record::operator=(const Record & rec) // Оператор копирования
{
	if (field == NULL) // Если поле пустое, то выделим память под хранение
	{
		field = new Cell*[rec.size];
		for (int i = 0; i < rec.size; i++)
			field[i] = new Cell[rec.size];
	}
	size = rec.size;
	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
			field[i][j] = rec.field[i][j];
	return *this;
}


bool TField::Turn(int i, int j, char side) // Ход
{
	if (database[turn].field[i][j].side == ' ') // Если текущее место пусто - ход
	{
		database[turn + 1] = Record(database[turn].field, size);
		database[turn + 1].field[i][j].side = side;
		turn++;
		reload++;
		return true;
	}
	return false;
}

// Конструктор от размера, начальных координат и размера зоны для отрисовки
TField::TField(int size_s, int st_x, int st_y, int size_wnd) :size(size_s), start_sizeX(st_x), start_sizeY(st_y), offset(size_wnd / size_s)
{
	turn = reload = 0;
	Record rec(size);
	database = new Record[size*size + 1];
	database[turn] = rec;
}

void TField::OnPaint(HDC hdc) // Отрисовка
{
	HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0)); // Создание карандаша
	SelectObject(hdc, pen);
	for (int i = 0; i < size + 1; i++) // Отрисовка горизонтальных и вертикальных линий
	{
		MoveToEx(hdc, start_sizeX + i*offset, start_sizeY, NULL);
		LineTo(hdc, start_sizeX + i*offset, start_sizeY + size*offset);
		MoveToEx(hdc, start_sizeX, start_sizeY + i* offset, NULL);
		LineTo(hdc, start_sizeX + size*offset, start_sizeY + i*offset);
	}
	DeleteObject(pen); // Удаление карандаша
	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
			if (database[reload].field[i][j].side != ' ') // Если клетка не пуста - рисовать то, что в ней
			{
				HDC hMem = CreateCompatibleDC(hdc); // Новый контекст, схожий с тем, что пришел
				if (database[turn].field[i][j].color == TField::Record::Color::BLACK)
					SelectObject(hMem, database[turn].field[i][j].side == 'x' ? TField::FIGURE[0] : TField::FIGURE[3]);
				else
					if (database[turn].field[i][j].color == TField::Record::Color::RED)
						SelectObject(hMem, database[turn].field[i][j].side == 'x' ? TField::FIGURE[1] : TField::FIGURE[4]);
					else
						SelectObject(hMem, database[turn].field[i][j].side == 'x' ? TField::FIGURE[2] : TField::FIGURE[5]);
				// Копирование из одного контекста в другой с исключение белого цвета
				TransparentBlt(hdc, start_sizeX + j*offset + 1, start_sizeY + i*offset + 1, offset, offset, hMem,
					0, 0, 98, 98, RGB(255, 255, 255));
				DeleteDC(hMem); // Удаление временного контекста
			}
}

// Надатие мыши
bool TField::OnClick(RECT wndRect, POINT CursorPos, char side)
{
	// Проверка на вхождение клика в зону поля
	if (abs(wndRect.left - CursorPos.x) >= start_sizeX + 9 && abs(wndRect.left - CursorPos.x) <= start_sizeX + size*offset + 9
		&& abs(wndRect.top - CursorPos.y) >= start_sizeY + 30 && abs(wndRect.top - CursorPos.y) <= start_sizeY + offset*size + 30)
	{
		int j = (abs(wndRect.left - CursorPos.x) - start_sizeX - 9) / offset;//Получение координаты j
		int i = (abs(wndRect.top - CursorPos.y) - start_sizeY - 30) / offset;
		return Turn(i, j, side); // Ход
	}
	return false;
}

// Проверка на завершенность партии
char TField::Finish()
{
	int k = 0;
	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
		{
			if (database[turn].field[i][j].side == ' ')
				k++;
			database[turn].field[i][j].color = TField::Record::Color::BLACK;
		}
	if (k == 0)
		return 'd';
	for (int i = 0; i < size; i++)
	{
		char * Line = new char[size];
		for (int j = 0; j < size; j++)
			Line[j] = database[turn].field[i][j].side;
		char res = this->PrevWin(Line);
		if (res != ' ')
			for (int j = 0; j < size; j++)
				if (database[turn].field[i][j].side == res)
					database[turn].field[i][j].color = Record::Color::BLUE;
		res = this->WinLine(Line);
		if (res != ' ')
		{
			for (int j = 0; j < size; j++)
				if (database[turn].field[i][j].side == res)
					database[turn].field[i][j].color = Record::Color::RED;
			return res;
		}
		for (int j = 0; j < size; j++)
			Line[j] = database[turn].field[j][i].side;
		res = this->PrevWin(Line);
		if (res != ' ')
			for (int j = 0; j < size; j++)
				if (database[turn].field[j][i].side == res)
					database[turn].field[j][i].color = Record::Color::BLUE;
		res = this->WinLine(Line);
		if (res != ' ')
		{
			for (int j = 0; j < size; j++)
				if (database[turn].field[j][i].side == res)
					database[turn].field[j][i].color = Record::Color::RED;
			return res;
		}
	}
	char *Line = new char[size];
	for (int i = 0, j = 0; i < size&&j < size; i++, j++)
		Line[i] = database[turn].field[i][j].side;
	char res = this->PrevWin(Line);
	if (res != ' ')
		for (int i = 0, j = 0; i < size&&j < size; i++, j++)
			if (database[turn].field[i][j].side == res)
				database[turn].field[i][j].color = Record::Color::BLUE;
	res = this->WinLine(Line);
	if (res != ' ')
	{
		for (int i = 0, j = 0; i < size&&j < size; i++, j++)
			database[turn].field[i][j].color = Record::Color::RED;
		return res;
	}
	for (int i = 0, j = size - 1; i < size&&j >= 0; i++, j--)
		Line[i] = database[turn].field[i][j].side;
	res = this->PrevWin(Line);
	if (res != ' ')
		for (int i = 0, j = size - 1; i < size&&j >= 0; i++, j--)
			if (database[turn].field[i][j].side == res)
				database[turn].field[i][j].color = Record::Color::BLUE;
	res = this->WinLine(Line);
	if (res != ' ')
	{
		for (int i = 0, j = size - 1; i < size&&j >= 0; i++, j--)
			database[turn].field[i][j].color = Record::Color::RED;
		return res;
	}
	return ' ';
}

// Констурктор с копированием поля с выделение памяти
TField::Record::Record(Cell** f, int _size) :size(_size)
{
	field = new Cell*[size];
	for (int i = 0; i < size; i++)
		field[i] = new Cell[size];
	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
			field[i][j] = f[i][j];
}