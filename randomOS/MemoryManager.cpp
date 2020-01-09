#include "MemoryManager.h"

Memory::Memory()
{

}
Memory::Memory(std::shared_ptr<VirtualMemory> vm) : vm(vm)
{
	for (unsigned int i = 0; i < 128; i++)
	{
		this->ram[i] = int(0);
	}
}

uint8_t Memory::writeInMem(int pid, int logical, int8_t data)
{

	//used for throwing errors
	int8_t errorByte = -1;
	uint8_t errorCode = 0;

	//if process is not in swap file return error
	if (vm->swapFile.find(pid) == vm->swapFile.end())
	{
		return errorByte;
	}

	//if process has no PageTable it assigns new one to it
	bool flag = 0;//chcecks if process got frame table
	//inicjowanie wszystkiego

	this->assignPageTable(pid);

	//szukamy czy strona jest w ramie

	for (auto & process : PageTable)
	{
		if (process.pid == pid)
		{
			if (process.pageTable[logical / 16].first == -1)
			{
				//nie ma strony w ramie
				std::pair<uint8_t, Page> temporaryPage = vm->getPage(pid, (logical / 16));
				if (temporaryPage.first != 0)return temporaryPage.first;

				int victimFrame = vm->getVictimFrameNumber();

				if (FrameTable[victimFrame].dirtyflag == 1)
				{//jeśli ramka jest brudan
					Page page;
					for (int i = 0; i < 16; i++)
					{//ładowanie danych z ramu do temp page
						page.data[i] = this->ram[(victimFrame * 16) + i];
					}
					//upload strony do vm
					int pageNumber = FrameTable[victimFrame].page;
					errorCode = vm->updateSwapFilePage(this->FrameTable[victimFrame].pid, pageNumber, page);
					if (errorCode != 0) { return errorCode; }
				}
				else
				{//jeśli strona jest czysta

				}
				//aktualizacja page table starego właściciela ramki
				for (auto it = PageTable.begin(); it != PageTable.end(); it++)
				{
					bool endFlag = 0;
					for (unsigned int i = 0; i < it->pageTable.size(); i++)
					{
						if (it->pageTable[i].first == victimFrame)
						{
							it->pageTable[i].first = -1;
							it->pageTable[i].second = 0;
							endFlag = 1;
							break;
						}
					}
					if (endFlag)break;
				}			

				//pobranie szukanej strony z vm
				std::pair<uint8_t, Page> dane = vm->getPage(pid, (logical / 16));
				if (dane.first != 0) { return errorByte; }

				for (int i = 0; i < 16; i++)
				{
					this->ram[i + (16 * victimFrame)] = dane.second.data[i];
				}

				//aktualizacja pageTable
				process.pageTable[logical / 16].first = victimFrame;
				process.pageTable[logical / 16].second = 1;

				this->FrameTable[victimFrame].pid = process.pid;
				this->FrameTable[victimFrame].page = logical/16;



				errorCode = vm->updateQueue(victimFrame);
				if (errorCode != 0) { return { errorCode }; }

				unsigned int byteIndex = process.pageTable[logical / 16].first * 16 + (logical % 16);
				ram[byteIndex] = data;
				this->FrameTable[victimFrame].dirtyflag = 1;
				return 0;
			}
			else
			{
				//jest strona w ramie
				errorCode = vm->updateQueue(process.pageTable[(logical / 16)].first);
				if (errorCode != 0) {
					return errorCode;
				}
				ram[process.pageTable[(logical / 16)].first * 16 + (logical % 16)] = data;
				this->FrameTable[process.pageTable[(logical / 16)].first].dirtyflag = 1;
				return 0;
			}
		}
	}

	/*
	//if process is not in swap file return error
	if (vm->swapFile.find(pid) == vm->swapFile.end())
	{
		return 81;
	}
	//if process has no PageTable it assigns new one to it
	if (PageTable.find(pid) == PageTable.end())
	{
		this->PageTable.insert(std::make_pair(pid, this->ProcessPages));
	}
	//process has PageTable and is written in RAM
	if (this->PageTable.find(pid)->first >= 0 && this->PageTable.find(pid)->second[logical / 16].second == 1)
	{
		this->ram[(16 * this->PageTable.find(pid)->second[logical / 16].first) + (logical % 16)] = data;
		vm->updateQueue(PageTable.find(pid)->second[logical / 16].first);
		//update information about frame
		this->FrameTable[PageTable.find(pid)->second[logical / 16].first].pid = pid;
		this->FrameTable[PageTable.find(pid)->second[logical / 16].first].dirtyflag = 1;
		this->FrameTable[PageTable.find(pid)->second[logical / 16].first].page = logical / 16;
		//set byte of correctness
		this->PageTable.find(pid)->second[logical / 16].second = 1;
		return 0;
	}
	//process has PageTable but is not written in RAM
	if (this->PageTable.find(pid)->first >= 0 && this->PageTable.find(pid)->second[logical / 16].second == 0)
	{
		int frame = vm->getVictimFrameNumber();
		//if Page in this Frame has changed it is uploaded back to swapFile
		if (this->FrameTable[frame].dirtyflag == 1)
		{
			Page page;
			for (int i = 0; i < 16; i++)
			{
				page.data[i] = this->ram[(frame * 16) + i];
			}
			vm->updateSwapFilePage(FrameTable[frame].pid, logical / 16, page);
		}
		//Page that we want to change is being downloaded from swapFile
		std::pair<uint8_t, Page> dane = vm->getPage(pid, logical / 16);
		for (int i = 0; i < 16; i++)
		{
			this->ram[i + (16 * frame)] = dane.second.data[i];
		}
		if (this->FrameTable[frame].pid != pid || (this->FrameTable[frame].pid == pid && this->FrameTable[frame].page != logical / 16))
		{
			if (PageTable.find(FrameTable[frame].pid) != PageTable.end())
			{
				this->PageTable.find(FrameTable[frame].pid)->second[frame].first = -1;
				this->PageTable.find(FrameTable[frame].pid)->second[frame].second = 0;
			}

		}
		//the byte that we wanted to write is being written at the logical adress
		this->ram[(logical % 16) + (16 * frame)] = data;
		//update PageTable
		this->PageTable.find(pid)->second[logical / 16].first = frame;
		this->PageTable.find(pid)->second[logical / 16].second = 1;
		vm->updateQueue(frame);
		//update information about frame
		this->FrameTable[frame].pid = pid;
		this->FrameTable[frame].dirtyflag = 1;
		this->FrameTable[frame].page = logical / 16;
		return 0;
	}
	*/
	return 0;

}

