#pragma once
#include "Tag.h"
template <NumberType Word, NumberType DWord>
struct IFD指针;
#pragma pack(4)
template <NumberType Word, NumberType DWord>
struct IFD偏移
{
	DWord 偏移;
	IFD指针<Word,DWord> 取指针(const BYTE* 基地址)const;
	DWord operator+(DWord 增量)const;
	IFD偏移& operator=(DWord 偏移);
	IFD偏移& operator+=(DWord 增量);
	IFD偏移(DWord 偏移);
	IFD偏移();
	bool operator<(DWord 偏移)const;
};
#pragma pack()
template <NumberType Word, NumberType DWord>
struct IFD指针
{
	BYTE* 指针;
	Word& NumberOfTags()const;
	Tag<DWord>* Tags()const;
	IFD偏移<Word, DWord>& NextIFD()const;
	IFD偏移<Word, DWord> 取偏移(const BYTE* 基地址)const;
};
extern template struct IFD指针<UINT16, UINT32>;
extern template struct IFD指针<UINT64, UINT64>;
extern template struct IFD偏移<UINT16, UINT32>;
extern template struct IFD偏移<UINT64, UINT64>;