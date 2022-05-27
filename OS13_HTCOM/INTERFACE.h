#pragma once
#define OUT
#include <objbase.h>
#include "HashTable.h"

#define FNAME L"SDA.OS_13.HTCOM"
#define VINDX L"SDA.OS_13.1"
#define PRGID L"SDA.OS_13"


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

	virtual HRESULT __stdcall  Snap(const HashTable::HTHANDLE * hthandle) = 0;

	virtual HRESULT __stdcall  Close(HashTable::HTHANDLE * hthandle) = 0;

	virtual HRESULT __stdcall  Insert(const HashTable::HTHANDLE * hthandle, const HashTable::Element * element ) = 0;

	virtual HRESULT __stdcall  Get(const HashTable::HTHANDLE * hthandle, const HashTable::Element * element, HashTable::Element* retelement) = 0;

	virtual HRESULT __stdcall  Delete(const HashTable::HTHANDLE * hthandle, const HashTable::Element * element) = 0;

	virtual HRESULT __stdcall  Update(const HashTable::HTHANDLE * hthandle, const HashTable::Element * oldelement, const void* newpayload, int newpayloadlength) = 0;

	virtual HRESULT __stdcall  IsProcessPresent(wchar_t* szExe) = 0;

	virtual HRESULT __stdcall GetLastErrorr(HashTable::HTHANDLE * ht) =0;

};