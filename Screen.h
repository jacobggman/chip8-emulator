#pragma once
//#include "CPU.h"
#include <sstream>     
#include <thread>
#include <string>
#include <Windows.h>
#include "defs.h"


using std::string;

/*
class Debuger : public olcConsoleGameEngine
{
public:
	std::thread t;

	Debuger(CPU* cpu)
	{
		this->cpu = cpu;
		this->ConstructConsole(SCREEN_WIDTH/5, SCREEN_HEIGHT, 16, 16);
		
		//Debuger::Start();
		//std::thread t = std::thread(&Debuger::Start, this);
	}

	void StartDebug()
	{
		t = std::thread(&olcConsoleGameEngine::Start, this);
	}

	auto getAndle()
	{
		return m_hConsole;
	}

protected:
	virtual bool OnUserCreate()
	{
		return true;
	}

	template <class num>
	static std::wstring toStr(num value)
	{
		return std::to_wstring(value);
	}

	void drawStr(string whatDraw, int high)
	{
		std::wstring wsTmp(whatDraw.begin(), whatDraw.end());

		this->DrawString(0, high, wsTmp);
	}

	void showInstactions(int numberOfInstactions)
	{
		int static line = 0xF + 5 + 1;
		int pc = cpu->PCRegister;
		for (int i = line; i < numberOfInstactions + line; i++)
		{
			std::stringstream ss;
			for (int j = 0; j < 2; j++)
				ss << std::hex << (int)cpu->ram[pc + j];

			std::string mystr = ss.str();
			drawStr(mystr, i);
			pc += 2;
		}
	}

	virtual bool OnUserUpdate(float fElapsedTime)
	{
		for (size_t i = 1; i < 0xF + 1; i++)
		{
			string v = "V" + std::to_string(i) + ": ";
			drawStr(v, i);

			this->DrawString(5, i, toStr(this->cpu->Vx[i]));
		}
		this->DrawString(5, 0xF + 1, toStr(this->cpu->IRegister));
		drawStr("I: ", 0xF + 1);
		this->DrawString(5, 0xF + 1 + 1, toStr(this->cpu->PCRegister));
		drawStr("PC: ", 0xF + 2);
		this->DrawString(5, 0xF + 1 + 2, toStr(this->cpu->SPRegister));
		drawStr("SP: ", 0xF + 3);
		this->DrawString(5, 0xF + 1 + 3, toStr(this->cpu->soundRegister));
		drawStr("SR: ", 0xF + 4);
		this->DrawString(5, 0xF + 1 + 4, toStr(this->cpu->delayRegister));
		drawStr("DR: ", 0xF + 5);

		showInstactions(10);
		return true;
	}

private:
	CPU* cpu;
	
};
*/

class Screen
{
public:

	Screen(bool debug, string programName);

	int ConstructConsole(int fontw, int fonth);
	~Screen();

	void Fill(int x, int y, short c, short col);

	static BOOL CloseHandler(DWORD evt);
	void startScreen();

	bool draw(int x, int y, unsigned char value);
	void Draw(int x, int y, short c, short col);
	void setValue(int x, int y, bool doDraw);

	bool isDraw(int x, int y);

	void updateScreen();

	int emptyColor = 0x0000;

	virtual bool OnUserCreate();


	virtual bool OnUserUpdate(float fElapsedTime);
	
private:

	SMALL_RECT rectWindow;
	int m_nScreenWidth = SCREEN_WIDTH;
	int m_nScreenHeight = SCREEN_HEIGHT;
	CHAR_INFO* m_bufScreen;
	std::wstring m_sAppName;
	HANDLE m_hOriginalConsole;
	CONSOLE_SCREEN_BUFFER_INFO m_OriginalConsoleInfo;
	HANDLE m_hConsole;
	HANDLE m_hConsoleIn;
	SMALL_RECT m_rectWindow;
};

