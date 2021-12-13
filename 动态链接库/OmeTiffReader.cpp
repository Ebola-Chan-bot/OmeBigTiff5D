#include "pch.h"
#include "OmeTiffReader.h"
#include "像素类型字符串.h"
#include "维度顺序字符串.h"
#include "寻找字符串.h"
#include "像素类型尺寸.h"
#include "XML内存释放.h"
#include "CZT重排.h"
#include "下标转索引.h"
using namespace pugi;
OmeTiffReader::~OmeTiffReader()noexcept
{
	free(iChannelColor);
	free(IFD像素指针);
	XML内存释放(iImageDescription);
}
void OmeTiffReader::加载文件(HANDLE 文件句柄)
{
	ReaderBase::加载文件(文件句柄);
	IFD像素指针 = (const BYTE**)malloc(sizeof(BYTE*) * iSizeI);
	xml_document IDDoc;
	载入图像描述(IDDoc);
	xml_node 节点 = IDDoc.child("Image");
	iFileName = 节点.attribute("Name").value();
	节点 = 节点.child("Pixels");
	iSizeX = 节点.attribute("SizeX").as_uint();
	iSizeY = 节点.attribute("SizeY").as_uint();
	iSizeC = 节点.attribute("SizeC").as_uint();
	iSizeZ = 节点.attribute("SizeZ").as_uint();
	iSizeT = 节点.attribute("SizeT").as_uint();
	iSizeI = UINT32(iSizeC) * iSizeZ * iSizeT;
	iDimensionOrder = 维度顺序(寻找字符串(节点.attribute("DimensionOrder").value(), 维度顺序字符串, 维度顺序个数));
	const UINT8 像素类型序号 = 寻找字符串(节点.attribute("Type").value(), 像素类型字符串, 像素类型个数);
	iPixelType = 像素类型(像素类型序号);
	iBytesPerSample = 像素类型尺寸[像素类型序号];
	iChannelColor = (颜色*)malloc(sizeof(颜色) * iSizeC);
	xml_named_node_iterator 通道迭代器 = 节点.children("Channel").begin();
	xml_attribute 颜色属性;
	for (UINT8 C = 0; C < iSizeC; ++C)
		iChannelColor[C] = (颜色属性 = (通道迭代器++)->attribute("Color")) ? 颜色{ .整数值 = 颜色属性.as_uint() } : 颜色{ .整数值 = UINT32(-1) };
}
UINT16 OmeTiffReader::SizeX()const
{
	return iSizeX;
}
UINT16 OmeTiffReader::SizeY()const
{
	return iSizeY;
}
UINT8 OmeTiffReader::SizeC()const
{
	return iSizeC;
}
UINT8 OmeTiffReader::SizeZ()const
{
	return iSizeZ;
}
UINT16 OmeTiffReader::SizeT()const
{
	return iSizeT;
}
UINT32 OmeTiffReader::SizeI()
{
	return iSizeI;
}
像素类型 OmeTiffReader::PixelType()const
{
	return iPixelType;
}
UINT8 OmeTiffReader::BytesPerSample()const
{
	return iBytesPerSample;
}
维度顺序 OmeTiffReader::DimensionOrder()const
{
	return iDimensionOrder;
}
颜色 OmeTiffReader::ChannelColor(UINT8 C)const
{
	return iChannelColor[C];
}
void OmeTiffReader::ChannelColor(颜色* Colors)const
{
	std::copy_n(iChannelColor, iSizeC, Colors);
}
const char* OmeTiffReader::ImageDescription()const
{
	return iImageDescription;
}
const char* OmeTiffReader::FileName()const
{
	return iFileName;
}
const BYTE* const* OmeTiffReader::GetIFD像素指针()const
{
	return IFD像素指针;
}
void OmeTiffReader::Read3D(UINT16 XSize, UINT16 YSize, UINT32 ISize, UINT64* XRange, UINT64* YRange, UINT64* IRange, BYTE* BytesOut)
{
	Read3DBase(XSize, YSize, ISize, XRange, YRange, IRange, BytesOut);
}
void OmeTiffReader::Read5D(UINT16 XSize, UINT16 YSize, UINT8 CSize, UINT8 ZSize, UINT16 TSize, UINT64* XRange, UINT64* YRange, UINT64* CRange, UINT64* ZRange, UINT64* TRange, BYTE* BytesOut)
{
	UINT32 各维尺寸[3];
	UINT32 下标长度[3];
	UINT64* 下标[3];
	const UINT16 CZTSize[] = { CSize,ZSize,TSize };
	UINT64* CZTRange[] = { CRange,ZRange,TRange };
	const UINT16 SizeCZT[] = { iSizeC,iSizeZ,iSizeT };
	CZT重排(各维尺寸, 下标长度, 下标, CZTSize, CZTRange, SizeCZT, 维度顺序字符串[UINT8(iDimensionOrder)] + 2);
	偏移向量 IFD索引;
	下标转索引(1, 3, 各维尺寸, 下标长度, 下标, IFD索引);
	Read3DBase(XSize, YSize, IFD索引.size(), XRange, YRange, IFD索引.data(), BytesOut);
}
UINT32 OmeTiffReader::缓存全部()
{
	缓存到(iSizeI - 1);
	return iSizeI;
}