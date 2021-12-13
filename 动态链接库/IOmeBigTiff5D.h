#pragma once
#include "IOmeTiffReader.h"
class IOmeBigTiff5D :public IOmeTiffReader
{
public:
	//对于尺寸的修改，仅修改逻辑上的尺寸，不会修改实际像素值数组的排布，因此原图将可能发生错位。如果尺寸扩大，多出来的部分的初始值是未定义的
	virtual void 修改基本参数(UINT16 SizeX, UINT16 SizeY, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, 维度顺序 DimensionOrder, 像素类型 PixelType, const 颜色* Colors, const char* 文件名) = 0;
	/*
	仅修改逻辑上的尺寸，不会修改实际像素值数组的排布，因此原图将可能发生错位。如果尺寸扩大，多出来的部分的初始值是未定义的
	如果要修改多个参数，请使用“修改基本参数”一次调用完成，性能更高。
	*/
	virtual void SizeX(UINT16 Size) = 0;
	/*
	仅修改逻辑上的尺寸，不会修改实际像素值数组的排布，因此原图将可能发生错位。如果尺寸扩大，多出来的部分的初始值是未定义的
	如果要修改多个参数，请使用“修改基本参数”一次调用完成，性能更高。
	*/
	virtual void SizeY(UINT16 Size) = 0;
	/*
	仅修改逻辑上的尺寸，不会修改实际像素值数组的排布，因此原图将可能发生错位。如果尺寸扩大，多出来的部分的初始值是未定义的
	如果要修改多个参数，请使用“修改基本参数”一次调用完成，性能更高。
	*/
	virtual void SizeC(UINT8 Size) = 0;
	/*
	仅修改逻辑上的尺寸，不会修改实际像素值数组的排布，因此原图将可能发生错位。如果尺寸扩大，多出来的部分的初始值是未定义的
	如果要修改多个参数，请使用“修改基本参数”一次调用完成，性能更高。
	*/
	virtual void SizeZ(UINT8 Size) = 0;
	/*
	仅修改逻辑上的尺寸，不会修改实际像素值数组的排布，因此原图将可能发生错位。如果尺寸扩大，多出来的部分的初始值是未定义的
	如果要修改多个参数，请使用“修改基本参数”一次调用完成，性能更高。
	*/
	virtual void SizeT(UINT16 Size) = 0;
	/*
	仅修改逻辑上的维度顺序，不会修改实际像素值数组的排布，因此原图将可能发生错位。
	如果要修改多个参数，请使用“修改基本参数”一次调用完成，性能更高。
	*/
	virtual void DimensionOrder(维度顺序 DO) = 0;
	/*
	仅修改逻辑上的尺寸，不会修改实际像素值数组的排布，因此原图将可能发生错色。如果尺寸扩大，多出来的部分的初始值是未定义的
	如果要修改多个参数，请使用“修改基本参数”一次调用完成，性能更高。
	*/
	virtual void PixelType(像素类型 PT) = 0;
	/*	
	一次性修改所有通道的颜色。输入的颜色数组必须具有至少为SizeC的尺寸。
	如果要修改多个参数，请使用“修改基本参数”一次调用完成，性能更高。
	请确保输入的Colors参数为常量指针，否则数组的值将会被修改为当前文件中的颜色，而不是修改文件中的颜色
	*/
	virtual void ChannelColor(const 颜色* Colors) = 0;
	/*
	如需修改多个通道的颜色，请改用void ChannelColor(const 颜色* Colors)，性能更高。频繁调用此函数性能较低。
	如果要修改多个参数，请使用“修改基本参数”一次调用完成，性能更高。
	*/
	virtual void ChannelColor(颜色 Color, UINT8 C) = 0;
	/*
	OME规范要求文件名必须在文件内部也有一致的记录，因此要修改文件名就必须同时也修改这个属性。
	如果要修改多个参数，请使用“修改基本参数”一次调用完成，性能更高。
	*/
	virtual void FileName(const char* 文件名) = 0;
	//一般用于将其它文件中取得的图像描述直接拷贝进来。如果缺少或不具有正确的文件名和TiffData信息，将会自动纠正。字符串应当是0结尾的。
	virtual void ImageDescription(const char* WriteIn) = 0;
	//5D写入，给定要写入的目标位置，其每个维度的索引及尺寸。如果要顺序写入某个维度的全部位置，可将该维度索引指针设为nullptr。
	virtual void Write5D(UINT16 XSize, UINT16 YSize, UINT8 CSize, UINT8 ZSize, UINT16 TSize, UINT64* XRange, UINT64* YRange, UINT64* CRange, UINT64* ZRange, UINT64* TRange, const BYTE* BytesIn) = 0;
	//打开现存的有效的文件。如果文件不存在或格式不正确，将产生意外结果。
	virtual void 打开现存(LPCWSTR 文件路径) = 0;
	//尝试打开现存的有效的文件。如果文件不存在或格式不正确，将返回false。
	virtual bool 尝试打开(LPCWSTR 文件路径)noexcept = 0;
	//尝试打开现存的有效的OmeBigTiff5D文件。如果文件不存在或格式不正确，将用输入参数覆盖创建新文件。返回是否实施了打开而非创建
	virtual bool 打开或创建(LPCWSTR 文件路径, UINT16 SizeX, UINT16 SizeY, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, 维度顺序 DimensionOrder, 像素类型 PixelType) = 0;
	//无论目标文件是否存在，覆盖创建新文件。
	virtual void 覆盖创建(LPCWSTR 文件路径, UINT16 SizeX, UINT16 SizeY, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, 维度顺序 DimensionOrder, 像素类型 PixelType) = 0;
};
/*
返回接口指针，使用完毕后调用方应当负责delete。请始终使用指针访问对象，不要尝试复制对象的值。
获取对象后需继续调用“打开现存”、“尝试打开”、“打开或创建”或“覆盖创建”以操作磁盘上的文件。
*/
OmeBigTiff5D导出(IOmeBigTiff5D*) 创建OmeBigTiff5D();
OmeBigTiff5D导出(void) 销毁OmeBigTiff5D(IOmeBigTiff5D*);