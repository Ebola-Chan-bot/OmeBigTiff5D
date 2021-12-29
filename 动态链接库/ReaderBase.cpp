#include "pch.h"
#include "ReaderBase.h"
#include "是否间断.h"
#include <algorithm>
#include "N维切片参数.h"
#include "尝试结果.h"
ReaderBase::~ReaderBase()noexcept
{
	UnmapViewOfFile(基地址);
}
void ReaderBase::加载文件(HANDLE 文件句柄)
{
	File = 文件句柄;
	FileMappingObject = CreateFileMappingW(文件句柄, NULL, PAGE_READONLY, 0, 0, NULL);
	if (FileMappingObject == INVALID_HANDLE_VALUE)
		throw 尝试结果{ .结果 = 结果分类::Win32异常,.错误代码 = GetLastError(),.错误消息 = "建立文件映射失败" };
	基地址 = (char*)MapViewOfFile(FileMappingObject, FILE_MAP_READ, 0, 0, 0);
	if (!基地址)
		throw 尝试结果{ .结果 = 结果分类::Win32异常,.错误代码 = GetLastError(),.错误消息 = "文件映射视图出错" };
}
template <NumberType Word, NumberType DWord>
const BYTE* ReaderBase::读像素指针(IFD偏移<Word, DWord>& 当前IFD偏移)const
{
	const IFD指针<Word, DWord> 当前IFD指针 = 当前IFD偏移.取指针(基地址);
	Tag<DWord>* 当前标签 = 当前IFD指针.Tags();
	Tag<DWord>* const 标签尾 = 当前标签 + 当前IFD指针.NumberOfTags();
	当前IFD偏移 = 当前IFD指针.NextIFD();
	while (当前标签 < 标签尾)
	{
		if (当前标签->Identifier == TagID::StripOffsets)
			return 当前标签->BYTE偏移.取指针(基地址);
		else
			当前标签++;
	}
}
template const BYTE* ReaderBase::读像素指针<UINT16, UINT32>(IFD偏移<UINT16, UINT32>& 当前IFD偏移)const;
template const BYTE* ReaderBase::读像素指针<UINT64, UINT64>(IFD偏移<UINT64, UINT64>& 当前IFD偏移)const;
void ReaderBase::Read3DBase(UINT16 XSize, UINT16 YSize, UINT32 ISize, UINT64* XRange, UINT64* YRange, UINT64* IRange, BYTE* BytesOut)
{
	UINT32 IStart, IEnd;
	if (IRange)
	{
		if (是否间断(IRange, ISize))
			缓存到(*(std::max_element(IRange, IRange + ISize)));
		else
		{
			IStart = IRange[0];
			IEnd = IStart + ISize;
			缓存到(IEnd - 1);
			IRange = nullptr;
		}
	}
	else
	{
		IStart = 0;
		IEnd = 缓存全部();
	}
	偏移向量 偏移;
	UINT64 段长度;
	UINT32 各维尺寸[] = { iSizeX,iSizeY };
	UINT32 下标长度[] = { XSize,YSize };
	UINT64* 下标[] = { XRange,YRange };
	N维切片参数(iSizeP, 2, 各维尺寸, 下标长度, 下标, 偏移, 段长度);
	const BYTE* 像素指针;
	const BYTE* const* IFD像素指针 = GetIFD像素指针();
	//BYTE* 缓存 = (BYTE*)malloc(段长度);
	if (IRange)
	{
		const UINT64* const IRangeEnd = IRange + ISize;
		while (IRange < IRangeEnd)
		{
			像素指针 = IFD像素指针[*(IRange++)];
			for (UINT64 O : 偏移)
			{
				memcpy(BytesOut, 像素指针 + O, 段长度);
				BytesOut += 段长度;
			}
		}
	}
	else
		while (IStart < IEnd)
		{
			像素指针 = IFD像素指针[IStart++];
			for (UINT64 O : 偏移)
			{
				/*
				memcpy(缓存, 像素指针 + O, 段长度);
				memcpy(BytesOut, 缓存, 段长度);
				*/
				memcpy(BytesOut, 像素指针 + O, 段长度);
				BytesOut += 段长度;
			}
		}
}