#pragma once
#ifndef   __monitorData__
#define   __monitorData__
#include "Z:\RTExamples\rt.h"
#include <iostream>
using namespace std;

struct monitorData {
		int Generalstatus; //1: working 0:not
		int floor; // 0-9
		int direction; // 0: stationary 1:up  2:down
		int door; //1: open 2:close
};

class Monitor{

private:
	CDataPool* MonitorDataPool;
	struct monitorData *pMonitorData;
	monitorData LocalData;
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