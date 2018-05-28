#include "TField.h"

HBITMAP TField::Cells_Horse[5]{ LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_CELL_WHITE)),
LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_CELL_BLACK)),
LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_RED)),
LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_GREEN)),
LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_HORSE)) }; // static bitmaps

bool TField::PaintField(PAINTSTRUCT & ps, int width, int height) // paint 
{
	HDC hMemDC, hTempDC; // context
	HGDIOBJ hMemBmp, hSysBmp; // temp bitmaps
	hMemDC = CreateCompatibleDC(ps.hdc); // compatible context
	hMemBmp = CreateCompatibleBitmap(ps.hdc, width, height); // create temp bitmap is size of window
	hSysBmp = SelectObject(hMemDC, hMemBmp); // select temp bitmap to temp context to set sizes
	hTempDC = CreateCompatibleDC(hMemDC); // create temp context
	SelectObject(hTempDC, *BackGround); // select background
	BitBlt(hMemDC, 0, 0, width, height, hTempDC, 0, 0, SRCCOPY); // copy from one context to another
	DeleteDC(hTempDC); // delete context
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			Field[i][j].Show(hMemDC, i, j, this); // paint cells
	bool result = UpperColors(hMemDC); // coloring adding cells
	BitBlt(ps.hdc, 0, 0, width, height, hMemDC, 0, 0, SRCCOPY); // copy to main context
	SelectObject(hMemDC, hSysBmp); // Deleting temp context and bitmaps
	DeleteObject(hMemBmp);
	DeleteObject(hSysBmp);
	DeleteDC(hMemDC);
	return result;
}

bool TField::Move(POINT mouse_click, RECT rect) // moving
{
	// chech coord for belonging to field coord
	if (abs(rect.left - mouse_click.x) >= startPos.first + 9 && abs(rect.left - mouse_click.x) <= startPos.first + 8 * offset + 9
		&& abs(rect.top - mouse_click.y) >= startPos.second + 30 && abs(rect.top - mouse_click.y) <= startPos.second + offset * 8 + 30)
	{
		// get coord
		int j = (abs(rect.left - mouse_click.x) - startPos.first - 9) / offset;
		int i = (abs(rect.top - mouse_click.y) - startPos.second - 30) / offset;
		// check coord for belonging to step of horse
		if ((abs(currentStep.first - i) == 2 && abs(currentStep.second - j) == 1) || (abs(currentStep.first - i) == 1 && abs(currentStep.second - j) == 2))
		{
			if (!Field[j][i].GetInfo()) // if this cell is empty and not passed
			{
				database.push(currentStep);
				Field[currentStep.second][currentStep.first].Step();
				currentStep.first = i;
				currentStep.second = j;
				return true; // success
			}
		}
	}
	return false; // fail somewhere
}

bool TField::UpperColors(HDC hdc) // coloring
{
	HDC hMem = CreateCompatibleDC(hdc); // compatible context
	SelectObject(hMem, Cells_Horse[4]); // select bitmap with horse
	// copy bitmap with horse to main context with erasing white color
	TransparentBlt(hdc, startPos.first + offset * currentStep.second, startPos.second + offset * currentStep.first, offset, offset,
		hMem, 0, 0, offset, offset, RGB(255, 255, 255));
	SelectObject(hMem, Cells_Horse[3]); // select red rect
	int k = 0; // check all possible steps and their counting
	if (currentStep.second - 1 >= 0 && currentStep.first - 2 >= 0 && !Field[currentStep.second - 1][currentStep.first - 2].GetInfo())
	{
		BitBlt(hdc, startPos.first + offset * (currentStep.second - 1), startPos.second + offset * (currentStep.first - 2),
			offset, offset, hMem, 0, 0, MERGECOPY);
		k++;
	}
	if (currentStep.second + 1 < 8 && currentStep.first - 2 >= 0 && !Field[currentStep.second + 1][currentStep.first - 2].GetInfo())
	{
		BitBlt(hdc, startPos.first + offset * (currentStep.second + 1), startPos.second + offset * (currentStep.first - 2),
			offset, offset, hMem, 0, 0, MERGECOPY);
		k++;
	}
	if (currentStep.second + 2 < 8 && currentStep.first - 1 >= 0 && !Field[currentStep.second + 2][currentStep.first - 1].GetInfo())
	{
		BitBlt(hdc, startPos.first + offset * (currentStep.second + 2), startPos.second + offset * (currentStep.first - 1),
			offset, offset, hMem, 0, 0, MERGECOPY);
		k++;
	}
	if (currentStep.second + 2 < 8 && currentStep.first + 1 < 8 && !Field[currentStep.second + 2][currentStep.first + 1].GetInfo())
	{
		BitBlt(hdc, startPos.first + offset * (currentStep.second + 2), startPos.second + offset * (currentStep.first + 1),
			offset, offset, hMem, 0, 0, MERGECOPY);
		k++;
	}
	if (currentStep.second + 1 < 8 && currentStep.first + 2 < 8 && !Field[currentStep.second + 1][currentStep.first + 2].GetInfo())
	{
		BitBlt(hdc, startPos.first + offset * (currentStep.second + 1), startPos.second + offset * (currentStep.first + 2),
			offset, offset, hMem, 0, 0, MERGECOPY);
		k++;
	}
	if (currentStep.second - 1 >= 0 && currentStep.first + 2 < 8 && !Field[currentStep.second - 1][currentStep.first + 2].GetInfo())
	{
		BitBlt(hdc, startPos.first + offset * (currentStep.second - 1), startPos.second + offset * (currentStep.first + 2),
			offset, offset, hMem, 0, 0, MERGECOPY);
		k++;
	}
	if (currentStep.second - 2 >= 0 && currentStep.first + 1 < 8 && !Field[currentStep.second - 2][currentStep.first + 1].GetInfo())
	{
		BitBlt(hdc, startPos.first + offset * (currentStep.second - 2), startPos.second + offset * (currentStep.first + 1),
			offset, offset, hMem, 0, 0, MERGECOPY);
		k++;
	}
	if (currentStep.second - 2 >= 0 && currentStep.first - 1 >= 0 && !Field[currentStep.second - 2][currentStep.first - 1].GetInfo())
	{
		BitBlt(hdc, startPos.first + offset * (currentStep.second - 2), startPos.second + offset * (currentStep.first - 1),
			offset, offset, hMem, 0, 0, MERGECOPY);
		k++;
	}
	DeleteDC(hMem);
	if (k == 0) // if there aren't steps
		return false;
	return true;
}