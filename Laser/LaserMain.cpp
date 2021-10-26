
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


#using <System.dll>







int main(void) {
	//declarations
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	
	SMObject PMObj(TEXT("processManagement"), sizeof(ProcessManagement));    
	PMObj.SMAccess();
	ProcessManagement* PMData = (ProcessManagement*)PMObj.pData;

	double TimeStamp;
	__int64 Frequency, Counter;
	int Shutdown = 0x00;
	ConsoleKeyInfo keyHit;
	
	QueryPerformanceFrequency((LARGE_INTEGER*)&Frequency);  // guessing this asks windows the frequncy of ure pc clock speed then chucks in freq
	
	while (!PMData->Ready) {
		Thread::Sleep(25);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/*
	// LMS151 port number must be 2111
	int PortNumber = 23000;
	// Pointer to TcpClent type object on managed heap
	TcpClient^ Client;
	// arrays of unsigned chars to send and receive data
	array<unsigned char>^ SendData;
	array<unsigned char>^ ReadData;
	// String command to ask for Channel 1 analogue voltage from the PLC
	// These command are available on Galil RIO47122 command reference manual
	// available online
	String^ AskScan = gcnew String("sRN LMDscandata");
	String^ StudID = gcnew String("z5162440\n");
	// String to store received data for display
	String^ ResponseData;

	// Creat TcpClient object and connect to it
	Client = gcnew TcpClient("192.168.1.200", PortNumber);
	// Configure connection
	Client->NoDelay = true;
	Client->ReceiveTimeout = 500;//ms
	Client->SendTimeout = 500;//ms
	Client->ReceiveBufferSize = 1024;
	Client->SendBufferSize = 1024;

	// unsigned char arrays of 16 bytes each are created on managed heap
	SendData = gcnew array<unsigned char>(16);
	ReadData = gcnew array<unsigned char>(2500);
	// Convert string command to an array of unsigned char
	SendData = System::Text::Encoding::ASCII->GetBytes(AskScan);


	// Get the network streab object associated with clien so we 
	// can use it to read and write
	NetworkStream^ Stream = Client->GetStream();

	// Authenticate
	// Convert string command to an array of unsigned char
	SendData = System::Text::Encoding::ASCII->GetBytes(StudID);
	Stream->Write(SendData, 0, SendData->Length);
	// Wait for the server to prepare the data, 1 ms would be sufficient, but used 10 ms
	System::Threading::Thread::Sleep(10);
	// Read the incoming data
	Stream->Read(ReadData, 0, ReadData->Length);
	ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);
	Console::WriteLine(ResponseData);

	while (!kbhit) {        //remove this

	}


	//tell the laser what u want
	SendData = System::Text::Encoding::ASCII->GetBytes(AskScan);


	//Loop
	while (!_kbhit())
	{
		// Write command asking for data
		Stream->WriteByte(0x02);
		Stream->Write(SendData, 0, SendData->Length);
		Stream->WriteByte(0x03);
		// Wait for the server to prepare the data, 1 ms would be sufficient, but used 10 ms
		System::Threading::Thread::Sleep(10);
		// Read the incoming data
		Stream->Read(ReadData, 0, ReadData->Length);
		// Convert incoming data from an array of unsigned char bytes to an ASCII string
		ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);
		// Print the received string on the screen
		Console::WriteLine(ResponseData);

		//ResponseData is a ascii string
		array<wchar_t>^ Space = { ' ' };
		array<String^>^ StringArray = ResponseData->Split(Space);

		double StartAngle = System::Convert::ToInt32(StringArray[23], 16);
		double Resolution = System::Convert::ToInt32(StringArray[24], 16)/10000.0;
		int NumRanges = System::Convert::ToInt32(StringArray[25], 16);

		array<double> ^Range = gcnew array<double>(NumRanges);
		array<double> ^RangeX = gcnew array<double>(NumRanges);
		array<double> ^RangeY = gcnew array<double>(NumRanges);

		for (int i = 0; i < NumRanges; i++) {
			Range[i] = System::Convert::ToInt32(StringArray[26 + i], 16);
			RangeX[i] = Range[i] * sin(i * Resolution);
			RangeY[i] = -Range[i] * cos(i * Resolution);
		}

		
	}

	Stream->Close();
	Client->Close();

	Console::ReadKey();
	Console::ReadKey();

	*/
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	

	while (1) {
		//printf("im alive !\n");
		QueryPerformanceCounter((LARGE_INTEGER*)&Counter);
		TimeStamp = (double)Counter / (double)Frequency * 1000; //ms
		//Console::WriteLine("Laser time stamp : {1,12:F3} {1:12:X2}", TimeStamp, Shutdown);
		Sleep(25);
		if (PMData->Shutdown.Status) {
			break;
		}
		if (_kbhit()) { // doesnt actually check what key hit hit, need to getchar it
			//char c = getchar();                       //this works but not what u want cos u need to press enter after key press
			//printf("peace brother, you pressed %c", c);
			break;
		}
		
		//check if PM is doing its job
		if (PMData->Heartbeat.Flags.Laser == 0) {
			PMData->Heartbeat.Flags.Laser = 1;
		}
		else {
			PMData->PMDownCount++;
		}
		if (PMData->PMDownCount > 50) {
			PMData->Shutdown.Status = 0xFF;
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// too send write ascii, convert into binary then send
		// recieve is the opposite

		
	}
	
	return 0;

}