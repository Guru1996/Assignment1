#include "C:\RTExamples\RTExamples\rt.h"
#include "../Monitor.h"
#include "../Screen.h"
#include <iostream>
//#include <conio.h>
using namespace std;

CMailbox MyMailBox;
CRendezvous s1("Start", 4);
Monitor e1("elevator_1");
Monitor e2("elevator_2");
monitorData e1_data;
monitorData e2_data;
Screen s("displayIO");

//CMutex* e1_mutex = new CMutex("e1_mutex");
//CMutex* e2_mutex = new CMutex("e2_mutex");

CTypedPipe <char[3]> DispatcherIOpipe("DIO", 1);
//Some of the code in fontsize function is taken from Microsoft's visual studio help forum 
void fontsize(int a, int b) {
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	PCONSOLE_FONT_INFOEX lpConsoleCurrentFontEx = new CONSOLE_FONT_INFOEX();
	lpConsoleCurrentFontEx->cbSize = sizeof(CONSOLE_FONT_INFOEX);
	GetCurrentConsoleFontEx(out, 0, lpConsoleCurrentFontEx);
	lpConsoleCurrentFontEx->dwFontSize.X = a;
	lpConsoleCurrentFontEx->dwFontSize.Y = b;
	SetCurrentConsoleFontEx(out, 0, lpConsoleCurrentFontEx);
}

void ShowConsoleCursor(bool showFlag)
{
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO     cursorInfo;

	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = showFlag; // set the cursor visibility
	SetConsoleCursorInfo(out, &cursorInfo);
}

void intialAnimationSetup(int status,int elevatorNumber) {
	// E for background Color(Light Yellow) 
	// 4 for text color(Red)
	if (status == 1) {
		s.WriteToScreen(elevatorNumber, 2, "green", "FLOOR 9");
		s.WriteToScreen(elevatorNumber, 4, "green", "FLOOR 8");
		s.WriteToScreen(elevatorNumber, 6, "green", "FLOOR 7");
		s.WriteToScreen(elevatorNumber, 8, "green", "FLOOR 6");
		s.WriteToScreen(elevatorNumber, 10, "green", "FLOOR 5");
		s.WriteToScreen(elevatorNumber, 12, "green", "FLOOR 4");
		s.WriteToScreen(elevatorNumber, 14, "green", "FLOOR 3");
		s.WriteToScreen(elevatorNumber, 16, "green", "FLOOR 2");
		s.WriteToScreen(elevatorNumber, 18, "green", "FLOOR 1");
		s.WriteToScreen(elevatorNumber, 20, "green", "FLOOR G");
	}
	else {
		s.WriteToScreen(elevatorNumber, 2, "red", "FLOOR 9");
		s.WriteToScreen(elevatorNumber, 4, "red", "FLOOR 8");
		s.WriteToScreen(elevatorNumber, 6, "red", "FLOOR 7");
		s.WriteToScreen(elevatorNumber, 8, "red", "FLOOR 6");
		s.WriteToScreen(elevatorNumber, 10, "red", "FLOOR 5");
		s.WriteToScreen(elevatorNumber, 12, "red", "FLOOR 4");
		s.WriteToScreen(elevatorNumber, 14, "red", "FLOOR 3");
		s.WriteToScreen(elevatorNumber, 16, "red", "FLOOR 2");
		s.WriteToScreen(elevatorNumber, 18, "red", "FLOOR 1");
		s.WriteToScreen(elevatorNumber, 20, "red", "FLOOR G");
	}
}
void printvalues(int status, int current_floor, int direction, int door ,int elevatorNumber)  {
	//Status should be printed at top 
	/*
	* Status:in service means arrow is in green and arrows will be in red
	* Current floor: represented by the arrow
	* Door:It's represented by the bar
	*
	*/
	if (status == 1) {
		s.WriteToScreen(elevatorNumber, 0, "white", "STATUS:");
		s.WriteToScreen(6+ elevatorNumber, 0, "green", " IN SERVICE      ");
		intialAnimationSetup(status, elevatorNumber);
	}
	else {
		s.WriteToScreen(elevatorNumber, 0, "white", "STATUS");
		s.WriteToScreen(6+elevatorNumber, 0, "red", " OUT OF SERVICE");
		intialAnimationSetup(status, elevatorNumber);
	}



	if (door == 0 && direction == 1) {
		s.WriteToScreen(8 + elevatorNumber, 2 * (9 - current_floor), "white", "          ");
		s.WriteToScreen(8 + elevatorNumber, 4 + 2 * (9 - current_floor), "white", "          ");
		s.WriteToScreen(8 + elevatorNumber, 2 + 2 * (9 - current_floor), "white", "<-[--]UP   ");
	}
	else {
		if (door == 0 && direction == 2) {
			s.WriteToScreen(8 + elevatorNumber, 2 * (9 - current_floor), "white", "          ");
			s.WriteToScreen(8 + elevatorNumber, 4 + 2 * (9 - current_floor), "white", "          ");
			s.WriteToScreen(8 + elevatorNumber, 2 + 2 * (9 - current_floor), "white", "<-[--]DOWN");
		}
		else if (door == 1 && direction == 1) {
			s.WriteToScreen(8 + elevatorNumber, 2 * (9 - current_floor), "white", "          ");
			s.WriteToScreen(8 + elevatorNumber, 4 + 2 * (9 - current_floor), "white", "          ");
			s.WriteToScreen(8 + elevatorNumber, 2 + 2 * (9 - current_floor), "white", "<-[  ]UP   ");
		}
		else if (door == 1 && direction == 2) {
			s.WriteToScreen(8 + elevatorNumber, 2 * (9 - current_floor), "white", "          ");
			s.WriteToScreen(8 + elevatorNumber, 4 + 2 * (9 - current_floor), "white", "         ");
			s.WriteToScreen(8 + elevatorNumber, 2 + 2 * (9 - current_floor), "white", "<-[  ]DOWN");
		}
		else if (door == 0 && direction == 0) {
			s.WriteToScreen(8 + elevatorNumber, 2 * (9 - current_floor), "white", "          ");
			s.WriteToScreen(8 + elevatorNumber, 4 + 2 * (9 - current_floor), "white", "          ");
			s.WriteToScreen(8 + elevatorNumber, 2 + 2 * (9 - current_floor), "white", "<-[--]     ");
		}
		else {
			s.WriteToScreen(8 + elevatorNumber, 2 * (9 - current_floor), "white", "          ");
			s.WriteToScreen(8 + elevatorNumber, 4 + 2 * (9 - current_floor), "white", "          ");
			s.WriteToScreen(8 + elevatorNumber, 2 + 2 * (9 - current_floor), "white", "<-[  ]     ");
		}

	}
}



