#pragma once
#include "Includes.h"
#include "Page.h"
#include "VirtualMemory.h"

class Memory
{
	struct Frame
	{
		int pid=-1;
		int page;
		bool dirtyflag = 0;
	};

	//pamiec wlasciwa
	int8_t ram[128];
	// informacja czy ramka jest zajeta i jaka strona jest w niej wpisana
	Frame FrameTable[8];

	std::shared_ptr<VirtualMemory> vm;
public:
	Memory();
	Memory(std::shared_ptr<VirtualMemory>);

	//Indicates if frame is taken by process
	struct tempFrame
	{
		unsigned int pid;
		std::vector<std::pair<int, bool>> pageTable;
	};

	//mapa strona_procesu, ramka, bit_obecno�ci
	std::vector<tempFrame> PageTable;
	//std::map<int, std::vector<std::pair<int, bool>>> PageTable;
	std::vector<std::pair<int,bool>> printPageTable(int, uint8_t& errorCode);



	//wpisuje bajt do ramki, czeka na zmiany jak bede dostawal page do wpisania
	uint8_t writeInMem(int pid, int logical, int8_t data);
	void printMemory();
	//przeglada wszystkie ramki, jesli znajdzie to zwraca jej adres.
	//jesli nie ma jej w RAMie to
	//    probuje sprowadzic ja z pliku wymiany
	//     jesli jest to ja laduje, ale jesli jej nie ma rzucam wyjatek o zlym adresie logicznym
	std::pair<uint8_t, int8_t&> getMemoryContent(int pid, int logical);

	//assigns page table to PID if not present, else does nothing
	void assignPageTable(unsigned int pid);

	//free frames taken by designed pid
	void removeProgram(int PID);

	friend class Shell;
};