#pragma once
#include "IOmeBigTiff5D.h"
#include "OmeBigTiff5D文件头.h"
#include "I使用内存映射文件.h"
#include "pugixml.hpp"
using namespace pugi;
class OmeBigTiff5D :public IOmeBigTiff5D, I使用内存映射文件
{
private:
	union
	{
		BYTE* 基地址;
		OmeBigTiff5D文件头* 文件头;
	};
	BYTE* 末地址;
	xml_document IDDoc;
	xml_attribute 唯一标识符;
	xml_attribute 文件名;
	xml_node Pixels;
	xml_attribute iSizeX;
	xml_attribute iSizeY;
	xml_attribute iSizeC;
	xml_attribute iSizeZ;
	xml_attribute iSizeT;
	xml_attribute iDimensionOrder;
	xml_attribute iPixelType;
	xml_node* iChannels = nullptr;
	IFD5D* FirstIFD;
	//总是返回false
	void 失败清理() noexcept;
	void 更改文件尺寸(LARGE_INTEGER 新尺寸);
	void 关闭映射()const;
	void 建立映射();
	BYTE* 像素指针()const;
	//返回图像描述是否被迫扩展了空间
	bool 更新图像描述并扩展文件(bool 必须重新生成, UINT32& SizeI, UINT32& SizePXY);
	bool 更新通道(UINT8 SizeC);
	void IFD更新必备(bool 必须重新生成);
	void Pixels节点缓存();
	void IDDoc解析(const char* ImageDescription, const char* 文件名);
	void 已知IDDoc构造文件();
	UINT32 i各维尺寸[5];
	void 生成各维尺寸();
	void 打开Core(LARGE_INTEGER& 文件尺寸, UINT64& 最小文件尺寸);
public:
	virtual ~OmeBigTiff5D()noexcept;
	//ITiffReader实现
	//返回是否打开成功
	UINT16 SizeX()const noexcept override;
	UINT16 SizeY()const noexcept override;
	UINT32 SizeI()noexcept override;
	像素类型 PixelType()const noexcept override;
	UINT8 SizeP()const noexcept override;
	BYTE* 内部像素指针3D(UINT16 X, UINT16 Y, UINT32 I)noexcept override;
	//将*Range参数设为nullptr表示完整读取该维度
	void 读入像素3D(UINT16 XSize, UINT16 YSize, UINT32 ISize, UINT64* XRange,UINT64* YRange, UINT64* IRange, BYTE* BytesOut)noexcept override;
	//IOmeTiffReader实现
	UINT8 SizeC()const noexcept override;
	UINT8 SizeZ()const noexcept override;
	UINT16 SizeT()const noexcept override;
	维度顺序 DimensionOrder()const noexcept override;
	颜色 读ChannelColor(UINT8 C)const noexcept override;
	void 读ChannelColor(颜色* Colors)const noexcept override;
	//只读内部指针，调用方不应当修改其中的值
	const char* ImageDescription()const noexcept override;
	const char* FileName()const noexcept override;
	尝试结果 读入像素5D(UINT16 XSize, UINT16 YSize, UINT16 Size2, UINT16 Size3, UINT16 Size4, UINT64* XRange, UINT64* YRange, UINT64* Range2, UINT64* Range3,UINT64* Range4, BYTE* BytesOut)noexcept override;
	BYTE* 内部像素指针5D(UINT16 X, UINT16 Y, UINT16 P2, UINT16 P3, UINT16 P4)noexcept override;
	//IOmeBigTiff5D实现
	尝试结果 只读打开(LPCWSTR 文件路径)noexcept override;
	尝试结果 打开现存(LPCWSTR 文件路径) noexcept override;
	//返回是否实施了打开而非创建
	尝试结果 打开或创建(LPCWSTR 文件路径, UINT16 SizeX, UINT16 SizeY, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, 维度顺序 DimensionOrder, 像素类型 PixelType, const 颜色* ChannelColors,bool& 打开而非创建) noexcept override;
	尝试结果 打开或创建(LPCWSTR 文件路径, const char* ImageDescription,bool& 打开而非创建) noexcept override;
	尝试结果 覆盖创建(LPCWSTR 文件路径, UINT16 SizeX, UINT16 SizeY, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, 维度顺序 DimensionOrder, 像素类型 PixelType, const 颜色* ChannelColors) noexcept override;
	尝试结果 覆盖创建(LPCWSTR 文件路径,const char* ImageDescription) noexcept override;
	//对于尺寸的修改，仅修改逻辑上的尺寸，不会修改实际像素值数组的排布，因此原图将可能发生错位。如果尺寸扩大，多出来的部分的初始值是未定义的
	void 修改基本参数(UINT16 SizeX, UINT16 SizeY, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, 维度顺序 DimensionOrder, 像素类型 PixelType, const 颜色* Colors, const char* 文件名) noexcept override;
	void SizeX(UINT16 Size) noexcept override;
	void SizeY(UINT16 Size) noexcept override;
	void SizeC(UINT8 Size) noexcept override;
	void SizeZ(UINT8 Size) noexcept override;
	void SizeT(UINT16 Size) noexcept override;
	void DimensionOrder(维度顺序 DO) noexcept override;
	void PixelType(像素类型 PT) noexcept override;
	void 写ChannelColor(const 颜色* Colors) noexcept override;
	void 写ChannelColor(UINT8 C, 颜色 Color) noexcept override;
	void FileName(const char* 文件名) noexcept override;
	//标准OME图像描述里不一定包含TiffData字段，可能需要手动添加
	void ImageDescription(const char* WriteIn) noexcept override;
	void 写出像素(UINT16 XSize, UINT16 YSize, UINT16 Size2, UINT16 Size3, UINT16 Size4,UINT64* XRange, UINT64* YRange,UINT64* Range2, UINT64* Range3, UINT64* Range4, const BYTE* BytesIn)noexcept override;
	const UINT32* 各维尺寸()const noexcept override;
};