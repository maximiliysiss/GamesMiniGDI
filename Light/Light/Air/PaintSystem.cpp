#include "PaintSystem.h"

void PaintSystem::SetBackGround(WORD hb_name)
{
	BackGround = new HBITMAP();
	*BackGround = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(hb_name));
}