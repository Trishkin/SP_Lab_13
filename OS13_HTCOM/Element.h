#pragma once
#include <Windows.h>

namespace HashTable
{

	__declspec(dllexport) struct Element
	{
		__declspec(dllexport) Element();
		__declspec(dllexport) Element(const void* key, int keylength);                                             // for Get
		__declspec(dllexport) Element(const void* key, int keylength, const void* payload, int  payloadlength);    // for Insert
		__declspec(dllexport) Element(Element* oldelement, const void* newpayload, int  newpayloadlength);         // for update
		const void* key;         // значение ключа 
		int keylength;           // рахмер ключа
		const void* payload;     // данные 
		int payloadlength;       // размер данных
	};
}