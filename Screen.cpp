#include "Screen.h"

Screen::Screen(bool debug, string programName)
{
	

	m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	m_hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);
	//SetConsoleActiveScreenBuffer(m_hConsole);
	
	m_sAppName = std::wstring(programName.begin(), programName.end());

	//this->debuger = nullptr;
	if (debug)
	{

		//this->debuger = new Debuger(cpu);
	}
}

void Screen::Fill(int x, int y, short c = 0x2588, short col = 0x000F)
{
	for (int i = 0; i < x; i++)
		for (int j = 0; j < y; j++)
			Draw(i, j, c, col);
}
BOOL Screen::CloseHandler(DWORD evt)
{
	// Note this gets called in a seperate OS thread, so it must
	// only exit when the game has finished cleaning up, or else
	// the process will be killed before OnUserDestroy() has finished
	if (evt == CTRL_CLOSE_EVENT)
	{
	}
	return true;
}
int Screen::ConstructConsole(int fontw, int fonth)
{
	if (m_hConsole == INVALID_HANDLE_VALUE)
		return 0;

	// Update 13/09/2017 - It seems that the console behaves differently on some systems
	// and I'm unsure why this is. It could be to do with windows default settings, or
	// screen resolutions, or system languages. Unfortunately, MSDN does not offer much
	// by way of useful information, and so the resulting sequence is the reult of experiment
	// that seems to work in multiple cases.
	//
	// The problem seems to be that the SetConsoleXXX functions are somewhat circular and 
	// fail depending on the state of the current console properties, i.e. you can't set
	// the buffer size until you set the screen size, but you can't change the screen size
	// until the buffer size is correct. This coupled with a precise ordering of calls
	// makes this procedure seem a little mystical :-P. Thanks to wowLinh for helping - Jx9

	// Change console visual size to a minimum so ScreenBuffer can shrink
	// below the actual visual size
	m_rectWindow = { 0, 0, 1, 1 };
	SetConsoleWindowInfo(m_hConsole, TRUE, &m_rectWindow);

	// Set the size of the screen buffer
	COORD coord = { (short)m_nScreenWidth, (short)m_nScreenHeight };
	if (!SetConsoleScreenBufferSize(m_hConsole, coord))
		return 0;

	// Assign screen buffer to the console
	if (!SetConsoleActiveScreenBuffer(m_hConsole))
		return 0;

	// Set the font size now that the screen buffer has been assigned to the console
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = fontw;
	cfi.dwFontSize.Y = fonth;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;

	/*	DWORD version = GetVersion();
		DWORD major = (DWORD)(LOBYTE(LOWORD(version)));
		DWORD minor = (DWORD)(HIBYTE(LOWORD(version)));*/

		//if ((major > 6) || ((major == 6) && (minor >= 2) && (minor < 4)))		
		//	wcscpy_s(cfi.FaceName, L"Raster"); // Windows 8 :(
		//else
		//	wcscpy_s(cfi.FaceName, L"Lucida Console"); // Everything else :P

		//wcscpy_s(cfi.FaceName, L"Liberation Mono");
	wcscpy_s(cfi.FaceName, L"Consolas");
	if (!SetCurrentConsoleFontEx(m_hConsole, false, &cfi))
		return 0;

	// Get screen buffer info and check the maximum allowed window size. Return
	// error if exceeded, so user knows their dimensions/fontsize are too large
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(m_hConsole, &csbi))
		return 0;
	if (m_nScreenHeight > csbi.dwMaximumWindowSize.Y)
		return 0;
	if (m_nScreenWidth > csbi.dwMaximumWindowSize.X)
		return 0;

	// Set Physical Console Window Size
	m_rectWindow = { 0, 0, (short)m_nScreenWidth - 1, (short)m_nScreenHeight - 1 };
	if (!SetConsoleWindowInfo(m_hConsole, TRUE, &m_rectWindow))
		return 0;

	// Set flags to allow mouse input		
	//if (!SetConsoleMode(m_hConsoleIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT))
		//return Error(L"SetConsoleMode");

	// Allocate memory for screen buffer
	m_bufScreen = new CHAR_INFO[m_nScreenWidth * m_nScreenHeight];
	memset(m_bufScreen, 0, sizeof(CHAR_INFO) * m_nScreenWidth * m_nScreenHeight);

	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CloseHandler, TRUE);
	return 1;
}

Screen::~Screen()
{
	//delete this->debuger;
}

void Screen::startScreen()
{
}

bool Screen::draw(int x, int y, unsigned char value)
{
	bool ifErased = false;

	for (int i = 0; i < 8; i++)
	{
		if ((value & (0x80 >> i)) != 0)
		{
			if (x > this->m_nScreenWidth || y > this->m_nScreenHeight)
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
	if (x >= 0 && x < m_nScreenWidth && y >= 0 && y < m_nScreenHeight)
	{
		m_bufScreen[y * m_nScreenWidth + x].Char.UnicodeChar = c;
		m_bufScreen[y * m_nScreenWidth + x].Attributes = col;
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
	return this->m_bufScreen[y * this->m_nScreenWidth + x].Attributes != emptyColor;
}

void Screen::updateScreen()
{
	// Update Title & Present Screen Buffer
	float fElapsedTime = 1;
	wchar_t s[256];
	swprintf_s(s, 256, L"OneLoneCoder.com - Console Game Engine - %s - FPS: %3.2f", m_sAppName.c_str(), 1.0f / fElapsedTime);
	SetConsoleTitle(s);
	WriteConsoleOutput(m_hConsole, m_bufScreen, { (short)m_nScreenWidth, (short)m_nScreenHeight }, { 0,0 }, &m_rectWindow);
}

bool Screen::OnUserCreate()
{
	return true;
}

bool Screen::OnUserUpdate(float fElapsedTime)
{
	//Sleep(1000 / (this->cpu->fpsLimit + fElapsedTime * 100));
	//this->cpu->fetch();

	//for (size_t i = 0; i < 0xF; i++)
	//{
	//	this->DrawString(0, i, toStr(this->cpu->Vx[i]));
	//}
	//this->DrawString(0, 0xF, toStr(this->cpu->IRegister));
	//this->DrawString(0, 0xF + 1, toStr(this->cpu->PCRegister));
	//this->DrawString(0, 0xF + 2, toStr(this->cpu->SPRegister));
	//this->DrawString(0, 0xF + 3, toStr(this->cpu->soundRegister));
	//this->DrawString(0, 0xF + 4, toStr(this->cpu->delayRegister));

	return true;
}


