// Memory.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "MemoryManager.h"


Memory::Memory(int size)
{
	this->ram[size];
	this->FrameTable[8];
}

int Memory::LogicaltoPhysical(int logical)
{
	int physical;
	if (logical % 8 == 0)
	{
		return physical = 0;
	}
	if (logical % 8 == 1)
	{
		return physical = 16;
	}
	if (logical % 8 == 2)
	{
		return physical = 32;
	}
	if (logical % 8 == 3)
	{
		return physical = 48;
	}
	if (logical % 8 == 4)
	{
		return physical = 64;
	}
	if (logical % 8 == 5)
	{
		return physical = 80;
	}
	if (logical % 8 == 6)
	{
		return physical = 96;
	}
	if (logical % 8 == 7)
	{
		return physical = 112;
	}
	else return 129;//nie mam wiecej niz 8 ramek
}



void Memory::writeInMem(int logical, std::string data)
{
	if (data.length() > 16)
	{
		std::cout << "to long data for a frame " << std::endl;
		return;
	}
	if (this->FrameTable[LogicaltoPhysical(logical)].taken == false)
	{
		int j = 0;
		for (int index = LogicaltoPhysical(logical); index < LogicaltoPhysical(logical) + 16; index++)
		{
			this->ram[index] = data[j];
			j++;
		}
		this->FrameTable[LogicaltoPhysical(logical)].taken = true;
		this->FrameTable[LogicaltoPhysical(logical)].lastUse++;

	}
	else
	{
		std::cout << "this frame is taken " << std::endl;
	}
	
};

void Memory::deleteFromMem(int logical)
{
	for (int index = LogicaltoPhysical(logical); index < LogicaltoPhysical(logical) + 16; index++)
	{
		this->ram[index] = 0;
		
	}
	this->FrameTable[LogicaltoPhysical(logical)].taken == false;
	this->FrameTable[LogicaltoPhysical(logical)].lastUse = 0;
}

void Memory::printMemory()
{
	for (int i = 0; i < 128; i++)
	{
		std::cout << this->ram[i];
		if (i % 16 == 15) std::cout << std::endl;
	}
}

char& Memory::getMemoryContent(int logical, int PC)
{
	return ram[LogicaltoPhysical(logical)+PC];
}





/*int main()
{
	Memory mem(128);
	mem.writeInMem(0, "dupaikamienikupa");
	mem.printMemory();
	std::cout << mem.getMemoryContent(0, 2) << std::endl;
	mem.deleteFromMem(0);
	mem.printMemory();
	std::cout << mem.FrameTable[0].taken << std::endl;
	std::cout << mem.FrameTable[1].taken << std::endl;
	

    std::cout << "Hello World!\n"; 
}*/

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
