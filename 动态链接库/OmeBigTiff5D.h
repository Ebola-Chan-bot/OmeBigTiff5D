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
		char* 基地址;
		OmeBigTiff5D文件头* 文件头;
	};
	xml_document IDDoc;
	xml_node OME;
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
	xml_node* iChannels;
	Tag<UINT64>* FirstTags;
	//总是返回false
	bool 失败清理() noexcept;
	void 更改文件尺寸(LARGE_INTEGER 新尺寸);
	void 关闭映射()const;
	void 建立映射();
	BYTE* 像素指针()const;
	//返回图像描述是否被迫扩展了空间
	bool 更新图像描述并扩展文件(bool 必须重新生成, UINT32& SizeI, UINT32& SizePXY);
	bool 更新通道(UINT8 SizeC);
	void IFD更新必备(bool 必须重新生成);
public:
	virtual ~OmeBigTiff5D()noexcept;
	//ITiffReader实现
	//返回是否打开成功
	UINT16 SizeX()const override;
	UINT16 SizeY()const override;
	UINT32 SizeI()override;
	像素类型 PixelType()const override;
	UINT8 BytesPerSample()const override;
	//将*Range参数设为nullptr表示完整读取该维度
	void Read3D(UINT16 XSize, UINT16 YSize, UINT32 ISize, UINT64* XRange, UINT64* YRange, UINT64* IRange, BYTE* BytesOut) override;
	//IOmeTiffReader实现
	UINT8 SizeC()const override;
	UINT8 SizeZ()const override;
	UINT16 SizeT()const override;
	维度顺序 DimensionOrder()const override;
	颜色 ChannelColor(UINT8 C)const override;
	void ChannelColor(颜色* Colors)const override;
	//只读内部指针，调用方不应当修改其中的值
	const char* ImageDescription()const override;
	const char* FileName()const override;
	void Read5D(UINT16 XSize, UINT16 YSize, UINT8 CSize, UINT8 ZSize, UINT16 TSize, UINT64* XRange, UINT64* YRange, UINT64* CRange, UINT64* ZRange, UINT64* TRange, BYTE* BytesOut) override;
	//IOmeBigTiff5D实现
	void 打开现存(LPCWSTR 文件路径) override;
	//返回打开是否成功
	bool 尝试打开(LPCWSTR 文件路径)noexcept override;
	//返回是否实施了打开而非创建
	bool 打开或创建(LPCWSTR 文件路径, UINT16 SizeX, UINT16 SizeY, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, 维度顺序 DimensionOrder, 像素类型 PixelType) override;
	void 覆盖创建(LPCWSTR 文件路径, UINT16 SizeX, UINT16 SizeY, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, 维度顺序 DimensionOrder, 像素类型 PixelType) override;
	//对于尺寸的修改，仅修改逻辑上的尺寸，不会修改实际像素值数组的排布，因此原图将可能发生错位。如果尺寸扩大，多出来的部分的初始值是未定义的
	void 修改基本参数(UINT16 SizeX, UINT16 SizeY, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, 维度顺序 DimensionOrder, 像素类型 PixelType, const 颜色* Colors, const char* 文件名) override;
	void SizeX(UINT16 Size) override;
	void SizeY(UINT16 Size) override;
	void SizeC(UINT8 Size) override;
	void SizeZ(UINT8 Size) override;
	void SizeT(UINT16 Size) override;
	void DimensionOrder(维度顺序 DO) override;
	void PixelType(像素类型 PT) override;
	void ChannelColor(const 颜色* Colors) override;
	void ChannelColor(颜色 Color, UINT8 C) override;
	void FileName(const char* 文件名) override;
	//标准OME图像描述里不一定包含TiffData字段，可能需要手动添加
	void ImageDescription(const char* WriteIn) override;
	void Write5D(UINT16 XSize, UINT16 YSize, UINT8 CSize, UINT8 ZSize, UINT16 TSize, UINT64* XRange, UINT64* YRange, UINT64* CRange, UINT64* ZRange, UINT64* TRange, const BYTE* BytesIn) override;
};