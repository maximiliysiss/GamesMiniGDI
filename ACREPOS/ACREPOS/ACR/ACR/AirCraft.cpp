#include "SceneObjects.h"


// Файл самолета

void AirCraft::Movement() // Передвижение
{
	position.first += direction ? 1 * speed : -1 * speed;
}

void AirCraft::Draw(HDC hdc) // Отрисовка
{
	if (Wait(time_wait, WaitTimeSek))
	{
		this->DrawObject(hdc); // Отрисовка объекта
		this->Movement(); // Движение
		if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - time_reload).count() >= reload)
		{
			// Проверка возможнсти сбросить бомбу
			reload = TIMER_BOMB + 1 / (1 + rand() % 5);
			time_reload = std::chrono::high_resolution_clock::now();
			this->CreateBomb(); // Создание бомбы
		}
	}
}

AirCraft::AirCraft(std::string _name, HINSTANCE _hInst) :Object(_name, _hInst) // Конструктор/ случайные величины
{
	direction = (rand() % 2 == 0) ? true : false;
	SizeS = GetModelSizes();
	if (direction)
		RotateHBITMAPFlip();
	indexLine = (int)SetLine();
	AccessLine[indexLine] = false;
	position.second = OFFSET_OF_LINES + (float)indexLine * OFFSET_OF_LINES;
	speed = abs(AIRCRAFT_SPEED - 1 / (1 + rand() % 5));
	reload = float(1 + 1 / (1 + rand() % 2));
}

void AirCraft::ReloadBMP() // Перезагрузка избражения
{
	WaitTimeSek = 1.0f + 1 / (float)(1 + rand() % 3) * (rand() % 2 == 0 ? 1 : -1);
	time_wait = std::chrono::high_resolution_clock::now();
	bool direct = direction;
	direction = (rand() % 2 == 0) ? true : false;
	if (direct != direction) // Проверка на изменение стороны вылета
		this->RotateHBITMAPFlip();
	position.first = (float)(direction ? 0 - OFFSET_AIRCRAFT : sizeX + OFFSET_AIRCRAFT);
	AccessLine[indexLine] = true;
	indexLine = (int)SetLine();
	AccessLine[indexLine] = false;
	position.second = OFFSET_OF_LINES + (float)indexLine * OFFSET_OF_LINES;
	speed = abs(AIRCRAFT_SPEED - 1 / (1 + rand() % 5));
}

void AirCraft::CreateBomb() // Создание бомбы
{
	Bombs.push_back(AirCraftBomb("Bomb", hInst, direction, (float)bitmap.bmWidth, (float)bitmap.bmHeight, position, speed*2));
}

std::pair<float, float> * AirCraft::GetModelPos() // Получение позиций
{
	std::pair<float, float> * result = new std::pair<float, float>[4];
	for (UINT i = 0; i < 3; i++)
		result[i] = std::pair<float, float>(position.first + 20 * i, position.second + 10);
	result[3] = std::pair<float, float>(position.first + 19, position.second + 10);
	return result;
}

float * AirCraft::GetModelSizes() // ПОлучение размеров
{
	float * result = new float[4]{ 20,20,20,19 };
	return result;
}

float AirCraft::SetLine() // ПОлучение линии пролета
{
	if (rand() % 500 > 250)
	{
		for (UINT i = 0; i < (int)((HEIGHT_OF_ACFLY - OFFSET_OF_LINES) / OFFSET_OF_LINES); i++)
			if (AccessLine[i])
				return (float)i;
	}
	else
		for (UINT i = (int)((HEIGHT_OF_ACFLY - OFFSET_OF_LINES) / OFFSET_OF_LINES); i >= 0; i--)
			if (AccessLine[i])
				return (float)i;
	return 0;
}