void Memory::deleteFromMem(int frame)
{
	for (int index = 16 * frame; index < frame + 16; index++)
	{
		this->ram[index] = 0;

	}
	this->FrameTable[frame].dirtyflag = false;

}

void Memory::printMemory()
{
	for (int i = 0; i < 128; i++)
	{
		if (i % 16 == 0)std::cout << "frame " << i / 16 << std::endl;
		std::cout << int(this->ram[i]);
		if (i % 16 == 15) std::cout << std::endl;
	}
}

std::pair<uint8_t, int8_t&> Memory::getMemoryContent(int pid, int logical)
{
	//used for throwing errors
	int8_t errorByte = -1;
	uint8_t errorCode = 0;

	//if process is not in swap file return error
	if (vm->swapFile.find(pid) == vm->swapFile.end())
	{
		return { 81, errorByte };
	}

	this->assignPageTable(pid);

	//--------------------------------------------------
	//szukamy czy strona jest w ramie
	for (auto & process : PageTable)
	{
		if (process.pid == pid)
		{
			if (process.pageTable[logical / 16].first == -1)
			{
				//nie ma strony w ramie
				std::pair<uint8_t, Page> temporaryPage = vm->getPage(pid, (logical / 16));
				int8_t a;
				if (temporaryPage.first != 0)return { temporaryPage.first,a };

				int victimFrame = vm->getVictimFrameNumber();

				if (FrameTable[victimFrame].dirtyflag == 1)
				{//jeśli ramka jest brudan
					Page page;
					for (int i = 0; i < 16; i++)
					{//ładowanie danych z ramu do temp page
						page.data[i] = this->ram[(victimFrame * 16) + i];
					}
					//upload strony do vm
					int pageNumber = FrameTable[victimFrame].page;
					errorCode = vm->updateSwapFilePage(this->FrameTable[victimFrame].pid, pageNumber, page);
					if (errorCode != 0) { return { errorCode,errorByte }; }
				}
				else
				{//jeśli strona jest czysta
					
				}
				//aktualizacja page table starego właściciela ramki
				for (auto it = PageTable.begin(); it != PageTable.end(); it++)
				{
					bool endFlag = 0;
					for (unsigned int i = 0; i < it->pageTable.size(); i++)
					{
						if (it->pageTable[i].first == victimFrame)
						{
							it->pageTable[i].first = -1;
							it->pageTable[i].second = 0;
							endFlag = 1;
							break;
						}
					}
					if (endFlag)break;
				}


				//pobranie szukanej strony z vm
				std::pair<uint8_t, Page> dane = vm->getPage(pid, (logical / 16));
				if (dane.first != 0) { return { dane.first,errorByte }; }

				for (int i = 0; i < 16; i++)
				{
					this->ram[i + (16 * victimFrame)] = dane.second.data[i];
				}

				//aktualizacja pageTable
				process.pageTable[logical / 16].first = victimFrame;
				process.pageTable[logical / 16].second = 1;

				this->FrameTable[victimFrame].pid = process.pid;
				this->FrameTable[victimFrame].page = logical / 16;


				errorCode = vm->updateQueue(victimFrame);
				if (errorCode != 0) { return { errorCode,errorByte }; }

				unsigned int byteIndex = process.pageTable[logical / 16].first * 16 + (logical % 16);
				int8_t& searchedByte = ram[byteIndex];
				return { 0,searchedByte };
			}
			else
			{
				//jest strona w ramie
				errorCode = vm->updateQueue(process.pageTable[(logical / 16)].first);
				if (errorCode != 0) {
					return { errorCode,errorByte };
				}
				int8_t& searchedByte = ram[process.pageTable[(logical / 16)].first * 16 + (logical % 16)];
				return { 0,searchedByte };
				break;
			}
		}
	}
}

void Memory::assignPageTable(unsigned int pid)
{
	//if process has no PageTable it assigns new one to it
	bool flag = 0;//chcecks if process got frame table

	//inicjowanie wszystkiego
	for (auto& process : PageTable)
	{
		if (process.pid == pid)
		{
			flag = 1;
			break;
		}
	}
	if (!flag)//process dont have PageTable
	{
		tempFrame temp;
		temp.pid = pid;
		PageTable.push_back(temp);//add page table
	}

	//add blank table
	for (auto& process : PageTable)
	{
		if (process.pid == pid)
		{
			if (process.pageTable.size() == 0)
			{
				//inicjujemy page table
				for (unsigned int i = 0; i < 8; i++)
				{
					process.pageTable.push_back({ -1,0 });
				}
			}
			break;
		}
	}
}

std::vector<std::pair<int, bool>> Memory::printPageTable(int pid, uint8_t& errorCode)
{
	for (auto & process : PageTable)
	{
		if (process.pid == pid)
		{
			errorCode = 0;
			return process.pageTable;
		}
	}
	errorCode = 255;
	return std::vector<std::pair<int, bool>>();
}