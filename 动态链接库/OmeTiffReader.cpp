#include "pch.h"
#include "OmeTiffReader.h"
#include "像素类型字符串.h"
#include "维度顺序字符串.h"
#include "寻找字符串.h"
#include "像素类型尺寸.h"
#include "XML内存释放.h"
#include "CZT重排.h"
#include "下标转索引.h"
#include "Tiff异常.h"
#include "XML文本接收器.h"
using namespace pugi;
OmeTiffReader::~OmeTiffReader()noexcept
{
	free(iChannelColor);
	free(IFD像素指针);
}
void OmeTiffReader::加载文件(HANDLE 文件句柄)
{
	ReaderBase::加载文件(文件句柄);
	xml_document IDDoc;
	载入图像描述(IDDoc);
	xml_node 节点 = IDDoc.child("OME").child("Image");
	iFileName = 节点.attribute("Name").value();
	节点 = 节点.child("Pixels");
	iSizeX = 节点.attribute("SizeX").as_uint();
	iSizeY = 节点.attribute("SizeY").as_uint();
	iSizeC = 节点.attribute("SizeC").as_uint();
	iSizeZ = 节点.attribute("SizeZ").as_uint();
	iSizeT = 节点.attribute("SizeT").as_uint();
	iSizeI = UINT32(iSizeC) * iSizeZ * iSizeT;
	IFD像素指针 = (const BYTE**)malloc(sizeof(BYTE*) * iSizeI);
	iDimensionOrder = 维度顺序(寻找字符串(节点.attribute("DimensionOrder").value(), 维度顺序字符串, 维度顺序个数));
	const UINT8 像素类型序号 = 寻找字符串(节点.attribute("Type").value(), 像素类型字符串, 像素类型个数);
	iPixelType = 像素类型(像素类型序号);
	iBytesPerSample = 像素类型尺寸[像素类型序号];
	iChannelColor = (颜色*)malloc(sizeof(颜色) * iSizeC);
	xml_named_node_iterator 通道迭代器 = 节点.children("Channel").begin();
	xml_attribute 颜色属性;
	for (UINT8 C = 0; C < iSizeC; ++C)
		iChannelColor[C] = (颜色属性 = (通道迭代器++)->attribute("Color")) ? 颜色{ .整数值 = 颜色属性.as_uint() } : 颜色{ .整数值 = UINT32(-1) };
	XML文本接收器 接收器;
	IDDoc.save(接收器);
	iImageDescription = std::move(接收器.缓存);
}
void OmeTiffReader::尝试加载(HANDLE 文件句柄)
{
	LARGE_INTEGER 文件大小{ .QuadPart = 0 };
	SetFilePointerEx(文件句柄, 文件大小, &文件大小, FILE_END);
	ReaderBase::尝试加载(文件句柄);
	const char* 尾指针 = 基地址 + 文件大小.QuadPart;
	xml_document IDDoc;
	尝试载入图像描述(IDDoc, 尾指针);
	xml_node 节点 = IDDoc.child("OME");
	if (!节点)
		throw Tiff异常(Tiff异常类型::OME规范, "OME XML 缺少OME节点");
	if (!(节点 = 节点.child("Image")))
		throw Tiff异常(Tiff异常类型::OME规范, "OME节点下缺少Image节点");
	xml_attribute 属性;
	if (属性 = 节点.attribute("Name"))
		iFileName = 属性.value();
	else
		throw Tiff异常(Tiff异常类型::OME规范, "Image节点缺少Name属性");
	if (!(节点 = 节点.child("Pixels")))
		throw Tiff异常(Tiff异常类型::OME规范, "Image节点下缺少Pixels节点");
	if(属性 = 节点.attribute("SizeX"))
		iSizeX =属性.as_uint();
	else
		throw Tiff异常(Tiff异常类型::OME规范, "Pixels节点缺少SizeX属性");
	if (属性 = 节点.attribute("SizeY"))
		iSizeY = 属性.as_uint();
	else
		throw Tiff异常(Tiff异常类型::OME规范, "Pixels节点缺少SizeY属性");
	if (属性 = 节点.attribute("SizeC"))
		iSizeC = 属性.as_uint();
	else
		throw Tiff异常(Tiff异常类型::OME规范, "Pixels节点缺少SizeC属性");
	if (属性 = 节点.attribute("SizeZ"))
		iSizeZ = 属性.as_uint();
	else
		throw Tiff异常(Tiff异常类型::OME规范, "Pixels节点缺少SizeZ属性");
	if (属性 = 节点.attribute("SizeT"))
		iSizeT = 属性.as_uint();
	else
		throw Tiff异常(Tiff异常类型::OME规范, "Pixels节点缺少SizeT属性");
	iSizeI = UINT32(iSizeC) * iSizeZ * iSizeT;
	IFD像素指针 = (const BYTE**)malloc(sizeof(BYTE*) * iSizeI);
	if (!IFD像素指针)
		throw Tiff异常(Tiff异常类型::文件太大, "从OME XML读取到的SizeI过大，无法分配足够内存。");
	if (!(属性 = 节点.attribute("DimensionOrder")))
		throw Tiff异常(Tiff异常类型::OME规范, "Pixels节点缺少DimensionOrder属性");
	INT8 序号;
	if((序号= 寻找字符串(属性.value(), 维度顺序字符串, 维度顺序个数))<0)
		throw Tiff异常(Tiff异常类型::OME规范, "DimensionOrder属性值无效");
	iDimensionOrder = 维度顺序(序号);
	if (!(属性 = 节点.attribute("Type")))
		throw Tiff异常(Tiff异常类型::OME规范, "Pixels节点缺少Type属性");
	if ((序号 = 寻找字符串(属性.value(), 像素类型字符串, 像素类型个数)) < 0)
		throw Tiff异常(Tiff异常类型::OME规范, "Type属性值无效");
	iPixelType = 像素类型(序号);
	iBytesPerSample = 像素类型尺寸[序号];
	iChannelColor = (颜色*)malloc(sizeof(颜色) * iSizeC);
	xml_object_range<xml_named_node_iterator> 通道范围 = 节点.children("Channel");
	xml_named_node_iterator 通道迭代器 = 通道范围.begin();
	xml_named_node_iterator 迭代器尾 = 通道范围.end();
	xml_attribute 颜色属性;
	for (UINT8 C = 0; C < iSizeC && 通道迭代器 != 迭代器尾; ++C)
		iChannelColor[C] = (颜色属性 = (通道迭代器++)->attribute("Color")) ? 颜色{ .整数值 = 颜色属性.as_uint() } : 颜色{ .整数值 = UINT32(-1) };
	XML文本接收器 接收器;
	IDDoc.save(接收器);
	iImageDescription = std::move(接收器.缓存);
}
UINT16 OmeTiffReader::SizeX()const noexcept
{
	return iSizeX;
}
UINT16 OmeTiffReader::SizeY()const noexcept
{
	return iSizeY;
}
UINT8 OmeTiffReader::SizeC()const noexcept
{
	return iSizeC;
}
UINT8 OmeTiffReader::SizeZ()const noexcept
{
	return iSizeZ;
}
UINT16 OmeTiffReader::SizeT()const noexcept
{
	return iSizeT;
}
UINT32 OmeTiffReader::SizeI() noexcept
{
	return iSizeI;
}
像素类型 OmeTiffReader::PixelType()const noexcept
{
	return iPixelType;
}
UINT8 OmeTiffReader::BytesPerSample()const noexcept
{
	return iBytesPerSample;
}
维度顺序 OmeTiffReader::DimensionOrder()const noexcept
{
	return iDimensionOrder;
}
颜色 OmeTiffReader::读ChannelColor(UINT8 C)const noexcept
{
	return iChannelColor[C];
}
void OmeTiffReader::读ChannelColor(颜色* Colors)const noexcept
{
	std::copy_n(iChannelColor, iSizeC, Colors);
}
const char* OmeTiffReader::ImageDescription()const noexcept
{
	return iImageDescription.c_str();
}
const char* OmeTiffReader::FileName()const noexcept
{
	return iFileName;
}
const BYTE* const* OmeTiffReader::GetIFD像素指针()const
{
	return IFD像素指针;
}
void OmeTiffReader::读入像素3D(UINT16 XSize, UINT16 YSize, UINT32 ISize, UINT64* XRange, UINT64* YRange, UINT64* IRange, BYTE* BytesOut) noexcept
{
	Read3DBase(XSize, YSize, ISize, XRange, YRange, IRange, BytesOut);
}
void OmeTiffReader::读入像素5D(UINT16 XSize, UINT16 YSize, UINT8 CSize, UINT8 ZSize, UINT16 TSize, UINT64* XRange, UINT64* YRange, UINT64* CRange, UINT64* ZRange, UINT64* TRange, BYTE* BytesOut) noexcept
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
const BYTE* OmeTiffReader::内部像素指针3D(UINT16 X, UINT16 Y, UINT32 I) noexcept
{
	缓存到(I);
	return IFD像素指针[I] + (UINT32(Y) * iSizeX + X) * iBytesPerSample;
}
const BYTE* OmeTiffReader::内部像素指针5D(UINT16 X, UINT16 Y, UINT8 C, UINT8 Z, UINT16 T) noexcept
{
	UINT32 偏移 = 0;
	const char* DO = 维度顺序字符串[UINT8(iDimensionOrder)] + 4;
	const char* const DOE = DO - 3;
	while (DO > DOE)
		switch (*(DO--))
		{
		case 'C':
			偏移 = 偏移 * iSizeC + C;
			break;
		case 'Z':
			偏移 = 偏移 * iSizeZ + Z;
			break;
		case 'T':
			偏移 = 偏移 * iSizeT + T;
			break;
		}
	缓存到(偏移);
	return IFD像素指针[偏移] + (UINT32(Y) * iSizeX + X) * iBytesPerSample;
}