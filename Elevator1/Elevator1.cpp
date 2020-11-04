#include "Z:\RTExamples\rt.h"
#include "../Monitor.h"
#include <iostream>
using namespace std;
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

int pending_floor(int current_floor, int direction)
{
	if (direction == 1) {
		for (int a = current_floor; a++; a < 9) {
			if (floors_to_stop[a]) {
				return 1;
			}
		}
	}
	else if (direction == 2) {
		for (int a = current_floor; a--; a > 0) {
			if (floors_to_stop[a]) {
				return 1;
			}
		}
	}

	return 0;

}

int main(void) {
	int current_floor = 0;
	int status = 1;
	int direction = 0;
	int door = 1;
	int tracker = 0;
	int dirr;
	//just keep moving in the same direction as long as there are any requests in that direction
	while (1) { //999 means fault +1,  111 means resolved , 555 meanss termination go back to 0---pending

		command1_mutex->Wait();
		if (floors_to_stop[current_floor]) {
			//stop and open the door, wait for it to close 
			floors_to_stop[current_floor] = 0; //clear request
			//direction = 0; //
			//...
		}

		else {
			if (direction = 1) {
				Sleep(1000);
				current_floor++;
			}
			else if (direction = 2) {
				Sleep(1000);
				current_floor--;
			}
			else {
				
				//decide where to go?
				if (pending_floor(current_floor, 1)) {
					direction = 1;
					//continue lift in that direction 
				}

				/*else if (pending_floor(current_floor, )) {
					direction = 2;
				}*/
				else {
					if (direction == 1) {
						dirr = 2;
					}
					else {
						dirr = 1;
					}

					if (pending_floor(current_floor, dirr)) {
						direction = dirr;
					}

					else {
						direction = 0;
					}
			}
			//continue -- sleep + counter ++ , -- 
		}

		/*
		if (direction == 1)
		{

			if (floors_to_stop[current_floor]) {
				//stop and open the door, wait for it to close 
				//...clear the floor from the array
				//...

				//check if any pending floors in the same direction
				if (pending_floor(current_floor, 1)) {
					direction = 1;
					Sleep(2000);


					
				}
				else if (pending_floor(current_floor, 2)) {
					direction = 2;
				}
				else {
					direction = 0;
				}
			}

		}
		
		//direction down
		else if (direction==2)
		{

		}

		//direction stationary
		else if (direction == 0) {

		}
		*/
		command1_mutex->Signal();
	}



	return 0;
}