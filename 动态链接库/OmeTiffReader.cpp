#include "pch.h"
#include "OmeTiffReader.h"
#include "像素类型字符串.h"
#include "维度顺序字符串.h"
#include "寻找字符串.h"
#include "像素类型尺寸.h"
#include "XML内存释放.h"
#include "下标转索引.h"
#include "XML文本接收器.h"
using namespace pugi;
OmeTiffReader::~OmeTiffReader()noexcept
{
	free(iChannelColor);
	free(IFD像素指针);
}
void OmeTiffReader::加载文件(HANDLE 文件句柄)
{
	LARGE_INTEGER 文件大小{ .QuadPart = 0 };
	SetFilePointerEx(文件句柄, 文件大小, &文件大小, FILE_END);
	ReaderBase::加载文件(文件句柄);
	末地址 = 基地址 + 文件大小.QuadPart;
	xml_document IDDoc;
	载入图像描述(IDDoc);
	xml_node 节点 = IDDoc.child("OME");
	if (!节点)
		throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::OME规范,.错误消息 = "OME XML 缺少OME节点" };
	if (!(节点 = 节点.child("Image")))
		throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::OME规范,.错误消息 = "OME节点下缺少Image节点" };
	xml_attribute 属性;
	if (属性 = 节点.attribute("Name"))
		iFileName = 属性.value();
	else
		throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::OME规范,.错误消息 = "Image节点缺少Name属性" };
	if (!(节点 = 节点.child("Pixels")))
		throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::OME规范,.错误消息 = "Image节点下缺少Pixels节点" };
	if(属性 = 节点.attribute("SizeX"))
		iSizeX =属性.as_uint();
	else
		throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::OME规范,.错误消息 = "Pixels节点缺少SizeX属性" };
	if (属性 = 节点.attribute("SizeY"))
		iSizeY = 属性.as_uint();
	else
		throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::OME规范,.错误消息 = "Pixels节点缺少SizeY属性" };
	if (属性 = 节点.attribute("SizeC"))
		iSizeC = 属性.as_uint();
	else
		throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::OME规范,.错误消息 = "Pixels节点缺少SizeC属性" };
	if (属性 = 节点.attribute("SizeZ"))
		iSizeZ = 属性.as_uint();
	else
		throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::OME规范,.错误消息 = "Pixels节点缺少SizeZ属性" };
	if (属性 = 节点.attribute("SizeT"))
		iSizeT = 属性.as_uint();
	else
		throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::OME规范,.错误消息 = "Pixels节点缺少SizeT属性" };
	iSizeI = UINT32(iSizeC) * iSizeZ * iSizeT;
	IFD像素指针 = (const BYTE**)malloc(sizeof(BYTE*) * iSizeI);
	if (!IFD像素指针)
		throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::文件太大,.错误消息 = "从OME XML读取到的SizeI过大，无法分配足够内存" };
	if (!(属性 = 节点.attribute("DimensionOrder")))
		throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::OME规范,.错误消息 = "Pixels节点缺少DimensionOrder属性" };
	INT8 序号;
	if((序号= 寻找字符串(属性.value(), 维度顺序字符串, 维度顺序个数))<0)
		throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::OME规范,.错误消息 = "DimensionOrder属性值无效" };
	iDimensionOrder = 维度顺序(序号);
	i各维尺寸[0] = iSizeX;
	i各维尺寸[1] = iSizeY;
	const char* const DO = 维度顺序字符串[序号];
	for (UINT8 D = 2; D < 5; ++D)
		switch (DO[D])
		{
		case 'C':
			i各维尺寸[D] = iSizeC;
			break;
		case 'Z':
			i各维尺寸[D] = iSizeZ;
			break;
		case 'T':
			i各维尺寸[D] = iSizeT;
			break;
		}
	if (!(属性 = 节点.attribute("Type")))
		throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::OME规范,.错误消息 = "Pixels节点缺少Type属性" };
	if ((序号 = 寻找字符串(属性.value(), 像素类型字符串, 像素类型个数)) < 0)
		throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::OME规范,.错误消息 = "Type属性值无效" };
	iPixelType = 像素类型(序号);
	iSizeP = 像素类型尺寸[序号];
	iChannelColor = (颜色*)malloc(sizeof(颜色) * iSizeC);
	xml_object_range<xml_named_node_iterator> 通道范围 = 节点.children("Channel");
	xml_named_node_iterator 通道迭代器 = 通道范围.begin();
	xml_named_node_iterator 迭代器尾 = 通道范围.end();
	xml_attribute 颜色属性;
	for (UINT8 C = 0; C < iSizeC && 通道迭代器 != 迭代器尾; ++C)
		iChannelColor[C] = (颜色属性 = (通道迭代器++)->attribute("Color")) ? 颜色{ .整数值 = 颜色属性.as_int() } : 颜色{ .整数值 = -1 };
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
UINT32 OmeTiffReader::SizeI()noexcept
{
	return iSizeI;
}
像素类型 OmeTiffReader::PixelType()const noexcept
{
	return iPixelType;
}
UINT8 OmeTiffReader::SizeP()const noexcept
{
	return iSizeP;
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
const BYTE* const* OmeTiffReader::GetIFD像素指针()
{
	return IFD像素指针;
}
void OmeTiffReader::读入像素3D(UINT16 XSize, UINT16 YSize, UINT32 ISize, UINT64* XRange, UINT64* YRange,UINT64* IRange, BYTE* BytesOut) noexcept
{
	Read3DBase(XSize, YSize, ISize, XRange, YRange, IRange, BytesOut);
}
尝试结果 OmeTiffReader::读入像素5D(UINT16 XSize, UINT16 YSize, UINT16 Size2, UINT16 Size3, UINT16 Size4, UINT64* XRange,  UINT64* YRange, UINT64* Range2,UINT64* Range3, UINT64* Range4, BYTE* BytesOut) noexcept
{
	UINT32 下标长度[3] = { Size2,Size3,Size4 };
	UINT64* 下标[3] = { Range2,Range3,Range4 };
	偏移向量 IFD索引;
	下标转索引(1, 3, i各维尺寸+2, 下标长度, 下标, IFD索引);
	Read3DBase(XSize, YSize, IFD索引.size(), XRange, YRange, IFD索引.data(), BytesOut);
	return 尝试结果{ .结果 = 结果分类::成功 };
}
UINT32 OmeTiffReader::缓存全部()
{
	缓存到(iSizeI - 1);
	return iSizeI;
}
BYTE* OmeTiffReader::内部像素指针3D(UINT16 X, UINT16 Y, UINT32 I)noexcept
{
	缓存到(I);
	return (BYTE*)IFD像素指针[I] + (UINT32(Y) * iSizeX + X) * iSizeP;
}
BYTE* OmeTiffReader::内部像素指针5D(UINT16 X, UINT16 Y, UINT16 P2, UINT16 P3, UINT16 P4)noexcept
{
	const UINT32 偏移 = (P4 * i各维尺寸[3] + P3) * i各维尺寸[2] + P2;
	缓存到(偏移);
	return (BYTE*)IFD像素指针[偏移] + (UINT32(Y) * iSizeX + X) * iSizeP;
}
const UINT32* OmeTiffReader::各维尺寸()const noexcept
{
	return i各维尺寸;
}