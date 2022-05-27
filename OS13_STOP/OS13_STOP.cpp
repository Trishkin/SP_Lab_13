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
	try
	{
		cout << argv[1] << endl;
		/*cout << "filename: ";
		char filename[100];
		cin >> filename;*/
		OS13HANDEL h1 = OS13::Init();
		HashTable::HTHANDLE* HT = OS13::HashTableLib::Open(h1, argv[1]);
		if (HT == NULL) throw "Invalid handle";
		TCHAR szExe[] = L"OS13_START.exe";
		if (OS13::HashTableLib::IsProcessPresentClose((wchar_t*)&szExe))
		{
			while (OS13::HashTableLib::IsProcessPresentClose((wchar_t*)&szExe));
		}
		else {
			throw "Error stop";
		}
		if (!OS13::HashTableLib::Snap(h1, HT)) throw "Error snap";
		if (!OS13::HashTableLib::Close(h1, HT)) throw "Error close";
		OS13::Dispose(h1);
	}
	catch (const char* error) { std::cout << "-- Error: " << error << std::endl; }
}