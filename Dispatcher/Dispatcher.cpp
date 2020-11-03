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
CMutex* cmd_mutex = new CMutex("cmd_mutex");

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
		cmd_mutex->Wait();
		if (DispatcherIOpipe.TestForData() == sizeof(char[3])) {
			DispatcherIOpipe.Read(&command);
			}
		cmd_mutex->Signal();
	}
	return 0;
}

int main(void) {

	//creating elevator 1, elevator 2 and IO proccesses
	CProcess   elevator1("Z:\\Users\\98ani\\Desktop\\CPEN_333\\Assignment_1\\Assignment1_local\\ASN1\\x64\\Debug\\Elevator1",
		NORMAL_PRIORITY_CLASS,		// a safe priority level
		OWN_WINDOW,			// process uses its own window  ,
		ACTIVE				// create process in running/active state
		);

	CProcess   elevator2("Z:\\Users\\98ani\\Desktop\\CPEN_333\\Assignment_1\\Assignment1_local\\ASN1\\x64\\Debug\\Elevator1",
		NORMAL_PRIORITY_CLASS,		// a safe priority level
		OWN_WINDOW,			// process uses its own window  ,
		ACTIVE				// create process in running/active state
	);

	CProcess   IO("Z:\\Users\\98ani\\Desktop\\CPEN_333\\Assignment_1\\Assignment1_local\\ASN1\\x64\\Debug\\IO",
		NORMAL_PRIORITY_CLASS,		// a safe priority level
		OWN_WINDOW,			// process uses its own window  ,
		ACTIVE				// create process in running/active state
	);

	//wait for processes to be created
	Sleep(1000);

	//dispatcher code
	while (1) {
		e1_mutex->Wait();
		e2_mutex->Wait(); // make sure e1 e2 continuously sends data
		cmd_mutex->Wait();

		// ------------dispatcher decisions-------------- 
		//specific commands to elevators
		if (command[0] == '1' && e1_data.Generalstatus == 1) {
			elevator1.Post(100 + (int)(command[1]) - (int)('0')); //command specific to e1
		}
		else if (command[0] == '2' && e2_data.Generalstatus == 1) {
			elevator2.Post(100 + (int)(command[1]) - (int)('0')); //command specific to e2
		}

		//fault commands to elevators
		else if (command[0] == '+') {
			if (command[1] == '1') {
				elevator1.Post(999); //send fault arrival to e1
			}
			else {
				elevator2.Post(999); //send fault arrival to e2
			}
		}
		else if (command[0] == '-') {
			if (command[1] == '1') {
				elevator1.Post(111); //send fault resolved to e1  111 means +1
			}
			else {
				elevator2.Post(111); //send fault resolved to e2
			}
		//terminate command to elevators
		}
		else if (command[0] == 'e' && command[1] == 'e') {
			elevator1.Post(555);
			elevator2.Post(555);//stop accepting commands
		}

		//Up and down general commands  -----PENDING----MAIN FUNCTIONALITY----==================
		else if (command[0] == 'u' || command[0] == 'd') {
			if (e1_data.Generalstatus == 0 && e2_data.Generalstatus == 1) {
				elevator2.Post(100 + (int)(command[1]) - (int)('0')); //send command to elevator 2
			}
			else if (e2_data.Generalstatus == 0 && e1_data.Generalstatus == 1) {
				elevator1.Post(100 + (int)(command[1]) - (int)('0')); //send command to elevator 1
			}


		}

		//
		e1_mutex->Signal();
		e2_mutex->Signal(); 
		cmd_mutex->Signal();
	}


	//wait for porcesses to end
	elevator1.WaitForProcess();
	elevator2.WaitForProcess();
	IO.WaitForProcess();
	return 0;
}