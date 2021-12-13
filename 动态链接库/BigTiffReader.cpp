#include "pch.h"
#include "BigTiffReader.h"
#include "BigTiff文件头.h"
#include "像素类型尺寸.h"
void BigTiffReader::加载文件(HANDLE 文件句柄)
{
	ReaderBase::加载文件(文件句柄);
	下个IFD = ((BigTiff文件头*)基地址)->FirstIFD;
	const IFD指针<UINT64, UINT64> 下个IFD指针 = 下个IFD.取指针(基地址);
	Tag<UINT64>* 当前标签 = 下个IFD指针.Tags();
	const Tag<UINT64>* 标签尾 = 当前标签 + 下个IFD指针.NumberOfTags();
	UINT8 完成进度 = 0;
	while (完成进度 < 4 && 当前标签 < 标签尾)
	{
		switch (当前标签->Identifier)
		{
		case TagID::StripOffsets:
			IFD像素指针.push_back(当前标签->BYTE偏移.取指针(基地址));
			完成进度++;
			break;
		case TagID::BitsPerSample:
			iBytesPerSample = 当前标签->BYTE值 / 8;
			for (UINT8 P = 0; P < 像素类型个数; ++P)
				if (像素类型尺寸[P] == iBytesPerSample)
				{
					iPixelType = 像素类型(P);
					break;
				}
			完成进度++;
			break;
		case TagID::ImageLength:
			iSizeX = 当前标签->SHORT值;
			完成进度++;
			break;
		case TagID::ImageWidth:
			iSizeY = 当前标签->SHORT值;
			完成进度++;
			break;
		}
		当前标签++;
	}
	下个IFD = 下个IFD指针.NextIFD();
}
UINT32 BigTiffReader::缓存全部()
{
	while (下个IFD.偏移)
		IFD像素指针.push_back(读像素指针(下个IFD));
	return IFD像素指针.size();
}
void BigTiffReader::缓存到(UINT32 IFD)
{
	while (IFD像素指针.size() <= IFD)
		IFD像素指针.push_back(读像素指针(下个IFD));
}