#pragma once
#include <Windows.h>
#include "Element.h"

namespace HashTable
{
	__declspec(dllexport) struct HTData
	{
		HTData();
		HTData(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength);
		int Capacity;               // емкость хранилища в количестве элементов 
		int SecSnapshotInterval;    // переодичность сохранения в сек. 
		int MaxKeyLength;           // максимальная длина ключа
		int MaxPayloadLength;       // максимальная длина данных
	};

	__declspec(dllexport) struct HTHANDLE    // блок управления HT
	{
		HTHANDLE();
		HTHANDLE(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char FileName[512]);
		HTHANDLE(HTData* pHTData, const char FileName[512]);
		HANDLE hSnapThread;
		bool alive = true;
		HTData Data;
		int MaxContentSize;
		char    FileName[512];          // имя файла 
		HANDLE  File;                   // File HANDLE != 0, если файл открыт
		HANDLE  FileMapping;            // Mapping File HANDLE != 0, если mapping создан  
		LPVOID  Addr;                   // Addr != NULL, если mapview выполнен  
		char    LastErrorMessage[512];  // сообщение об последней ошибке или 0x00  
		time_t  lastsnaptime;           // дата последнего snap'a (time()) 
		LPVOID keyAddr(int n) const;
		LPVOID payloadAddr(int n) const;
	};

};
