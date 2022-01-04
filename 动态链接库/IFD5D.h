#pragma once
#include "Tag.h"
#pragma pack(4)
struct IFD5D
{
	UINT64 NumberOfTags = 13;
	Tag<UINT64> 图像描述;
	Tag<UINT64> 像素偏移;
	Tag<UINT64> 图像宽度;
	Tag<UINT64> 图像长度;
	Tag<UINT64> 每个样本的位数;
	Tag<UINT64> 每条行数;
	Tag<UINT64> 像素字节数;
	Tag<UINT64> 样本格式;
	Tag<UINT64> 压缩;
	Tag<UINT64> 光度解释;
	Tag<UINT64> X分辨率;
	Tag<UINT64> Y分辨率;
	Tag<UINT64> 分辨率单位;
	文件偏移<UINT64, IFD5D> NextIFD;
	IFD5D(UINT64 ImageDescription长度, 文件偏移<UINT64, char>ImageDescription偏移, 文件偏移<UINT64, BYTE> StripOffsets, UINT8 SizeP, UINT16 SizeX, UINT16 SizeY, SampleFormat SampleFormat)noexcept;
};
#pragma pack(4)