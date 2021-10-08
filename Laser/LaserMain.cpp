#using <System.dll>
#include <Windows.h>
#include <tchar.h>
#include <TlHelp32.h>
#include <stdio.h>
#include <iostream>
#include <conio.h>

#include <Windows.h>
#include "SMStructs.h"
#include "SMObject.h"

using namespace System;
using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::Text;
using namespace System::Diagnostics;
using namespace System::Threading;




int main(void) {
	//declarations
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SMObject PMObj(TEXT("processManagement"), sizeof(ProcessManagement));    

	//creatng the shared mem
	
	PMObj.SMAccess();

	ProcessManagement* PMData = (ProcessManagement*)PMObj.pData;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////



	double TimeStamp;
	__int64 Frequency, Counter;
	int Shutdown = 0x00;
	ConsoleKeyInfo keyHit;
	

	QueryPerformanceFrequency((LARGE_INTEGER*)&Frequency);  // guessing this asks windows the frequncy of ure pc clock speed then chucks in freq
	



	while (1) {
		//printf("im alive !\n");
		QueryPerformanceCounter((LARGE_INTEGER*)&Counter);
		TimeStamp = (double)Counter / (double)Frequency * 1000; //ms
		Console::WriteLine("Laser time stamp : {1,12:F3} {1:12:X2}", TimeStamp, Shutdown);
		Sleep(25);
		if (PMData->Shutdown.Status) {
			break;
		}
		if (_kbhit()) { // doesnt actually check what key hit hit, need to getchar it
			
			//char c = getchar();                       //this works but not what u want cos u need to press enter after key press
			//printf("peace brother, you pressed %c", c);
			break;
		}
		
	}
	keyHit = Console::ReadKey();

}