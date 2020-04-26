#pragma once
#include <Windows.h>
// to do:
// make screen draw
// make sound
// getInput
// connect to cpu

class Screen
{
public:
	Screen(int h, int w, int fontw, int fonth);
	~Screen();

	bool isDraw(int x, int y);

	void setValue(int x, int y, bool doDraw);

	bool draw(int x, int y, unsigned char value);
	
	void update();
	//void makeSound(float last);
	//bool getInput(unsigned char input);


private:
	CHAR_INFO* screenBuff;
	HANDLE console;
	HANDLE m_hConsoleIn;
	SMALL_RECT m_rectWindow;
	int h;
	int w;
};

