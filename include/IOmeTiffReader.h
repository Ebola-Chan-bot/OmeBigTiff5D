#pragma once
#include "ITiffReader.h"
#include "颜色.h"
#include "维度顺序.h"
class IOmeTiffReader :public ITiffReader
{
public:
	//返回当前文件通道数
	virtual UINT8 SizeC()const noexcept = 0;
	//返回当前文件纵深层数
	virtual UINT8 SizeZ()const noexcept = 0;
	//返回当前文件时间帧数
	virtual UINT16 SizeT()const noexcept = 0;
	//返回当前文件维度顺序
	virtual 维度顺序 DimensionOrder()const noexcept = 0;
	//返回某一通道的颜色
	virtual 颜色 读ChannelColor(UINT8 C)const noexcept = 0;
	//调用方负责分配内存，本函数仅负责将当前各通道颜色写入该内存。内存空间不应小于SizeC。
	virtual void 读ChannelColor(颜色* Colors)const noexcept = 0;
	//返回图像描述字符串，具有0结尾。只读内部指针，调用方不应当修改其中的字符，否则将发生意外结果。
	virtual const char* ImageDescription()const noexcept = 0;
	//返回文件名字符串，具有0结尾。只读内部指针，调用方不应当修改其中的字符，否则将发生意外结果
	virtual const char* FileName()const noexcept = 0;
	/*
	5D读取函数。给定要读取的目标位置，其每个维度的索引及尺寸。如果要顺序读取某个维度的全部位置，可将该维度索引指针设为nullptr。此时对应的索引Size会被忽略，任何值均不影响结果。
	调用方应当负责为BytesOut分配足够的内存，本函数只负责写入该内存。
	需要缓存，因此不是常量
	*/
	virtual void 读入像素5D(UINT16 XSize, UINT16 YSize, UINT8 CSize, UINT8 ZSize, UINT16 TSize, UINT64* XRange, UINT64* YRange, UINT64* CRange, UINT64* ZRange, UINT64* TRange, BYTE* BytesOut)noexcept = 0;
	//直接返回指定位置像素存储位置的内部指针，只读
	virtual const BYTE* 内部像素指针5D(UINT16 X, UINT16 Y, UINT8 C, UINT8 Z, UINT16 T)noexcept = 0;
};
/*
返回接口指针，使用完毕后应当调用销毁。请始终使用指针访问对象，不要尝试复制对象的值。
获取对象后需继续调用“打开现存”或“尝试打开”以操作磁盘上的文件。
调用方有义务确保指定的路径存在合法的OmeTiff文件。如果不确定，请使用“尝试创建”以免发生意外结果
*/
OmeBigTiff5D导出(IOmeTiffReader*) 创建OmeTiffReader(LPCWSTR 文件路径);
OmeBigTiff5D导出(IOmeTiffReader*) 尝试创建OmeTiffReader(LPCWSTR 文件路径, 尝试结果& 结果);
OmeBigTiff5D导出(void) 销毁OmeTiffReader(IOmeTiffReader*);