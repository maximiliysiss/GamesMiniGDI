#include "SceneObjects.h"
#include "Settings.h"
#include "resource.h"
#define TIMER_GRAPHIC 10 // Обновление графики раз в 1/60 секунды


POINT pDrag{ 0,0 }; // Вектора смещения
bool drag = false; // Захват?

bool RELOAD = false; // Перезагрузка игры
bool WAIT = false; // Ожидание 3,2,1
std::chrono::steady_clock::time_point start_ch; // Старт!

bool *AccessLine; // Массив линий
AntiAirCraft * AACraft; // Зенитка
std::vector<AirCraft> AllAirCrafts; //Массив Самолеты
std::vector<AirCraftBomb> Bombs; //Массив Бомбы
HBITMAP BGround; // Задник
HBITMAP hReload, hStart; // Перезагрузка и старт
HBITMAP h1, h2, h3; //1, 2, 3

bool GAME = false; // Старт игры

void Stop() // Остановка
{
	for (UINT i = 0; i < AllAirCrafts.size(); i++) // Перезагрузка
		AllAirCrafts[i].ReloadBMP();
	Bombs.clear(); // Отчистка бомб
	AACraft->ClearPatron();
	GAME = false;
	RELOAD = true;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT command, WPARAM wParam, LPARAM lParam); // Обработчик событий

bool LoadResource(HINSTANCE hInstant) // Загрузка ресурсорв
{
	BGround = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BGROUND));
	hStart = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_START));
	hReload = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_RESTART));
	h1 = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_FIRST));
	h2 = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_SECOND));
	h3 = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_THIRD));
	return BGround != NULL && hStart != NULL && hReload != NULL && h1 != NULL && h2 != NULL && h3 != NULL;
}

