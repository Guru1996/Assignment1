#include "Z:\RTExamples\rt.h"
#include "../Monitor.h"
#include <iostream>
using namespace std;

Monitor e1("elevator_1");
Monitor e2("elevator_2");
monitorData e1_data;
monitorData e2_data;

CMutex* e1_mutex = new CMutex("e1_mutex");
CMutex* e2_mutex = new CMutex("e2_mutex");

CTypedPipe <char[3]> DispatcherIOpipe("DIO", 1);
char command[3];

UINT __stdcall Elevator1_status_dealer(void* ThreadArgs) {
	
	while (1) {
		e1_mutex->Wait(); //local variable needs to be protected, when writing or reading from it.
		e1_data = e1.get_elevator_status(1);
		e1_mutex->Signal();
	}

	return 0;
}

UINT __stdcall Elevator2_status_dealer(void* ThreadArgs) {

	while (1) {
		e2_mutex->Wait(); //local variable needs to be protected, when writing or reading from it.
		e2_data = e2.get_elevator_status(1); 
		e2_mutex->Signal();
	}

	return 0;
}

UINT __stdcall Get_commands(void* ThreadArgs) {

	while (1) {
		DispatcherIOpipe.Read(&command);
	}
	return 0;
}

int main(void) {

	return 0;
}