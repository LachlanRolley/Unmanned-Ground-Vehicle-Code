
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

value struct UGVProcesses
{
	String^ ModuleName;
	int Critical;
	int CrashCount;
	int CrashCountLimit;
	Process^ ProcessName;
};

//C:\Users\z5162440\source\repos\UGV_Assignment\ProcessManagement\ProcessManagement.cpp
int main()
{

	
	
	//declaring the shared mem, not creating
	SMObject PMObj(TEXT("processManagement"), sizeof(ProcessManagement));   
	//creatng the shared mem
	PMObj.SMCreate();									//quick thing about shared mem, Start by making an object but make it the size of the thing u want in smStructs
	PMObj.SMAccess();
	ProcessManagement* PMData = (ProcessManagement*)PMObj.pData;	//then typecast the object into the correct type u want, like daughter class shit




	__int64 Counter, Frequency;
	double PMTimeStamp;
	
	array<UGVProcesses>^ ProcessList = gcnew array<UGVProcesses>
	{
		{"Laser", 1, 0, 10, gcnew Process},
		{ "Display", 1, 0, 10, gcnew Process },
		{ "VehicleControl", 1, 0, 10, gcnew Process },
		{ "GPS",		0, 0, 10, gcnew Process },
		{ "Camera",	0, 0, 10, gcnew Process },

	};

	QueryPerformanceFrequency((LARGE_INTEGER*)&Frequency);

	for (int i = 0; i < ProcessList->Length; i++) {
		if (Process::GetProcessesByName(ProcessList[i].ModuleName)->Length == 0) {
			ProcessList[i].ProcessName = gcnew Process;
			ProcessList[i].ProcessName->StartInfo->WorkingDirectory = "C:\\Users\\rolle\\source\\repos\\UGV_Assignment\\Executables";
			ProcessList[i].ProcessName->StartInfo->FileName = ProcessList[i].ModuleName;
			ProcessList[i].ProcessName->Start();
			Console::WriteLine("The Process" + ProcessList[i].ModuleName + ".exe has started");

		}
	}



	




	


	while (!_kbhit()) {


		QueryPerformanceCounter((LARGE_INTEGER*)&Counter);
		PMData->PMTimeStamp = PMTimeStamp = (double)Counter / (double)Frequency * 1000.0; // ms
		PMData->Ready = true;
		PMData->PMDownCount = 0;
		//check heartbeats
			//itterate through all process
			for(int i = 0; i < ProcessList->Length; i++){
				unsigned short checkmask = 1;      // we gona use this with an bitwise & to see if things are on
				unsigned short ProcBeat = PMData->Heartbeat.Status >> i;  // with this the rightmost bit will alway be the specific heartbeat of the process
				unsigned short check;
				check = ProcBeat & checkmask;
				//is the heartbeat of proccess[i] up ?
				if(check == 1){
					//true -> put the bit for process[i] down and reset CrashCounter
					//Console::WriteLine("The Process" + ProcessList[i].ModuleName + ".exe is ALIVE");
					unsigned short BeatdownMask = 1 << i;
					BeatdownMask = ~BeatdownMask;
					//at this point we got a mask full of 1s execpt has a 0 in the heartbeat of this process
					// just gota and it with heartbeat and it will only change this specific bit to 0;
					PMData->Heartbeat.Status = PMData->Heartbeat.Status & BeatdownMask;
					ProcessList[i].CrashCount = 0;
				}
				else {
					//false increment heartbeat loss counter
					//Console::WriteLine("The Process" + ProcessList[i].ModuleName + ".exe is DEAD");
					ProcessList[i].CrashCount++;
				}
				// is the counter passed the limit ?
				if (ProcessList[i].CrashCount > ProcessList[i].CrashCountLimit) {
					//true -> is it critical ?
					if (ProcessList[i].Critical == 1) {
						//true -> shutdown all
						PMData->Shutdown.Status = 0xFF;
						return 0;
					}
					//false -> has process[i] exited the operating system (HasExited())
					else {
						if (ProcessList[i].ProcessName->HasExited) {
							//true -> Start();
							ProcessList[i].ProcessName = gcnew Process;
							ProcessList[i].ProcessName->StartInfo->WorkingDirectory = "C:\\Users\\rolle\\source\\repos\\UGV_Assignment\\Executables";
							ProcessList[i].ProcessName->StartInfo->FileName = ProcessList[i].ModuleName;
							ProcessList[i].ProcessName->Start();
							Console::WriteLine("The Process" + ProcessList[i].ModuleName + ".exe has started");
							ProcessList[i].CrashCount = 0;

						}
						else {
							//false -> kill() 
							ProcessList[i].ProcessName->Kill();
							//start();
							ProcessList[i].ProcessName = gcnew Process;
							ProcessList[i].ProcessName->StartInfo->WorkingDirectory = "C:\\Users\\rolle\\source\\repos\\UGV_Assignment\\Executables";
							ProcessList[i].ProcessName->StartInfo->FileName = ProcessList[i].ModuleName;
							ProcessList[i].ProcessName->Start();
							Console::WriteLine("The Process" + ProcessList[i].ModuleName + ".exe has started");
							ProcessList[i].CrashCount = 0;

						}
					}
				}										
			}	


		Thread::Sleep(100);
	}

	PMData->Shutdown.Status = 0xFF;  // USE SHARE MEMORY TO EDIT SHUTDOWN STATUS
	

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

