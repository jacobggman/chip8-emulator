#pragma once
#include <string>
#include <vector>
#include <iterator>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#include <Windows.h>




#define RAM_SIZE 4096
#define READ_SIZE 512

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

// todo: 
// refactor: make screen in cpu
// add debug tools


using std::string;
using std::vector;
using std::istream_iterator;
using std::ifstream;

#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

typedef unsigned short int opcode;
typedef unsigned short nnn;
typedef unsigned char  x;
typedef unsigned char  kk;

int const NUMBERS_SPRITES[] = { 0xF0, 0x90, 0x90, 0x90, 0xF0, 0x20, 0x60, 0x20, 0x20, 0x70, 0xF0, 0x10, 0xF0, 0x80, 0xF0, 0xF0, 0x10, 0xF0, 0x10, 0xF0, 0x90, 0x90, 0xF0, 0x10, 0x10, 0xF0, 0x80, 0xF0, 0x10, 0xF0, 0xF0, 0x80, 0xF0, 0x90, 0xF0, 0xF0, 0x10, 0x20, 0x40, 0x40, 0xF0, 0x90, 0xF0, 0x90, 0xF0, 0xF0, 0x90, 0xF0, 0x10, 0xF0, 0xF0, 0x90, 0xF0, 0x90, 0x90, 0xE0, 0x90, 0xE0, 0x90, 0xE0, 0xF0, 0x80, 0x80, 0x80, 0xF0, 0xE0, 0x90, 0x90, 0x90, 0xE0, 0xF0, 0x80, 0xF0, 0x80, 0xF0, 0xF0, 0x80, 0xF0, 0x80, 0x80 };

class Screen;

class CPU
{
public:
	/*
	Init the chip8
	Input: Chip8 program path and max frames per secend
	Output: None
	*/
	CPU(string ROMfilePath, int fpsLimit);
	/*
	destruct the chip8
	Input: None
	Output: None
	*/
	~CPU();

	/*
	Run clock of cpu
	Input: None
	Output: None
	*/
	void fetch();

	/*
	Return last two bytes of a value
	Input: The value
	Output: Last two bytes
	Example: ABCD -> CD
	*/
	kk static getLastTwo(opcode opCode);
	/*
	Return secend byte of a value
	Input: The value
	Output: Secend byte
	Example: ABCD -> B
	*/
	x static getSecend(opcode opCode);
	/*
	Return third byte of a value
	Input: The value
	Output: third byte
	Example: ABCD -> C
	*/
	x static getThird(opcode opCode);



	typedef void (CPU::*arithmeticFunc)(x, x);

	int fpsLimit;
	unsigned char ram[RAM_SIZE] = { 0 };
	unsigned char Vx[16] = { 0 };
	unsigned char delayRegister;
	unsigned char soundRegister;
	unsigned char SPRegister;
	unsigned short IRegister;
	unsigned short PCRegister;
private:
	Screen* screen;


	
	/*
	Make instructions from opcode
	Input: Opcode
	Output: None
	*/
	void decode(opcode opCode);
	
	/*
	Decrease counters
	Input: None
	Output: None
	*/
	void downCounters();

	/*
	chip8 instructions
	Input: Operand/s or None
	Output: None
	*/

	void clearDisplay();  // 00E0
	void ret();  // 00EE
	void setPC(nnn addr);  // 1nnn 
	void call(nnn addr);  // 2nnn 
	void VxEqual(x vx, kk value);  // 3xkk 
	void VxNotEqual(x vx, kk value);  // 4xkk 
	void compereVxs(x vx, x vy);  // 5xy0 
	void setVx(x vx, kk value);  // 6xkk
	void addVx(x vx, kk value);  // 7xkk 
	void SetVxYx(x vx, x vy);  // 8xy0  
	void orVxYx(x vx, x vy);  // 8xy1  
	void andVxYx(x vx, x vy);  // 8xy2  
	void xorVxYx(x vx, x vy);  // 8xy3  
	void addVxYx(x vx, x vy);  // 8xy4  
	void subVxYx(x vx, x vy);  // 8xy5  
	void shrVxYx(x vx, x vy);  // 8xy6  
	void subnVxYx(x vx, x vy);  // 8xy7  
	void shlVxYx(x vx, x vy);  // 8xyE 
	void VxNotEqualYx(x vx, x vy);  // 9xy0 
	void setI(nnn addr);  // Annn 
	void jumpAddV0(nnn addr);  // Bnnn 
	void random(x vx, kk andValue);  // Cxkk 
	void display(x vx, x yx, x f);  // Dxyn
	void isPressed(x vx);  // Ex9E 
	void isNotPressed(x vx);  // ExA1 
	void setVxDelay(x vx);  // Fx07
	void waitForKey(x vx);  // Fx0A 
	void setDelayVx(x vx);  // Fx15 
	void setSoundVx(x vx);  // Fx18 
	void addIVx(x vx);  // Fx1E 
	void setIPointerOfVxNumber(x vx);  // Fx29
	void storeBCDVx(x vx);  // Fx33
	void saveRegisters(x vx);  // Fx55
	void loadRegisters(x vx);  // Fx65


	
	arithmeticFunc arithmeticFuncs[15] = { &CPU::SetVxYx, &CPU::orVxYx , &CPU::andVxYx, &CPU::xorVxYx, &CPU::addVxYx, &CPU::subVxYx, &CPU::shrVxYx, &CPU::subnVxYx, &CPU::shlVxYx, &CPU::shlVxYx, &CPU::shlVxYx, &CPU::shlVxYx, &CPU::shlVxYx, &CPU::shlVxYx, &CPU::shlVxYx };



	
	unsigned short stack[16] = { 0 };

	static const char chars[];
	int pi = 0;
};

