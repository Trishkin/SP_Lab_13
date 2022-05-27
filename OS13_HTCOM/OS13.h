#pragma once
#include <objbase.h>
#include <iostream>
#include "INTERFACE.h"
#include "Element.h"
#include "HashTable.h"


class OS13 :public IHashTable
{
public:
	OS13();
	~OS13();


	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv);
	virtual ULONG STDMETHODCALLTYPE AddRef(void);
	virtual ULONG STDMETHODCALLTYPE Release(void);

	virtual HRESULT __stdcall  Create   //  создать HT             
	(
		int	  Capacity,					   // емкость хранилища
		int   SecSnapshotInterval,		   // переодичность сохранения в сек.
		int   MaxKeyLength,                // максимальный размер ключа
		int   MaxPayloadLength,            // максимальный размер данных
		const char  FileName[512],          // имя файла 
		OUT HashTable::HTHANDLE* hthandle
	);

	virtual HRESULT __stdcall  Open(const char FileName[512], HashTable::HTHANDLE* hthandle);

	virtual HRESULT __stdcall  Close(HashTable::HTHANDLE* hthandle);

	virtual HRESULT __stdcall  Snap(const HashTable::HTHANDLE* hthandle);

	virtual HRESULT __stdcall  Insert(const HashTable::HTHANDLE* hthandle, const HashTable::Element* element);

	virtual HRESULT __stdcall  Get(const HashTable::HTHANDLE* hthandle, const HashTable::Element* element, HashTable::Element* retelement);

	virtual HRESULT __stdcall  Delete(const HashTable::HTHANDLE* hthandle, const HashTable::Element* element);

	virtual HRESULT __stdcall  Update(const HashTable::HTHANDLE* hthandle, const HashTable::Element* oldelement, const void* newpayload, int newpayloadlength);

	virtual HRESULT __stdcall  IsProcessPresent(wchar_t* szExe);

	virtual HRESULT __stdcall GetLastErrorr(HashTable::HTHANDLE* ht);

private:
	volatile ULONG m_Ref;
};
