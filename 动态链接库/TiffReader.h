#pragma once
#include "ITiffReader.h"
#include "ReaderBase.h"
#include <vector>
class TiffReader :public ITiffReader, public ReaderBase
{
protected:
	std::vector<const BYTE*> IFD像素指针;
	const BYTE* const* GetIFD像素指针()const override;
public:
	UINT16 SizeX()const noexcept override;
	UINT16 SizeY()const noexcept override;
	UINT32 SizeI()noexcept override;
	像素类型 PixelType()const noexcept override;
	UINT8 BytesPerSample()const noexcept override;
	//将*Range参数设为nullptr表示完整读取该维度
	void 读入像素3D(UINT16 XSize, UINT16 YSize, UINT32 ISize, UINT64* XRange, UINT64* YRange, UINT64* IRange, BYTE* BytesOut)noexcept override;
	const BYTE* 内部像素指针3D(UINT16 X, UINT16 Y, UINT32 I)noexcept override;
	virtual ~TiffReader()noexcept;
	TiffReader()noexcept;
};