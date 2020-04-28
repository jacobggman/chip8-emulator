#pragma once
#include "olcConsoleGameEngine.h"
#include "CPU.h"
// to do:
// make screen draw
// make sound
// getInput
// connect to cpu

class Screen : public olcConsoleGameEngine
{
public:
	Screen(CPU* cpu)
	{
		this->cpu = cpu;
	}
	bool draw(int x, int y, unsigned char value)
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
				this->setValue(x + i, y, doDraw ^ true);
			}
		}
		return ifErased;
	}
	void setValue(int x, int y, bool doDraw)
	{

		if (doDraw)
		{

			this->Draw(x, y);
		}
		else
		{
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
		return;
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
		
		for (size_t i = 0; i < 0xF; i++)
		{
			this->DrawString(0, i, toStr(this->cpu->Vx[i]));
		}
		this->DrawString(0, 0xF, toStr(this->cpu->IRegister));
		this->DrawString(0, 0xF + 1, toStr(this->cpu->PCRegister));
		this->DrawString(0, 0xF + 2, toStr(this->cpu->SPRegister));
		this->DrawString(0, 0xF + 3, toStr(this->cpu->soundRegister));
		this->DrawString(0, 0xF + 4, toStr(this->cpu->delayRegister));
		
		return true;
	}
private:
	CPU* cpu;
	//void update();
	//void makeSound(float last);
	//bool getInput(unsigned char input);


};

