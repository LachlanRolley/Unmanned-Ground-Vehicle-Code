#using <System.dll>
#include "GPS.h"
#include "SMStructs.h"
#include "SMObject.h"
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


int main() {
	

	SMObject PMObj(TEXT("processManagement"), sizeof(ProcessManagement));
	PMObj.SMAccess();
	ProcessManagement* PMData = (ProcessManagement*)PMObj.pData;
	
	
	
	
	
	
	
	while (1) {

		
		if (PMData->Heartbeat.Flags.GPS == 0) {
			PMData->Heartbeat.Flags.GPS = 1;
		}
		else {
			PMData->PMDownCount++;
		}
		if (PMData->PMDownCount > 50) {
			PMData->Shutdown.Status = 0xFF;
		}

		printf("im alive !\n");
		Sleep(100);
		if (PMData->Shutdown.Status) {
			break;
		}
	}

	return 0;
}