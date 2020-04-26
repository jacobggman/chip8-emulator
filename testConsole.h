#pragma once
#include "olcConsoleGameEngine.h"
#include <math.h>       /* sin */
#include <complex> // for complex numbers

#define INTPUT_STRONG_X 10
#define INTPUT_STRONG_Y 10
#define nbr_iter 16

int value(int x, int y, int h, int w)
{
	using std::complex;
	complex<float> point((float)x / w - 1.5, (float)y / h - 0.5);
	complex<float> z(0, 0);
	unsigned int nb_iter = 0;
	while (abs(z) < 2 && nb_iter <= 34) {
		z = z * z + point;
		nb_iter++;
	}
	if (nb_iter < 34) return (255 * nbr_iter) / 33;
	else return 0;

}


class Con : public olcConsoleGameEngine
{
	float xi = 10;
	float yi = 10;
protected:
	virtual bool OnUserCreate()
	{
		return true;
	}

	void changeInput(float fElapsedTime)
	{
		float speed = 1;
		if (m_keys[VK_SHIFT].bHeld)
		{
			speed = 2;
		}
		if (m_keys[VK_CONTROL].bHeld)
		{
			speed = 0.5;
		}

		if (m_keys[VK_LEFT].bHeld)
		{
			xi += INTPUT_STRONG_X * speed * fElapsedTime;
		}
		if (m_keys[VK_RIGHT].bHeld)
		{
			xi -= INTPUT_STRONG_X * speed * fElapsedTime;
		}

		if (m_keys[VK_UP].bHeld)
		{
			yi += 0.5 * INTPUT_STRONG_Y * speed * fElapsedTime;
		}
		if (m_keys[VK_DOWN].bHeld)
		{
			yi -= 0.5 * INTPUT_STRONG_Y * speed * fElapsedTime;
		}
	}

	virtual bool OnUserUpdate(float fElapsedTime)
	{
		changeInput(fElapsedTime);

		for (int x = 0; x < m_nScreenHeight; x++)
		{
			for (int y = 0; y < m_nScreenWidth; y++)
			{
				//Draw(x, y, 0x2588, int(((x * y) / m)) % 16);
				if (y)
					Draw(x, y, 0x2588, int(x*x*xi / y * yi) % 16);
			}
		}
		return true;
	}
};