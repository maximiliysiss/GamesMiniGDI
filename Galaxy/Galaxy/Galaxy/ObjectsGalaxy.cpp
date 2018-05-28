#include "ObjectsGalaxy.h"

WORD GetResByName(std::string name) // Получение id относительно имени
{
	if (name == "Kometa")
		return IDB_KOMETA;
	if (name == "Earth")
		return IDB_EARTH;
	if (name == "Mars")
		return IDB_MARS;
	if (name == "Mercury")
		return IDB_MERCURY;
	if (name == "Neptune")
		return IDB_NEPTUNE;
	if (name == "Uranus")
		return IDB_URANUS;
	if (name == "Venus")
		return IDB_VENUS;
	if (name == "Jupiter")
		return IDB_JUPITER;
	if (name == "Saturn")
		return IDB_SATURN;
	if (name == "Sun")
		return IDB_SUN;
	return NULL;
}

ObjectGalaxy::ObjectGalaxy(std::string _name, HINSTANCE hInstance) :name(_name), hInst(hInstance)
{
	if (!LoadObject()) // Загрузка объекта
	{
		MessageBox(NULL, std::string("Error load bitmap for " + name).c_str(), "Error BMP", NULL);
		this->~ObjectGalaxy();
	}
	size = (float)bitmap.bmWidth; // Установка размеров
	if (name != "Kometa")
		hOrigin = (HBITMAP)CopyImage(hBitmap, IMAGE_BITMAP, 0, 0, LR_COPYRETURNORG); // Создании копии оригинала
	else
		hOrigin = NULL;
}


void ObjectGalaxy::DrawObject(HDC hdc)
{
	if (this->GetDrawChance())
		return;
	HDC hMem = CreateCompatibleDC(hdc); // Соотнесение с полученным контекстом
	HGDIOBJ hOld = SelectObject(hMem, hBitmap); // Двойная буферизация
	TransparentBlt(hdc, (int)position.first - bitmap.bmWidth / 2, (int)position.second - bitmap.bmHeight / 2, bitmap.bmWidth, bitmap.bmHeight, hMem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, RGB(0, 0, 0)); // Копирование с пропуском белого
	SelectObject(hMem, hOld);
	DeleteDC(hMem); // Удаление контекста
	DeleteObject(hOld);
}

bool ObjectGalaxy::LoadObject()
{
	hBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(GetResByName(name)));
	if (hBitmap == NULL)
		return false;
	GetObject(hBitmap, sizeof(BITMAP), &bitmap); // Получение информации в битмап 
	return true;
}

void ObjectGalaxy::Resize(bool direction) // Изменение размеров (перерасчеты в основном)
{
	size = (direction ? size*sZoom : size / sZoom); // Новый размер
	gravity_rad = (direction ? gravity_rad*sZoom : gravity_rad / sZoom); // Новый радиус гравитации
	DeleteObject(hBitmap);
	hBitmap = (HBITMAP)CopyImage((name != "Kometa" ? hOrigin : Kometa_Static_Origin_bmp), IMAGE_BITMAP, (int)size, (int)size, LR_COPYRETURNORG); // Из оригинального изображения делаем итоговое через сжатие
	GetObject(hBitmap, sizeof(BITMAP), &bitmap); // Для получения реальной информации о битмапе
}

void ObjectGalaxy::DrawPower(HDC hdc) // Отрисовка силового поля
{
	if (this->GetDrawChanceGravity()) // Далеко от центра - не рисуем
		return;
	HPEN pen = CreatePen(PS_SOLID, 1, RGB(0xff, 0, 0)); // Ручка
	HBRUSH brush = (HBRUSH)GetStockObject(NULL_BRUSH); // Кисть, которая симулирует пустоту
	HGDIOBJ hold1 = SelectObject(hdc, pen); // Выбор предметов в контекст
	HGDIOBJ hold2 = SelectObject(hdc, brush);
	Ellipse(hdc, (int)(GetPos().first - (int)gravity_rad), // Отрисовка эллипса
		(int)(GetPos().second - (int)gravity_rad),
		(int)(GetPos().first + (int)gravity_rad),
		(int)GetPos().second + (int)gravity_rad);
	DeleteObject(pen); // Удаление объектов
	DeleteObject(brush);
	DeleteObject(hold1);
	DeleteObject(hold2);
}

void ObjectGalaxy::RotateHBITMAP(float angle) // Поворот битмапа на любой угол
{
	POINT point[3]; // Указатель на 3 точки, формирующие полигон с битмапом
	point[0].x = (LONG)GetXYMatrix((float)bitmap.bmWidth / 2, (float)bitmap.bmHeight / 2, 0, 0, angle).first;
	point[0].y = (LONG)GetXYMatrix((float)bitmap.bmWidth / 2, (float)bitmap.bmHeight / 2, 0, 0, angle).second;
	point[1].x = (LONG)GetXYMatrix((float)bitmap.bmWidth / 2, (float)bitmap.bmHeight / 2, (float)bitmap.bmWidth, 0, angle).first;
	point[1].y = (LONG)GetXYMatrix((float)bitmap.bmWidth / 2, (float)bitmap.bmHeight / 2, (float)bitmap.bmWidth, 0, angle).second;
	point[2].x = (LONG)GetXYMatrix((float)bitmap.bmWidth / 2, (float)bitmap.bmHeight / 2, 0, (float)bitmap.bmHeight, angle).first;
	point[2].y = (LONG)GetXYMatrix((float)bitmap.bmWidth / 2, (float)bitmap.bmHeight / 2, 0, (float)bitmap.bmHeight, angle).second;
	HDC hdc = CreateCompatibleDC(NULL); // Создание простого контекста для хранения
	DeleteObject(hBitmap);
	hBitmap = (HBITMAP)CopyImage((name != "Kometa" ? hOrigin : Kometa_Static_Origin_bmp), IMAGE_BITMAP, (int)size, (int)size, LR_COPYRETURNORG);
	HGDIOBJ hOld = SelectObject(hdc, hBitmap);
	PlgBlt(hdc, point, hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, NULL, 0, 0); // Поворот к контексте
	DeleteObject(hBitmap);
	hBitmap = (HBITMAP)CopyImage(GetCurrentObject(hdc, OBJ_BITMAP), IMAGE_BITMAP, bitmap.bmWidth, bitmap.bmHeight, LR_COPYRETURNORG); // Копирование изображения из контекста
	DeleteDC(hdc);
	DeleteObject(hOld);
}