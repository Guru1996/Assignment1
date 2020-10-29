#pragma once
#include "C:\RTExamples\RTExamples\rt.h"
#include <string>
using namespace std;
struct monitorData {
		int status;
};

class Monitor
{
	CDataPool *monitorDataPool;
	CMutex *theMutex;
	struct monitorData *pointerToData;
	CSemaphore* ps1;
	CSemaphore*	cs1;
	CSemaphore*	ps2;
	CSemaphore*	cs2;
public:
	Monitor(string theName);
	~Monitor();
	int get_elevator_status(int elevator_number);
	void update_status(int status);

};

