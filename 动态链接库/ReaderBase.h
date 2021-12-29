#pragma once
#include "I使用内存映射文件.h"
#include "像素类型.h"
#include "IFD.h"
class ReaderBase :public I使用内存映射文件
{
protected:
	const char* 基地址;
	virtual ~ReaderBase()noexcept;
	UINT16 iSizeX;
	UINT16 iSizeY;
	UINT32 iSizeI = 0;
	像素类型 iPixelType;
	UINT8 iSizeP;
	template <NumberType Word, NumberType DWord>
	const BYTE* 读像素指针(IFD偏移<Word, DWord>& 当前IFD偏移)const;
	virtual void 缓存到(UINT32 IFD) = 0;
	virtual UINT32 缓存全部() = 0;
	void Read3DBase(UINT16 XSize, UINT16 YSize, UINT32 ISize, UINT64* XRange, UINT64* YRange, UINT64* IRange, BYTE* BytesOut);
	virtual const BYTE* const* GetIFD像素指针() = 0;
public:
	virtual void 加载文件(HANDLE 文件句柄);
};
extern template const BYTE* ReaderBase::读像素指针(IFD偏移<UINT16, UINT32>& 当前IFD偏移)const;
extern template const BYTE* ReaderBase::读像素指针(IFD偏移<UINT64, UINT64>& 当前IFD偏移)const;