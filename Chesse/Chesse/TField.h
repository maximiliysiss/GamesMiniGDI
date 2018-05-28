#pragma once
#include <Windows.h> // libs
#include <string>
#include <stack>
#include <ctime>
#pragma comment(lib,"MSIMG32.lib")
#pragma comment(lib, "Gdi32.lib")
#include "resource.h"

class TField
{
private:
	TField() = delete; // hide default construct 
public:
	TField(std::pair<int, int> stpos, int size) : // construct
		startPos(stpos), offset(size / 8)
	{
		srand((unsigned int)time(NULL)); // create seed of random generation
		currentStep.first = rand() % 8; // random start
		currentStep.second = rand() % 8;
	}
	inline void SetBackGround(WORD wBackGround) // Set background
	{
		BackGround = new HBITMAP;
		*BackGround = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(wBackGround));
	}
	void Restart()
	{
		while (!database.empty())
			database.pop();
		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 8; j++)
				Field[i][j].ClearInfo(); // clear all cells
		currentStep.first = rand() % 8; // random start;
		currentStep.second = rand() % 8;
	}
	void Reverse() // back moving
	{
		if (database.size() != 0) // if there are some steps
		{
			currentStep = database.top();
			Field[currentStep.second][currentStep.first].ClearInfo();
			database.pop();
		}
	}
	bool PaintField(PAINTSTRUCT & ps, int width, int height); // paint
	inline bool EndDB() { return database.size() == 63; } // end of game?
	bool Move(POINT mouse_click, RECT rect); // moving
private:
	static HBITMAP Cells_Horse[5]; // static bitmaps
	class Cell // class Cells
	{
	public:
		Cell() { color = false; } // default construct
		inline void ClearInfo() { color = false; } // Cleaer
		inline void Step() { color = true; } // if step
		inline bool GetInfo() { return color; } // information
		void Show(HDC hdc, int i, int j, TField * field) // paint cell
		{
			HDC hMem = CreateCompatibleDC(hdc); // create compitable context
			SelectObject(hMem, Cells_Horse[this->GetInfoColor(i, j)]); // select some bitmap in context
			BitBlt(hdc, field->startPos.first + i * field->offset, field->startPos.second + j * field->offset,
				field->offset, field->offset, hMem, 0, 0, SRCCOPY); // copy from one to another context
			DeleteDC(hMem); // delete unusable context
			if (color) // if this cell is passed
			{
				HDC hMem = CreateCompatibleDC(hdc); // similar situation higher
				SelectObject(hMem, Cells_Horse[2]);
				BitBlt(hdc, field->startPos.first + i * field->offset, field->startPos.second + j * field->offset,
					field->offset, field->offset, hMem, 0, 0, SRCERASE);
				DeleteDC(hMem);
			}
		}
	private:
		inline int GetInfoColor(int i, int j) // information about color of cell (wbwbwbwbwb and so on)
		{
			if (i % 2 == 0)
				if (j % 2 == 0)
					return 0;
				else
					return 1;
			else
				if (j % 2 == 0)
					return 1;
				else
					return 0;
		}
		bool color; // passed?
	};

	bool UpperColors(HDC hdc); // coloring passed and active for step cells
	Cell Field[8][8]; // field
	HBITMAP * BackGround; // background
	std::pair<int, int> startPos; // coord of left high angle of field on window
	std::pair<int, int> currentStep; // current step
	std::stack<std::pair<int, int>> database; // db of steps
	int offset; // offset for painting
};