#define _CRT_SECURE_NO_WARNINGS
#define OS13HANDEL void*
#include <Windows.h>
#include <iostream>
#include "../OS13_HTCOM/Element.h"
#include "../OS13_HTCOM/HashTable.h"
#include "../OS13_HTCOMLIB/OS13.h"
using namespace std;

int main(int argc, char** argv)
{
	HANDLE hf = NULL;
	try
	{
		//cout << argv[1] << endl;

		OS13HANDEL h1 = OS13::Init();

		TCHAR szExe[] = L"OS13_START.exe";
		if (!OS13::HashTableLib::IsProcessPresent(h1, (wchar_t*)&szExe))
		{
			std::cout << "OS13_START does not work \n";
			system("pause");
			return -1;

		}
		//HashTable::HTHANDLE* HT = OS13::HashTableLib::Open(h1, argv[1]);
		HashTable::HTHANDLE* HT = OS13::HashTableLib::Open(h1, "D:\\1\\Lab_13\\Debug\\HashTable.ht");
		if (HT == NULL) throw "Invalid handle";
		hf = CreateFile(L"D:\\1\\Lab_13\\update.log", GENERIC_WRITE | GENERIC_READ, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hf == INVALID_HANDLE_VALUE) throw "INVALID_HANDLE_VALUE";
		SetFilePointer(hf, NULL, NULL, FILE_END);

		int key;
		int pload;
		HashTable::Element* updating;
		HashTable::Element* getting;
		if (!WriteFile(hf, "START UPDATE\n", 13, NULL, NULL)) throw "WriteFile";

		for (int i = 1; i <= 100; i++)
		{
			if (HT->Addr == NULL) throw "HT IS CLOSEN";
			key = i;
			pload = i+7;
			updating = new HashTable::Element(&key, sizeof(key), &pload, sizeof(pload));
			std::cout << "updating..." << std::endl;
			if (!OS13::HashTableLib::Update(h1, HT, updating, &pload, sizeof(pload)))
			{

				if (!WriteFile(hf, "-- Error: Updating element\n", 27, NULL, NULL)) throw "WriteFile";
				std::cout << "error update" << std::endl;
			}
			else {
				if ((OS13::HashTableLib::Get(h1,HT, updating)) == NULL)
				{
					if (!WriteFile(hf, "-- Error: Getting element\n", 26, NULL, NULL)) throw "WriteFile";
				}
				if (!WriteFile(hf, "-- Updating and getting element\n", 32, NULL, NULL)) throw "WriteFile";
				OS13::HashTableLib::print(updating);
			}
			if (!OS13::HashTableLib::IsProcessPresent(h1, (wchar_t*)&szExe))
			{
				if (!CloseHandle(hf)) throw "CloseHandle";
				if (!OS13::HashTableLib::Close(h1, HT)) throw "Error close";
				OS13::Dispose(h1);
				std::cout << "OS11_START does not work \n";
				system("pause");
				return -1;
			}
			Sleep(1000);
		}


		if (!OS13::HashTableLib::Close(h1, HT)) throw "Error close";
		OS13::Dispose(h1);
		system("pause");
	}
	catch (const char* error) { std::cout << "-- Error: " << error << std::endl; }
}