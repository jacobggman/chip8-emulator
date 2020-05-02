#include "Screen.h"

Screen::Screen(bool debug, string programName)
{
	console = GetStdHandle(STD_OUTPUT_HANDLE);
	
	appName = std::wstring(programName.begin(), programName.end());

}

void Screen::Fill(int x, int y, short c = 0x2588, short col = 0x000F)
{
	for (int i = 0; i < x; i++)
		for (int j = 0; j < y; j++)
			Draw(i, j, c, col);
}
BOOL Screen::CloseHandler(DWORD evt)
{
	if (evt == CTRL_CLOSE_EVENT)
	{
	}
	return true;
}
int Screen::ConstructConsole(int fontw, int fonth)
{
	if (console == INVALID_HANDLE_VALUE)
		return 0;

	rectWindow = { 0, 0, 1, 1 };
	SetConsoleWindowInfo(console, TRUE, &rectWindow);

	COORD coord = { (short)screenWidth, (short)screenHeight };
	if (!SetConsoleScreenBufferSize(console, coord))
		return 0;

	if (!SetConsoleActiveScreenBuffer(console))
		return 0;

	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = fontw;
	cfi.dwFontSize.Y = fonth;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;

	wcscpy_s(cfi.FaceName, L"Consolas");
	if (!SetCurrentConsoleFontEx(console, false, &cfi))
		return 0;

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(console, &csbi))
		return 0;
	if (screenHeight > csbi.dwMaximumWindowSize.Y)
		return 0;
	if (screenWidth > csbi.dwMaximumWindowSize.X)
		return 0;

	rectWindow = { 0, 0, (short)screenWidth - 1, (short)screenHeight - 1 };
	if (!SetConsoleWindowInfo(console, TRUE, &rectWindow))
		return 0;

	screenBuffer = new CHAR_INFO[screenWidth * screenHeight];
	memset(screenBuffer, 0, sizeof(CHAR_INFO) * screenWidth * screenHeight);

	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CloseHandler, TRUE);
	return 1;
}

bool Screen::draw(int x, int y, unsigned char value)
{
	bool ifErased = false;

	for (int i = 0; i < 8; i++)
	{
		if ((value & (0x80 >> i)) != 0)
		{
			if (x > this->screenWidth || y > this->screenHeight)
			{
				throw  "out of range";
			}
			// check if destory
			if (isDraw(x + i, y))
			{
				ifErased = true;
				setValue(x + i, y, 0);
			}
			else
			{
				setValue(x + i, y, 1);
			}
			// draw
		}
		// loop bits
	}
	updateScreen();
	return ifErased;
}

void Screen::Draw(int x, int y, short c = 0x2588, short col = 0x000F)
{
	if (x >= 0 && x < screenWidth && y >= 0 && y < screenHeight)
	{
		screenBuffer[y * screenWidth + x].Char.UnicodeChar = c;
		screenBuffer[y * screenWidth + x].Attributes = col;
	}
}

void Screen::setValue(int x, int y, bool doDraw)
{
	if (doDraw)
	{
		// drawing
		this->Draw(x, y);
	}
	else
	{
		// deleting
		this->Draw(x, y, 9608, emptyColor);
	}
}

bool Screen::isDraw(int x, int y)
{
	return this->screenBuffer[y * this->screenWidth + x].Attributes != emptyColor;
}

void Screen::updateScreen()
{
	// Update Title & Present Screen Buffer
	float fElapsedTime = 1;
	wchar_t s[256];
	swprintf_s(s, 256, L"OneLoneCoder.com - Console Game Engine - %s - FPS: %3.2f", appName.c_str(), 1.0f / fElapsedTime);
	SetConsoleTitle(s);
	WriteConsoleOutput(console, screenBuffer, { (short)screenWidth, (short)screenHeight }, { 0,0 }, &rectWindow);
}
