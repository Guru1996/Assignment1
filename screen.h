#pragma once
#ifndef   __Screen__
#define   __Screen__
#include "Z:\RTExamples\rt.h"
#include <string>
#include <iostream>
using namespace std;

class Screen{

private:
	CMutex *theMutex;

public:
	Screen(string theName);
	~Screen();
	void WriteToScreen(int x,int y, string color,string message);
};

#endif