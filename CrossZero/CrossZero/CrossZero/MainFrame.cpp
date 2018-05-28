#include <Windows.h>
#include "TField.h"
#include "resource.h"
#include "Button_IMG.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // Обработчик

char* AppClassName = "MainFraim"; // Имя класса окна
TField field(5, 30, 130, 500); // Игровое поле

char player = 'x'; // Игрок
char winner = ' '; // Победитель

HBITMAP BackGround, Turn_X, Turn_O; // Битпмап фона, хода X и хода O
HBITMAP Win_X, Win_O, Win_Draw; // Битмап Победитель X, Победитель O, Ничья

void Left_B_Action() // Действие при нажатии кнопки промотать влево
{
	field.MoveR(true);
	InvalidateRect(GetActiveWindow(), NULL, TRUE);
}

void Right_B_Action() // Дествие при промотке вправо
{
	field.MoveR(false);
	InvalidateRect(GetActiveWindow(), NULL, TRUE);
}

void Start_B() // При нажатии кнопки старт
{
	if (winner != ' ')
	{
		winner = ' ';
		player = 'x';
		field.Clear();
		InvalidateRect(GetActiveWindow(), NULL, TRUE); // Перерисовать окно
	}
}

// Кнопки
Button_IMG buttons[3]{ Button_IMG(IDB_START,250,10,Start_B) , Button_IMG(IDB_LEFT_B,310,10,Left_B_Action),
	Button_IMG(IDB_RIGHT_B,370,10,Right_B_Action) };

void LoadResource() // Загрузка ресурсов
{
	BackGround = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BK));
	Turn_X = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_TX));
	Turn_O = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_TO));
	Win_Draw = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_DRAW));
	Win_X = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_WX));
	Win_O = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_WO));
}

// Главная функция
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR cmd, int nCmd)
{
	HWND hwnd; // Определение окна
	MSG msg; // Сообщение
	WNDCLASS wc; // Определение класса окна
	wc.cbClsExtra = wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)(WHITE_BRUSH); // Фон
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); // Курсор
	wc.hIcon = LoadIcon(hInst, IDI_APPLICATION); // Иконка
	wc.hInstance = hInst; // АЙДИ
	wc.lpfnWndProc = WndProc; // Обработчик
	wc.lpszClassName = AppClassName; // Имя класса
	wc.lpszMenuName = NULL; // Меню
	wc.style = CS_VREDRAW | CS_HREDRAW; // Стиль
	RegisterClass(&wc); // Регистрация класса
	LoadResource(); // Загрузка ресурсов
	// Создание окна
	hwnd = CreateWindow(AppClassName, "X-O Game", WS_OVERLAPPEDWINDOW^WS_THICKFRAME^WS_MAXIMIZEBOX, 0, 0, 600, 700, NULL, NULL, hInst, NULL);
	ShowWindow(hwnd, SW_SHOW); // Показать окно
	UpdateWindow(hwnd); // Обновить окно
	while (GetMessage(&msg, 0, 0, 0)) // Обработчки сообщений
	{
		if (!IsDialogMessage(hwnd, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return msg.message;
}

void OnPaint(PAINTSTRUCT & ps) // Отрисовка
{
	HDC hMemDC, hTempDC; // Теневой буффер
	HGDIOBJ hMemBmp, hSysBmp;
	hMemDC = CreateCompatibleDC(ps.hdc); // Создать конекст схожии с основным
	hMemBmp = CreateCompatibleBitmap(ps.hdc, 600, 700); // Создание битмап для фона
	hSysBmp = SelectObject(hMemDC, hMemBmp);
	hTempDC = CreateCompatibleDC(hMemDC);
	SelectObject(hTempDC, BackGround);
	BitBlt(hMemDC, 0, 0, 600, 700, hTempDC, 0, 0, SRCCOPY); // Копироание одного контекста в другой контекст
	DeleteDC(hTempDC); // Удаление временного контекста
	BITMAP info; // Информация о битмап
	HDC hdc = CreateCompatibleDC(hMemDC);
	if (winner == ' ') // Если не известен победитель
	{
		SelectObject(hdc, player == 'x' ? Turn_X : Turn_O);
		GetObject(player == 'x' ? Turn_X : Turn_O, sizeof(BITMAP), &info); // Получение информации о битмап
	}
	else
		if (winner == 'd') // Если ничья
		{
			SelectObject(hdc, Win_Draw);
			GetObject(Win_Draw, sizeof(BITMAP), &info);
		}
		else // Если есть победитель
		{
			SelectObject(hdc, winner == 'x' ? Win_X : Win_O);
			GetObject(winner == 'x' ? Win_X : Win_O, sizeof(BITMAP), &info);
		}
	// Копирование контекста в контекст с исключение белого цвета
	TransparentBlt(hMemDC, 10, 10, info.bmWidth, info.bmHeight, hdc, 0, 0,
		info.bmWidth, info.bmHeight, RGB(255, 255, 255));
	DeleteDC(hdc);
	field.OnPaint(hMemDC); // Отрисовка поля
	for (int i = 0; i < 3; i++) // Отрисовка кнопок
		buttons[i].OnPaint(hMemDC);
	BitBlt(ps.hdc, 0, 0, 600, 700, hMemDC, 0, 0, SRCCOPY); // Отправка в основной контекст
	SelectObject(hMemDC, hSysBmp);
	DeleteObject(hMemBmp); // Удаление Битмапа
	DeleteObject(hSysBmp);
	DeleteDC(hMemDC); // Удаление контекстов
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) // Обработчик
{
	HDC hdc; // Контекст
	PAINTSTRUCT ps; // Структура рисования
	switch (msg)
	{
	case WM_LBUTTONDOWN: // Нажатие левой кнопки на мыши
	{
		RECT wndR; // Область окна
		POINT pCursor; // Координаты курсора
		GetCursorPos(&pCursor); // ПОлучить координаты курсора
		GetWindowRect(hwnd, &wndR); //Получить координаты окна
		if (winner == ' ') // Если победитель неизвестен
		{
			if (field.OnClick(wndR, pCursor, player)) // Если ход засчитан
			{
				player = player == 'x' ? 'o' : 'x'; // Сменить игрока
				char fin = field.Finish(); // Проверка на конец партии
				if (fin != ' ') // Если конец партии
					winner = fin;
				InvalidateRect(hwnd, NULL, true); // Перерисовать окно
			}
		}
		else
			for (int i = 0; i < 3; i++) // Обработка кнопок
				buttons[i].OnClick(wndR, pCursor);
	}
	break;
	case WM_PAINT: // Отрисовка
		hdc = BeginPaint(hwnd, &ps); // Начало отрисовки
		OnPaint(ps); // Отрисовка
		EndPaint(hwnd, &ps); // Конец отрисовки
		break;
	case WM_DESTROY: // Разрушение окна
		PostQuitMessage(NULL);
		return 0;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}