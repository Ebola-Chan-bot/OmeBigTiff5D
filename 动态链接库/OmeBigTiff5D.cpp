#include "pch.h"
#include "OmeBigTiff5D.h"
#include <algorithm>
#include "维度顺序字符串.h"
#include "像素类型字符串.h"
#include "寻找字符串.h"
#include <rpc.h>
#include "像素类型尺寸.h"
#include "通道ID模板.h"
#include <vector>
#include "XML文本接收器.h"
#include "N维切片参数.h"
#include "resource.h"
#include "模块句柄.h"
#include "像素类型采样格式.h"
void OmeBigTiff5D::建立映射()
{
	FileMappingObject = CreateFileMappingW(File, NULL, PAGE_READWRITE, 0, 0, NULL);
	基地址 = (BYTE*)MapViewOfFile(FileMappingObject, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
}
void OmeBigTiff5D::关闭映射()const
{
	UnmapViewOfFile(基地址);
	CloseHandle(FileMappingObject);
}
void OmeBigTiff5D::更改文件尺寸(LARGE_INTEGER 新尺寸)
{
	SetFilePointerEx(File, 新尺寸, NULL, FILE_BEGIN);
	SetEndOfFile(File);
	关闭映射();
	建立映射();
	末地址 = 基地址 + 新尺寸.QuadPart;
}
void OmeBigTiff5D::失败清理()noexcept
{
	free(iChannels);
	关闭映射();
	CloseHandle(File);
}
OmeBigTiff5D::~OmeBigTiff5D()noexcept
{
	free(iChannels);
	UnmapViewOfFile((LPVOID)基地址);
	//随后自动调用基类析构
}
void OmeBigTiff5D::Pixels节点缓存()
{
	iSizeX = Pixels.attribute("SizeX");
	iSizeY = Pixels.attribute("SizeY");
	iSizeC = Pixels.attribute("SizeC");
	iSizeZ = Pixels.attribute("SizeZ");
	iSizeT = Pixels.attribute("SizeT");
	iDimensionOrder = Pixels.attribute("DimensionOrder");
	iPixelType = Pixels.attribute("Type");
	iChannels = (xml_node*)malloc(sizeof(xml_node) * SizeC());
	xml_object_range<xml_named_node_iterator> 通道节点迭代器 = Pixels.children("Channel");
	std::copy(通道节点迭代器.begin(), 通道节点迭代器.end(), iChannels);
}
void OmeBigTiff5D::生成各维尺寸()
{
	i各维尺寸[0] = SizeX();
	i各维尺寸[1] = SizeY();
	const char* const DO = iDimensionOrder.value();
	for (UINT8 D = 2; D < 5; ++D)
		switch (DO[D])
		{
		case 'C':
			i各维尺寸[D] = SizeC();
			break;
		case 'Z':
			i各维尺寸[D] = SizeZ();
			break;
		case 'T':
			i各维尺寸[D] = SizeT();
			break;
		}
}
void OmeBigTiff5D::打开Core(LARGE_INTEGER& 文件尺寸,UINT64& 最小文件尺寸)
{
	文件尺寸.QuadPart = 0;
	SetFilePointerEx(File, 文件尺寸, &文件尺寸, FILE_END);
	末地址 = 基地址 + 文件尺寸.QuadPart;
	if (文件尺寸.QuadPart < sizeof(OmeBigTiff5D文件头))
		throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::文件太小,.错误消息 = "读文件头时意外遇到文件结尾" };
	FirstIFD = 文件头->FirstIFD.取指针(基地址);
	if (末地址 <(BYTE*)( FirstIFD+1))
		throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::文件太小,.错误消息 = "读IFD时意外遇到文件结尾" };
	const UINT64 NoValues = FirstIFD->图像描述.NoValues;
	const char* const iImageDescription = FirstIFD->图像描述.ASCII偏移.取指针(基地址);
	if (末地址 < (BYTE*)iImageDescription + NoValues)
		throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::文件太小,.错误消息 = "读图像描述时意外遇到文件结尾" };
	const xml_parse_result 解析结果 = IDDoc.load_buffer(iImageDescription, NoValues, parse_declaration);
	const xml_parse_status XML异常 = 解析结果.status;
	if (XML异常)
		throw 尝试结果{ .结果 = 结果分类::XML异常,.XML异常 = XML解析结果(XML异常),.错误消息 = 解析结果.description() };
	xml_node 节点 = IDDoc.child("OME");
	if (!(节点))
		throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::OME规范,.错误消息 = "OME XML 缺少OME节点" };
	if (!(唯一标识符 = 节点.attribute("UUID")))
		throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::OME规范,.错误消息 = "OME节点缺少UUID属性" };
	if (!(节点 = 节点.child("Image")))
		throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::OME规范,.错误消息 = "OME节点下缺少Image节点" };
	if (!(文件名 = 节点.attribute("Name")))
		throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::OME规范,.错误消息 = "Image节点缺少Name属性" };
	if (!(Pixels = 节点.child("Pixels")))
		throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::OME规范,.错误消息 = "Image节点下缺少Pixels节点" };
	if (!(iSizeX = Pixels.attribute("SizeX")))
		throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::OME规范,.错误消息 = "Pixels节点缺少SizeX属性" };
	if (!(iSizeY = Pixels.attribute("SizeY")))
		throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::OME规范,.错误消息 = "Pixels节点缺少SizeY属性" };
	if (!(iSizeC = Pixels.attribute("SizeC")))
		throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::OME规范,.错误消息 = "Pixels节点缺少SizeC属性" };
	if (!(iSizeZ = Pixels.attribute("SizeZ")))
		throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::OME规范,.错误消息 = "Pixels节点缺少SizeZ属性" };
	if (!(iSizeT = Pixels.attribute("SizeT")))
		throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::OME规范,.错误消息 = "Pixels节点缺少SizeT属性" };
	if (!(iDimensionOrder = Pixels.attribute("DimensionOrder")))
		throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::OME规范,.错误消息 = "Pixels节点缺少DimensionOrder属性" };
	生成各维尺寸();
	if (!(iPixelType = Pixels.attribute("Type")))
		throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::OME规范,.错误消息 = "Pixels节点缺少Type属性" };
	xml_object_range<xml_named_node_iterator> 通道节点迭代器 = Pixels.children("Channel");
	std::vector<xml_node> ChannelBuffer;
	ChannelBuffer.insert(ChannelBuffer.end(), 通道节点迭代器.begin(), 通道节点迭代器.end());
	const UINT8 SizeC = OmeBigTiff5D::SizeC();
	if (ChannelBuffer.size() != SizeC)
		throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::OME规范,.错误消息 = "Pixels节点下Channel节点数目不等于SizeC" };
	iChannels = (xml_node*)malloc(sizeof(xml_node) * SizeC);
	copy(ChannelBuffer.cbegin(), ChannelBuffer.cend(), iChannels);
	最小文件尺寸 = FirstIFD->像素偏移.LONG8值 + UINT64(SizeX()) * SizeY() * SizeC * SizeZ() * SizeT() * SizeP();
}
尝试结果 OmeBigTiff5D::只读打开(LPCWSTR 文件路径)noexcept
{
	File = CreateFileW(文件路径, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (File == INVALID_HANDLE_VALUE)
		return 尝试结果{ .结果 = 结果分类::Win32异常,.错误代码 = GetLastError(),.错误消息 = "打开文件失败" };
	FileMappingObject = CreateFileMappingW(File, NULL, PAGE_READONLY, 0, 0, NULL);
	基地址 = (BYTE*)MapViewOfFile(FileMappingObject, FILE_MAP_READ, 0, 0, 0);
	LARGE_INTEGER 文件尺寸;
	UINT64 最小文件尺寸;
	try 
	{
		打开Core(文件尺寸, 最小文件尺寸);
	}
	catch (尝试结果 ex)
	{
		失败清理();
		return ex;
	}
	if (文件尺寸.QuadPart < 最小文件尺寸)
	{
		失败清理();
		return 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::文件太小,.错误消息 = "读像素值时意外遇到文件尾" };
	}
	return 尝试结果{ .结果 = 结果分类::成功 };
}
尝试结果 OmeBigTiff5D::打开现存(LPCWSTR 文件路径)noexcept
{
	File = CreateFileW(文件路径, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (File == INVALID_HANDLE_VALUE)
		return 尝试结果{ .结果 = 结果分类::Win32异常,.错误代码 = GetLastError(),.错误消息 = "打开文件失败" };
	建立映射();
	LARGE_INTEGER 文件尺寸;
	UINT64 最小文件尺寸;
	try
	{
		打开Core(文件尺寸, 最小文件尺寸);
	}
	catch (尝试结果 ex)
	{
		失败清理();
		return ex;
	}
	if (文件尺寸.QuadPart< 最小文件尺寸)//可以尝试修复的错误
	{
		文件尺寸.QuadPart = 最小文件尺寸;
		更改文件尺寸(文件尺寸);
	}
	return 尝试结果{ .结果 = 结果分类::成功 };
}
inline void 设置TiffData(xml_node& TiffData, xml_node Pixels, xml_node 模板, const char* First, UINT16 I, UINT32& Index)
{
	TiffData = Pixels.append_copy(模板);
	TiffData.attribute(First).set_value(I);
	TiffData.attribute("IFD").set_value(++Index);
}
//输入的Pixels节点应当只有首个TiffData子节点并包含无需修改的必要信息
void 填充TiffData(xml_node Pixels)
{
	xml_node 模板 = Pixels.child("TiffData");
	UINT8 SizeC = Pixels.attribute("SizeC").as_uint();
	UINT8 SizeZ = Pixels.attribute("SizeZ").as_uint();
	UINT16 SizeT = Pixels.attribute("SizeT").as_uint();
	const char* DO = Pixels.attribute("DimensionOrder").value() + 2;
	UINT16 Sizes[3];
	char Firsts[3][7] = { "FirstT","FirstZ","FirstC" };
	for (UINT8 D = 0; D < 3; ++D)
		switch (Firsts[D][5] = DO[D])
		{
		case 'C':
			Sizes[D] = SizeC;
			break;
		case 'Z':
			Sizes[D] = SizeZ;
			break;
		case 'T':
			Sizes[D] = SizeT;
			break;
		}
	const UINT16 Size0 = Sizes[0];
	const UINT16 Size1 = Sizes[1];
	const UINT16 Size2 = Sizes[2];
	const char* const First0 = Firsts[0];
	const char* const First1 = Firsts[1];
	const char* const First2 = Firsts[2];
	xml_node 模板1;
	xml_node 模板0;
	xml_node TiffData;
	UINT32 Index = 0;
	for (UINT16 I0 = 1; I0 < Size0; ++I0)
		设置TiffData(TiffData, Pixels, 模板, First0, I0, Index);
	for (UINT16 I1 = 1; I1 < Size1; ++I1)
	{
		设置TiffData(模板1, Pixels, 模板, First1, I1, Index);
		for (UINT16 I0 = 1; I0 < Size0; ++I0)
			设置TiffData(TiffData, Pixels, 模板1, First0, I0, Index);
	}
	for (UINT16 I2 = 1; I2 < Size2; ++I2)
	{
		设置TiffData(模板1, Pixels, 模板, First2, I2, Index);
		for (UINT16 I0 = 1; I0 < Size0; ++I0)
			设置TiffData(TiffData, Pixels, 模板1, First0, I0, Index);
		for (UINT16 I1 = 1; I1 < Size1; ++I1)
		{
			设置TiffData(模板0, Pixels, 模板1, First1, I1, Index);
			for (UINT16 I0 = 1; I0 < Size0; ++I0)
				设置TiffData(TiffData, Pixels, 模板0, First0, I0, Index);
		}
	}
}
void 填充IFD(UINT32 SizeI, 文件偏移<UINT64,IFD5D> 当前IFD偏移, BYTE* 基地址, bool 必须重新生成, UINT32 SizePXY)
{
	IFD5D* 当前IFD指针 = 当前IFD偏移.取指针(基地址);
	if (SizeI > 1)
	{
		if (必须重新生成)
		{
			当前IFD指针->NextIFD = 当前IFD偏移 += sizeof(IFD5D);
			当前IFD指针[1] = 当前IFD指针[0];
			(++当前IFD指针)->图像描述.NoValues = 0;
			当前IFD指针->图像描述.LONG8值 = 0;
		}
		当前IFD指针->像素偏移.LONG8值 += SizePXY;
	}
	IFD5D* const 最后IFD指针 = 当前IFD指针 + SizeI - 2;
	while (当前IFD指针<最后IFD指针)
	{
		if (必须重新生成)
		{
			当前IFD指针->NextIFD = 当前IFD偏移 += sizeof(IFD5D);
			当前IFD指针[1] = 当前IFD指针[0];
		}
		(++当前IFD指针)->像素偏移.LONG8值 += SizePXY;
	}
	当前IFD指针->NextIFD = 文件偏移<UINT64, IFD5D>{ .偏移 = 0 };
}
xml_attribute 添加UUID属性(xml_node OME,char* URN)
{
	UUID uuid;
	UuidCreate(&uuid);
	RPC_CSTR UUID字符串;
	UuidToStringA(&uuid, &UUID字符串);
	strcat(URN, (char*)UUID字符串);
	RpcStringFreeA(&UUID字符串);
	xml_attribute 唯一标识符 = OME.append_attribute("UUID");
	唯一标识符.set_value(URN);
	return 唯一标识符;
}
char* 取文件名(LPCWSTR 文件路径)
{
	const size_t 字节数 = sizeof(wchar_t) * (wcslen(文件路径) + 1);
	wchar_t* const 宽文件名 = (wchar_t*)malloc(字节数);
	wchar_t* const 扩展名 = (wchar_t*)malloc(字节数);
	_wsplitpath(文件路径, nullptr, nullptr, 宽文件名, 扩展名);
	wcscat(宽文件名, 扩展名);
	char* const 窄文件名 = (char*)malloc(字节数);
	WideCharToMultiByte(CP_UTF8, NULL, 宽文件名, -1, 窄文件名, 字节数, NULL, NULL);
	//wcstombs(窄文件名, 宽文件名, 字节数);  垃圾函数不好用
	free(宽文件名);
	free(扩展名);
	return 窄文件名;
}
void OmeBigTiff5D::已知IDDoc构造文件()
{
	生成各维尺寸();
	XML文本接收器 接收器;
	IDDoc.save(接收器);
	const std::string& 图像描述 = 接收器.缓存;
	const UINT32 图像描述字节数 = 图像描述.length() + 1;
	文件偏移<UINT64, IFD5D> IFD偏移对象(图像描述字节数 * 2 + sizeof(OmeBigTiff5D文件头));
	const UINT32 SizeI = OmeBigTiff5D::SizeI();
	const UINT64 像素偏移 = IFD偏移对象 + SizeI * sizeof(IFD5D)*2;
	const UINT8 SizeP = OmeBigTiff5D::SizeP();
	const UINT16 SizeX = OmeBigTiff5D::SizeX();
	const UINT16 SizeY = OmeBigTiff5D::SizeY();
	const UINT32 SizePXY = UINT32(SizeX) * SizeY * SizeP;
	const LARGE_INTEGER 文件尺寸{ .QuadPart = INT64(像素偏移 + UINT64(SizePXY) * SizeI) };
	SetFilePointerEx(File, 文件尺寸, NULL, FILE_BEGIN);
	SetEndOfFile(File);
	建立映射();
	末地址 = 基地址 + 文件尺寸.QuadPart;
	(*文件头 = OmeBigTiff5D文件头()).FirstIFD = IFD偏移对象;
	FirstIFD = IFD偏移对象.取指针(基地址);
	文件偏移<UINT64, char> 图像描述偏移{ .偏移 = sizeof(OmeBigTiff5D文件头) };
	strcpy(图像描述偏移.取指针(基地址), 接收器.缓存.c_str());
	*FirstIFD = IFD5D(图像描述字节数, 文件偏移<UINT64, char>{.偏移 = sizeof(OmeBigTiff5D文件头)}, 文件偏移<UINT64, BYTE>{.偏移 = IFD偏移对象 + SizeI * sizeof(IFD5D)}, SizeP, SizeX, SizeY, 像素类型采样格式[UINT16(PixelType())]);
	填充IFD(SizeI, IFD偏移对象, 基地址, true, SizePXY);
}
尝试结果 OmeBigTiff5D::覆盖创建(LPCWSTR 文件路径, UINT16 SizeX, UINT16 SizeY, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, 维度顺序 DimensionOrder, 像素类型 PixelType,const 颜色* ChannelColors)noexcept
{
	File = CreateFileW(文件路径, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (File == INVALID_HANDLE_VALUE)
		return 尝试结果{ .结果 = 结果分类::Win32异常,.错误代码 = GetLastError(),.错误消息 = "文件打开失败" };
	const HRSRC 资源信息 = FindResourceW(模块句柄, MAKEINTRESOURCEW(IDR_XmlTemplate), L"XML");
	IDDoc.load_buffer(LockResource(LoadResource(模块句柄, 资源信息)), SizeofResource(模块句柄, 资源信息),parse_declaration);
	char URN[46] = "urn:uuid:";
	xml_node 子节点 = IDDoc.child("OME");
	唯一标识符 = 添加UUID属性(子节点, URN);
	char* const 窄文件名 = 取文件名(文件路径);
	(文件名 = (子节点 = 子节点.child("Image")).append_attribute("Name")).set_value(窄文件名);
	//窄文件名后面还要用到不能释放
	Pixels = 子节点.child("Pixels");
	(iSizeX = Pixels.append_attribute("SizeX")).set_value(SizeX);
	(iSizeY = Pixels.append_attribute("SizeY")).set_value(SizeY);
	(iSizeC = Pixels.append_attribute("SizeC")).set_value(SizeC);
	(iSizeZ = Pixels.append_attribute("SizeZ")).set_value(SizeZ);
	(iSizeT = Pixels.append_attribute("SizeT")).set_value(SizeT);
	const char* DO = 维度顺序字符串[UINT8(DimensionOrder)];
	(iDimensionOrder = Pixels.append_attribute("DimensionOrder")).set_value(DO);
	(iPixelType = Pixels.append_attribute("Type")).set_value(像素类型字符串[UINT8(PixelType)]);
	iChannels = (xml_node*)malloc(sizeof(xml_node) * SizeC);
	iChannels[0] = Pixels.child("Channel");
	char ChannelID[12];
	for (UINT8 C = 1; C < SizeC; ++C)
	{
		sprintf(ChannelID, 通道ID模板, C);
		(iChannels[C] = Pixels.insert_copy_after(iChannels[C - 1], iChannels[C - 1])).attribute("ID").set_value(ChannelID);
	}
	if (ChannelColors)
		for (UINT8 C = 0; C < SizeC; ++C)
			iChannels[C].append_attribute("Color").set_value(ChannelColors[C].整数值);
	子节点 = Pixels.child("TiffData").child("UUID");
	子节点.append_attribute("FileName").set_value(窄文件名);
	free(窄文件名);
	子节点.text().set(URN);
	填充TiffData(Pixels);
	已知IDDoc构造文件();
	return 尝试结果{ .结果 = 结果分类::成功 };
}
void OmeBigTiff5D::IDDoc解析(const char* ImageDescription, const char* 新文件名)
{
	IDDoc.load_string(ImageDescription,parse_declaration);
	xml_node 节点 = IDDoc.child("OME");
	唯一标识符 = 节点.attribute("UUID");
	char URN[46] = "urn:uuid:";
	if (唯一标识符)
		strcpy(URN, 唯一标识符.value());
	else
		唯一标识符 = 添加UUID属性(节点, URN);
	文件名 = (节点 = 节点.child("Image")).attribute("Name");
	if (!文件名)
		文件名 = 节点.append_attribute("Name");
	文件名.set_value(新文件名);
	Pixels = 节点.child("Pixels");
	Pixels节点缓存();
	if (Pixels.child("TiffData"))
		for (xml_node TiffData : Pixels.children("TiffData"))
			TiffData.child("UUID").attribute("FileName").set_value(新文件名);
	else
	{
		xml_document TiffData模板;
		const HRSRC 资源信息 = FindResourceW(模块句柄, MAKEINTRESOURCEW(IDR_TiffData), L"XML");
		TiffData模板.load_buffer(LockResource(LoadResource(模块句柄, 资源信息)), SizeofResource(模块句柄, 资源信息));
		节点 = Pixels.append_copy(TiffData模板.first_child()).child("UUID");
		节点.append_attribute("FileName").set_value(新文件名);
		节点.text().set(URN);
		填充TiffData(Pixels);
	}
}
尝试结果 OmeBigTiff5D::覆盖创建(LPCWSTR 文件路径, const char* ImageDescription)noexcept
{
	File = CreateFileW(文件路径, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (File == INVALID_HANDLE_VALUE)
		return 尝试结果{ .结果 = 结果分类::Win32异常,.错误代码 = GetLastError(),.错误消息 = "文件创建失败" };
	char* const 窄文件名 = 取文件名(文件路径);
	IDDoc解析(ImageDescription, 窄文件名);
	已知IDDoc构造文件();
	return 尝试结果{ .结果 = 结果分类::成功 };
}
尝试结果 OmeBigTiff5D::打开或创建(LPCWSTR 文件路径, UINT16 SizeX, UINT16 SizeY, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, 维度顺序 DimensionOrder, 像素类型 PixelType, const 颜色* ChannelColors, bool& 成功打开)noexcept
{
	尝试结果 结果 = 打开现存(文件路径);
	return (成功打开 = 结果.结果 == 结果分类::成功) ? std::move(结果) : 覆盖创建(文件路径, SizeX, SizeY, SizeC, SizeZ, SizeT, DimensionOrder, PixelType, ChannelColors);
}
尝试结果 OmeBigTiff5D::打开或创建(LPCWSTR 文件路径, const char* ImageDescription, bool& 成功打开)noexcept
{
	尝试结果 结果 = 打开现存(文件路径);
	return (成功打开 = 结果.结果 == 结果分类::成功) ? std::move(结果) : 覆盖创建(文件路径, ImageDescription);
}
UINT16 OmeBigTiff5D::SizeX()const noexcept
{
	return iSizeX.as_uint();
}
UINT16 OmeBigTiff5D::SizeY()const noexcept
{
	return iSizeY.as_uint();
}
UINT8 OmeBigTiff5D::SizeC()const noexcept
{
	return iSizeC.as_uint();
}
UINT8 OmeBigTiff5D::SizeZ()const noexcept
{
	return iSizeZ.as_uint();
}
UINT16 OmeBigTiff5D::SizeT()const noexcept
{
	return iSizeT.as_uint();
}
UINT32 OmeBigTiff5D::SizeI()noexcept
{
	return UINT32(SizeC()) * SizeZ() * SizeT();
}
维度顺序 OmeBigTiff5D::DimensionOrder()const noexcept
{
	return 维度顺序(寻找字符串(iDimensionOrder.value(), 维度顺序字符串, 维度顺序个数));
}
像素类型 OmeBigTiff5D::PixelType()const noexcept
{
	return 像素类型(寻找字符串(iPixelType.value(), 像素类型字符串, 像素类型个数));
}
颜色 OmeBigTiff5D::读ChannelColor(UINT8 C)const noexcept
{
	const xml_attribute 颜色属性 = iChannels[C].attribute("Color");
	return 颜色{ .整数值 = 颜色属性 ? 颜色属性.as_int() : -1 };
}
void OmeBigTiff5D::读ChannelColor(颜色* Colors)const noexcept
{
	xml_attribute 颜色属性;
	for (UINT8 C = 0; C < SizeC(); ++C)
		Colors[C] = 颜色{ .整数值 = (颜色属性 = iChannels[C].attribute("Color")) ? 颜色属性.as_int() : -1 };
}
const char* OmeBigTiff5D::ImageDescription()const noexcept
{
	return FirstIFD->图像描述.ASCII偏移.取指针(基地址);
}
const char* OmeBigTiff5D::FileName()const noexcept
{
	return 文件名.value();
}
UINT8 OmeBigTiff5D::SizeP()const noexcept
{
	return 像素类型尺寸[寻找字符串(iPixelType.value(), 像素类型字符串, 像素类型个数)];
}
BYTE* OmeBigTiff5D::像素指针()const
{
	return FirstIFD->像素偏移.BYTE偏移.取指针(基地址);
}
void N维切片(UINT64 单位尺寸, UINT8 维数, UINT32* 各维尺寸, UINT32* 下标长度, UINT64** 下标, const BYTE* 源, BYTE* 目标, bool 切源, const BYTE* 末地址)
{
	偏移向量 偏移;
	UINT64 段长度;
	N维切片参数(单位尺寸, 维数, 各维尺寸, 下标长度, 下标, 偏移, 段长度);
	if (切源)
		if (偏移.maxCoeff() + 段长度 + 源 > 末地址)
			throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::下标越界,.错误消息 = "读入像素值时索引超出文件范围" };
		else
			for (UINT64 O : 偏移)
			{
				memcpy(目标, 源 + O, 段长度);
				目标 += 段长度;
			}
	else
		if (偏移.maxCoeff() + 段长度 + 目标 > 末地址)
			throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::下标越界,.错误消息 = "写出像素值时索引超出文件范围" };
		else
			for (UINT64 O : 偏移)
			{
				memcpy(目标 + O, 源, 段长度);
				源 += 段长度;
			}
}
void OmeBigTiff5D::读入像素3D(UINT16 XSize, UINT16 YSize, UINT32 ISize, UINT64* XRange, UINT64* YRange, UINT64* IRange, BYTE* BytesOut)noexcept
{
	UINT32 各维尺寸[] = { SizeX(),SizeY(),SizeI() };
	UINT32 下标长度[] = { XSize,YSize,ISize };
	UINT64* 下标[] = { XRange,YRange,IRange };
	N维切片(SizeP(), 3, 各维尺寸, 下标长度, 下标, 像素指针(), BytesOut, true, 末地址);
}
尝试结果 OmeBigTiff5D::读入像素5D(UINT16 XSize, UINT16 YSize, UINT16 Size2, UINT16 Size3, UINT16 Size4, UINT64* XRange, UINT64* YRange, UINT64* Range2, UINT64* Range3, UINT64* Range4, BYTE* BytesOut)noexcept
{
	UINT32 下标长度[5] = { XSize,YSize,Size2,Size3,Size4 };
	UINT64* 下标[5] = { XRange,YRange,Range2,Range3,Range4 };
	try
	{
		N维切片(SizeP(), 5, i各维尺寸, 下标长度, 下标, 像素指针(), BytesOut, true, 末地址);
	}
	catch (尝试结果 ex)
	{
		return ex;
	}
	return 尝试结果{ .结果 = 结果分类::成功 };
}
void OmeBigTiff5D::写出像素(UINT16 XSize, UINT16 YSize, UINT16 Size2, UINT16 Size3, UINT16 Size4, UINT64* XRange, UINT64* YRange, UINT64* Range2, UINT64* Range3, UINT64* Range4,const BYTE* BytesIn)noexcept
{
	UINT32 下标长度[5] = { XSize,YSize,Size2,Size3,Size4 };
	UINT64* 下标[5] = { XRange,YRange,Range2,Range3,Range4 };
	N维切片(SizeP(), 5, i各维尺寸, 下标长度, 下标, BytesIn, 像素指针(), false, 末地址);
}
bool OmeBigTiff5D::更新图像描述并扩展文件(bool 必须重新生成,UINT32& SizeI, UINT32& SizePXY)
{
	XML文本接收器 接收器;
	IDDoc.save(接收器);
	const std::string& 图像描述 = 接收器.缓存;
	const UINT32 图像描述字节数 = 图像描述.length() + 1;
	SizeI = OmeBigTiff5D::SizeI();
	SizePXY = UINT32(SizeP()) * SizeX() * SizeY();
	const 文件偏移<UINT64, char> 图像描述偏移 = FirstIFD->图像描述.ASCII偏移;
	UINT64 像素偏移 = FirstIFD->像素偏移.LONG8值;
	文件偏移<UINT64, IFD5D> 当前IFD偏移 = 文件头->FirstIFD;
	const bool 空间不足 = 当前IFD偏移 < 图像描述偏移 + 图像描述字节数;
	if (空间不足)
	{
		const 文件偏移<UINT64, IFD5D> 新FirstIFD偏移{ .偏移 = 图像描述偏移 + (图像描述字节数) * 2 };
		const UINT64 全IFD尺寸 = sizeof(IFD5D) * SizeI;
		if (像素偏移 < 新FirstIFD偏移 + 全IFD尺寸)
		{
			const UINT64 新像素偏移 = 新FirstIFD偏移 + 全IFD尺寸 * 2;
			const UINT64 新文件大小 = 新像素偏移 + UINT64(SizePXY) * SizeI;
			LARGE_INTEGER 文件大小;
			SetFilePointerEx(File, LARGE_INTEGER{ .QuadPart = 0 }, &文件大小, FILE_END);
			if (新文件大小 > 文件大小.QuadPart)
			{
				文件大小.QuadPart = 新文件大小;
				更改文件尺寸(文件大小);
			}
			//此时的FirstTags是不对的
			像素偏移 = 新像素偏移;
		}
		//IFD偏移存储一个偏移量，不受重映射影响
		memmove(新FirstIFD偏移.取指针(基地址), 当前IFD偏移.取指针(基地址), 必须重新生成 ? sizeof(IFD5D) : 全IFD尺寸);
		文件头->FirstIFD = 新FirstIFD偏移;
		FirstIFD = 新FirstIFD偏移.取指针(基地址);
		FirstIFD->像素偏移.BYTE偏移 = 文件偏移<UINT64, BYTE>{ .偏移 = 像素偏移 };
	}
	strcpy(图像描述偏移.取指针(基地址), 图像描述.c_str());
	//即使空间充足，图像描述字节数也可能发生缩小
	FirstIFD->图像描述.NoValues = 图像描述字节数;
	return 空间不足;
}
bool OmeBigTiff5D::更新通道(UINT8 SizeC)
{
	UINT8 原SizeC = OmeBigTiff5D::SizeC();
	const bool 更新 = SizeC != 原SizeC;
	if (更新)
	{
		if (SizeC < 原SizeC)
			for (UINT8 C = SizeC; C < 原SizeC; ++C)
				Pixels.remove_child(iChannels[C]);
		else
		{
			iChannels = (xml_node*)realloc(iChannels, sizeof(xml_node) * SizeC);
			char ChannelID[12];
			sprintf(ChannelID, 通道ID模板, 原SizeC);
			xml_node 新通道 = Pixels.append_child("Channel");
			新通道.append_attribute("ID").set_value(ChannelID);
			新通道.append_attribute("SamplesPerPixel").set_value(1);
			iChannels[原SizeC] = 新通道;
			for (UINT8 C = 原SizeC + 1; C < SizeC; ++C)
			{
				sprintf(ChannelID, 通道ID模板, C);
				(iChannels[C] = Pixels.append_copy(iChannels[C - 1])).attribute("ID").set_value(ChannelID);
			}
		}
		iSizeC.set_value(SizeC);
	}
	return 更新;
}
void 重置TiffData(xml_node Pixels)
{
	xml_object_range<xml_named_node_iterator> TiffData范围 = Pixels.children("TiffData");
	xml_named_node_iterator TiffData结束 = TiffData范围.end();
	for (xml_named_node_iterator 当前TiffData = ++TiffData范围.begin(); 当前TiffData != TiffData结束; )
		Pixels.remove_child(*(当前TiffData++));
	填充TiffData(Pixels);
}
void 设置颜色(xml_node 节点, 颜色 Color)
{
	xml_attribute 属性;
	if (!(属性 = 节点.attribute("Color")))
		属性 = 节点.append_attribute("Color");
	属性.set_value(Color.整数值);
}
void OmeBigTiff5D::修改基本参数(UINT16 SizeX, UINT16 SizeY, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, 维度顺序 DimensionOrder, 像素类型 PixelType, const 颜色* Colors,const char* 新文件名) noexcept
{
	const bool 像素数改变 = SizeX || SizeY;
	const bool 单帧尺寸改变 = 像素数改变 || PixelType != 像素类型::缺省;
	if (SizeX)
		iSizeX.set_value(SizeX);
	else
		SizeX = OmeBigTiff5D::SizeX();
	if (SizeY)
		iSizeY.set_value(SizeY);
	else
		SizeY = OmeBigTiff5D::SizeY();
	const bool IFD数目改变 = SizeC || SizeZ || SizeT;
	const bool IFD顺序改变 = DimensionOrder != 维度顺序::缺省;
	const bool IFD重新生成 = 单帧尺寸改变 || IFD数目改变;
	if (SizeC)
		更新通道(SizeC);
	else
		SizeC = OmeBigTiff5D::SizeC();
	if (SizeZ)
		iSizeZ.set_value(SizeZ);
	else
		SizeZ = OmeBigTiff5D::SizeZ();
	if (SizeT)
		iSizeT.set_value(SizeT);
	else
		SizeT = OmeBigTiff5D::SizeT();
	if (IFD顺序改变)
		iDimensionOrder.set_value(维度顺序字符串[UINT8(DimensionOrder)]);
	if (像素数改变 || IFD数目改变 || IFD顺序改变)
		生成各维尺寸();
	UINT8 SizeP;
	if (PixelType == 像素类型::缺省)
		SizeP = OmeBigTiff5D::SizeP();
	else
	{
		SizeP = UINT8(PixelType);
		iPixelType.set_value(像素类型字符串[SizeP]);
		SizeP = 像素类型尺寸[SizeP];
	}
	if (Colors)
		for (UINT8 C = 0; C < SizeC; ++C)
			设置颜色(iChannels[C], Colors[C]);
	if (新文件名)
	{
		文件名.set_value(新文件名);
		Pixels.child("TiffData").child("UUID").attribute("FileName").set_value(新文件名);
	}
	if (IFD顺序改变 || IFD数目改变 || 新文件名)
		重置TiffData(Pixels);
	UINT32 SizeI;
	UINT32 SizePXY;
	更新图像描述并扩展文件(IFD重新生成, SizeI, SizePXY);
	if (单帧尺寸改变)
	{
		FirstIFD->图像宽度.SHORT值 = SizeX;
		FirstIFD->图像长度.SHORT值 = SizeY;
		FirstIFD->每个样本的位数.SHORT值 = SizeP * 8;
		FirstIFD->每条行数.SHORT值 = SizeY;
		FirstIFD->像素字节数.LONG值 = SizePXY;
	}
	填充IFD(SizeI, 文件头->FirstIFD, 基地址, IFD重新生成, SizePXY);
}
void OmeBigTiff5D::SizeX(UINT16 SizeX) noexcept
{
	if (SizeX != OmeBigTiff5D::SizeX())
	{
		iSizeX.set_value(SizeX);
		UINT32 SizeI;
		UINT32 SizePXY;
		更新图像描述并扩展文件(false, SizeI, SizePXY);
		FirstIFD->图像宽度.SHORT值 = SizeX;
		FirstIFD->像素字节数.LONG值 = SizePXY;
		填充IFD(SizeI, 文件头->FirstIFD, 基地址, false, SizePXY);
		i各维尺寸[0] = SizeX;
	}
}
void OmeBigTiff5D::SizeY(UINT16 SizeY) noexcept
{
	if (SizeY != OmeBigTiff5D::SizeY())
	{
		iSizeY.set_value(SizeY);
		UINT32 SizeI;
		UINT32 SizePXY;
		更新图像描述并扩展文件(false, SizeI, SizePXY);
		FirstIFD->图像长度.SHORT值 = SizeY;
		FirstIFD->每条行数.SHORT值 = SizeY;
		FirstIFD->像素字节数.LONG值 = SizePXY;
		填充IFD(SizeI, 文件头->FirstIFD, 基地址, false, SizePXY);
		i各维尺寸[1] = SizeY;
	}
}
void OmeBigTiff5D::IFD更新必备(bool 必须重新生成)
{
	重置TiffData(Pixels);
	UINT32 SizeI;
	UINT32 SizePXY;
	更新图像描述并扩展文件(必须重新生成, SizeI, SizePXY);
	填充IFD(SizeI, 文件头->FirstIFD, 基地址, 必须重新生成, SizePXY);
}
void OmeBigTiff5D::SizeC(UINT8 SizeC) noexcept
{
	if (更新通道(SizeC))
	{
		IFD更新必备(true);
		const char* const DO = iDimensionOrder.value();
		i各维尺寸[std::find(DO, DO + 5, 'C') - DO] = SizeC;
	}
}
void OmeBigTiff5D::SizeZ(UINT8 SizeZ) noexcept
{
	if (SizeZ != OmeBigTiff5D::SizeZ())
	{
		iSizeZ.set_value(SizeZ);
		IFD更新必备(true);
		const char* const DO = iDimensionOrder.value();
		i各维尺寸[std::find(DO, DO + 5, 'Z') - DO] = SizeZ;
	}
}
void OmeBigTiff5D::SizeT(UINT16 SizeT) noexcept
{
	if (SizeT != OmeBigTiff5D::SizeT())
	{
		iSizeT.set_value(SizeT);
		IFD更新必备(true);
		const char* const DO = iDimensionOrder.value();
		i各维尺寸[std::find(DO, DO + 5, 'T') - DO] = SizeT;
	}
}
void OmeBigTiff5D::DimensionOrder(维度顺序 DO) noexcept
{
	const char* const DO字符串 = 维度顺序字符串[UINT8(DO)];
	if (!strcmp(DO字符串, iDimensionOrder.value()))
	{
		iDimensionOrder.set_value(DO字符串);
		IFD更新必备(false);
		生成各维尺寸();
	}
}
void OmeBigTiff5D::PixelType(像素类型 PT) noexcept
{
	UINT8 SizeP = UINT8(PT);
	const char* const PT字符串 = 像素类型字符串[SizeP];
	if (!strcmp(PT字符串, iPixelType.value()))
	{
		iPixelType.set_value(PT字符串);
		SizeP = 像素类型尺寸[SizeP];
		UINT32 SizeI;
		UINT32 SizePXY;
		更新图像描述并扩展文件(true, SizeI, SizePXY);
		FirstIFD->每个样本的位数.SHORT值 = SizeP * 8;
		FirstIFD->像素字节数.LONG值 = SizePXY;
		填充IFD(SizeI, 文件头->FirstIFD, 基地址, true, SizePXY);
	}
}
void OmeBigTiff5D::写ChannelColor(const 颜色* Colors) noexcept
{
	const UINT8 SizeC = OmeBigTiff5D::SizeC();
	for (UINT8 C = 0; C < SizeC; ++C)
		设置颜色(iChannels[C], Colors[C]);
	UINT32 SizeI;
	UINT32 SizePXY;
	if (更新图像描述并扩展文件(false, SizeI, SizePXY))
		填充IFD(SizeI, 文件头->FirstIFD, 基地址, false, SizePXY);
}
void OmeBigTiff5D::写ChannelColor(UINT8 C, 颜色 Color) noexcept
{
	设置颜色(iChannels[C], Color);
	UINT32 SizeI;
	UINT32 SizePXY;
	if (更新图像描述并扩展文件(false, SizeI, SizePXY))
		填充IFD(SizeI, 文件头->FirstIFD, 基地址, false, SizePXY);
}
void OmeBigTiff5D::ImageDescription(const char* WriteIn) noexcept
{
	const char* const 保留文件名 = FileName();
	char* const 新文件名 = (char*)malloc(strlen(保留文件名) + 1);
	strcpy(新文件名, 保留文件名);
	IDDoc解析(WriteIn, 新文件名);
	free(新文件名);
	UINT32 SizeI;
	UINT32 SizePXY;
	更新图像描述并扩展文件(true, SizeI, SizePXY);
	FirstIFD->图像宽度.SHORT值 = SizeX();
	FirstIFD->图像长度.SHORT值 = SizeY();
	FirstIFD->每个样本的位数.SHORT值 =SizeP() * 8;
	FirstIFD->每条行数.SHORT值 = SizeY();
	FirstIFD->像素字节数.LONG值 = SizePXY;
	填充IFD(SizeI, 文件头->FirstIFD, 基地址, true, SizePXY);
}
void OmeBigTiff5D::FileName(const char* 新文件名) noexcept
{
	文件名.set_value(新文件名);
	for (xml_node TiffData : Pixels.children("TiffData"))
		TiffData.child("UUID").attribute("FileName").set_value(新文件名);
	UINT32 SizeI;
	UINT32 SizePXY;
	if (更新图像描述并扩展文件(false, SizeI, SizePXY))
		填充IFD(SizeI, 文件头->FirstIFD, 基地址, false, SizePXY);
}
BYTE* OmeBigTiff5D::内部像素指针3D(UINT16 X, UINT16 Y, UINT32 I)noexcept
{
	return 像素指针() + ((UINT64(I) * SizeY() + Y) * SizeX() + X) * SizeP();
}
BYTE* OmeBigTiff5D::内部像素指针5D(UINT16 X, UINT16 Y, UINT16 P2, UINT16 P3, UINT16 P4)noexcept
{
	return 像素指针() + (((UINT64(P4) * i各维尺寸[3] + P3) * i各维尺寸[2] + P2) * i各维尺寸[1] + Y) * i各维尺寸[0] + X;
}
const UINT32* OmeBigTiff5D::各维尺寸()const noexcept
{
	return i各维尺寸;
}