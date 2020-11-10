#include "C:\RTExamples\RTExamples\rt.h"
#include "../Monitor.h"
#include "../screen.h"
#include <iostream>
#include<stdio.h>
#include <windows.h>
using namespace std;

CRendezvous s1("Start", 4);
Monitor e2("elevator_2");
monitorData e2_data;
int floors_to_stop[10] = { 0 };

UINT command = 0;
UINT local_command = 0;
CMailbox MyMailBox;
CMutex* command1_mutex = new CMutex("command2_mutex");
CMutex* local_command_mutex = new CMutex("local_command2_mutex");
//update_status(int status, int floor, int direction, int door)
/***************************************************************************************/
Screen elevator2_screen("elevator2_screen");

void ShowConsoleCursor(bool showFlag)
{
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO     cursorInfo;

	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = showFlag; // set the cursor visibility
	SetConsoleCursorInfo(out, &cursorInfo);
}

void intialAnimationSetup(int status) {
	// E for background Color(Light Yellow) 
	// 4 for text color(Red)
	if (status == 1) {
		elevator2_screen.WriteToScreen(0, 2, "green", "FLOOR 9");
		elevator2_screen.WriteToScreen(0, 4, "green", "FLOOR 8");
		elevator2_screen.WriteToScreen(0, 6, "green", "FLOOR 7");
		elevator2_screen.WriteToScreen(0, 8, "green", "FLOOR 6");
		elevator2_screen.WriteToScreen(0, 10, "green", "FLOOR 5");
		elevator2_screen.WriteToScreen(0, 12, "green", "FLOOR 4");
		elevator2_screen.WriteToScreen(0, 14, "green", "FLOOR 3");
		elevator2_screen.WriteToScreen(0, 16, "green", "FLOOR 2");
		elevator2_screen.WriteToScreen(0, 18, "green", "FLOOR 1");
		elevator2_screen.WriteToScreen(0, 20, "green", "FLOOR G");
	}
	else {
		elevator2_screen.WriteToScreen(0, 2, "red", "FLOOR 9");
		elevator2_screen.WriteToScreen(0, 4, "red", "FLOOR 8");
		elevator2_screen.WriteToScreen(0, 6, "red", "FLOOR 7");
		elevator2_screen.WriteToScreen(0, 8, "red", "FLOOR 6");
		elevator2_screen.WriteToScreen(0, 10, "red", "FLOOR 5");
		elevator2_screen.WriteToScreen(0, 12, "red", "FLOOR 4");
		elevator2_screen.WriteToScreen(0, 14, "red", "FLOOR 3");
		elevator2_screen.WriteToScreen(0, 16, "red", "FLOOR 2");
		elevator2_screen.WriteToScreen(0, 18, "red", "FLOOR 1");
		elevator2_screen.WriteToScreen(0, 20, "red", "FLOOR G");
	}
}
void printvalues(int status, int current_floor, int direction, int door) {
	//Status should be printed at top 
	/*
	* Status:in service means arrow is in green and arrows will be in red
	* Current floor: represented by the arrow
	* Door:It's represented by the bar
	*
	*/
	if (status == 1) {
		elevator2_screen.WriteToScreen(0, 0, "white", "STATUS:");
		elevator2_screen.WriteToScreen(6, 0, "green", " IN SERVICE      ");
		intialAnimationSetup(status);
	}
	else {
		elevator2_screen.WriteToScreen(0, 0, "white", "STATUS");
		elevator2_screen.WriteToScreen(6, 0, "red", " OUT OF SERVICE");
		intialAnimationSetup(status);
	}



	if (door == 0 && direction==1) {
		elevator2_screen.WriteToScreen(8, 2 * (9-current_floor), "white", "          ");
		elevator2_screen.WriteToScreen(8, 4 + 2 * (9 - current_floor), "white", "          ");
		elevator2_screen.WriteToScreen(8, 2 + 2 * (9 - current_floor), "white", "<-[--]UP   ");
	}
	else {
		if (door == 0 && direction == 2) {
			elevator2_screen.WriteToScreen(8, 2 * (9 - current_floor), "white", "          ");
			elevator2_screen.WriteToScreen(8, 4 + 2 * (9 - current_floor), "white", "          ");
			elevator2_screen.WriteToScreen(8, 2 + 2 * (9 - current_floor), "white", "<-[--]DOWN");
		}
		else if (door == 1 && direction == 1) {
			elevator2_screen.WriteToScreen(8, 2 * (9 - current_floor), "white", "          ");
			elevator2_screen.WriteToScreen(8, 4 + 2 * (9 - current_floor), "white", "          ");
			elevator2_screen.WriteToScreen(8, 2 + 2 * (9 - current_floor), "white", "<-[  ]UP   ");
		}
		else if (door == 1 && direction == 2) { 
			elevator2_screen.WriteToScreen(8, 2 * (9 - current_floor), "white", "          ");
			elevator2_screen.WriteToScreen(8, 4 + 2 * (9 - current_floor), "white", "         ");
			elevator2_screen.WriteToScreen(8, 2 + 2 * (9 - current_floor), "white", "<-[  ]DOWN");
		}
		else if (door == 0 && direction == 0) {
			elevator2_screen.WriteToScreen(8, 2 * (9 - current_floor), "white", "          ");
			elevator2_screen.WriteToScreen(8, 4 + 2 * (9 - current_floor), "white", "          ");
			elevator2_screen.WriteToScreen(8, 2 + 2 * (9 - current_floor), "white", "<-[--]     ");
		}
		else {
			elevator2_screen.WriteToScreen(8, 2 * (9 - current_floor), "white", "          ");
			elevator2_screen.WriteToScreen(8, 4 + 2 * (9 - current_floor), "white", "          ");
			elevator2_screen.WriteToScreen(8, 2 + 2 * (9 - current_floor), "white", "<-[  ]     ");
		}
		
	}
}

