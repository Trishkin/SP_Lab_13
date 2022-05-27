#pragma once
#include "../OS13_HTCOM/Element.h"
#include "../OS13_HTCOM/HashTable.h"

#define OS13HANDEL void*



namespace OS13
{
	__declspec(dllexport) OS13HANDEL Init();                                // инициализация OS12
	//   if CoCreateInstance(... IID_Unknown)!= succesfull --> throw (int)HRESULT  
	namespace HashTableLib
	{
		double __declspec(dllexport) Add(OS13HANDEL h, double x, double y);        // return x+y
		//  if QueryInteface(IID_IAdder) != succesfull -->  throw (int)HRESULT     
		extern "C" __declspec(dllexport) HashTable::HTHANDLE * Create   //  создать HT             
		(
			OS13HANDEL h,
			int	  Capacity,					   // емкость хранилища
			int   SecSnapshotInterval,		   // переодичность сохранения в сек.
			int   MaxKeyLength,                // максимальный размер ключа
			int   MaxPayloadLength,            // максимальный размер данных
			const char  FileName[512]          // имя файла 
		);

		extern "C" __declspec(dllexport) HashTable::HTHANDLE * Open(OS13HANDEL h, const char FileName[512]);

		extern "C" __declspec(dllexport) BOOL Snap(OS13HANDEL h, const HashTable::HTHANDLE * hthandle);

		extern "C" __declspec(dllexport) BOOL Close(OS13HANDEL h, HashTable::HTHANDLE * hthandle);

		extern "C" __declspec(dllexport) BOOL Insert(OS13HANDEL h, const HashTable::HTHANDLE * hthandle, const HashTable::Element * element);

		extern "C" __declspec(dllexport) HashTable::Element * Get(OS13HANDEL h, const HashTable::HTHANDLE * hthandle, const HashTable::Element * element);

		extern "C" __declspec(dllexport) BOOL Delete(OS13HANDEL h, const HashTable::HTHANDLE * hthandle, const HashTable::Element * element);

		extern "C" __declspec(dllexport) BOOL Update(OS13HANDEL h, const HashTable::HTHANDLE * hthandle, const HashTable::Element * oldelement, const void* newpayload, int newpayloadlength);

		extern "C" __declspec(dllexport) bool IsProcessPresent(OS13HANDEL h, wchar_t* szExe);

		extern "C" __declspec(dllexport) bool IsProcessPresentClose(wchar_t* szExe);

		extern "C" __declspec(dllexport) void print(const HashTable::Element * element);
	}
	__declspec(dllexport) void Dispose(OS13HANDEL h);                       // завершение работы с OS12                  
}