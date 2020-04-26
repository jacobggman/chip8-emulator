#include "Test.h"

Screen::Screen(int h, int w, int fontw, int fonth)
{
	this->h = h;
	this->w = w;
	console = GetStdHandle(STD_OUTPUT_HANDLE);
	m_hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);

	if (console == INVALID_HANDLE_VALUE)
		throw (L"Bad Handle");

	m_rectWindow = { 0, 0, 1, 1 };
	SetConsoleWindowInfo(console, TRUE, &m_rectWindow);

	// Set the size of the screen buffer
	COORD coord = { (short)h, (short)w };
	if (!SetConsoleScreenBufferSize(console, coord))
		throw (L"SetConsoleScreenBufferSize");

	// Assign screen buffer to the console
	if (!SetConsoleActiveScreenBuffer(console))
		throw (L"SetConsoleActiveScreenBuffer");

	// Set the font size now that the screen buffer has been assigned to the console
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = fontw;
	cfi.dwFontSize.Y = fonth;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;

	wcscpy_s(cfi.FaceName, L"Consolas");
	if (!SetCurrentConsoleFontEx(console, false, &cfi))
		throw (L"SetCurrentConsoleFontEx");

	// Get screen buffer info and check the maximum allowed window size. Return
	// error if exceeded, so user knows their dimensions/fontsize are too large
	if (true)
	{
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		if (!GetConsoleScreenBufferInfo(console, &csbi))
			throw (L"GetConsoleScreenBufferInfo");
		if (h > csbi.dwMaximumWindowSize.Y)
			throw (L"Screen Height / Font Height Too Big");
		if (w > csbi.dwMaximumWindowSize.X)
			throw (L"Screen Width / Font Width Too Big");
	}
	// Set Physical Console Window Size
	m_rectWindow = { 0, 0, (short)w - 1, (short)h - 1 };
	if (!SetConsoleWindowInfo(console, TRUE, &m_rectWindow))
		throw "SetConsoleWindowInfo";

	//if (!SetConsoleMode(m_hConsoleIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT))
		//throw (L"SetConsoleMode");

	// Allocate memory for screen buffer
	screenBuff = new CHAR_INFO[w * h];
	memset(screenBuff, 0, sizeof(CHAR_INFO) * w * h);
}

Screen::~Screen()
{
	delete[] this->screenBuff;
}

bool Screen::isDraw(int x, int y)
{
	return true;
	//return this->m_bufScreen[y * this->m_nScreenWidth + x].Attributes != FG_BLACK;
}

void Screen::setValue(int x, int y, bool doDraw)
{
	if (doDraw)
	{
	}
		//this->Draw(x, y);
	else
	{
	}
		//this->Draw(x, y, 9608, FG_BLACK);
}

bool Screen::draw(int x, int y, unsigned char value)
{
	bool ifErased = false;

	for (int i = 0; i < 8; i++)
	{
		// loop bits
		if ((value >> i) & 1)
		{
			bool doDraw = this->isDraw(x + i, y);
			if (!doDraw)
				ifErased = true;
			this->setValue(x + i, y, doDraw);
		}
	}
	return ifErased;
}

void Screen::update()
{
	WriteConsoleOutput(console, screenBuff, { (short)w, (short)h }, { 0,0 }, &m_rectWindow);
}

