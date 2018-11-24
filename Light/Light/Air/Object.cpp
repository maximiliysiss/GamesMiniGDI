#include "EventSystem.h"

Object::Object(HBITMAP* hb, bool gr, Vector st, TYPE _type, Vector normal) // Конструктор
	:hBitmap(hb), Gravity(gr), StartPos(st), type(_type), Normal(new Vector[1]{ normal })
{
	count_normal = 1;
	hide = false;
	GetObject(*hBitmap, sizeof(BITMAP), &bm_info);
	Center.x = StartPos.x + bm_info.bmWidth / 2;
	Center.y = StartPos.y + bm_info.bmHeight / 2;
	EventSystem::Instance()->AddObject(this);
}

Object::Object(WORD hb_name, bool gr, Vector st, TYPE _type, Vector normal) // Конструктор
	:Gravity(gr), StartPos(st), type(_type), Normal(new Vector[1]{ normal })
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

LineLight::LineLight(Vector sp, Vector dir, COLORREF c) // Конструктор линии света
{
	color = c;
	StartPos = sp;
	Dir = dir;
	Dir.Normalize();
	PaintSystem::Instance().AddObject(this);
}

void Setup::ReCreate(int c_l)
{
	PaintSystem::Instance().objects_line.clear();
	for (int i = 0; i < size; i++)
		delete lights[i];
	size = c_l;
	std::vector<Vector> direction = this->GetDirection(size);
	lights = new LineLight*[size];
	for (int i = 0; i < size; i++)
		lights[i] = new LineLight[1]{ LineLight(this->Center,direction[i], RGB(rand() % 255, rand() % 255, rand() % 255)) };
}