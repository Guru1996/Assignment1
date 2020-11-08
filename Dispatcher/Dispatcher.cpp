#include "Z:\RTExamples\rt.h"
#include "../Monitor.h"
#include "../Screen.h"
#include <iostream>
using namespace std;

CRendezvous s1("Start", 4);
Monitor e1("elevator_1");
Monitor e2("elevator_2");
monitorData e1_data;
monitorData e2_data;
CMailbox    MyMailBox;

Screen s("displayDis");

CMutex* e1_mutex = new CMutex("e1_mutex");
CMutex* e2_mutex = new CMutex("e2_mutex");
CMutex* cmd_mutex = new CMutex("cmd_mutex");

CTypedPipe <char[3]> DispatcherIOpipe("DIO", 1);
char command[3] = {'0'};


UINT __stdcall Elevator1_status_dealer(void* ThreadArgs) {
	char buff[50];
	int counter=0;
	while (1) {
		//s.WriteToScreen(15, 13, "white", "waintng for e1 mutex");
		e1_mutex->Wait(); //local variable needs to be protected, when writing or reading from it.
		e1_data = e1.get_elevator_status(1);
		sprintf_s(buff, "e1 direction: %i ", e1_data.direction);
		//Sleep(100);
		//counter++;
		s.WriteToScreen(15, 5, "white", buff);
		sprintf_s(buff, "e1 floor: %i ", e1_data.floor);
		s.WriteToScreen(15, 6, "white", buff);
		sprintf_s(buff, "e1 door: %i ", e1_data.door);
		s.WriteToScreen(15, 7, "white", buff);
		sprintf_s(buff, "e1 status: %i ", e1_data.Generalstatus);
		s.WriteToScreen(15, 8, "white", buff);
		e1_mutex->Signal();
	}

	return 0;
}

UINT __stdcall Elevator2_status_dealer(void* ThreadArgs) {

	while (1) {
		char buff[50];
		e2_mutex->Wait(); //local variable needs to be protected, when writing or reading from it.
		e2_data = e2.get_elevator_status(1); 
		sprintf_s(buff, "e2 direction: %i ", e2_data.direction);
		s.WriteToScreen(45, 5, "white", buff);
		sprintf_s(buff, "e2 floor: %i ", e2_data.floor);
		s.WriteToScreen(45, 6, "white", buff);
		sprintf_s(buff, "e2 door: %i ", e2_data.door);
		s.WriteToScreen(45, 7, "white", buff);
		sprintf_s(buff, "e2 status: %i ", e2_data.Generalstatus);
		s.WriteToScreen(45, 8, "white", buff);
		e2_mutex->Signal();
	}

	return 0;
}

UINT __stdcall Get_commands(void* ThreadArgs) {
	char buff[30];
	while (1) {
			cmd_mutex->Wait(); // maybe semaphore can be added
			if (DispatcherIOpipe.TestForData() == 1) {
				DispatcherIOpipe.Read(&command);
				//cout << command << endl;
				sprintf_s(buff, "Command from IO: %s ", command);
				s.WriteToScreen(1, 3, "white", buff);
			}
			cmd_mutex->Signal();
	}
	return 0;
}

