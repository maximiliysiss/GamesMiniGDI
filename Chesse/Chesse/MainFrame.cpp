#include "TField.h" // for game field
#include "ButtonImage.h" // buttons

const char* AppName = "Chesse Game"; // application name
const char* ClassName = "MainFrame"; // class name

LRESULT CALLBACK _WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam); // fucntion of processing

TField field(std::pair<int, int>(50, 70), 440); // main field

void ReStart(HWND hwnd) // function to restart
{
	field.Restart();
	InvalidateRect(hwnd, NULL, TRUE);
}

void BackMove(HWND hwnd) // function to back step
{
	field.Reverse();
	InvalidateRect(hwnd, NULL, TRUE);
}

Button_IMG buttons[2]{ Button_IMG(IDB_B_RESTART,50,10,ReStart), Button_IMG(IDB_B_REVERSE,420,10,BackMove) }; // buttons

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) // main function
{
	MSG msg; // for messages
	HWND hwnd; // var for window
	WNDCLASS wc; // class for window
	wc.cbClsExtra = wc.cbWndExtra = 0; // some properties of window's class
	wc.hbrBackground = (HBRUSH)WHITE_BRUSH;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wc.lpfnWndProc = _WindowProcedure;
	wc.lpszMenuName = NULL;
	wc.hInstance = hInstance;
	wc.lpszClassName = ClassName;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&wc); // register class in runtime and OS
	field.SetBackGround(IDB_BACKGROUND); // set background
	hwnd = CreateWindow(ClassName, AppName, WS_OVERLAPPEDWINDOW, 0, 0, 600, 600, NULL, NULL, hInstance, NULL); // create window
	ShowWindow(hwnd, SW_SHOW); // show
	UpdateWindow(hwnd); // first update
	while (GetMessage(&msg, 0, 0, 0)) // messages processing
	{
		if (!IsDialogMessage(hwnd, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return msg.message;
}

LRESULT CALLBACK _WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) // window's message processing
{
	HDC hdc; // context (like a piece of sheet)
	PAINTSTRUCT ps; // strcuture for 
	RECT rect; // struct for rectangle
	POINT mouse_click; // point about pos of click
	switch (message)
	{
	case WM_LBUTTONDOWN: // left mouse button press
		GetCursorPos(&mouse_click); // get about pos of click
		GetWindowRect(hwnd, &rect); // get info about window area
		if (field.Move(mouse_click, rect)) // process move
			InvalidateRect(hwnd, NULL, TRUE); // if move is correct to repaint window
		for (int i = 0; i < 2; i++) // process button
			buttons[i].OnClick(rect, mouse_click, hwnd);
		break;
	case WM_PAINT: // paint
		hdc = BeginPaint(hwnd, &ps); // start paint
		if (!field.PaintField(ps, 600, 600)) // paint and get info about result
			if (!field.EndDB())
				MessageBox(0, "No step", "Game Over", 0);
			else
				MessageBox(0, "You Win!!!", "Win", 0);
		for (int i = 0; i < 2; i++) // paint buttons
			buttons[i].OnPaint(ps.hdc);
		EndPaint(hwnd, &ps); // end paint
		break;
	case WM_CLOSE: // close window
		PostQuitMessage(NULL);
		return 0;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}