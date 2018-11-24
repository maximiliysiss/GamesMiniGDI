#include "Object.h"
#include "EventSystem.h"
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // Обработчик событий

EventSystem * EventSystem::events = NULL; // Система событий
GravitySystem * GravitySystem::it = NULL; // Система Гравитации
PaintSystem * PaintSystem::SysPaint = NULL; // Система отрисовки
const UINT TIMER_FRAME = 10; // Таймер отрисовки и математики

char* AppClassName = "MainFraim"; // Имчя приложениz

bool ButtonPress; // Нажата ли кнопка мыши
Setup setup(IDB_SETUP, Vector(0.0f, 0.0f), 10);

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR cmd, int nCmd) // Главная функция
{
	HWND hwnd; // Окно
	MSG msg; // Сообщение
	WNDCLASS wc; // Класс окна
	wc.cbClsExtra = wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)(BLACK_BRUSH);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); // Курсор
	wc.hIcon = LoadIcon(hInst, IDI_APPLICATION);
	wc.hInstance = hInst;
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = AppClassName;
	wc.lpszMenuName = NULL;
	wc.style = CS_VREDRAW | CS_HREDRAW;
	RegisterClass(&wc); // Регистрация окна
	hwnd = CreateWindow(AppClassName, "Light", WS_OVERLAPPEDWINDOW, 0, 0, 600, 600, NULL, NULL, hInst, NULL); // Создание окна
	PaintSystem::Instance().SetWndSize(600, 600); // Указание размеров для системы рисования
	PaintSystem::Instance().SetBackGround(IDB_BK);
	Linz l1(IDB_LINZIN, false, Vector(200.0f, 200.0f), Object::TYPE::RECT, -0.05f); // Линзы
	Linz l2(IDB_LINZOUT, false, Vector(300.0f, 200.0f), Object::TYPE::RECT, 1.0f);
	Linz l3(IDB_LINZIN, false, Vector(400.0f, 200.0f), Object::TYPE::RECT, -0.04f);
	Linz l4(IDB_LINZOUT, false, Vector(100.0f, 200.0f), Object::TYPE::RECT, 0.2f);
	SetTimer(hwnd, TIMER_FRAME, 10, NULL); // Установка таймера
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
	ButtonPress = false;
	HWND hwndEdit = CreateWindowEx( // Текст бокс
		0, "EDIT",
		NULL,
		WS_CHILD | WS_VISIBLE | WS_BORDER |
		ES_LEFT | ES_AUTOVSCROLL,
		30, 500, 50, 20,
		hwnd,
		(HMENU)0,
		(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
		NULL);
	HWND Button = CreateWindowEx( // Кнопка
		0, "BUTTON",
		"Edit",
		WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP | BS_PUSHBUTTON,
		70, 500, 50, 20,
		hwnd,
		(HMENU)1,
		(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
		NULL);
	while (GetMessage(&msg, 0, 0, 0))
	{
		if (!IsDialogMessage(hwnd, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return msg.message;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	switch (msg)
	{
	case WM_COMMAND:
		switch (wParam)
		{
		case (UINT)1: // Нажатие кнопки
			char buffer[15];
			GetWindowText(GetDlgItem(hwnd, 0), buffer, 15);
			setup.ReCreate(std::stoi(std::string(buffer)));
			break;
		}
		break;
	case WM_TIMER: // Сработал таймер
		switch (wParam)
		{
		case TIMER_FRAME:
			InvalidateRect(hwnd, NULL, true); // Перерисовать
			break;
		}
		break;
	case WM_LBUTTONDOWN: // Нажатие левой мыши
	{
		POINT cursor;
		GetCursorPos(&cursor); // Получить позицию мыши
		RECT rect;
		GetWindowRect(hwnd, &rect); // Получить координаты окна
		if (EventSystem::Instance()->MouseButton(Vector(cursor) - Vector(rect.left, rect.top), EventSystem::DOWN))
			ButtonPress = true;
		break;
	}
	case WM_MOUSEMOVE: // Движение
	{
		if (ButtonPress) // Только при зажатой левой
		{
			POINT cursor;
			GetCursorPos(&cursor);
			RECT rect;
			GetWindowRect(hwnd, &rect); // Получить координаты окна
			EventSystem::Instance()->MouseButton(Vector(cursor) - Vector(rect.left, rect.top), EventSystem::MOVE);
		}
		break;
	}
	case WM_LBUTTONUP: // Отпустил
		ButtonPress = false;
		break;
	case WM_ERASEBKGND:
		return TRUE;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		EventSystem::Instance()->TickEvent(ps); // Отрисовка
		EndPaint(hwnd, &ps);
		break;
	case WM_DESTROY: // Закрытие
		PostQuitMessage(NULL);
		return 0;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}