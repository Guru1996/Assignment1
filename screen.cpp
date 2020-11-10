#include "screen.h"

Screen::Screen(string theName)
{
	theMutex = new CMutex("__ScreenMutex__" + theName);
}

Screen::~Screen()
{
	delete theMutex;
	// ScreenDataPool;
}

void Screen::WriteToScreen(int x, int y,string color, string message)
{
	//BLUE = 1
	//GREEN = 2
	//RED = 4
	//WHITE = 15
	//YELLOW=6
	//LIGHT BLUE=9
	//char buff[20];

	theMutex->Wait();
	MOVE_CURSOR(x, y);
	
	HANDLE hConsole;

	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if(color=="blue"){
		SetConsoleTextAttribute(hConsole, 1);
	}
	else if (color == "red") {
		SetConsoleTextAttribute(hConsole, 4);
	}
	else if (color == "green") {
		SetConsoleTextAttribute(hConsole, 2);
	}
	else if(color=="lightBlue") {
		SetConsoleTextAttribute(hConsole, 9);
	}
	else if (color == "yellow") {
		SetConsoleTextAttribute(hConsole, 6);
	}
	else {
		SetConsoleTextAttribute(hConsole, 15);
	}
	
	cout << message;
	cout.flush();

	theMutex->Signal();
}