int WINAPI WinMain(HINSTANCE hInstant, HINSTANCE hPrev, LPSTR lpsCmd, int iCommand)
{
	srand((UINT)time(NULL)); // Установка seed относительно времени для генератора случ чисел
	HWND hwnd; // Дескриптор окна
	MSG msg;
	WNDCLASS wnd; // Описание окна
	wnd.style = CS_VREDRAW | CS_HREDRAW;
	wnd.cbClsExtra = wnd.cbWndExtra = 0;
	wnd.hInstance = hInstant;
	wnd.hIcon = LoadIcon(GetModuleHandle(NULL), IDI_APPLICATION);
	wnd.hCursor = LoadCursor(NULL, IDC_ARROW);
	wnd.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME);
	wnd.lpfnWndProc = WndProc;
	wnd.lpszMenuName = NULL;
	wnd.lpszClassName = AppName;
	RegisterClass(&wnd); // Регистрация класс окна
	// VARIABLE BEGIN
	if (COUNT_AIRCRAFT > MAX_AIRCRAFT)
		COUNT_AIRCRAFT = MAX_AIRCRAFT;
	if (COUNT_AIRCRAFT <= 0)
		COUNT_AIRCRAFT = 1;
	AccessLine = new bool[(int)((HEIGHT_OF_ACFLY - OFFSET_OF_LINES) / OFFSET_OF_LINES) + 4];
	for (UINT i = 0; i < (int)((HEIGHT_OF_ACFLY - OFFSET_OF_LINES) / OFFSET_OF_LINES) + 4; i++)
		AccessLine[i] = true;
	LoadResource(hInstant);
	BITMAP bm;
	GetObject(BGround, sizeof(BITMAP), &bm); // Ассоциация bitmap с hbitmap
	AACraft = new AntiAirCraft("AntiAirCraft", hInstant, SPEED_AAC);
	if (AACraft == NULL)
		return 0;
	AACraft->SetSize(bm.bmWidth, bm.bmHeight);
	for (UINT i = 0; i < (UINT)COUNT_AIRCRAFT; i++)
	{
		AllAirCrafts.push_back(AirCraft("Aircraft", hInstant));
		AllAirCrafts[i].SetSize(bm.bmWidth, bm.bmHeight);
	}

	// VARIABLE END
	hwnd = CreateWindow(AppName,
		AppName,
		WS_POPUP,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		bm.bmWidth, bm.bmHeight, 0, 0,
		hInstant, 0); // Создание окна
	ShowWindow(hwnd, SW_SHOW); // Показ окна
	UpdateWindow(hwnd); // Обновление окна
	SetTimer(hwnd, TIMER_GRAPHIC, 10, NULL); // Создание таймера для обновления графики
	while (GetMessage(&msg, 0, 0, 0))
	{
		if (!IsDialogMessage(hwnd, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return 0;
}

void OnPaint(PAINTSTRUCT & ps) // Отрисовка
{
	HDC hMemDC, hTempDC;
	HGDIOBJ hMemBmp, hSysBmp;
	int cx, cy;

	// Создание теневого буфера
	cx = ps.rcPaint.right - ps.rcPaint.left;
	cy = ps.rcPaint.bottom - ps.rcPaint.top;
	hMemDC = CreateCompatibleDC(ps.hdc);
	hMemBmp = CreateCompatibleBitmap(ps.hdc, cx, cy);
	hSysBmp = SelectObject(hMemDC, hMemBmp);
	SetViewportOrgEx(hMemDC, ps.rcPaint.left, ps.rcPaint.top, nullptr);

	hTempDC = CreateCompatibleDC(ps.hdc);
	// Фон
	SelectObject(hTempDC, BGround);
	BitBlt(hMemDC, ps.rcPaint.left, ps.rcPaint.top, cx, cy, hTempDC, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY); // Копирование из одного 
	// контекста в другой контекст

	SelectObject(hTempDC, hSysBmp);
	DeleteDC(hTempDC);

	if (WAIT) // Если ожидание ставим определенную цифру, относительно таймера
	{
		HDC hdc = CreateCompatibleDC(hMemDC);
		HGDIOBJ tempOld = NULL;
		int chet = (int)round(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_ch).count() / 1000);
		// Получение разницы от начала отсчета к текущему моменту
		switch (chet)
		{
		case 0:case 1:
			tempOld = SelectObject(hdc, h3);
			TransparentBlt(hMemDC, 220, 300, 20, 26, hdc, 0, 0, 20, 26, RGB(255, 255, 255));
			// Копирование в другой контекст с удалением определенного цвета
			SelectObject(hdc, tempOld);
			break;
		case 2:
			tempOld = SelectObject(hdc, h2);
			TransparentBlt(hMemDC, 220, 300, 22, 24, hdc, 0, 0, 22, 24, RGB(255, 255, 255));
			SelectObject(hdc, tempOld);
			break;
		case 3:
			tempOld = SelectObject(hdc, h1);
			TransparentBlt(hMemDC, 220, 300, 21, 27, hdc, 0, 0, 21, 27, RGB(255, 255, 255));
			SelectObject(hdc, tempOld);
			break;
		default:
			WAIT = false;
			GAME = true;
			break;
		}
		DeleteDC(hdc); // Удаление контекста
		DeleteObject(tempOld);
	}

	if (!GAME && !WAIT) // Предстартовое состояние
	{
		HDC htDC = CreateCompatibleDC(hMemDC);
		HGDIOBJ tempOld = NULL;
		if (RELOAD) // Перезагрузка
		{
			tempOld = SelectObject(htDC, hReload);
			TransparentBlt(hMemDC, 20, 120, 417, 207, htDC, 0, 0, 417, 207, RGB(255, 255, 255));
			SelectObject(htDC, tempOld);
		}
		else // Начальные титры
		{
			tempOld = SelectObject(htDC, hStart);
			TransparentBlt(hMemDC, 50, 150, 391, 395, htDC, 0, 0, 391, 395, RGB(255, 255, 255));
			SelectObject(htDC, tempOld);
		}
		DeleteDC(htDC);
		DeleteObject(tempOld);
	}
	if (GAME) // Игра
	{
		AACraft->Draw(hMemDC); // Отрисовка зенитки
		for (UINT i = 0; i < Bombs.size(); i++) // Отрисовка бомб
		{
			Bombs[i].DrawObject(hMemDC);
			Bombs[i].Movement();
		}
		for (UINT i = 0; i < AllAirCrafts.size(); i++) // Отрисовка самолетов
			AllAirCrafts[i].Draw(hMemDC);
	}
	// Вывод итога
	BitBlt(ps.hdc, ps.rcPaint.left, ps.rcPaint.top, cx, cy, hMemDC, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
	// Копирование готового кадра в главный контекст
	SelectObject(hMemDC, hSysBmp);
	DeleteObject(hMemBmp);
	DeleteObject(hSysBmp);
	DeleteDC(hMemDC);
	if (GAME)
	{
		for (UINT i = 0; i < AllAirCrafts.size(); i++) // Проверка самолетов на вылет или попадание
		{
			if (AllAirCrafts[i].Reload()) // Вылетел за пределы
				AllAirCrafts[i].ReloadBMP();
			if (AACraft->isConnect(AllAirCrafts[i].GetModelPos(), AllAirCrafts[i].GetSizes())) // Зенитка попала по самолету
				AllAirCrafts[i].ReloadBMP();
		}
		for (UINT i = 0; i < Bombs.size(); i++) // Проверка бомб на пересечение с самолетом
		{
			if (Bombs[i].isConnect(AACraft->GetModelPosP(), AACraft->GetSizesP())) // Бомба попала в зенитку
			{
				Bombs.erase(Bombs.begin() + i);
				i--;
				Stop();
				continue;
			}
			if (Bombs[i].Delete() || AACraft->isConnect(Bombs[i].GetPos(), Bombs[i].GetSizeH())) // Удаление бомб при выходе в зону удаления или попадания по ним пули
			{
				Bombs.erase(Bombs.begin() + i);
				i--;
			}
		}
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT messg, WPARAM wParam, LPARAM lParam) //Обработчик событий
{
	HDC hdc;
	PAINTSTRUCT ps;
	switch (messg)
	{
	case WM_TIMER:
		switch (wParam)
		{
		case TIMER_GRAPHIC:
			InvalidateRect(hwnd, NULL, TRUE); // Перерисовка окна
			break;
		}
		break;
	case WM_KEYDOWN: // Клавиши
		switch (wParam)
		{
		case 'S':case's': // Старт
			if (!GAME)
			{
				WAIT = true;
				start_ch = std::chrono::high_resolution_clock::now();
			}
			break;
		case 'A':case 'a': // Влево
			if (GAME)
				AACraft->Moving(false);
			break;
		case 'D':case 'd': // Вправо
			if (GAME)
				AACraft->Moving(true);
			break;
		case VK_SPACE: // Выстрел
			if (GAME)
				AACraft->CreatePatron();
			break;
		}
		break;
	case WM_LBUTTONDOWN: // Начало перетягивания окна
	{
		RECT wndrect; // Прямоугольник
		POINT CursorPos{ 0,0 };
		GetCursorPos(&CursorPos);
		GetWindowRect(hwnd, &wndrect); // Получаем размеры экрана
		pDrag.x = CursorPos.x - wndrect.left;
		pDrag.y = CursorPos.y - wndrect.top;
		drag = true;
		SetCapture(hwnd); // Относится к этому окну
	}
	case WM_MOUSEMOVE: // Передвижение перетягивания
		if (drag)
		{
			POINT CursorPos{ 0,0 };
			GetCursorPos(&CursorPos);
			SetWindowPos(hwnd, NULL, CursorPos.x - pDrag.x, CursorPos.y - pDrag.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		}
		break;
	case WM_LBUTTONUP: // Освобождения окна
		if (drag)
		{
			drag = false;
			ReleaseCapture(); // Освобождение захвата
		}
		break;
	case WM_ERASEBKGND: // Перерисовка окна
		return TRUE;
	case WM_PAINT: // Отрисовка окна
		hdc = BeginPaint(hwnd, &ps);
		OnPaint(ps);
		EndPaint(hwnd, &ps);
		break;
	case WM_DESTROY: // Уничтожение окна
		delete AACraft;
		delete[] AccessLine;
		PostQuitMessage(NULL); // Отправка окну сообщения о закрытии
		break;
	default:
		return DefWindowProc(hwnd, messg, wParam, lParam);
	}
	return 0;
}