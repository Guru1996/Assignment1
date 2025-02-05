#pragma once
#ifndef   __Monitor__
#define   __Monitor__
#include "C:\RTExamples\RTExamples\rt.h"
//#include "screen.h"
#include <iostream>
#include "struct_header.h"
using namespace std;

class Monitor{
private:
	CDataPool* MonitorDataPool;
	struct monitorData *pMonitorData;
	monitorData LocalDataIO;
	monitorData LocalDataDIS;
	CSemaphore* ps1;
	CSemaphore*	cs1;
	CSemaphore*	ps2;
	CSemaphore*	cs2;

public:
	Monitor(string theName);
	~Monitor();
	struct monitorData get_elevator_status(int consumer);
	void update_status(int Generalstatus, int floor, int direction, int door);

};

#endif 