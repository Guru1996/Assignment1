#include "Z:\RTExamples\rt.h"
#include "../Monitor.h"
#include <iostream>
using namespace std;

CRendezvous s1("Start", 4);
Monitor e1("elevator_1");
monitorData e1_data;
int floors_to_stop[10] = { 0 };

UINT command = 0;
UINT local_command = 0;
CMailbox MyMailBox;
CMutex* command1_mutex = new CMutex("command1_mutex");
CMutex* local_command_mutex = new CMutex("local_command1_mutex");
//update_status(int status, int floor, int direction, int door)


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
		if(current_floor==0){
			return 0;
		}
		else{
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

	e1.update_status(status, current_floor, direction, door);// sending intital variables
	cout << "status= " << status << "current_floor= " << current_floor << "direction= " << direction << "door= " << door << "    " << endl;
	//cout << "Elevator 1 started" << endl;

	//keep moving in the same direction as long as there are any requests in that direction
	int counter = 0;
	while (1) {//999 means fault +1,  111 means resolved , 555 meanss termination go back to 0
		cout << counter << endl;
		counter++;
		local_command_mutex->Wait();
		dummy_command = local_command;
		local_command_mutex->Signal();
		if (dummy_command == 555) {
			while (current_floor > 0) {
				Sleep(1000);
				current_floor--;
				e1.update_status(status, current_floor, direction, door); //updating current status
				cout << "status= " << status << "current_floor= " << current_floor << "direction= " << direction << "door= " << door << "    " << endl;

			}
			//open the door at 0 floor
			if (current_floor == 0) {
				Sleep(200); //wait for door to open
				door = 1;
				direction = 0;
				e1.update_status(status, current_floor, direction, door); //updating current status
				cout << "status= " << status << "current_floor= " << current_floor << "direction= " << direction << "door= " << door << "    " << endl;

			}

		}

		else {
			local_command_mutex->Wait();
			dummy_command = local_command;
			local_command_mutex->Signal();
			if (dummy_command == 999) {
				status = 0;
				e1.update_status(status, current_floor, direction, door); //updating current status
				cout << "status= " << status << "current_floor= " << current_floor << "direction= " << direction << "door= " << door << "    " << endl;
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
					e1.update_status(status, current_floor, direction, door); //updating current status
					cout << "status= " << status << "current_floor= " << current_floor << "direction= " << direction << "door= " << door << "    " << endl;

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
			cout << "entered loop" << endl;
			command1_mutex->Signal();
			//stopping at requested stop //stop and open the door, wait for it to close 
			if (dummy) {
				Sleep(200);
				door = 1; //open door
				e1.update_status(status, current_floor, direction, door); //updating current status
				cout << "status= " << status << "current_floor= " << current_floor << "direction= " << direction << "door= " << door << "    " << endl;


				//protecting the array with mutex //clear floor request
				command1_mutex->Wait();
				floors_to_stop[current_floor] = 0;
				command1_mutex->Signal();

				//close the door and update the status
				Sleep(1000); //wait for door close
				door = 0;
				e1.update_status(status, current_floor, direction, door); //updating current status
				cout << "status= " << status << "current_floor= " << current_floor << "direction= " << direction << "door= " << door << "    " << endl;


				//DECIDE where to go next: change direction if no more pending requests in the same direction
				if (!pending_request(current_floor, direction)) {
					direction = get_direction(current_floor);
					e1.update_status(status, current_floor, direction, door); //updating current status
					cout << "status= " << status << "current_floor= " << current_floor << "direction= " << direction << "door= " << door << "    " << endl;

				}
			}

			//move the lift, change floors
			if (direction == 0) {
				cout << "entered next loop" << endl;
				direction = get_direction(current_floor);
				cout << "direction " << direction << endl;
				e1.update_status(status, current_floor, direction, door); //updating current status
				cout << "status= " << status << "current_floor= " << current_floor << "direction= " << direction << "door= " << door << "    " << endl;

			}

			if (direction == 1) {
				Sleep(1500);
				current_floor++;
				e1.update_status(status, current_floor, direction, door); //updating current status
				cout << "status= " << status << "current_floor= " << current_floor << "direction= " << direction << "door= " << door << "    " << endl;

			}

			if (direction == 2) {
				Sleep(1500);
				current_floor--;
				e1.update_status(status, current_floor, direction, door); //updating current status
				cout << "status= " << status << "current_floor= " << current_floor << "direction= " << direction << "door= " << door << "    " << endl;

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
			cout << "command e1 " << command << endl;
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