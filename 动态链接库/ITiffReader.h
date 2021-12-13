#pragma once
#include "导出.h"
#include "像素类型.h"
class ITiffReader
{
public:
	virtual UINT16 SizeX()const = 0;
	virtual UINT16 SizeY()const = 0;
	virtual UINT32 SizeI() = 0;
	virtual 像素类型 PixelType()const = 0;
	virtual UINT8 BytesPerSample()const = 0;
	//将*Range参数设为nullptr表示完整读取该维度
	virtual void Read3D(UINT16 XSize, UINT16 YSize, UINT32 ISize, UINT64* XRange, UINT64* YRange, UINT64* IRange, BYTE* BytesOut) = 0;
};
/*
返回接口指针，使用完毕后调用方应当负责delete。请始终使用指针访问对象，不要尝试复制对象的值。
获取对象后需继续调用“打开现存”或“尝试打开”以操作磁盘上的文件。
调用方有义务确保指定的路径存在合法的Tiff文件。如果不确定，请使用“尝试创建”以免发生意外结果
*/
OmeBigTiff5D导出(ITiffReader*) 创建TiffReader(LPCWSTR 文件路径);
OmeBigTiff5D导出(void) 销毁TiffReader(ITiffReader*);