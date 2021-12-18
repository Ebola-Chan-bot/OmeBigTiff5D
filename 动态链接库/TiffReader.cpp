#include "pch.h"
#include "TiffReader.h"
#include <algorithm>
#include "是否间断.h"
#include "N维切片参数.h"
TiffReader::~TiffReader()noexcept{}
TiffReader::TiffReader()noexcept
{
	iSizeI = 0;
}
UINT16 TiffReader::SizeX()const noexcept
{
	return iSizeX;
}
UINT16 TiffReader::SizeY()const noexcept
{
	return iSizeY;
}
像素类型 TiffReader::PixelType()const noexcept
{
	return iPixelType;
}
UINT8 TiffReader::BytesPerSample()const noexcept
{
	return iBytesPerSample;
}
UINT32 TiffReader::SizeI() noexcept
{
	if (!iSizeI)
		iSizeI = 缓存全部();
	return iSizeI;
}
const BYTE* const* TiffReader::GetIFD像素指针()const
{
	return IFD像素指针.data();
}
void TiffReader::读入像素3D(UINT16 XSize, UINT16 YSize, UINT32 ISize, UINT64* XRange, UINT64* YRange, UINT64* IRange, BYTE* BytesOut) noexcept
{
	Read3DBase(XSize, YSize, ISize, XRange, YRange, IRange, BytesOut);
}
const BYTE* TiffReader::内部像素指针3D(UINT16 X, UINT16 Y, UINT32 I) noexcept
{
	缓存到(I);
	return IFD像素指针[I] + (UINT32(Y) * iSizeX + X) * iBytesPerSample;
}