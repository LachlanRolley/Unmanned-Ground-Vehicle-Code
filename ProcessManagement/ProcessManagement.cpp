
#using <System.dll>
#include <Windows.h>
#include <tchar.h>
#include <TlHelp32.h>
#include <stdio.h>
#include <iostream>
#include <conio.h>

#include "SMStructs.h"
#include "SMObject.h"

using namespace System;
using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::Text;
using namespace System::Diagnostics;
using namespace System::Threading;

#define NUM_UNITS 5 // yo this number should be how many processes we have made, if only gps, set to 1
void keepNoncritRunning();
bool IsProcessRunning(const char* processName);
void StartProcesses();

//defining start up sequence
TCHAR Units[10][20] = //
{
	TEXT("GPS.exe"),
	TEXT("Camera.exe"),
	TEXT("Display.exe"),

	TEXT("LASER.exe"),
	TEXT("VehicleControl.exe")
};


int main()
{

	

	

	//start all 5 modules
	

	//practicing Shared memory
	
	//declaring the shared mem, not creating
	SMObject PMObj(TEXT("processManagement"), sizeof(ProcessManagement));   
	
	//creatng the shared mem
	PMObj.SMCreate();									//quick thing about shared mem, Start by making an object but make it the size of the thing u want in smStructs
	PMObj.SMAccess();

	ProcessManagement* PMData = (ProcessManagement*)PMObj.pData;	//then typecast the object into the correct type u want, like daughter class shit




	StartProcesses();  // yo mby change this to mimic lecture 2 so we dont have to clean up all the threads


	


	while (!_kbhit()) {
		// StartProcesses();  // this will reopen anything that is closed
		//keepNoncritRunning();
		Thread::Sleep(1000);
	}

	PMData->Shutdown.Status = 0xFF;                          // USE SHARE MEMORY TO EDIT SHUTDOWN STATUS

	return 0;
}

void keepNoncritRunning() {        // this just checks if somthing is running         use (const char*)Units[1]   and we can cycle through the array to reopoen stuff
	bool i = IsProcessRunning((const char*)Units[1]);
	if (i) {
		printf("cams running\n");
	}
	else{
		printf("cams dead\n");
	}
}


//Is process running function
bool IsProcessRunning(const char* processName)
{
	bool exists = false;
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry))
		while (Process32Next(snapshot, &entry))
			if (!_stricmp((const char*)entry.szExeFile, processName))
				exists = true;

	CloseHandle(snapshot);
	return exists;
}


void StartProcesses()
{
	STARTUPINFO s[10];
	PROCESS_INFORMATION p[10];

	for (int i = 0; i < NUM_UNITS; i++)
	{
		if (!IsProcessRunning((const char*)Units[i]))
		{
			ZeroMemory(&s[i], sizeof(s[i]));
			s[i].cb = sizeof(s[i]);
			ZeroMemory(&p[i], sizeof(p[i]));

			if (!CreateProcess(NULL, Units[i], NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &s[i], &p[i]))
			{
				printf("%s failed (%d).\n", Units[i], GetLastError());
				//_getch();
			}
			std::cout << "Started: " << Units[i] << std::endl;
			Sleep(100);
		}
	}
}

