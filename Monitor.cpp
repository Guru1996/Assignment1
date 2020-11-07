#include "Monitor.h"
//#include "screen.h"
Monitor::Monitor(string theName)
{
	MonitorDataPool = new CDataPool("__MonitorDataPool__" + theName, sizeof(struct monitorData));
	pMonitorData = (struct monitorData*)MonitorDataPool->LinkDataPool();

	ps1 = new CSemaphore("__MonitorProducer1__" + theName, 0, 1);
	ps2 = new CSemaphore("__MonitorProducer2__" + theName, 0, 1);
	cs1 = new CSemaphore("__MonitorConsumer1__" + theName, 1, 1);
	cs2 = new CSemaphore("__MonitorConsumer2__" + theName, 1, 1);

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
		LocalDataDIS.Generalstatus = pMonitorData->Generalstatus;
		LocalDataDIS.floor= pMonitorData->floor;
		LocalDataDIS.direction= pMonitorData->direction;
		LocalDataDIS.door = pMonitorData->door;
		cs1->Signal();
		return LocalDataDIS;
		}
		else {
			return LocalDataDIS;
		}
	}
	else{
		if (ps2->Read() > 0) {
			ps2->Wait();
			LocalDataIO.Generalstatus = pMonitorData->Generalstatus;
			LocalDataIO.floor = pMonitorData->floor;
			LocalDataIO.direction = pMonitorData->direction;
			LocalDataIO.door = pMonitorData->door;
			cs2->Signal();
			return LocalDataIO;
		}
		else {
			return LocalDataIO;
		}
	}
}

void Monitor::update_status(int status, int floor, int direction, int door)
{
	//cout << "waiting for cs1" << endl;
	cs1->Wait();
	//cout << "waiting for cs2" << endl;
	cs2->Wait();

	//updating data
	pMonitorData->Generalstatus = status;
	pMonitorData->floor = floor; 
	pMonitorData->direction = direction;
	pMonitorData->door = door;

	ps1->Signal();
	ps2->Signal();

}