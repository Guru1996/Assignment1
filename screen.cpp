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
	//char buff[20];
	theMutex->Wait();
	MOVE_CURSOR(x, y);
	/*
	if (color == "red") {
		printf("\033[1;31m");
	}
	else if (color == "green") {
		printf("\033[0;32m");
	}
	else {
		printf("\033[0m");
	}
	*/
	cout << message;
	cout.flush();

	theMutex->Signal();
}
