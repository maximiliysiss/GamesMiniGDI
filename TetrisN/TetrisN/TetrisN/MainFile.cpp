#include "TField.h"

char * AppName = "MainFrame"; // Название класса
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // Обработчик сообщений

const UINT TIMER_FRAME = 10; // Таймер для обновлений кадра и математики
const UINT TIMER_DIFF = 11; // Таймер для ускорения 

UINT SPEED_FRAME = 200; // Текущий скорость кадра
const UINT SPEED_FRAME_ORG = 200; // Оригинальная скорость кадра
const UINT TIME_OFFSET = 0; // Смещение кадра
const UINT SPEED_TIME_DEL = 5000; // Через сколько ускорить течение времени

bool GAME = false; // Игра
bool RESTART = false; // Рестарт

TField field(20, 10, 10, 85); // Поле

HBITMAP BackGround, Start, GameOver; // Битмапы под фон, старт и рестарт

HWND * main_hwnd; // Указатель на главное окно

void LoadResource() // Загрузка ресурсов
{
	BackGround = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BK));
	Start = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_START));
	GameOver = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_GAMEOVER));
}


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR cmd, int cCmd) // Главная функция
{
	LoadResource();
	HWND hwnd; // Окно
	MSG msg; // Сообщение
	WNDCLASS wc; // Класс окна
	wc.style = CS_VREDRAW | CS_HREDRAW; // Стильы
	wc.cbClsExtra = wc.cbWndExtra = 0;
	wc.lpszMenuName = NULL; // Наличие меню
	wc.hbrBackground = (HBRUSH)WHITE_BRUSH; // Бэкграунд
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); // Курсор
	wc.hIcon = LoadIcon(hInst, IDI_APPLICATION); // Иконка
	wc.lpfnWndProc = WndProc; // Установка обработчика
	wc.hInstance = hInst; // Идентификатор
	wc.lpszClassName = AppName; // Имя класса
	RegisterClass(&wc); // Регитсрация класса
	// Создание окна - установка размеров, стилей, меню
	hwnd = CreateWindow(AppName, AppName, WS_OVERLAPPEDWINDOW^WS_THICKFRAME^WS_MAXIMIZEBOX, 0, 0, 400, 500, NULL, NULL, hInst, NULL);
	SetTimer(hwnd, TIMER_FRAME, SPEED_FRAME, NULL); // Установка таймера на SPEED_FRAME 
	SetTimer(hwnd, TIMER_DIFF, SPEED_TIME_DEL, NULL);
	main_hwnd = &hwnd; // Создание указателя
	ShowWindow(hwnd, SW_SHOW); // Показать окно
	UpdateWindow(hwnd); // Обновить
	// Обработчик сообщений
	while (GetMessage(&msg, 0, 0, 0))
	{
		if (!IsDialogMessage(hwnd, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.lParam;
}

void DrawCenter(HDC hdc, HBITMAP hb) // Отрисовать в центре определенные фигуры
{
	HDC hMem = CreateCompatibleDC(hdc); // Создание контекста схожего с пришедшим
	BITMAP bm; // Информация о битмапе
	GetObject(hb, sizeof(BITMAP), &bm); // Извлечение этой информации
	SelectObject(hMem, hb); // Выбор в контекст объекта - тут ьитмапа
	// Копирование из одного контекста в другой с пропуском белого цвета
	TransparentBlt(hdc, 70, 125, bm.bmWidth, bm.bmHeight, hMem, 0, 0, bm.bmWidth, bm.bmHeight, RGB(255, 255, 255));
	DeleteDC(hMem); // Удаление не используемого контекста
}

void OnPaint(PAINTSTRUCT & ps)
{
	HDC hMemDC, hTempDC; // Теневой буффер
	HGDIOBJ hMemBmp, hSysBmp;
	hMemDC = CreateCompatibleDC(ps.hdc); // Создать конекст схожии с основным
	hMemBmp = CreateCompatibleBitmap(ps.hdc, 500, 500); // Создание битмапа, который будет представлять размеры кадра
	hSysBmp = SelectObject(hMemDC, hMemBmp);
	// Отрисовка фона
	hTempDC = CreateCompatibleDC(hMemDC); // Создать схожий контекст
	SelectObject(hTempDC, BackGround); // Выбор фона
	BitBlt(hMemDC, 0, 0, 500, 500, hTempDC, 0, 0, SRCCOPY);
	DeleteDC(hTempDC);

	if (GAME) // Игра
		field.OnPaint(hMemDC);
	else
		if (RESTART) // Рестарт
			DrawCenter(hMemDC, GameOver);
		else
			DrawCenter(hMemDC, Start); // Старт
	BitBlt(ps.hdc, 0, 0, 500, 500, hMemDC, 0, 0, SRCCOPY); // Отправка в основной контекст
	SelectObject(hMemDC, hSysBmp);
	DeleteObject(hMemBmp); // Удаление Битмапа
	DeleteObject(hSysBmp);
	DeleteDC(hMemDC); // Удаление контекстов
	if (field.Finish()) // Если игра завершилась
	{
		RESTART = true; // Рестарт
		GAME = false;
		field.Clear(); // Очистить поле
		SPEED_FRAME = SPEED_FRAME_ORG; // Скорость с нуля
		SetTimer(*main_hwnd, TIMER_FRAME, SPEED_FRAME, NULL); // Заново устанавливаем таймер
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) // Обработчик сообщений
{
	HDC hdc; // Контекст
	PAINTSTRUCT ps; // Структура для рисования
	switch (msg)
	{
	case WM_ERASEBKGND: // Если перетаскиваешь окно - оно перерисовывается
		return TRUE;
	case WM_PAINT: // Отрисовка
		hdc = BeginPaint(hwnd, &ps); // Начало отрисовки
		OnPaint(ps); // Отрисовка
		EndPaint(hwnd, &ps); // Конец отрисовки
		break;
	case WM_TIMER: // Обработка таймеров
		switch (wParam)
		{
		case TIMER_FRAME: // Таймер на отрисовку
			InvalidateRect(hwnd, NULL, true);
			break;
		case TIMER_DIFF: // Таймер на смену скорости
			if (SPEED_FRAME > 20) // Некое ограничение на 20
				SPEED_FRAME -= TIME_OFFSET;
			SetTimer(hwnd, TIMER_FRAME, SPEED_FRAME, NULL); // Переустанвока таймера
			break;
		}
		break;
	case WM_KEYDOWN: //  Нажатие клавиш
		switch (wParam)
		{
		case 'A':
		case 'a':
			if (GAME)
				field.Turns(false); // Ход влево
			break;
		case 'D':
		case 'd':
			if (GAME)
				field.Turns(true); // Ход вправо
			break;
		case 'S':
		case 's':
			if (GAME)
				field.RotateFigure(); // Поворот
			break;
		default:
			if (!GAME) // Переутсановка таймеров при начале игры
			{
				SPEED_FRAME = SPEED_FRAME_ORG;
				SetTimer(*main_hwnd, TIMER_FRAME, SPEED_FRAME, NULL);
				GAME = true;
				RESTART = false;
				break;
			}
		}
		break;
	case WM_DESTROY:// Разрушение окна
		PostQuitMessage(NULL); // Отправка сообщения о закрытии
		return 0;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}