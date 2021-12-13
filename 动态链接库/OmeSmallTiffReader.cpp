#include "pch.h"
#include "OmeSmallTiffReader.h"
#include "XML内存分配.h"
#include "SmallTiff文件头.h"
void OmeSmallTiffReader::载入图像描述(pugi::xml_document& XML文档)
{
	下个IFD = ((SmallTiff文件头*)基地址)->FirstIFD;
	const IFD指针<UINT16, UINT32> 下个IFD指针 = 下个IFD.取指针(基地址);
	Tag<UINT32>* 当前标签 = 下个IFD指针.Tags();
	const Tag<UINT32>* 标签尾 = 当前标签 + 下个IFD指针.NumberOfTags();
	UINT32 图像描述长度;
	UINT8 完成进度 = 0;
	while (完成进度 < 2 && 当前标签 < 标签尾)
	{
		switch (当前标签->Identifier)
		{
		case TagID::ImageDescription:
			图像描述长度 = 当前标签->NoValues;
			iImageDescription = XML内存分配(图像描述长度 + 1);
			memcpy(iImageDescription, 当前标签->ASCII偏移.取指针(基地址), 图像描述长度);
			iImageDescription[图像描述长度] = 0;
			XML文档.load_buffer_inplace_own(iImageDescription, 图像描述长度);
			完成进度++;
			break;
		case TagID::StripOffsets:
			IFD像素指针[0] = 当前标签->BYTE偏移.取指针(基地址);
			完成进度++;
			break;
		}
		当前标签++;
	}
	下个IFD = 下个IFD指针.NextIFD();
	已缓存数 = 1;
}
void OmeSmallTiffReader::缓存到(UINT32 IFD)
{
	while (已缓存数 <= IFD)
		IFD像素指针[已缓存数++] = 读像素指针(下个IFD);
}