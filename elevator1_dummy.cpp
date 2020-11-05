#include "C:\RTExamples\RTExamples\rt.h"
#include "../Monitor.h"
#include <iostream>
using namespace std;

CRendezvous     s1("Start", 4);
Monitor e1("elevator_1");
monitorData e1_data;
int floors_to_stop[10] = { 0 };

UINT command;
UINT local_command=0;
CMailbox   MyMailBox;
CMutex* command1_mutex = new CMutex("command1_mutex");
CMutex* local_command_mutex = new CMutex("local_command_mutex");
//update_status(int status, int floor, int direction, int door)

UINT __stdcall Get_commands(void* ThreadArgs) {
	//999 means fault +1,  111 means resolved , 555 meanss termination go back to 0---pending
	while (1) {
		command = MyMailBox.GetMessage();
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
	return 0;
}
//return true if elevator has to move in the direction of current input direction
int pending_floor(int current_floor, int direction)
{
	int dummy;
	if (direction == 1) {
		for (int a = current_floor; a++; a < 9) {

			command1_mutex->Wait();
			dummy = floors_to_stop[a];
			command1_mutex->Signal();

			if (dummy) {//protect it using mutex
				return 1;
			}
		}
	}
	else if (direction == 2) {
		for (int a = current_floor; a--; a > 0) {
			command1_mutex->Wait();
			dummy = floors_to_stop[a];
			command1_mutex->Signal();
			if (dummy) {//protect it using mutex
				return 1;
			}
		}
	}

	return 0;

}

int getting_direction(int current_floor) {

	//deciding the direction if current direction is zero i.e. elevator is stationary
	if (pending_floor(current_floor, 1)) { //this will only return true if elevator has UP request
		//// pending floor will return true if elevator has to move in the direction of current input direction which is one here
		return 1;
	}
	else {
		if (pending_floor(current_floor, 2)) { //this will return true if elevator has a down request
			return 2;
		}
		else {
			return 0;
		}
	}

}
int main(void) {
	s1.Wait();
	int current_floor = 0;
	int status = 1;
	int direction = 0;
	int door = 1;
	int dummy;
	int dummy_command;
	CThread   t1(Get_commands, ACTIVE, NULL);
	//just keep moving in the same direction as long as there are any requests in that direction
	while (1) {//999 means fault +1,  111 means resolved , 555 meanss termination go back to 0---pending
		local_command_mutex->Wait();
		dummy_command = local_command;
		local_command_mutex->Signal();
		if (dummy_command == 555) {
			while(current_floor>0) {
				Sleep(1000);
				current_floor--;
				e1.update_status(status, current_floor, direction, door); //updating current status
			}
			if (current_floor == 0) {
				Sleep(500); //wait
				door = 1;
				e1.update_status(status, current_floor, direction, door); //updating current status
			}
		}
		else {
			local_command_mutex->Wait();
			dummy_command = local_command;
			local_command_mutex->Signal();
			if (dummy_command == 999) {
				status = 0;
				e1.update_status(status, current_floor, direction, door); //updating current status
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
					e1.update_status(status, current_floor, direction, door); //updating current status
				}
			}
		}




		//main code
		local_command_mutex->Wait();
		dummy_command = local_command;
		local_command_mutex->Signal();

		if(status==1&& dummy_command!=555){
		command1_mutex->Wait();
		dummy = floors_to_stop[current_floor];
		command1_mutex->Signal();
		if (dummy) { //stopping at requested stop
			//stop and open the door, wait for it to close 
			e1.update_status(status, current_floor, direction, door); //updating current status
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
			e1.update_status(status, current_floor, direction, door); //updating current status

			if (!pending_floor(current_floor, direction)) {
				direction = getting_direction(current_floor);
				e1.update_status(status, current_floor, direction, door); //updating current status
			}
			
		}

		else { //moving in the directed direction
			if (direction == 1) {
				Sleep(1000);
				current_floor++;
				e1.update_status(status, current_floor, direction, door); //updating current status
			}
			else if (direction == 2) {
				Sleep(1000);
				current_floor--;
				e1.update_status(status, current_floor, direction, door); //updating current status
			}
			else {

				direction = getting_direction(current_floor);
				e1.update_status(status, current_floor, direction, door); //updating current status

			}
		}



		}//while loop

		t1.WaitForThread();

		return 0;
	}
