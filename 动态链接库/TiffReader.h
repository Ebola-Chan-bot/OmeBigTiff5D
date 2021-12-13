#pragma once
#include "ITiffReader.h"
#include "ReaderBase.h"
#include <vector>
class TiffReader :public ITiffReader, public ReaderBase
{
protected:
	std::vector<BYTE*> IFD像素指针;
	const BYTE* const* GetIFD像素指针()const override;
public:
	UINT16 SizeX()const override;
	UINT16 SizeY()const override;
	UINT32 SizeI()override;
	像素类型 PixelType()const override;
	UINT8 BytesPerSample()const override;
	//将*Range参数设为nullptr表示完整读取该维度
	void Read3D(UINT16 XSize, UINT16 YSize, UINT32 ISize, UINT64* XRange, UINT64* YRange, UINT64* IRange, BYTE* BytesOut)override;
	virtual ~TiffReader()noexcept;
	TiffReader()noexcept;
};