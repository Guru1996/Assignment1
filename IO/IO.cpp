#include "Z:\RTExamples\rt.h"
#include "../Monitor.h"
#include "../Screen.h"
#include <iostream>
#include <conio.h>
using namespace std;

CRendezvous     s1("Start", 4);
Monitor e1("elevator_1");
Monitor e2("elevator_2");
monitorData e1_data;
monitorData e2_data;
Screen s("display");

CMutex* e1_mutex = new CMutex("e1_mutex");
CMutex* e2_mutex = new CMutex("e2_mutex");

CTypedPipe <char[3]> DispatcherIOpipe("DIO", 1);

UINT __stdcall Elevator1_status_dealer(void* ThreadArgs) {
	char buff[30];
	while (1) {
		//e1_mutex->Wait();
		e1_data = e1.get_elevator_status(2); //reading data and writing to the screen
		sprintf_s(buff, "direction: %i ", e1_data.direction);
		s.WriteToScreen(1, 5, "white", buff);
		sprintf_s(buff, "floor: %i ", e1_data.floor);
		s.WriteToScreen(15, 5, "white", buff);
		sprintf_s(buff, "door: %i ", e1_data.door);
		s.WriteToScreen(25, 5, "white", buff);
		sprintf_s(buff, "status: %i ", e1_data.Generalstatus);
		s.WriteToScreen(35, 5, "white", buff);
		//e1_mutex->Signal();
	}

	return 0;
}

UINT __stdcall Elevator2_status_dealer(void* ThreadArgs) {
	char buff[30];
	while (1) {
		e2_data = e2.get_elevator_status(2);
		sprintf_s(buff, "direction: %i ", e2_data.direction);
		s.WriteToScreen(45, 5, "white", buff);
		sprintf_s(buff, "floor: %i ", e2_data.floor);
		s.WriteToScreen(55, 5, "white", buff);
		sprintf_s(buff, "door: %i ", e2_data.door);
		s.WriteToScreen(65, 5, "white", buff);
		sprintf_s(buff, "status: %i ", e2_data.Generalstatus);
		s.WriteToScreen(75, 5, "white", buff);
	}

	return 0;
}

UINT __stdcall get_command_keyboard(void* ThreadArgs) {
	char c;
	char prev;
	int valid = 0;
	char command[3];
	while (1) {
		//intialize variables
		command[0] = '0';
		command[1] = '0';
		c = '0';
		valid = 0;

		c = _getch();
		if (c == '1' || c == '2') {
			command[0] = c;
			valid = 0;
			c = _getch();
			if ((((int)(c)-(int)('0')) >= 0 && ((int)(c)-(int)('0')) <= 9)) {
				command[1] = c;
				valid = 1;
			}
		}

		if (c == 'u') {
			command[0] = c;
			valid = 0;
			c = _getch();
			if ((((int)(c)-(int)('0')) >= 0 && ((int)(c)-(int)('0')) < 9)) {
				command[1] = c;
				valid = 1;
			}
		}

		if (c == 'd') {
			command[0] = c;
			valid = 0;
			c = _getch();
			if ((((int)(c)-(int)('0')) > 0 && ((int)(c)-(int)('0')) <= 9)) {
				command[1] = c;
				valid = 1;
			}
		}


		else if (c == '+' || c == '-') {
			command[0] = c;
			valid = 0;
			c = _getch();
			if (c == '1' || c == '2') {
				command[1] = c;
				valid = 1;
			}
		}
		else if (c == 'e') {
			command[0] = c;
			valid = 0;
			c = _getch();
			if (c == 'e') {
				command[1] = c;
				valid = 1;
			}
		}

		if (valid) {
			command[2] = '\0';
			DispatcherIOpipe.Write(&command); //send command to pipeline	//int TestForData (void) might need this at the other end		
		}
	}

}

int main(void) {

	s1.Wait();
	CThread   t1(Elevator1_status_dealer, ACTIVE, NULL);
	CThread   t2(Elevator2_status_dealer, ACTIVE, NULL);
	CThread   t3(get_command_keyboard, ACTIVE, NULL);
	t1.WaitForThread();
	t2.WaitForThread();
	t3.WaitForThread();
	return 0;
}
