#pragma once
#ifndef   __Screen__
#define   __Screen__
#include "C:\RTExamples\RTExamples\rt.h"
#include <string>
#include <iostream>
#include <stdio.h>
#include <windows.h>
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