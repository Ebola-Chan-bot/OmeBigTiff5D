#pragma once
#include "IOmeTiffReader.h"
class IOmeBigTiff5D :public IOmeTiffReader
{
public:
	/*
	对于尺寸的修改，仅修改逻辑上的尺寸，不会修改实际像素值数组的排布，因此原图将可能发生错位。如果尺寸扩大，多出来的部分的初始值是未定义的
	无需修改的值设为0、缺省或nullptr。
	*/
	virtual void 修改基本参数(UINT16 SizeX, UINT16 SizeY, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, 维度顺序 DimensionOrder, 像素类型 PixelType, const 颜色* Colors, const char* 文件名)noexcept = 0;
	/*
	仅修改逻辑上的尺寸，不会修改实际像素值数组的排布，因此原图将可能发生错位。如果尺寸扩大，多出来的部分的初始值是未定义的
	如果要修改多个参数，请使用“修改基本参数”一次调用完成，性能更高。
	*/
	virtual void SizeX(UINT16 Size)noexcept = 0;
	/*
	仅修改逻辑上的尺寸，不会修改实际像素值数组的排布，因此原图将可能发生错位。如果尺寸扩大，多出来的部分的初始值是未定义的
	如果要修改多个参数，请使用“修改基本参数”一次调用完成，性能更高。
	*/
	virtual void SizeY(UINT16 Size)noexcept = 0;
	/*
	仅修改逻辑上的尺寸，不会修改实际像素值数组的排布，因此原图将可能发生错位。如果尺寸扩大，多出来的部分的初始值是未定义的
	如果要修改多个参数，请使用“修改基本参数”一次调用完成，性能更高。
	*/
	virtual void SizeC(UINT8 Size)noexcept = 0;
	/*
	仅修改逻辑上的尺寸，不会修改实际像素值数组的排布，因此原图将可能发生错位。如果尺寸扩大，多出来的部分的初始值是未定义的
	如果要修改多个参数，请使用“修改基本参数”一次调用完成，性能更高。
	*/
	virtual void SizeZ(UINT8 Size)noexcept = 0;
	/*
	仅修改逻辑上的尺寸，不会修改实际像素值数组的排布，因此原图将可能发生错位。如果尺寸扩大，多出来的部分的初始值是未定义的
	如果要修改多个参数，请使用“修改基本参数”一次调用完成，性能更高。
	*/
	virtual void SizeT(UINT16 Size)noexcept = 0;
	/*
	仅修改逻辑上的维度顺序，不会修改实际像素值数组的排布，因此原图将可能发生错位。
	如果要修改多个参数，请使用“修改基本参数”一次调用完成，性能更高。
	*/
	virtual void DimensionOrder(维度顺序 DO)noexcept = 0;
	/*
	仅修改逻辑上的尺寸，不会修改实际像素值数组的排布，因此原图将可能发生错色。如果尺寸扩大，多出来的部分的初始值是未定义的
	如果要修改多个参数，请使用“修改基本参数”一次调用完成，性能更高。
	*/
	virtual void PixelType(像素类型 PT)noexcept = 0;
	/*	
	一次性修改所有通道的颜色。输入的颜色数组必须具有至少为SizeC的尺寸。
	如果要修改多个参数，请使用“修改基本参数”一次调用完成，性能更高。
	请确保输入的Colors参数为常量指针，否则数组的值将会被修改为当前文件中的颜色，而不是修改文件中的颜色
	由于ImageJ的Bug，含有红色分量的颜色会导致ImageJ无法解析整个OME XML。但生成的文件是正确的，仅仅是ImageJ的bug。
	*/
	virtual void 写ChannelColor(const 颜色* Colors)noexcept = 0;
	/*
	如需修改多个通道的颜色，请改用void ChannelColor(const 颜色* Colors)，性能更高。频繁调用此函数性能较低。
	如果要修改多个参数，请使用“修改基本参数”一次调用完成，性能更高。
	由于ImageJ的Bug，含有红色分量的颜色会导致ImageJ无法解析整个OME XML。但生成的文件是正确的，仅仅是ImageJ的bug。
	*/
	virtual void 写ChannelColor(UINT8 C, 颜色 Color)noexcept = 0;
	/*
	OME规范要求文件名必须在文件内部也有一致的记录，因此要修改文件名就必须同时也修改这个属性。
	如果要修改多个参数，请使用“修改基本参数”一次调用完成，性能更高。
	*/
	virtual void FileName(const char* 文件名)noexcept = 0;
	//一般用于将其它文件中取得的图像描述直接拷贝进来。如果缺少或不具有正确的文件名和TiffData信息，将会自动纠正。字符串应当是0结尾的。
	virtual void ImageDescription(const char* WriteIn)noexcept = 0;
	/*
	5D写出，给定要写入的目标位置，其每个维度的索引及尺寸。如果要顺序写入某个维度的全部位置，可将该维度索引指针设为nullptr。此时对应的Size会被忽略，任何值均不影响结果
	调用方应当保证写入的数据源BytesIn具有和目标一致的维度顺序。
	*/
	virtual void 写出像素(UINT16 XSize, UINT16 YSize, UINT16 Size2, UINT16 Size3, UINT16 Size4, UINT64* XRange,UINT64* YRange, UINT64* Range2, UINT64* Range3,UINT64* Range4,const BYTE* BytesIn)noexcept = 0;
	//打开现存文件，只需要读取权限
	virtual 尝试结果 只读打开(LPCWSTR 文件路径)noexcept = 0;
	//打开现存文件
	virtual 尝试结果 打开现存(LPCWSTR 文件路径)noexcept = 0;
	//尝试打开现存的有效的OmeBigTiff5D文件。如果文件不存在或格式不正确，将用输入参数覆盖创建新文件；否则指定的参数将被忽略。返回是否实施了打开而非创建。ChannelColors可以设为nullptr以使用默认白色。
	virtual 尝试结果 打开或创建(LPCWSTR 文件路径, UINT16 SizeX, UINT16 SizeY, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, 维度顺序 DimensionOrder, 像素类型 PixelType, const 颜色* ChannelColors,bool& 打开而非创建)noexcept = 0;
	//尝试打开现存的有效的OmeBigTiff5D文件。如果文件不存在或格式不正确，将用输入参数覆盖创建新文件；否则指定的参数将被忽略。返回是否实施了打开而非创建。输入的ImageDescription参数应当具有0结尾
	virtual 尝试结果 打开或创建(LPCWSTR 文件路径, const char* ImageDescription,bool& 打开而非创建)noexcept = 0;
	//无论目标文件是否存在，覆盖创建新文件。ChannelColors可以设为nullptr以使用默认白色。
	virtual 尝试结果 覆盖创建(LPCWSTR 文件路径, UINT16 SizeX, UINT16 SizeY, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, 维度顺序 DimensionOrder, 像素类型 PixelType, const 颜色* ChannelColors)noexcept = 0;
	//无论目标文件是否存在，覆盖创建新文件。输入的ImageDescription参数应当具有0结尾
	virtual 尝试结果 覆盖创建(LPCWSTR 文件路径, const char* ImageDescription)noexcept = 0;
};
/*
返回接口指针，使用完毕后应当调用销毁。请始终使用指针访问对象，不要尝试复制对象的值。
获取对象后需继续调用“打开现存”、“打开或创建”或“覆盖创建”以操作磁盘上的文件。
*/
OmeBigTiff5D导出(IOmeBigTiff5D*) 创建OmeBigTiff5D();
OmeBigTiff5D导出(void) 销毁OmeBigTiff5D(IOmeBigTiff5D*);