/**********************************************************************************************************/

//return true if elevator has pending requests in the current input direction
int pending_request(int current_floor, int direction)
{
	int dummy = 0;
	if (direction == 1) {
		if (current_floor == 9) {
			return 0;
		}
		else {
			for (int a = current_floor; a <= 9; a++) {
				//protecting floor_to_stop using mutex
				command1_mutex->Wait();
				dummy = floors_to_stop[a];
				command1_mutex->Signal();
				if (dummy) {
					return 1;
				}
			}
		}
	}
	else if (direction == 2) {
		if (current_floor == 0) {
			return 0;
		}
		else {
			for (int a = current_floor; a >= 0; a--) {
				//protecting floor_to_stop using mutex
				command1_mutex->Wait();
				dummy = floors_to_stop[a];
				command1_mutex->Signal();
				if (dummy) {
					return 1;
				}
			}
		}
	}

	return 0;

}

//gives dirrection where requests are pending, else returns 0 = stationary
int get_direction(int current_floor) {

	if (pending_request(current_floor, 1)) { //this will only return true if elevator has UP request
		return 1;
	}
	else if (pending_request(current_floor, 2)) { //this will return true if elevator has a down request
		return 2;
	}
	else {
		return 0;
	}
}


UINT __stdcall move_elevator(void* ThreadArgs) {
	int current_floor = 0;
	int status = 1;
	int direction = 0;
	int door = 0;
	int dummy;
	int dummy_command;

	ShowConsoleCursor(false);
	intialAnimationSetup(status);


	e2.update_status(status, current_floor, direction, door);// sending intital variables
	printvalues(status, current_floor, direction, door);
	//cout << "status= " << status << "current_floor= " << current_floor << "direction= " << direction << "door= " << door << "    " << endl;
	//cout << "Elevator 1 started" << endl;

	//keep moving in the same direction as long as there are any requests in that direction
	int counter = 0;
	while (1) {//999 means fault +1,  111 means resolved , 555 meanss termination go back to 0
		//cout << counter << endl;
		counter++;
		local_command_mutex->Wait(); 
		dummy_command = local_command;
		local_command_mutex->Signal();
		if (dummy_command == 555) {
			direction = 2;
			door = 0;
			e2.update_status(status, current_floor, direction, door); //updating current status
			printvalues(status, current_floor, direction, door);
			while (current_floor > 0) {
				Sleep(1000);
				current_floor--;
				e2.update_status(status, current_floor, direction, door); //updating current status
				printvalues(status, current_floor, direction, door);
				//cout << "status= " << status << "current_floor= " << current_floor << "direction= " << direction << "door= " << door << "    " << endl;

			}
			//open the door at 0 floor
			if (current_floor == 0) {
				Sleep(200); //wait for door to open
				door = 1;
				direction = 0;
				status = 0;
				e2.update_status(status, current_floor, direction, door); //updating current status
				printvalues(status, current_floor, direction, door);
				//cout << "status= " << status << "current_floor= " << current_floor << "direction= " << direction << "door= " << door << "    " << endl;
				local_command_mutex->Wait();
				local_command = 0;
				local_command_mutex->Signal();
			}
		}

		else {
			local_command_mutex->Wait();
			dummy_command = local_command;
			local_command_mutex->Signal();
			if (dummy_command == 999) {
				status = 0;
				e2.update_status(status, current_floor, direction, door); //updating current status
				printvalues(status, current_floor, direction, door);
				//cout << "status= " << status << "current_floor= " << current_floor << "direction= " << direction << "door= " << door << "    " << endl;
				//clear all pending requests
				for (int i = 0; i < 10; i++) {
					command1_mutex->Wait();
					floors_to_stop[i] = 0; //clear request
					command1_mutex->Signal();
				}

			}
			else {
				local_command_mutex->Wait();
				dummy_command = local_command;
				local_command_mutex->Signal();
				if (dummy_command == 111) {
					status = 1;
					direction = 0;
					door = 0;
					e2.update_status(status, current_floor, direction, door); //updating current status
					printvalues(status, current_floor, direction, door);
					//cout << "status= " << status << "current_floor= " << current_floor << "direction= " << direction << "door= " << door << "    " << endl;

				}
			}
		}

		//main code
		local_command_mutex->Wait();
		dummy_command = local_command;
		local_command_mutex->Signal();

		if (status == 1 && dummy_command != 555) {
			command1_mutex->Wait();
			dummy = floors_to_stop[current_floor];
			//cout << "entered loop" << endl;
			command1_mutex->Signal();
			//stopping at requested stop //stop and open the door, wait for it to close 
			if (dummy) {
				e2.update_status(status, current_floor, direction, door); //updating current status
				printvalues(status, current_floor, direction, door);
				Sleep(200);
				door = 1; //open door
				e2.update_status(status, current_floor, direction, door); //updating current status
				printvalues(status, current_floor, direction, door);
				//cout << "status= " << status << "current_floor= " << current_floor << "direction= " << direction << "door= " << door << "    " << endl;


				//protecting the array with mutex //clear floor request
				command1_mutex->Wait();
				floors_to_stop[current_floor] = 0;
				command1_mutex->Signal();

				//close the door and update the status
				Sleep(1000); //wait for door close
				door = 0;
				e2.update_status(status, current_floor, direction, door); //updating current status
				printvalues(status, current_floor, direction, door);
				//cout << "status= " << status << "current_floor= " << current_floor << "direction= " << direction << "door= " << door << "    " << endl;


				//DECIDE where to go next: change direction if no more pending requests in the same direction
				if (!pending_request(current_floor, direction)) {
					direction = get_direction(current_floor);
					e2.update_status(status, current_floor, direction, door); //updating current status
					printvalues(status, current_floor, direction, door);
					//cout << "status= " << status << "current_floor= " << current_floor << "direction= " << direction << "door= " << door << "    " << endl;

				}
			}

			//move the lift, change floors
			if (direction == 0 && current_floor >= 0 && current_floor <= 9) {
				//cout << "entered next loop" << endl;
				direction = get_direction(current_floor);
				//cout << "direction " << direction << endl;
				e2.update_status(status, current_floor, direction, door); //updating current status
				printvalues(status, current_floor, direction, door);
				//cout << "status updated222222 " << endl;
				//cout << "status= " << status << "current_floor= " << current_floor << "direction= " << direction << "door= " << door << "    " << endl;

			}

			if (direction == 1 && current_floor >= 0 && current_floor <= 9) {
				Sleep(1500);
				current_floor++;
				e2.update_status(status, current_floor, direction, door); //updating current status
				printvalues(status, current_floor, direction, door);
				//cout << "status= " << status << "current_floor= " << current_floor << "direction= " << direction << "door= " << door << "    " << endl;

			}

			if (direction == 2 && current_floor >= 0 && current_floor <= 9) {
				Sleep(1500);
				current_floor--;
				e2.update_status(status, current_floor, direction, door); //updating current status
				printvalues(status, current_floor, direction, door);
				//cout << "status= " << status << "current_floor= " << current_floor << "direction= " << direction << "door= " << door << "    " << endl;

			}

		}
		//cout << "direction from while loop " << direction << endl;
		//Sleep(500);
	}//while loop end
	return 0;
}

int main(void) {

	CThread   t1(move_elevator, ACTIVE, NULL);
	Sleep(200);
	s1.Wait();

	//
	//999 means fault +1,  111 means resolved -1 , 555 meanss termination go back to 0
	while (1) {
		if (MyMailBox.TestForMessage() > 0) {
			command = MyMailBox.GetMessage();
			//cout << "command e2 " << command << endl;
			if (command >= 100 && command <= 109) {
				command1_mutex->Wait();
				floors_to_stop[command % 100] = 1;
				command1_mutex->Signal();
			}
			else {
				local_command_mutex->Wait();
				local_command = command;
				local_command_mutex->Signal();
			}
		}
	}
	//
	t1.WaitForThread();
	return 0;
}
