#include "Screen.h"

Screen::Screen(bool debug, string programName)
{
	console = GetStdHandle(STD_OUTPUT_HANDLE);
	
	appName = std::wstring(programName.begin(), programName.end());

}

void Screen::fill(int x, int y, short c = 0x2588, short col = 0x000F)
{
	for (int i = 0; i < x; i++)
		for (int j = 0; j < y; j++)
			draw(i, j, c, col);
}

int Screen::constructConsole(int fontw, int fonth)
{
	if (console == INVALID_HANDLE_VALUE)
		return 1;

	rectWindow = { 0, 0, 1, 1 };
	if (!SetConsoleWindowInfo(console, TRUE, &rectWindow))
		return -1;

	COORD coord = { (short)screenWidth, (short)screenHeight };
	if (!SetConsoleScreenBufferSize(console, coord))
		return 2;

	if (!SetConsoleActiveScreenBuffer(console))
		return 3;

	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = fontw;
	cfi.dwFontSize.Y = fonth;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;

	wcscpy_s(cfi.FaceName, L"Consolas");
	if (!SetCurrentConsoleFontEx(console, false, &cfi))
		return 4;

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(console, &csbi))
		return 5;
	if (screenHeight > csbi.dwMaximumWindowSize.Y)
		return 6;
	if (screenWidth > csbi.dwMaximumWindowSize.X)
		return 7;

	rectWindow = { 0, 0, (short)screenWidth - 1, (short)screenHeight - 1 };
	if (!SetConsoleWindowInfo(console, TRUE, &rectWindow))
		return 8;

	screenBuffer = new CHAR_INFO[screenWidth * screenHeight];
	memset(screenBuffer, 0, sizeof(CHAR_INFO) * screenWidth * screenHeight);

	SetConsoleCtrlHandler(NULL, TRUE);
	return 0;
}

bool Screen::instactionDraw(int x, int y, unsigned char value)
{
	bool ifErased = false;

	for (int i = 0; i < 8; i++)
	{
		if ((value & (0x80 >> i)) != 0)
		{
			if (x > this->screenWidth || y > this->screenHeight)
			{
				return ifErased;
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

void Screen::draw(int x, int y, short c = 0x2588, short col = 0x000F)
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
		this->draw(x, y);
	}
	else
	{
		// deleting
		this->draw(x, y, 9608, emptyColor);
	}
}

bool Screen::isDraw(int x, int y)
{
	return this->screenBuffer[y * this->screenWidth + x].Attributes != emptyColor;
}

void Screen::updateScreen()
{
	wchar_t s[256];
	swprintf_s(s, 256, L"Chip8 Emulator - %s", appName.c_str());
	SetConsoleTitle(s);
	WriteConsoleOutput(console, screenBuffer, { (short)screenWidth, (short)screenHeight }, { 0,0 }, &rectWindow);
	std::this_thread::sleep_for(std::chrono::microseconds(DRAW_SPEED));
}
