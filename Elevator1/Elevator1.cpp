#include "Z:\RTExamples\rt.h"
#include "../Monitor.h"
#include <iostream>
using namespace std;

CRendezvous     s1("Start", 4);
Monitor e1("elevator_1");
monitorData e1_data;
int floors_to_stop[10] = { 0 };

UINT command;
CMailbox   MyMailBox;

CMutex* command1_mutex = new CMutex("command1_mutex");

//update_status(int status, int floor, int direction, int door)

UINT __stdcall Get_commands(void* ThreadArgs) {

	while (1) {
		command = MyMailBox.GetMessage();
		if (command >= 100 && command <= 109) {
			command1_mutex->Wait();
			floors_to_stop[command % 100] = 1;
			command1_mutex->Signal();
		}
	}
	return 0;
}

int main(void) {
	s1.Wait();
	int current_floor = 0;
	int status = 1;
	int direction = 0;
	int door = 1;
	int tracker = 0;

	while (1) { //999 means fault +1 111 means resolved   ---pending
		for (int i = 0; i++; i <= 9) {

		}
	}



	return 0;
}
