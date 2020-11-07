#include "Monitor.h"

Monitor::Monitor(string theName)
{
	MonitorDataPool = new CDataPool("__MonitorDataPool__" + theName, sizeof(struct monitorData));
	pMonitorData = (struct monitorData*)MonitorDataPool->LinkDataPool();

	ps1 = new CSemaphore("__MonitorProducer1__" + theName, 0, 1);
	ps2 = new CSemaphore("__MonitorProducer2__" + theName, 0, 1);
	cs1 = new CSemaphore("__MonitorConsumer1__" + theName, 1, 1);
	cs2 = new CSemaphore("__MonitorConsumer2__" + theName, 1, 1);

	//initializatoin values
	pMonitorData->Generalstatus = 1;
	pMonitorData->floor = 0;
	pMonitorData->direction = 0;
	pMonitorData->door = 1;
}

Monitor::~Monitor()
{
	delete MonitorDataPool;
	delete pMonitorData;
	delete ps1;
	delete cs1;
	delete ps2;
	delete cs2;
}

struct monitorData Monitor::get_elevator_status(int consumer)
{
	if (consumer == 1)  //1 for dispatcher, 2 for IO
	{
		if (ps1->Read()>0){
		ps1->Wait();
		LocalData.Generalstatus = pMonitorData->Generalstatus;
		LocalData.floor= pMonitorData->floor;
		LocalData.direction= pMonitorData->direction;
		LocalData.door = pMonitorData->door;
		return LocalData;
		cs1->Signal();
		}
		else {
			LocalData = LocalData;
			return LocalData;
		}
	}
	else{
		if (ps2->Read() > 0) {
			ps2->Wait();
			LocalData.Generalstatus = pMonitorData->Generalstatus;
			LocalData.floor = pMonitorData->floor;
			LocalData.direction = pMonitorData->direction;
			LocalData.door = pMonitorData->door;
			return LocalData;
			cs2->Signal();
		}
		else {
			LocalData = LocalData;
			return LocalData;
		}
	}
}

void Monitor::update_status(int status, int floor, int direction, int door)
{
	cs1->Wait();
	cs2->Wait();

	//updating data
	pMonitorData->Generalstatus = status;
	pMonitorData->floor = floor; 
	pMonitorData->direction = direction;
	pMonitorData->door = door;

	ps1->Signal();
	ps2->Signal();

}