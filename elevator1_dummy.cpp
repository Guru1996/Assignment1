#include "C:\RTExamples\RTExamples\rt.h"
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
//return true if elevator has to move in the direction of current input direction
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
	s1.Wait();
	int current_floor = 0;
	int status = 1;
	int direction = 0;
	int door = 1;
	int tracker = 0;
	int dirr;
	//just keep moving in the same direction as long as there are any requests in that direction
	while (1) { //999 means fault +1,  111 means resolved , 555 meanss termination go back to 0---pending

		
		if (floors_to_stop[current_floor]) { //stopping at requested stop
			//stop and open the door, wait for it to close 
			direction = 0; // elevator is stationary
			e1.update_status(status, current_floor, direction, door); //updating current status
			Sleep(200);
			door = 1; //open door
			//protecting the array with mutex
			command1_mutex->Wait();
			floors_to_stop[current_floor] = 0; //clear request
			command1_mutex->Signal();

			e1.update_status(status, current_floor, direction, door); //updating current status

			Sleep(1000); //wait

			//////////////////////////////////////////////////////////////////////////////////////////////
			//closing door this request will go where we have to decide which direction it's going to move 
			door = 2;
			//direction = 0; //
			//..
			
		}

		else { //moving in the directed direction
			if (direction = 1) {
				Sleep(1000);
				current_floor++;
			}
			else if (direction = 2) {
				Sleep(1000);
				current_floor--;
			}
			else {

				//deciding the direction if current direction is zero i.e. elevator is stationary
				if (pending_floor(current_floor, 1)) { //this will only return true if elevator has UP request
					//// pending floor will return true if elevator has to move in the direction of current input direction which is one here
					direction = 1;
					Sleep(1000);
					current_floor++;
				}

				/*else if (pending_floor(current_floor, )) {
					direction = 2;
				}*/
				else {
					if (pending_floor(current_floor, 2)) { //this will return true if elevator has a down request
						direction = 2;
						Sleep(1000);
						current_floor--;
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
			
		}



		return 0;
	}
