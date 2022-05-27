#define _CRT_SECURE_NO_WARNINGS
#include "pch.h"
#include "OS13.h"
#include "SEQLOG.h"
#include <Windows.h>
#include <iostream>
#include <string>
#include <regex>
#include <TCHAR.H>
#include <time.h>
#include <stdio.h>
#include <tlhelp32.h>

extern ULONG g_Components;


OS13::OS13() :m_Ref(1) {
	SEQ;
	InterlockedIncrement((LONG*)&g_Components);
	LOG("OS12::Adder g_Components = ", g_Components);
};
OS13::~OS13() {
	SEQ;
	InterlockedDecrement((LONG*)&g_Components);
	LOG("OS12::~Adder g_Components = ", g_Components);
};


HRESULT STDMETHODCALLTYPE OS13::QueryInterface(REFIID riid, void** ppv)
{
	SEQ;
	HRESULT rc = S_OK;
	*ppv = NULL;
	if (riid == IID_IUnknown)	*ppv = (IHashTable*)this;
	else if (riid == IID_IHashTable)	*ppv = (IHashTable*)this;
	else rc = E_NOINTERFACE;

	if (rc == S_OK) this->AddRef();
	LOG("OS13::QueryInterface rc = ", rc);
	return rc;
};
ULONG STDMETHODCALLTYPE OS13::AddRef(void) {
	SEQ;
	InterlockedIncrement((LONG*)&(this->m_Ref));
	LOG("OS13::AddRef m_Ref = ", this->m_Ref);
	return this->m_Ref;
};
ULONG STDMETHODCALLTYPE OS13::Release(void) {
	SEQ;

	ULONG rc = this->m_Ref;
	if ((rc = InterlockedDecrement((LONG*)&(this->m_Ref))) == 0) delete this;
	LOG("OS13::Release rc = ", rc);
	return rc;
};



	int CurrentSize = 0;

	HashTable::Element::Element()
	{
	}

	HashTable::Element::Element(const void* key, int keylength)
	{
		this->key = (char*)key;
		this->keylength = keylength;
	}

	HashTable::Element::Element(const void* key, int keylength, const void* payload, int  payloadlength)
	{
		this->key = (char*)key;
		this->keylength = keylength;
		this->payload = (char*)payload;
		this->payloadlength = payloadlength;
	}

	HashTable::Element::Element(Element* oldelement, const void* newpayload, int  newpayloadlength)
	{
		oldelement->payload = newpayload;
		oldelement->payloadlength = newpayloadlength;
	}


	HashTable::HTData::HTData() {
	}

	HashTable::HTData::HTData(
		int Capacity,
		int SecSnapshotInterval,
		int MaxKeyLength,
		int MaxPayloadLength
	)
	{
		this->Capacity = Capacity;
		this->SecSnapshotInterval = SecSnapshotInterval;
		this->MaxKeyLength = MaxKeyLength;
		this->MaxPayloadLength = MaxPayloadLength;
	}

	HashTable::HTHANDLE::HTHANDLE()
	{
		HTData* Data = new HTData(10, 10, 10, 10);
		this->Data = *Data;
	}

	HashTable::HTHANDLE::HTHANDLE(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char FileName[512])
	{
		HashTable::HTData* Data = new HashTable::HTData(Capacity, SecSnapshotInterval, MaxKeyLength, MaxPayloadLength);
		this->Data = *Data;
		strcpy(this->FileName, FileName);
	}

	HashTable::HTHANDLE::HTHANDLE(HashTable::HTData* pHTData, const char FileName[512])
	{
		this->Data = *pHTData;
		strcpy(this->FileName, FileName);
	}

	HRESULT  STDMETHODCALLTYPE OS13::Snap(const HashTable::HTHANDLE* hthandle)
	{
		try
		{
			FlushViewOfFile(hthandle->Addr, sizeof(HashTable::HTData) + hthandle->MaxContentSize);
			FlushFileBuffers(hthandle->File);
			std::cout << "Snap Created \n";
			return S_OK;
		}
		catch (const std::exception&)
		{
			return E_FAIL;
		}
	}

	HRESULT  STDMETHODCALLTYPE Snap1(const HashTable::HTHANDLE* hthandle)
	{
		try
		{
			FlushViewOfFile(hthandle->Addr, sizeof(HashTable::HTData) + hthandle->MaxContentSize);
			FlushFileBuffers(hthandle->File);
			std::cout << "Snap Created \n";
			return S_OK;
		}
		catch (const std::exception&)
		{
			return E_FAIL;
		}
	}

	DWORD WINAPI snapCallback(LPVOID hthandle)
	{
		int snapTime = ((HashTable::HTHANDLE*)hthandle)->Data.SecSnapshotInterval * 1000;
		int slept = 0;

		for (;;)
		{
			if (!((HashTable::HTHANDLE*)hthandle)->alive)
				return TRUE;
			int sleepTime = snapTime < 1000 ? snapTime : 500;
			Sleep(sleepTime);
			slept += sleepTime;

			if (snapTime <= slept)
			{
				Snap1(((HashTable::HTHANDLE*)hthandle));
				slept = 0;
			}


		}
	}

	int HashBytes(LPCVOID bytes, int numOfBytes)
	{
		LPBYTE pByte = (LPBYTE)bytes;
		int res = 0;
		for (int i = 0; i < numOfBytes; i++)
		{
			res ^= *(pByte + i) << (8 * (i % sizeof(int)));
		}

		return (int)abs(((float)res / 1.61803));
	}

	LPVOID HashTable::HTHANDLE::keyAddr(int n) const
	{
		return (LPVOID)((LPBYTE)(LPBYTE)this->Addr + sizeof(HTData) + n * (this->Data.MaxKeyLength + this->Data.MaxPayloadLength));
	}


	LPVOID HashTable::HTHANDLE::payloadAddr(int n) const
	{
		return (LPVOID)((LPBYTE)(LPBYTE)this->Addr + sizeof(HTData) + n * (this->Data.MaxKeyLength + this->Data.MaxPayloadLength)
			+ this->Data.MaxKeyLength);
	}

	bool isMemEmpty(LPCVOID mem, int len)
	{
		LPVOID zeroBuf = malloc(len);
		memset(zeroBuf, 0, len);
		bool res = (0 == memcmp(zeroBuf, mem, len));
		free(zeroBuf);
		return res;
	}

	HRESULT  STDMETHODCALLTYPE OS13::Create(int	Capacity, int SecSnapshotInterval, int MaxKeyLength,
		int MaxPayloadLength, const char FileName[512], HashTable::HTHANDLE* htret)
	{
		CurrentSize = 0;
		HashTable::HTHANDLE* ht = new HashTable::HTHANDLE(Capacity, SecSnapshotInterval, MaxKeyLength, MaxPayloadLength, FileName);
		wchar_t wtext[60];
		mbstowcs(wtext, ht->FileName, strlen(ht->FileName) + 1);

		ht->File = CreateFile
		(wtext, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		HashTable::HTData* Data = new HashTable::HTData(Capacity, SecSnapshotInterval, MaxKeyLength, MaxPayloadLength);

		DWORD numberOfDataBytesWritten;
		WriteFile(ht->File, Data, sizeof(HashTable::HTData), &numberOfDataBytesWritten, NULL);

		if (ht->FileMapping != NULL)
		{
			OS13::Open(FileName, htret);
			return S_OK;
		}
		
		else
			return E_FAIL;
	}

	HRESULT  STDMETHODCALLTYPE OS13::Open(const char FileName[512], HashTable::HTHANDLE* ht)
	{
		CurrentSize = 0;
		strcpy(ht->FileName, FileName);

		wchar_t wtext[60];
		mbstowcs(wtext, ht->FileName, strlen(ht->FileName) + 1);

		ht->File = CreateFile
		(wtext, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);


		DWORD numberOfBytesRead;
		bool r = ReadFile(ht->File, &ht->Data, sizeof(HashTable::HTData), &numberOfBytesRead, NULL);
		ht->MaxContentSize = (ht->Data.Capacity * (ht->Data.MaxKeyLength + ht->Data.MaxPayloadLength));
		ht->FileMapping = CreateFileMapping(ht->File, NULL, PAGE_READWRITE, NULL, sizeof(HashTable::HTData) + ht->MaxContentSize, NULL);
		if (!ht->FileMapping) {
			GetLastErrorr((HashTable::HTHANDLE*)ht);
		}
		ht->Addr = MapViewOfFile(ht->FileMapping, FILE_MAP_ALL_ACCESS, NULL, 0, sizeof(HashTable::HTData) + ht->MaxContentSize);
		if (!ht->Addr) {
			GetLastErrorr((HashTable::HTHANDLE*)ht);
		}

		ht->hSnapThread = CreateThread(NULL, NULL, snapCallback, (LPVOID)ht, NULL, NULL);

		if (ht->FileMapping != NULL)
		{
			return S_OK;
		}
		else
			return E_FAIL;
	}

	

	HRESULT  STDMETHODCALLTYPE OS13::Close(HashTable::HTHANDLE* hthandle)
	{
		try
		{
			hthandle->alive = false;
			WaitForSingleObject(hthandle->hSnapThread, INFINITE);
			UnmapViewOfFile(hthandle->Addr);
			if (!CloseHandle(hthandle->File))
				throw "Close File failed";
			if (!CloseHandle(hthandle->FileMapping))
				throw "Close File failed";
			return S_OK;
		}
		catch (const std::exception&)
		{
			return E_FAIL;
		}
	}

	HRESULT  STDMETHODCALLTYPE OS13::Insert(const HashTable::HTHANDLE* hthandle, const HashTable::Element* element)
	{
		try
		{
			int firstEmptyRecordInd = -1;
			int maxKeyLength = hthandle->Data.MaxKeyLength;
			int hash = HashBytes(element->key, element->keylength) % hthandle->Data.Capacity;

			for (int ii = hash; ii < hthandle->Data.Capacity + hash; ii++)
			{
				int i = ii % hthandle->Data.Capacity;

				LPCVOID nKey = hthandle->keyAddr(i);

				if (isMemEmpty(nKey, maxKeyLength))
					firstEmptyRecordInd = firstEmptyRecordInd == -1 ? i : firstEmptyRecordInd;

				LPVOID maxLenKey = malloc(maxKeyLength);
				memset(maxLenKey, 0, maxKeyLength);
				memcpy(maxLenKey, element->key, element->keylength);

				if (!memcmp(nKey, maxLenKey, maxKeyLength))
				{
					free(maxLenKey);
					std::cout << GetLastErrorr((HashTable::HTHANDLE*)hthandle);
					return E_FAIL;
				}
				free(maxLenKey);
			}

			if (firstEmptyRecordInd == -1)
			{
				std::cout << GetLastErrorr((HashTable::HTHANDLE*)hthandle);
				return E_FAIL;
			}

			LPVOID nKey = hthandle->keyAddr(firstEmptyRecordInd);
			LPVOID nPayload = hthandle->payloadAddr(firstEmptyRecordInd);

			memcpy(nKey, element->key, element->keylength);
			memcpy(nPayload, element->payload, element->payloadlength);

		}
		catch (const std::exception&)
		{
			std::cout << GetLastErrorr((HashTable::HTHANDLE*)hthandle);
			return E_FAIL;
		}
	}
	HRESULT  STDMETHODCALLTYPE OS13::Get(const HashTable::HTHANDLE* hthandle, const HashTable::Element* element, HashTable::Element* retelement)
	{
		try
		{
			int maxKeyLength = hthandle->Data.MaxKeyLength;
			int maxPayloadLength = hthandle->Data.MaxPayloadLength;
			int hash = HashBytes(element->key, element->keylength) % hthandle->Data.Capacity;
			for (int ii = hash; ii < hthandle->Data.Capacity + hash; ii++)
			{
				int i = ii % hthandle->Data.Capacity;
				LPVOID maxLenKey = malloc(maxKeyLength);
				memset(maxLenKey, 0, maxKeyLength);
				memcpy(maxLenKey, element->key, element->keylength);
				LPVOID nKey = hthandle->keyAddr(i);
				if (0 == memcmp(maxLenKey, nKey, maxKeyLength))
				{
					LPVOID newElKey = malloc(element->keylength);
					memcpy(newElKey, nKey, element->keylength);
					LPVOID newElPayload = malloc(maxPayloadLength);
					memcpy(newElPayload, hthandle->payloadAddr(i), maxPayloadLength);
					HashTable::Element* el = new HashTable::Element(
						newElKey,
						element->keylength,
						newElPayload,
						hthandle->Data.MaxPayloadLength
					);
					free(maxLenKey);
					retelement->key = el->key;
					retelement->keylength= el->keylength;
					retelement->payload = el->payload;
					retelement->payloadlength = el->payloadlength;
					return S_OK;
				}
				free(maxLenKey);
			}
			return E_FAIL;
		}
		catch (const std::exception&)
		{
			std::cout << GetLastErrorr((HashTable::HTHANDLE*)hthandle);
			return E_FAIL;
		}
	}

	HRESULT  STDMETHODCALLTYPE OS13::Delete(const HashTable::HTHANDLE* hthandle, const HashTable::Element* element)
	{
		try
		{
			int maxKeyLength = hthandle->Data.MaxKeyLength;
			int hash = HashBytes(element->key, element->keylength) % hthandle->Data.Capacity;
			for (int ii = hash; ii < hthandle->Data.Capacity + hash; ii++)
			{
				int i = ii % hthandle->Data.Capacity;
				LPVOID maxLenKey = malloc(maxKeyLength);
				memset(maxLenKey, 0, maxKeyLength);
				memcpy(maxLenKey, element->key, element->keylength);
				LPVOID nKey = hthandle->keyAddr(i);
				if (0 == memcmp(maxLenKey, nKey, maxKeyLength))
				{
					memset(nKey, 0, maxKeyLength + hthandle->Data.MaxPayloadLength);
					free(maxLenKey);
					return S_OK;
				}
				free(maxLenKey);
			}

		}
		catch (const std::exception&)
		{
			std::cout << GetLastErrorr((HashTable::HTHANDLE*)hthandle);
			return E_FAIL;
		}
	}


	HRESULT  STDMETHODCALLTYPE OS13::Update(const HashTable::HTHANDLE* hthandle, const HashTable::Element* oldelement, const void* newpayload, int newpayloadlength)
	{
		try
		{
			int maxKeyLength = hthandle->Data.MaxKeyLength;
			int maxPayloadLength = hthandle->Data.MaxPayloadLength;
			int hash = HashBytes(oldelement->key, oldelement->keylength) % hthandle->Data.Capacity;
			for (int ii = hash; ii < hthandle->Data.Capacity + hash; ii++)
			{
				int i = ii % hthandle->Data.Capacity;
				LPVOID maxLenKey = malloc(maxKeyLength);
				memset(maxLenKey, 0, maxKeyLength);
				memcpy(maxLenKey, oldelement->key, oldelement->keylength);
				LPVOID nKey = hthandle->keyAddr(i);
				if (0 == memcmp(nKey, maxLenKey, maxKeyLength))
				{
					LPVOID oldPayload = hthandle->payloadAddr(i);
					memset(oldPayload, 0, maxPayloadLength);
					memcpy(oldPayload, newpayload, newpayloadlength);
					free(maxLenKey);
					return S_OK;
				}
				free(maxLenKey);
			}
			return E_FAIL;
		}
		catch (const std::exception&)
		{
			GetLastErrorr((HashTable::HTHANDLE*)hthandle);
			return E_FAIL;
		}
	}
	HRESULT  STDMETHODCALLTYPE OS13::IsProcessPresent(wchar_t* szExe)
	{
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		PROCESSENTRY32 pe;
		pe.dwSize = sizeof(PROCESSENTRY32);
		Process32First(hSnapshot, &pe);

		if (!_wcsicmp((wchar_t*)&pe.szExeFile, szExe))
		{
			return S_OK;
		}

		while (Process32Next(hSnapshot, &pe))
		{
			if (!_wcsicmp((wchar_t*)&pe.szExeFile, szExe))
			{
				return S_OK;
			}
		}

		return E_FAIL;
	}
	HRESULT  STDMETHODCALLTYPE OS13::GetLastErrorr(HashTable::HTHANDLE* ht)
	{
		
		return E_FAIL;
	}

	void print(const HashTable::Element* element)
	{
		std::cout << "Key: " << *(int*)element->key << "\n"
			<< "Key lenght: " << element->keylength << "\n"
			<< "Payload: " << *(int*)element->payload << "\n"
			<< "Payload lenght: " << element->payloadlength << "\n";
	}
