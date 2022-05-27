#pragma once
#include "pch.h"
#include "framework.h"
#include <iostream>
#include "OS13.h"
#include <objbase.h>
#include <tlhelp32.h>

// {88E169DE-099E-4ED5-8B07-6B4A6E826938}
static const GUID CLSID_CA =
{ 0x88e169de, 0x99e, 0x4ed5, { 0x8b, 0x7, 0x6b, 0x4a, 0x6e, 0x82, 0x69, 0x38 } };


// {0A27F58B-7A27-4B74-B766-255B4DA26370}
static const GUID IID_IHashTable =
{ 0xa27f58b, 0x7a27, 0x4b74, { 0xb7, 0x66, 0x25, 0x5b, 0x4d, 0xa2, 0x63, 0x70 } };

interface IHashTable :IUnknown {

	virtual HRESULT __stdcall  Create   //  создать HT             
	(
		int	  Capacity,					   // емкость хранилища
		int   SecSnapshotInterval,		   // переодичность сохранения в сек.
		int   MaxKeyLength,                // максимальный размер ключа
		int   MaxPayloadLength,            // максимальный размер данных
		const char  FileName[512],          // имя файла 
		OUT HashTable::HTHANDLE* hthandle
	) = 0;

	virtual HRESULT __stdcall  Open(const char FileName[512], HashTable::HTHANDLE* hthandle) = 0;

	virtual HRESULT __stdcall  Snap(const HashTable::HTHANDLE* hthandle) = 0;

	virtual HRESULT __stdcall  Close(HashTable::HTHANDLE* hthandle) = 0;

	virtual HRESULT __stdcall  Insert(const HashTable::HTHANDLE* hthandle, const HashTable::Element* element) = 0;

	virtual HRESULT __stdcall  Get(const HashTable::HTHANDLE* hthandle, const HashTable::Element* element, HashTable::Element* retelement) = 0;

	virtual HRESULT __stdcall  Delete(const HashTable::HTHANDLE* hthandle, const HashTable::Element* element) = 0;

	virtual HRESULT __stdcall  Update(const HashTable::HTHANDLE* hthandle, const HashTable::Element* oldelement, const void* newpayload, int newpayloadlength) = 0;

	virtual HRESULT __stdcall  IsProcessPresent(wchar_t* szExe) = 0;

	virtual HRESULT __stdcall GetLastErrorr(HashTable::HTHANDLE* ht) = 0;

};

bool OS13::HashTableLib::IsProcessPresentClose(wchar_t* szExe)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hSnapshot, &pe);

	if (!_wcsicmp((wchar_t*)&pe.szExeFile, szExe))
	{
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pe.th32ProcessID);
		if (TerminateProcess(hProcess, 0)) return true;
		else throw "Не могу убить процесс";
		return true;
	}

	while (Process32Next(hSnapshot, &pe))
	{
		if (!_wcsicmp((wchar_t*)&pe.szExeFile, szExe))
		{
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pe.th32ProcessID);
			if (TerminateProcess(hProcess, 0)) return true;
			else throw "Не могу убить процесс";
			return true;
		}
	}

	return false;
}

void OS13::HashTableLib::print(const HashTable::Element* element)
{
	std::cout << "Key: " << *(int*)element->key << "\n"
		<< "Key lenght: " << element->keylength << "\n"
		<< "Payload: " << *(int*)element->payload << "\n"
		<< "Payload lenght: " << element->payloadlength << "\n";
}

IHashTable* pIHashTable = nullptr;

OS13HANDEL OS13::Init() {
	IUnknown* pIUnknown = NULL;
	CoInitialize(NULL);                        // инициализация библиотеки OLE32
	HRESULT hr0 = CoCreateInstance(CLSID_CA, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pIUnknown);
	if (SUCCEEDED(hr0))
	{
		return pIUnknown;
	}
	else {
		throw (int)hr0;
		return NULL;
	}
}


void OS13::Dispose(OS13HANDEL h) {
	((IUnknown*)h)->Release();
	CoFreeUnusedLibraries();
}


HashTable::HTHANDLE* OS13::HashTableLib::Create(OS13HANDEL h,
	int	  Capacity,					   // емкость хранилища
	int   SecSnapshotInterval,		   // переодичность сохранения в сек.
	int   MaxKeyLength,                // максимальный размер ключа
	int   MaxPayloadLength,            // максимальный размер данных
	const char  FileName[512]          // имя файла 
)
{
	HashTable::HTHANDLE* ht = new HashTable::HTHANDLE();
	HRESULT hr0 = ((IUnknown*)h)->QueryInterface(IID_IHashTable, (void**)&pIHashTable);
	if (SUCCEEDED(hr0))
	{
		HRESULT hr1 = pIHashTable->Create(Capacity,SecSnapshotInterval, MaxKeyLength, MaxPayloadLength, FileName, ht);
		if (!SUCCEEDED(hr1)) {
			pIHashTable->Release();
			return NULL;
		}
		else {
			pIHashTable->Release();
			return ht;
		}
	}
	else {

		throw (int)hr0;
		return NULL;
	}
}

