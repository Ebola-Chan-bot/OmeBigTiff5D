#pragma once
#include "导出.h"
#include "像素类型.h"
#include "尝试结果.h"
class ITiffReader
{
public:
	virtual UINT16 SizeX()const noexcept = 0;
	virtual UINT16 SizeY()const noexcept = 0;
	//不是常量，因为需要缓存
	virtual UINT32 SizeI()noexcept = 0;
	virtual 像素类型 PixelType()const noexcept = 0;
	virtual UINT8 BytesPerSample()const noexcept = 0;
	//将Range参数设为nullptr表示完整读取该维度，此时对应的Size值会被忽略。像素值的维度顺序是XYI
	virtual void 读入像素3D(UINT16 XSize, UINT16 YSize, UINT32 ISize, UINT64* XRange, UINT64* YRange, UINT64* IRange, BYTE* BytesOut) noexcept = 0;
	//直接返回指定位置像素存储位置的内部指针，只读
	virtual const BYTE* 内部像素指针3D(UINT16 X, UINT16 Y, UINT32 I) noexcept = 0;
};
/*
返回接口指针，使用完毕后调用方应当调用销毁。请始终使用指针访问对象，不要尝试复制对象的值。
获取对象后需继续调用“打开现存”或“尝试打开”以操作磁盘上的文件。
调用方有义务确保指定的路径存在合法的Tiff文件。如果不确定，请使用“尝试创建”以免发生意外结果
*/
OmeBigTiff5D导出(ITiffReader*) 创建TiffReader(LPCWSTR 文件路径);
OmeBigTiff5D导出(ITiffReader*) 尝试创建TiffReader(LPCWSTR 文件路径, 尝试结果& 结果);
OmeBigTiff5D导出(void) 销毁TiffReader(ITiffReader*);