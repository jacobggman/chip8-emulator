#pragma once
#include "olcConsoleGameEngine.h"
#include "CPU.h"
#include <sstream>      // std::stringstream

// to do:
// make screen draw
// make sound
// getInput
// connect to cpu

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


class Screen : public olcConsoleGameEngine
{
public:
	Screen(CPU* cpu, bool debug)
	{
		this->cpu = cpu;

		this->debuger = nullptr;
		if (debug)
		{

			this->debuger = new Debuger(cpu);

			
		}
	}
	~Screen()
	{
		delete this->debuger;
	}

	void startScreen()
	{ 
		if (debuger)
		{
			debuger->StartDebug();
			if (m_hConsole == debuger->getAndle())
			{
				printf("fuck");
			}
		}
		
		Start();
	}

	bool draw(int x, int y, unsigned char value)
	{
		bool ifErased = false;

		for (int i = 0; i < 8; i++)
		{
			if ((value & (0x80 >> i)) != 0)
			{
				if (x > this->m_nScreenWidth || y > this->m_nScreenHeight)
				{
					throw  "WTF";
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
		return ifErased;
	}
	void setValue(int x, int y, bool doDraw)
	{

		if (doDraw)
		{
			// drawing
			this->Draw(x, y);
		}
		else
		{
			// deleting
			this->Draw(x, y, 9608, FG_BLACK);
		}
	}

	bool isDraw(int x, int y)
	{
		return this->m_bufScreen[y * this->m_nScreenWidth + x].Attributes != FG_BLACK;
	}

	void updateScreen()
	{
		// Update Title & Present Screen Buffer
		float fElapsedTime = 1;
		wchar_t s[256];
		swprintf_s(s, 256, L"OneLoneCoder.com - Console Game Engine - %s - FPS: %3.2f", m_sAppName.c_str(), 1.0f / fElapsedTime);
		SetConsoleTitle(s);
		WriteConsoleOutput(m_hConsole, m_bufScreen, { (short)m_nScreenWidth, (short)m_nScreenHeight }, { 0,0 }, &m_rectWindow);
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

	virtual bool OnUserUpdate(float fElapsedTime)
	{
		Sleep(1000 / (this->cpu->fpsLimit + fElapsedTime * 100));
		this->cpu->fetch();
		
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
private:
	CPU* cpu;
	Debuger* debuger;
	//void update();
	//void makeSound(float last);
	//bool getInput(unsigned char input);


};

