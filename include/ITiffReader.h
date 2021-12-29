#pragma once
#include "导出.h"
#include "像素类型.h"
#include "尝试结果.h"
class ITiffReader
{
public:
	virtual UINT16 SizeX()const noexcept = 0;
	virtual UINT16 SizeY()const noexcept = 0;
	//即IFD总数。
	virtual UINT32 SizeI()noexcept = 0;
	virtual 像素类型 PixelType()const noexcept = 0;
	//每个像素的字节数
	virtual UINT8 SizeP()const noexcept = 0;
	//将Range参数设为nullptr表示完整读取该维度，此时对应的Size值会被忽略。像素值的维度顺序是XYI
	virtual void 读入像素3D(UINT16 XSize, UINT16 YSize, UINT32 ISize,UINT64* XRange,  UINT64* YRange,  UINT64* IRange, BYTE* BytesOut)noexcept = 0;
	//直接返回指定位置像素存储位置的内部指针。只有IOmeBigTiff5D继承的该函数可以返回跨IFD访问指针。该内存只读，尝试写入将发生意外结果。
	virtual BYTE* 内部像素指针3D(UINT16 X, UINT16 Y, UINT32 I)noexcept = 0;
};
/*
返回接口指针。请始终使用指针访问对象，不要尝试复制对象的值。
创建可能失败，即使文件指针返回非0值。请始终检查返回结果。如果创建失败，则对象已销毁，不要再次销毁。
创建成功的对象，使用完毕后调用方应当调用销毁函数。
*/
OmeBigTiff5D导出(尝试结果) 创建TiffReader(LPCWSTR 文件路径,ITiffReader*& 文件指针);
//成功创建的对象，使用后务必使用此函数销毁。
OmeBigTiff5D导出(void) 销毁TiffReader(ITiffReader*);