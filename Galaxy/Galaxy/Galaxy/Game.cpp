#include "ObjectsGalaxy.h"
#define TIMER_MAIN 10 // Таймер обновление кадров


bool GAME = false; // Игра
bool RESET = false; // Перезагрузка
int SIZABLE = 0; // Ресайз
int sizeX, sizeY; // Размеры экрана

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
POINT pDrag{ 0,0 }; // Вектор смещения
bool drag = false; // Передвижение окна?

// Имя класса окна
char const szClassName[] = "Milkyway";

// Заголовок окна
char const szAppName[] = "Milky Way";

const std::string PlanetsName[8] = { "Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune" };
// Название планет
float Rad[8] = { 400,600,800,1000,1220,1445,1668,1872 };
// Радиусы планет
const float Speed[8] = { 2.2f,1.4f,1.3f,1.2f,1.1f,0.8f,0.5f,0.1f };
// Скорости планет
std::vector<Kometa> KometOrigin; // Все кометы

HINSTANCE hInst; // айди окна
HBITMAP BGround = NULL, Start, End, Reset, Exit, Kometa_Static_Origin_bmp = NULL; // Битмапы фона, и "кнопок"
std::vector<Planet> PlanetOrigin; // Вектор планет
Sun *SunOrigin; // Солнце (оно одно)

void LoadRecource() // Подгрузка ресурсов
{
	Start = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_START)); // Загрузка из ресурсов битмапа по id
	End = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_END));
	Reset = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_RESET));
	Exit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_EXIT));
	BGround = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BGROUND));
	Kometa_Static_Origin_bmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_KOMETA));
	for (int i = 0; i < PLANET_COUNT; i++) // Создание пула планет
		PlanetOrigin.push_back(Planet(PlanetsName[i], Rad[i], Speed[i], hInst));
	for (UINT i = 0; i < KOMETA_COUNT; i++) // Создание пула комет
		KometOrigin.push_back(Kometa("Kometa", hInst));
	SunOrigin = new Sun("Sun", hInst);
}

void SetWndSize() // Установка размеров окна
{
	for (int i = 0; i < PLANET_COUNT; i++) // Для планет
		PlanetOrigin[i].SetWndSize(sizeX, sizeY);
	for (UINT i = 0; i < KOMETA_COUNT; i++) // Для комет
		KometOrigin[i].SetWndSize(sizeX, sizeY);
	SunOrigin->SetWndSize(sizeX, sizeY); // Для солнца
}

void Mathematic() // Вычисление или математика
{
	for (UINT i = 0; i < PlanetOrigin.size(); i++)
		PlanetOrigin[i].Movement(); // Движение планет
	for (UINT i = 0; i < KometOrigin.size(); i++)
	{
		KometOrigin[i].Movement(); // Движение комет
		if (KometOrigin[i].Delete()) // Проверка на удаление и удаление комет
			KometOrigin[i].ReloadKometa();
		if (KometOrigin[i].isConnetc(SunOrigin->GetPos(), SunOrigin->GetSize())) // Проверка на пересечение с солнцем
			KometOrigin[i].ReloadKometa();
		if (KometOrigin[i].isGravity(SunOrigin->GetPos(), SunOrigin->GetGravity().first)) // Проверка на воздействие гравитации солнца
			KometOrigin[i].ChangeMovement(SunOrigin->GetPos(), SunOrigin->GetGravity()); // Изменение движения
		for (UINT j = 0; j < PlanetOrigin.size(); j++) // Проверка для каждой планеты
		{
			if (!PlanetOrigin[j].isThere()) // Если планета исключена, то и проверять дальше нет смысла
				continue;
			if (KometOrigin[i].isConnetc(PlanetOrigin[j].GetPos(), PlanetOrigin[j].GetSize())) // Проверка на пересечение
			{
				PlanetOrigin[j].Block(); // Блокирование планеты
				KometOrigin[i].ReloadKometa();
				break;
			}
			if (KometOrigin[i].isGravity(PlanetOrigin[j].GetPos(), PlanetOrigin[j].GetGravity().first)) // Проверка на влияние гравитации
				KometOrigin[i].ChangeMovement(PlanetOrigin[j].GetPos(), PlanetOrigin[j].GetGravity());
		}
	}
}


void ResetSituation() // Возврат ситауации к изначальной
{
	for (int i = 0; i < PLANET_COUNT; i++) // Презагрузка планет
		PlanetOrigin[i].Reload();
	for (UINT i = 0; i < KOMETA_COUNT; i++) // Перезагрузка комет
		KometOrigin[i].ReloadKometa();
}

