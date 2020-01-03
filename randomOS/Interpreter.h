#pragma once
#include "Includes.h"
#include "FileMenager.h"
#include "MemoryManager.h"
#include "ProcessManager.h"
#include "PCB.h"
#include "RUNNING.h"

class Interpreter
{
private:

	std::shared_ptr<Memory> memory;
	std::shared_ptr<FileMenager> fileSystem;
	std::shared_ptr<ProcessManager> processManager;

	std::shared_ptr<PCB> PCB;
	int8_t AX, BX, CX, DX;
	uint8_t PC;
	unsigned int PID;
	int8_t code;
	bool changeToTerminated;
	std::vector<int8_t> instructionHex;
	std::string instructionString;

	const int8_t maxValue = 127;
	const int8_t minValue = -128;

	void loadPCB();
	void loadCode();
	void interpret();
	int8_t& loadArgAdrOrReg();
	int8_t loadArgNum();
	std::string loadArgText(int n);
	void returnToPCB();

	void RET(); //Koniec                                    (0   | 0x00)
	void MOV(); //Przenie�                                  (1   | 0x01)
	void WRI(); //Wpisz                                     (2   | 0x02)
	void ADD(); //Dodaj                                     (3   | 0x03)
	void SUB(); //Odejmij                                   (4   | 0x04)
	void MUL(); //Pomn�                                    (5   | 0x05)
	void DIV(); //Podziel                                   (6   | 0x06)
	void MOD(); //Reszta z dzielenia                        (7   | 0x07)
	void INC(); //Dodaj 1                                   (8   | 0x08)
	void DEC(); //Odejmnij 1                                (9   | 0x09)
	void JUM(); //Skok bezwarunkowy                         (10  | 0x0A)
	void IFS(); //Skok gdy <0                               (11  | 0x0B)
	void IFE(); //Skok gdy ==0                              (12  | 0x0C)
	void IFB(); //Skok gdy >0                               (13  | 0x0D)
	void IFN(); //Skok gdy !=0                              (14  | 0x0E)
	void CFI(); //Stw�rz plik                               (15  | 0x0F)
	void DFI(); //Usu� plik                                 (16  | 0x10)
	void OFI(); //Otw�rz plik                               (17  | 0x11)
	void SFI(); //Zamknij plik                              (18  | 0x12)
	void EFI(); //Wpisz do pliku (na koniec)                (19  | 0x13)
	void WFI(); //Wpisz do pliku (liczba)                   (20  | 0x14)
	void PFI(); //Wpisz do pliku (adres lub rejestr)        (21  | 0x15)
	void RFI(); //Odczytaj z pliku (liczba)                 (22  | 0x16)
	void AFI(); //Odczytaj z pliku (adres lub rejestr)      (23  | 0x17)
	void LFI(); //Odczytaj d�ugo�� pliku					(24  | 0x18)
	void CPR(); //Stw�rz proces                             (25  | 0x19)

	void NOP(); //Nic nie r�b                               (255 | 0xFF)

public:

	Interpreter(std::shared_ptr<Memory> memory, std::shared_ptr<FileMenager> filesystem, std::shared_ptr<ProcessManager> processManager);
	uint8_t go();
	std::vector<uint8_t> convertToMachine(std::string m);





};

