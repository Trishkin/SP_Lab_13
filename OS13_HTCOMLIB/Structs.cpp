#define _CRT_SECURE_NO_WARNINGS
#include "pch.h"
#include "framework.h"
#include <iostream>
#include "OS13.h"


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