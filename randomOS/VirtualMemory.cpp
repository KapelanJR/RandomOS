#include "VirtualMemory.h"

VirtualMemory::Page::Page() = default;

VirtualMemory::Page::Page(const int8_t data[]) {
	for (int i = 0; i < 16; i++)
		this->data[i] = data[i];
}

void VirtualMemory::Page::print() const {
	for (auto &x : data) {
		if (x == 0) std::cout << "_";
		else std::cout << std::to_string(x);
	}
	std::cout << std::endl;
}

void VirtualMemory::insertProgram(std::pair<int, std::vector<Page>> program) {
	swapFile.insert(program);
}

void VirtualMemory::updateQueue(int frameNumber) {
	for (auto &frame : queue)
		if (frame.first == frameNumber) frame.second = true;
}

void VirtualMemory::updateSwapFilePage(int pid, int pageNumber, VirtualMemory::Page page) {
	for (auto &program : swapFile) {
		if (program.first == pid)
			for (int i = 0; i < sizeof(page.data); i++)
				program.second[pageNumber].data[i] = page.data[i];
	}
}

int VirtualMemory::getVictimFrameNumber() {
	std::pair<int, bool> temp;
	while (queue.front().second) {
		temp = queue.front();
		queue.erase(queue.begin());
		temp.second = false;
		queue.push_back(temp);
	}
	temp = queue.front();
	queue.erase(queue.begin());
	temp.second = true;
	queue.push_back(temp);
	return temp.first;
}

VirtualMemory::Page VirtualMemory::getPage(int pid, int pageNumber) {
	return swapFile.find(pid)->second[pageNumber];
}

void VirtualMemory::printSwapFile() {
	std::cout << "---------- SWAP FILE ----------" << std::endl;
	for (auto &program : swapFile) {
		std::cout << "PID: " << program.first << std::endl;
		for (auto &page : program.second)
			page.print();
		//std::cout << std::endl;
	}
}

void VirtualMemory::printQueue() {
	std::cout << "---------- QUEUE ----------" << std::endl;
	for (auto &frame : queue)
		std::cout << "{" << frame.first << ", " << frame.second << "} ";
	std::cout << std::endl;
}

void VirtualMemory::test() {
	std::cout
		<< "1. [INSERTING PROGRAMS]\nCreating 2 programs of 3 pages and inserting them into swapFile with pid = 1 and pid = 2."
		<< std::endl;
	std::cout << std::endl;

	int8_t data[16] = { 0 };
	std::vector<Page> pages;
	for (int8_t i = 0; i < 3; i++) {
		for (int8_t &j : data) {
			j = i;
		}
		pages.emplace_back(Page(data));
	}
	insertProgram(std::make_pair(1, pages));
	insertProgram(std::make_pair(2, pages));
	printSwapFile();
	std::cout << std::endl << std::endl;


	std::cout << "2. [GETTING VICTIM FRAMES, SWAPPING PAGES & UPDATING QUEUE]\nGetting victim frames." << std::endl;
	std::cout << "RAM is full, have to swap page in 1st frame" << std::endl;
	std::cout << std::endl;

	printQueue();
	std::cout << std::endl;
	std::cout << "Victim frame (got using getVictimFrameNumber()): " << getVictimFrameNumber() << std::endl;
	std::cout << std::endl;
	printQueue();
	std::cout << std::endl;

	std::cout
		<< "Imaging that during processes execution a page in 1st frame has changed to all 5 and it's the 2nd page of process with pid = 2"
		<< std::endl;
	Page *changedPage = new Page();
	for (int8_t &ch : changedPage->data) {
		ch = 5;
	}
	std::cout << "That page: ";
	changedPage->print();
	std::cout << std::endl;

	std::cout << "So we first get the desired page, e.g. 3rd page of process with pid = 1, and put it into RAM"
		<< std::endl;
	std::cout << "That page (got using getPage()): ";
	getPage(1, 2).print();
	std::cout << std::endl;

	std::cout << "Now we have to update the page that was taken down in SwapFile" << std::endl;
	updateSwapFilePage(2, 1, *changedPage);
	std::cout << "After update" << std::endl;
	printSwapFile();
	std::cout << std::endl;

	std::cout << "Referencing page in frame number 3" << std::endl;
	std::cout << "Queue after reference" << std::endl;
	updateQueue(3);
	printQueue();
}