HashTable::HTHANDLE* OS13::HashTableLib::Open(OS13HANDEL h,
	const char  FileName[512]          // имя файла 
)
{
	try
	{
		HashTable::HTHANDLE* ht = new HashTable::HTHANDLE();
		HRESULT hr0 = ((IUnknown*)h)->QueryInterface(IID_IHashTable, (void**)&pIHashTable);
		if (SUCCEEDED(hr0))
		{
			HRESULT hr1 = pIHashTable->Open(FileName, ht);
			if (!SUCCEEDED(hr1)) {
				pIHashTable->Release();
				throw (int)hr1;
				return ht;
			}
			else {
				pIHashTable->Release();
				return ht;
			}
		}
		else {

			throw (int)hr0;
			return NULL;
		}
	}
	catch (const char* error)
	{
		std::cout << "-- Error: " << error << std::endl;
	}
}

BOOL OS13::HashTableLib::Insert(OS13HANDEL h, const HashTable::HTHANDLE* hthandle, const HashTable::Element* element)
{
	HRESULT hr0 = ((IUnknown*)h)->QueryInterface(IID_IHashTable, (void**)&pIHashTable);
	if (SUCCEEDED(hr0))
	{
		HRESULT hr1 = pIHashTable->Insert(hthandle, element);
		if (!SUCCEEDED(hr1)) {
			pIHashTable->Release();
			return false;
		}
		else {
			pIHashTable->Release();
			return true;
		}
	}
	else {

		throw (int)hr0;
		return false;
	}
}

HashTable::Element* OS13::HashTableLib::Get(OS13HANDEL h, const HashTable::HTHANDLE* hthandle, const HashTable::Element* element)
{
	HashTable::Element* el = new HashTable::Element();
	HRESULT hr0 = ((IUnknown*)h)->QueryInterface(IID_IHashTable, (void**)&pIHashTable);
	if (SUCCEEDED(hr0))
	{
		HRESULT hr1 = pIHashTable->Get(hthandle, element, el);
		if (!SUCCEEDED(hr1)) {
			pIHashTable->Release();
			return NULL;
		}
		else {
			pIHashTable->Release();
			return el;
		}
	}
	else {

		throw (int)hr0;
		return NULL;
	}
}

BOOL OS13::HashTableLib::Delete(OS13HANDEL h, const HashTable::HTHANDLE* hthandle, const HashTable::Element* element)
{
	HRESULT hr0 = ((IUnknown*)h)->QueryInterface(IID_IHashTable, (void**)&pIHashTable);
	if (SUCCEEDED(hr0))
	{
		HRESULT hr1 = pIHashTable->Delete(hthandle, element);
		if (!SUCCEEDED(hr1)) {
			pIHashTable->Release();
			return false;
		}
		else {
			pIHashTable->Release();
			return true;
		}
	}
	else {

		throw (int)hr0;
		return false;
	}
}

BOOL OS13::HashTableLib::Update(OS13HANDEL h, const HashTable::HTHANDLE* hthandle, const HashTable::Element* element, const void* newpayload, int newpayloadlength)
{
	HRESULT hr0 = ((IUnknown*)h)->QueryInterface(IID_IHashTable, (void**)&pIHashTable);
	if (SUCCEEDED(hr0))
	{
		HRESULT hr1 = pIHashTable->Update(hthandle, element, newpayload, newpayloadlength);
		if (!SUCCEEDED(hr1)) {
			pIHashTable->Release();
			return false;
		}
		else {
			pIHashTable->Release();
			return true;
		}
	}
	else {

		throw (int)hr0;
		return false;
	}
}

BOOL OS13::HashTableLib::Snap(OS13HANDEL h, const HashTable::HTHANDLE* hthandle)
{
	HRESULT hr0 = ((IUnknown*)h)->QueryInterface(IID_IHashTable, (void**)&pIHashTable);
	if (SUCCEEDED(hr0))
	{
		HRESULT hr1 = pIHashTable->Snap(hthandle);
		if (!SUCCEEDED(hr1)) {
			pIHashTable->Release();
			return false;
		}
		else {
			pIHashTable->Release();
			return true;
		}
	}
	else {

		throw (int)hr0;
		return false;
	}
}

bool OS13::HashTableLib::IsProcessPresent(OS13HANDEL h, wchar_t* szExe)
{
	HRESULT hr0 = ((IUnknown*)h)->QueryInterface(IID_IHashTable, (void**)&pIHashTable);
	if (SUCCEEDED(hr0))
	{
		HRESULT hr1 = pIHashTable->IsProcessPresent(szExe);
		if (!SUCCEEDED(hr1)) {
			pIHashTable->Release();
			return false;
		}
		else {
			pIHashTable->Release();
			return true;
		}
	}
	else {

		throw (int)hr0;
		return false;
	}
}

BOOL OS13::HashTableLib::Close(OS13HANDEL h, HashTable::HTHANDLE* hthandle)
{
	HRESULT hr0 = ((IUnknown*)h)->QueryInterface(IID_IHashTable, (void**)&pIHashTable);
	if (SUCCEEDED(hr0))
	{
		HRESULT hr1 = pIHashTable->Close(hthandle);
		if (!SUCCEEDED(hr1)) {
			pIHashTable->Release();
			return false;
		}
		else {
			pIHashTable->Release();
			return true;
		}
	}
	else {

		throw (int)hr0;
		return false;
	}
}