int WINAPI WinMain(HINSTANCE hInstance, // Главная функция
	HINSTANCE hPrevInstance,
	LPSTR lpszCmdLine,
	int  nCmdShow)
{
	srand(UINT(time(NULL))); // Генератор случ чисел
	HWND hwnd; // Дескриптор окна
	MSG msg; // Сообщения
	hInst = hInstance;
	WNDCLASS wc; // Описание класса окна
	wc.style = CS_VREDRAW | CS_HREDRAW; // Стиль
	wc.lpfnWndProc = WindowProc; // Функция обработки
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance; // Айди
	wc.hIcon = LoadIcon(GetModuleHandle(NULL), IDI_APPLICATION); // Иконка
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); // Курсор
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME); // Фон
	wc.lpszMenuName = NULL;
	wc.lpszClassName = szAppName; // Имя
	RegisterClass(&wc); // Регистрация окна
	LoadRecource(); // Загрузка ресурсов
	BITMAP bm;
	GetObject(BGround, sizeof(BITMAP), &bm); // Получение информации о размерах
	sizeX = bm.bmWidth;
	sizeY = bm.bmHeight;
	SetWndSize(); // Установка размеров
	hwnd = CreateWindow(szAppName, // Создание окна
		szAppName,
		WS_POPUP | WS_OVERLAPPED,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		bm.bmWidth, bm.bmHeight, 0, 0,
		hInstance, 0);
	SetTimer(hwnd, TIMER_MAIN, 4, NULL); // Установить таймер окна
	ShowWindow(hwnd, SW_SHOW); // Показать окно
	UpdateWindow(hwnd); // Обновить окно
	while (GetMessage(&msg, 0, 0, 0)) // Проверка пока есть сообщения
	{
		if (!IsDialogMessage(hwnd, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return msg.wParam;
}

int ButtonReact(POINT CursorPos, RECT wndRect) // Проверка нажатия кнопки
{
	if (abs(wndRect.left - CursorPos.x) >= 0 && abs(wndRect.left - CursorPos.x) < 50 && abs(wndRect.top - CursorPos.y) >= 0 && abs(wndRect.top - CursorPos.y) < 50) // Первая кнопка
		return 1;
	if (abs(wndRect.left - CursorPos.x) >= 50 && abs(wndRect.left - CursorPos.x) < 100 && abs(wndRect.top - CursorPos.y) >= 0 && abs(wndRect.top - CursorPos.y) < 50) // Вторая
		return 2;
	if (abs(wndRect.left - CursorPos.x) >= 100 && abs(wndRect.left - CursorPos.x) <= 150 && abs(wndRect.top - CursorPos.y) >= 0 && abs(wndRect.top - CursorPos.y) < 50) // Третья
		return 3;
	if (abs(wndRect.left - CursorPos.x) >= sizeX - 50 && abs(wndRect.left - CursorPos.x) <= sizeX && abs(wndRect.top - CursorPos.y) >= 0 && abs(wndRect.top - CursorPos.y) < 50) // Третья
		return 4;
	return 0; // Ничего не нажато
}

void PaintButtons(HDC hdc) // Отрисовка кнопок
{
	HDC hMem = CreateCompatibleDC(hdc); // Создание сходего контекста
	HGDIOBJ hOld = SelectObject(hMem, Start); // Двойная буфферизация
	TransparentBlt(hdc, -sizeX / 2, -sizeY / 2, 50, 50, hMem, 0, 0, 50, 50, RGB(0, 0, 0)); // Отрисовка с пропуском черного
	SelectObject(hMem, End);
	TransparentBlt(hdc, -sizeX / 2 + 50, -sizeY / 2, 50, 50, hMem, 0, 0, 50, 50, RGB(0, 0, 0));
	SelectObject(hMem, Reset);
	TransparentBlt(hdc, -sizeX / 2 + 100, -sizeY / 2, 50, 50, hMem, 0, 0, 50, 50, RGB(0, 0, 0));
	SelectObject(hMem, Exit);
	TransparentBlt(hdc, sizeX / 2 - 50, -sizeY / 2, 50, 50, hMem, 0, 0, 50, 50, RGB(0, 0, 0));
	SelectObject(hMem, hOld);
	DeleteDC(hMem);
	DeleteObject(hOld);
}

void Paint(PAINTSTRUCT &ps) // Отрисовка
{
	HDC hMemDC, hTempDC; // Теневой буффер
	HGDIOBJ hMemBmp, hSysBmp;
	hMemDC = CreateCompatibleDC(ps.hdc); // Создать конекст схожии с основным
	hMemBmp = CreateCompatibleBitmap(ps.hdc, sizeX, sizeY);
	hSysBmp = SelectObject(hMemDC, hMemBmp);
	SetViewportOrgEx(hMemDC, sizeX / 2, sizeY / 2, nullptr);
	// Отрисовка фона
	hTempDC = CreateCompatibleDC(hMemDC);
	SelectObject(hTempDC, BGround);
	BitBlt(hMemDC, -sizeX / 2, -sizeY / 2, sizeX, sizeY, hTempDC, 0, 0, SRCCOPY);
	DeleteDC(hTempDC);

	// Отрисовка наполнения
	for (UINT i = 0; i < PlanetOrigin.size(); i++)
		PlanetOrigin[i].Draw(hMemDC);
	SunOrigin->Draw(hMemDC);
	if (RESET)
		for (UINT i = 0; i < KometOrigin.size(); i++)
			KometOrigin[i].Draw(hMemDC);
	PaintButtons(hMemDC); // Отрисовка кнопок
	BitBlt(ps.hdc, 0, 0, sizeX, sizeY, hMemDC, -sizeX / 2, -sizeY / 2, SRCCOPY); // Отправка в основной контекст
	SelectObject(hMemDC, hSysBmp);
	DeleteObject(hMemBmp); // Удаление Битмапа
	DeleteObject(hSysBmp);
	DeleteDC(hMemDC); // Удаление контекстов
}

#pragma comment(lib,"Winmm.lib")

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps; // структура для рисования
	HDC hdc; // Контекст
	switch (message)
	{
	case WM_SIZE:
		PlaySound(TEXT("C:\\SpaceSound.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
		break;
	case WM_TIMER: // Таймеры
		switch (wParam)
		{
		case TIMER_MAIN:
			if (GAME) // ИГра?
				Mathematic(); // Расчеты
			InvalidateRect(hwnd, NULL, TRUE); // Перерисовать все
			break;
		}
		break;
	case WM_MOUSEWHEEL: // Колесико мышки
	{
		if (GAME) // Только в игре
		{
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) // Если движение вверх
			{
				if (SIZABLE >= 10) // Не превышает ли допустимого значения
					break;
				SIZABLE++;
				for (UINT i = 0; i < PlanetOrigin.size(); i++) // Для планет
				{
					PlanetOrigin[i].Resize(true);
					PlanetOrigin[i].ResizeOrbit(true);
				}
				SunOrigin->Resize(true); // Для солнца
				for (UINT i = 0; i < KometOrigin.size(); i++) // Для комет
				{
					KometOrigin[i].Resize(true);
					KometOrigin[i].ResizeCoord(true);
				}
			}
			else
			{
				if (SIZABLE <= -40)
					break;
				SIZABLE--;
				for (UINT i = 0; i < PlanetOrigin.size(); i++)
				{
					PlanetOrigin[i].Resize(false);
					PlanetOrigin[i].ResizeOrbit(false);
				}
				for (UINT i = 0; i < KometOrigin.size(); i++)
				{
					KometOrigin[i].Resize(false);
					KometOrigin[i].ResizeCoord(false);
				}
				SunOrigin->Resize(false);
			}
		}
	}
	break;
	case WM_LBUTTONDOWN: // Нажата левая кнопка мыши
	{
		RECT wndrect; // информация о размерах окна
		POINT CursorPos{ 0,0 }; // Информация о позиции курсора
		GetCursorPos(&CursorPos); // ПОлучени позиции
		GetWindowRect(hwnd, &wndrect); // Получение размеров окна
		int button_id = ButtonReact(CursorPos, wndrect); // Проверка на попадание по кнопке
		if (button_id == 0) // Нет?
		{
			pDrag.x = CursorPos.x - wndrect.left; // Вычисление вектора смещения относительно точки захвата
			pDrag.y = CursorPos.y - wndrect.top;
			drag = true; // Тащим!
			SetCapture(hwnd); // Захват окна поверх всех
		}
		else
		{
			switch (button_id) // Обработка кнопок
			{
			case 1: // Старт
				GAME = true;
				RESET = true;
				break;
			case 2: // Стоп
				GAME = false;
				break;
			case 3: // Перещагрузка
				GAME = false;
				RESET = false;
				ResetSituation();
				break;
			case 4:
				SendMessage(hwnd, WM_DESTROY, wParam, lParam);
				break;
			}
		}
	}
	break;
	case WM_MOUSEMOVE: // Двигаем мышкой
		if (drag) // Если зажата левая клавиша
		{
			POINT CursorPos{ 0,0 };
			GetCursorPos(&CursorPos); // Новые позиции 
			// Смещаем на вектор смещения окно
			SetWindowPos(hwnd, NULL, CursorPos.x - pDrag.x, CursorPos.y - pDrag.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		}
		break;
	case WM_LBUTTONUP: // Отпустили кнопку мыши
		if (drag) // Если тащили
		{
			drag = false; // Не тащим
			ReleaseCapture(); // Отпускаем фокусировку окна
		}
		break;
	case WM_ERASEBKGND: // Отрисовка окна при выходе вне экрана
		return TRUE;
	case WM_PAINT: // Отрисовка
	{
		hdc = BeginPaint(hwnd, &ps); // Получаем поле
		Paint(ps);
		EndPaint(hwnd, &ps); // Отпускаем поле
	}
	break;
	case WM_DESTROY: // Разрушение окна
		delete SunOrigin;
		PostQuitMessage(NULL); // Отправить сообщение
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam); // Если ничего не случилось
	}
	return 0;
}