int main(void) {
	char buff[40];
	e1_data.direction = 0;
	e1_data.door = 1;
	e1_data.floor = 0;
	e1_data.Generalstatus = 0;
	e2_data.direction = 0;
	e2_data.door = 1;
	e2_data.floor = 0;
	e2_data.Generalstatus = 0;
	//creating elevator 1, elevator 2 and IO proccesses
	CProcess   elevator1("Z:\\Users\\98ani\\Desktop\\CPEN_333\\Assignment_1\\Assignment1_local\\ASN1\\x64\\Debug\\Elevator1.exe",
		NORMAL_PRIORITY_CLASS,		// a safe priority level
		OWN_WINDOW,			// process uses its own window  ,
		ACTIVE				// create process in running/active state
		);

	CProcess   elevator2("Z:\\Users\\98ani\\Desktop\\CPEN_333\\Assignment_1\\Assignment1_local\\ASN1\\x64\\Debug\\Elevator2.exe",
		NORMAL_PRIORITY_CLASS,		// a safe priority level
		OWN_WINDOW,			// process uses its own window  ,
		ACTIVE				// create process in running/active state
	);

	CProcess   IO("Z:\\Users\\98ani\\Desktop\\CPEN_333\\Assignment_1\\Assignment1_local\\ASN1\\x64\\Debug\\IO.exe",
		NORMAL_PRIORITY_CLASS,		// a safe priority level
		OWN_WINDOW,			// process uses its own window  ,
		ACTIVE				// create process in running/active state
	);
	s1.Wait();
	//Sleep(1000);
	CThread   t1(Elevator1_status_dealer, ACTIVE, NULL);
	CThread   t2(Elevator2_status_dealer, ACTIVE, NULL);
	CThread   t3(Get_commands, ACTIVE, NULL);

	//wait for processes and threads to be created in other programs
	
	//Sleep(1000);
	cout << "while starting" << endl;
	s.WriteToScreen(1, 15, "white", "commands to elevator 1");
	s.WriteToScreen(1, 17, "white", "commands to elevator 2");
	int counter = 0;
	//dispatcher code
	//elevator1.Post(7);
	while (1) {
		//sprintf_s(buff, "waiting for e1_mutex: %i", counter);
		//s.WriteToScreen(1, 25, "white", buff);
		e1_mutex->Wait();
		//sprintf_s(buff, "waiting for e2_mutex: %i", counter);
		//s.WriteToScreen(1, 26, "white", buff);
		e2_mutex->Wait(); // make sure e1 e2 continuously sends data
		//sprintf_s(buff, "waiting for cmd_mutex: %i", counter);
		//s.WriteToScreen(1, 27, "white", buff);
		cmd_mutex->Wait();

		//s.WriteToScreen(3, 4, "white", "entered the decison making loop");
		counter++;
		// ------------dispatcher decisions-------------- 
		//specific commands to elevators
		if (command[0] == '1' && e1_data.Generalstatus == 1) {
			elevator1.Post(100 + (int)(command[1]) - (int)('0')); //command specific to e1
			sprintf_s(buff, "e1: %i", 100 + (int)(command[1]) - (int)('0'));
			s.WriteToScreen(1, 16, "white", buff);
		}
		else if (command[0] == '2' && e2_data.Generalstatus == 1) {
			elevator2.Post(100 + (int)(command[1]) - (int)('0')); //command specific to e2
			sprintf_s(buff, "e2: %i", 100 + (int)(command[1]) - (int)('0'));
			s.WriteToScreen(1, 18, "white", buff);

		}

		//fault commands to elevators
		else if (command[0] == '+') {
			if (command[1] == '1') {
				elevator1.Post(999); //send fault arrival to e1
				sprintf_s(buff, "e1: %i", 999);
				s.WriteToScreen(1, 16, "white", buff);
			}
			else {
				elevator2.Post(999); //send fault arrival to e2
				sprintf_s(buff, "e2: %i", 999);
				s.WriteToScreen(1, 18, "white", buff);
			}
		}
		else if (command[0] == '-') {
			if (command[1] == '1') {
				elevator1.Post(111); //send fault resolved to e1  111 means +1
				sprintf_s(buff, "e1: %i", 111);
				s.WriteToScreen(1, 16, "white", buff);
			}
			else {
				elevator2.Post(111); //send fault resolved to e2
				sprintf_s(buff, "e2: %i", 111);
				s.WriteToScreen(1, 18, "white", buff);
			}

		//terminate command to elevators
		}
		else if (command[0] == 'e' && command[1] == 'e') {
			elevator1.Post(555);
			elevator2.Post(555);//stop accepting commands
			sprintf_s(buff, "e1: %i", 555);
			s.WriteToScreen(1, 16, "white", buff);
			sprintf_s(buff, "e2: %i", 555);
			s.WriteToScreen(1, 18, "white", buff);
			IO.Post(555);
		}

		//incase both elevators at fault
		else if (e1_data.Generalstatus == 0 && e2_data.Generalstatus == 0){
		//do nothing
		}

		//Up and down general commands  ---------MAIN FUNCTIONALITY----==================
		else if (command[0] == 'u' || command[0] == 'd') {
			//CATEGORY 0: one of them not working
			if (e1_data.Generalstatus == 0 && e2_data.Generalstatus == 1) {
				elevator2.Post(100 + (int)(command[1]) - (int)('0')); //send command to elevator 2 if 1 not working
				sprintf_s(buff, "e2: %i", 100 + (int)(command[1]) - (int)('0'));
				s.WriteToScreen(1, 18, "white", buff);
			}
			else if (e2_data.Generalstatus == 0 && e1_data.Generalstatus == 1) {
				elevator1.Post(100 + (int)(command[1]) - (int)('0')); //send command to elevator 1 if 2 not working
				sprintf_s(buff, "e1: %i", 100 + (int)(command[1]) - (int)('0'));
				s.WriteToScreen(1, 16, "white", buff);
			}

			//CATERGORY 1: same direction requests
			//up request and lift 1 going up and floor requested higher than current floor
			else if (command[0] == 'u' && e1_data.direction== 1 && ( (((int)(command[1]) - (int)('0')) - e1_data.floor) > 0)) {
				elevator1.Post(100 + (int)(command[1]) - (int)('0'));
				sprintf_s(buff, "e1: %i", 100 + (int)(command[1]) - (int)('0'));
				s.WriteToScreen(1, 16, "white", buff);

			}
			//up request and lift 2 going up and floor requested higher than current floor
			else if (command[0] == 'u' && e2_data.direction == 1 && ( (((int)(command[1]) - (int)('0')) - e2_data.floor) > 0)) {
				elevator2.Post(100 + (int)(command[1]) - (int)('0'));
				sprintf_s(buff, "e2: %i", 100 + (int)(command[1]) - (int)('0'));
				s.WriteToScreen(1, 18, "white", buff);
			}
			//down request and lift 1 going down and floor requested lower than current floor
			else if (command[0] == 'd' && e1_data.direction == 2 && ((e1_data.floor - ((int)(command[1]) - (int)'0')) > 0)) {
				elevator1.Post(100 + (int)(command[1]) - (int)('0'));
				sprintf_s(buff, "e1: %i", 100 + (int)(command[1]) - (int)('0'));
				s.WriteToScreen(1, 16, "white", buff);
			}
			//down request and lift 2 going down and floor requested lower than current floor
			else if (command[0] == 'd' && e2_data.direction == 2 && ((e2_data.floor- ((int)(command[1]) - (int)('0'))) > 0)) {
				elevator2.Post(100 + (int)(command[1]) - (int)('0'));
				sprintf_s(buff, "e2: %i", 100 + (int)(command[1]) - (int)('0'));
				s.WriteToScreen(1, 18, "white", buff);
			}

			//CATEGORY 2: stationary elevators
			// both elevators stationary, give preference to lesser floor difference
			else if (e2_data.direction == 0 && e1_data.direction == 0)
				{
				if (abs(e2_data.floor - ((int)(command[1]) - (int)('0'))) < abs(e1_data.floor - ((int)(command[1]) - (int)('0')))) {
					elevator2.Post(100 + (int)(command[1]) - (int)('0'));
					sprintf_s(buff, "e2: %i", 100 + (int)(command[1]) - (int)('0'));
					s.WriteToScreen(1, 18, "white", buff);
					}
				else {
					elevator1.Post(100 + (int)(command[1]) - (int)('0'));
					sprintf_s(buff, "e1: %i", 100 + (int)(command[1]) - (int)('0'));
					s.WriteToScreen(1, 16, "white", buff);
				}
				}
			//elevator 2 is stationary, send that
			else if (e2_data.direction == 0) {
				elevator2.Post(100 + (int)(command[1]) - (int)('0'));
				sprintf_s(buff, "e2: %i", 100 + (int)(command[1]) - (int)('0'));
				s.WriteToScreen(1, 18, "white", buff);
			}
			//elevator 1 is stationary, send that
			else if (e1_data.direction == 0) {
				elevator1.Post(100 + (int)(command[1]) - (int)('0'));
				sprintf_s(buff, "e1: %i", 100 + (int)(command[1]) - (int)('0'));
				s.WriteToScreen(1, 16, "white", buff);
			}
			
			//CATEGORY 3: elevators moving in unfavourable direction, send command to the one farther from it 
			else if (abs(e2_data.floor - ((int)(command[1]) - (int)('0'))) > abs(e1_data.floor - ((int)(command[1]) - (int)('0')))) {
				elevator2.Post(100 + (int)(command[1]) - (int)('0'));
				sprintf_s(buff, "e2: %i", 100 + (int)(command[1]) - (int)('0'));
				s.WriteToScreen(1, 18, "white", buff);
			}
			else if (abs(e2_data.floor - ((int)(command[1]) - (int)('0'))) < abs(e1_data.floor - ((int)(command[1]) - (int)('0')))) {
				elevator1.Post(100 + (int)(command[1]) - (int)('0'));
				sprintf_s(buff, "e1: %i", 100 + (int)(command[1]) - (int)('0'));
				s.WriteToScreen(1, 16, "white", buff);
			}
			
			//CATEGORY 4: incase a command doesn't satisty any of the conditions above
			else {
				elevator2.Post(100 + (int)(command[1]) - (int)('0'));
				sprintf_s(buff, "e1: %i", 100 + (int)(command[1]) - (int)('0'));
				s.WriteToScreen(1, 16, "white", buff);
			}
			
		}

		//clearing command becuase already executed
		command[0] = '0';
		cmd_mutex->Signal();

		e1_mutex->Signal();
		e2_mutex->Signal(); 
		
	}


	//wait for porcesses and threads to end
	elevator1.WaitForProcess();
	elevator2.WaitForProcess();
	IO.WaitForProcess();
	t1.WaitForThread();
	t2.WaitForThread();
	t3.WaitForThread();
	return 0;
}