UINT __stdcall Elevator1_status_dealer(void* ThreadArgs) {
	char buff[50] = {'\0'};
	while (1) {
		//e1_mutex->Wait();
		e1_data = e1.get_elevator_status(2); //reading data and writing to the screen
		printvalues(e1_data.Generalstatus, e1_data.floor, e1_data.direction, e1_data.door, 0);
		/*sprintf_s(buff, "e1 direction: %i ", e1_data.direction);
		s.WriteToScreen(1, 5, "white", buff);
		sprintf_s(buff, "e1 floor: %i ", e1_data.floor);
		s.WriteToScreen(1, 6, "white", buff);
		sprintf_s(buff, "e1 door: %i ", e1_data.door);
		s.WriteToScreen(1, 7, "white", buff);
		sprintf_s(buff, "e1 status: %i ", e1_data.Generalstatus);
		s.WriteToScreen(1, 8, "white", buff);*/
		//e1_mutex->Signal();
	}

	return 0;
}

UINT __stdcall Elevator2_status_dealer(void* ThreadArgs) {
	char buff[50] = {'\0'};
	while (1) {
		e2_data = e2.get_elevator_status(2);
		printvalues(e2_data.Generalstatus, e2_data.floor, e2_data.direction, e2_data.door, 40);
		/*sprintf_s(buff, "e2 direction: %i ", e2_data.direction);
		s.WriteToScreen(25, 5, "white", buff);
		sprintf_s(buff, "e2 floor: %i ", e2_data.floor);
		s.WriteToScreen(25, 6, "white", buff);
		sprintf_s(buff, "e2 door: %i ", e2_data.door);
		s.WriteToScreen(25, 7, "white", buff);
		sprintf_s(buff, "e2 status: %i ", e2_data.Generalstatus);
		s.WriteToScreen(25, 8, "white", buff);*/
	}

	return 0;
}

UINT __stdcall get_command_keyboard(void* ThreadArgs) {
	char c;
	int valid = 0;
	char buff[30];
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

		else if (c == 'u') {
			command[0] = c;
			valid = 0;
			c = _getch();
			if ((((int)(c)-(int)('0')) >= 0 && ((int)(c)-(int)('0')) < 9)) {
				command[1] = c;
				valid = 1;
			}
		}

		else if (c == 'd') {
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
	return 0;
}

int main(void) {
	
	CThread   t1(Elevator1_status_dealer, ACTIVE, NULL);
	CThread   t2(Elevator2_status_dealer, ACTIVE, NULL);
	CThread   t3(get_command_keyboard, ACTIVE, NULL);
	s1.Wait();
	fontsize(12, 24);
	ShowConsoleCursor(0);
	while (1) {
		if (MyMailBox.TestForMessage()) {
			if (MyMailBox.GetMessage() == 555) {
				t3.Suspend();
				s.WriteToScreen(16, 25, "yellow", "SIMULATION TERMINATED");
			}
				
		}
	}
	t1.WaitForThread();
	t2.WaitForThread();
	t3.WaitForThread();
	return 0;
}
