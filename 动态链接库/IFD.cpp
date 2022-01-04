#include "pch.h"
#include "IFD.h"
template <NumberType Word, NumberType DWord>
IFD指针<Word, DWord> IFD偏移<Word, DWord>::取指针(const BYTE* 基地址)const
{
	return IFD指针<Word, DWord>{.指针 = (BYTE*)(基地址 + 偏移)};
}
template <NumberType Word, NumberType DWord>
DWord IFD偏移<Word, DWord>::operator+(DWord 增量)const
{
	return 偏移 + 增量;
}
template <NumberType Word, NumberType DWord>
IFD偏移<Word, DWord>::IFD偏移(DWord 偏移):偏移(偏移){}
template <NumberType Word, NumberType DWord>
IFD偏移<Word, DWord>::IFD偏移() {}
template <NumberType Word, NumberType DWord>
IFD偏移<Word, DWord>& IFD偏移<Word, DWord>::operator=(DWord 偏移)
{
	IFD偏移::偏移 = 偏移;
	return *this;
}
template <NumberType Word, NumberType DWord>
IFD偏移<Word, DWord>& IFD偏移<Word, DWord>::operator+=(DWord 增量)
{
	偏移 += 增量;
	return *this;
}
template <NumberType Word, NumberType DWord>
bool IFD偏移<Word, DWord>::operator<(DWord 偏移)const
{
	return IFD偏移::偏移 < 偏移;
}

template <NumberType Word, NumberType DWord>
Word& IFD指针<Word, DWord>::NumberOfTags()const
{
	return *(Word*)(指针);
}
template <NumberType Word, NumberType DWord>
Tag<DWord>* IFD指针<Word, DWord>::Tags()const
{
	return (Tag<DWord>*)(指针 + sizeof(Word));
}
template <NumberType Word, NumberType DWord>
IFD偏移<Word, DWord>& IFD指针<Word, DWord>::NextIFD()const
{
	return *(IFD偏移<Word, DWord>*)(指针 + sizeof(Word) + sizeof(Tag<DWord>) * NumberOfTags());
}
template <NumberType Word, NumberType DWord>
IFD偏移<Word, DWord> IFD指针<Word, DWord>::取偏移(const BYTE* 基地址)const
{
	return IFD偏移<Word, DWord>(指针 - 基地址);
}
template struct IFD指针<UINT16, UINT32>;
template struct IFD指针<UINT64, UINT64>;
template struct IFD偏移<UINT16, UINT32>;
template struct IFD偏移<UINT64, UINT64>;