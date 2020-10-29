#include "Monitor.h"


Monitor::Monitor(string theName)
{
	theMutex = new CMutex("__MonitorMutex__" + theName);
	monitorDataPool = new CDataPool("__MonitorDataPool__" + theName, sizeof(struct monitorData));
	pointerToData = (struct monitorData*)monitorDataPool->LinkDataPool();
	ps1 = new CSemaphore("__MonitorProducer1__" + theName, 0, 1);
	ps2 = new CSemaphore("__MonitorProducer2__" + theName, 0, 1);
	cs1 = new CSemaphore("__MonitorConsumer1__" + theName, 0, 1);
	cs2 = new CSemaphore("__MonitorConsumer2__" + theName, 0, 1);
	pointerToData->status = 0;//some random value
}

Monitor::~Monitor()
{
	delete theMutex;
	delete monitorDataPool;
}

int Monitor::get_elevator_status(int elevator_number)
{
	int x;
	theMutex->Wait();
	
	x=pointerToData->status;
	
	theMutex->Signal();
	return x ;
}

void Monitor::update_status(int status)
{
	theMutex->Wait();
	pointerToData->status = status;
	theMutex->Signal();
}

//void Monitor::WriteToScreen(int x, int y,string message)
//{
//	theMutex->Wait();
//	//adding data to the datapool
//	pointerToData->x = x;
//	pointerToData->y = y;
//	strcpy_s(pointerToData->Message, message.c_str());
//	//accessing data from datapool
//	MOVE_CURSOR(pointerToData->x, pointerToData->y);             	
//	printf("%s", pointerToData->Message);
//	fflush(stdout);
//	theMutex->Signal();
//}
