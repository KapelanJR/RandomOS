#pragma once
#include "RUNNING.h"
#include <memory>

class Semaphore{
private:
	int value;
	std::vector<std::shared_ptr<PCB>> list;
public:
	Semaphore(int new_value);
	Semaphore();
	bool wait(); //returns 1 if a semaphore is down, 0 if it's not
	int signal();
	void deleteFromList(int PID);
	const int& getValue();
	const std::vector<std::shared_ptr<PCB>>& getList();
	bool firstElement(unsigned int PID);
};