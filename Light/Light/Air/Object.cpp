#include "EventSystem.h"

Object::Object(HBITMAP* hb, bool gr, Vector st, TYPE _type, Vector normal, Object::MoveType moveType) // Конструктор
	:hBitmap(hb), Gravity(gr), StartPos(st), type(_type), Normal(new Vector[1]{ normal }), moveType(moveType)
{
	count_normal = 1;
	hide = false;
	GetObject(*hBitmap, sizeof(BITMAP), &bm_info);
	Center.x = StartPos.x + bm_info.bmWidth / 2;
	Center.y = StartPos.y + bm_info.bmHeight / 2;
	EventSystem::Instance()->AddObject(this);
}

Object::Object(WORD hb_name, bool gr, Vector st, TYPE _type, Vector normal, Object::MoveType moveType) // Конструктор
	:Gravity(gr), StartPos(st), type(_type), Normal(new Vector[1]{ normal }), moveType(moveType)
{
	count_normal = 1;
	hide = false;
	hBitmap = new HBITMAP();
	*hBitmap = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(hb_name));
	GetObject(*hBitmap, sizeof(BITMAP), &bm_info);
	Center.x = StartPos.x + bm_info.bmWidth / 2;
	Center.y = StartPos.y + bm_info.bmHeight / 2;
	EventSystem::Instance()->AddObject(this);
}

void Object::Movement()
{
	if (this->moveType == Ox || this->moveType == Oxy) {
		StartPos.x += Velocity.x;
		Center.x = StartPos.x + bm_info.bmWidth / 2;
	}
	if (this->moveType == Oy || this->moveType == Oxy) {
		StartPos.y += Velocity.y;
		Center.y = StartPos.y + bm_info.bmHeight / 2;
	}
}
void Object::Paint(HDC hdc)
{
	if (hide)
		return;
	HDC hMem = CreateCompatibleDC(hdc); // Доп контекст
	HGDIOBJ hOld = SelectObject(hMem, *hBitmap); // Выбор в него битмапа
	TransparentBlt(hdc, (int)StartPos.x, (int)StartPos.y, bm_info.bmWidth, bm_info.bmHeight,
		hMem, 0, 0, bm_info.bmWidth, bm_info.bmHeight, RGB(255, 255, 255)); // Отрисовка с исключением белого
	SelectObject(hMem, hOld);
	DeleteObject(hOld);
	DeleteDC(hMem);
}

void Object::SetPosition(Vector vec) {
	if (this->moveType == Ox || this->moveType == Oxy) {
		StartPos.x = vec.x;
	}
	if (this->moveType == Oy || this->moveType == Oxy) {
		StartPos.y = vec.y;
	}
}

LineLight::LineLight(Vector sp, Vector dir, COLORREF c) // Конструктор линии света
{
	color = c;
	StartPos = sp;
	Dir = dir;
	Dir.Normalize();
	PaintSystem::Instance().AddObject(this);
}

void LineLight::ChangeDir(Linz * obj, Vector & w, Vector & d, HDC & hdc)
{
	auto copy = w;
	d.Normalize();
	auto copyVec = w;
	while (GetPixel(hdc, (int)w.x, (int)w.y) != RGB(0, 0, 0) && havePoint(obj, w)) // Пока не достигнем края линзы
		w = w + d;
	if (!havePoint(obj, w)) // Если вдруг мимо пролетели
		return;
	while (obj->Center.x > (int)w.x && obj->Center.y * (w.y > obj->Center.y ? 1 : -1) > w.y)
		w += d;
	LineTo(hdc, (int)w.x, (int)w.y);
	while (GetPixel(hdc, (int)w.x, (int)w.y) != RGB(0, 0, 0) && havePoint(obj, w)) // Пока не достигнем края линзы
		w = w + d;
	LineTo(hdc, (int)w.x, (int)w.y);
	float k = obj->angle;
	k = obj->Center.y < w.y ? -k + 90 : k + 90;
	d.RotateDegree(w, k); // Повернули относительно прелом способности
	d.Normalize();

}

void LineLight::Paint(HDC hdc, std::vector<Object*> objects)
{
	Vector way = StartPos;
	Vector t_dir = Dir;
	t_dir.Normalize();
	HPEN pen = CreatePen(PS_SOLID, 2, color); // Ручка
	HGDIOBJ obj = SelectObject(hdc, pen);
	MoveToEx(hdc, (int)way.x, (int)way.y, NULL); // Начальная позиция луча
	while (way.x >= -10 && way.x <= 610 && way.y >= -10 && way.y <= 610) // Пока не вышли за пределы 
	{
		for (auto i = ++objects.begin(); i != objects.end(); i++) // Проверка на пересечение со всеми объектами
			if (havePoint(*i, way)) {
				ChangeDir((Linz*)*i, way, t_dir, hdc);
			}
		way = way + t_dir;
	}
	LineTo(hdc, (int)way.x, (int)way.y);
	SelectObject(hdc, obj);
	DeleteObject(pen); // Удаление ручки
}

void Setup::Movement()
{
	Moving();
	Object::Movement();
}

void Setup::SetPosition(Vector vec)
{
	Object::SetPosition(vec);
	this->Moving();
}

void Setup::ReCreate(int c_l)
{
	if (c_l == 0)
		return;
	PaintSystem::Instance().objects_line.clear();
	for (int i = 0; i < size; i++)
		delete lights[i];
	size = c_l;
	std::vector<Vector> direction = this->GetDirection(size);
	lights = new LineLight*[size];
	for (int i = 0; i < size; i++)
		lights[i] = new LineLight[1]{ LineLight(this->Center,direction[i], RGB(rand() % 255, rand() % 255, rand() % 255)) };
}