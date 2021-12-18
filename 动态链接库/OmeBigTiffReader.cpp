#include "pch.h"
#include "OmeBigTiffReader.h"
#include "BigTiff文件头.h"
#include "Tiff异常.h"
void OmeBigTiffReader::载入图像描述(pugi::xml_document& XML文档)
{
	下个IFD = ((BigTiff文件头*)基地址)->FirstIFD;
	const IFD指针<UINT64, UINT64> 下个IFD指针 = 下个IFD.取指针(基地址);
	Tag<UINT64>* 当前标签 = 下个IFD指针.Tags();
	while (当前标签->Identifier != TagID::ImageDescription)
		当前标签++;
	XML文档.load_buffer(当前标签->ASCII偏移.取指针(基地址), 当前标签->NoValues);
}
void OmeBigTiffReader::尝试载入图像描述(pugi::xml_document& XML文档, const char* 内存尾)
{
	if (内存尾 < 基地址 + sizeof(BigTiff文件头))
		throw Tiff异常(Tiff异常类型::文件太小, "读文件头时意外地遇到文件尾");
	下个IFD = ((BigTiff文件头*)基地址)->FirstIFD;
	const IFD指针<UINT64, UINT64> 下个IFD指针 = 下个IFD.取指针(基地址);
	Tag<UINT64>* 当前标签 = 下个IFD指针.Tags();
	if (内存尾 < 下个IFD指针.指针 + sizeof(UINT64))
		throw Tiff异常(Tiff异常类型::文件太小, "读标签个数时意外地遇到文件尾");
	const Tag<UINT64>* 标签尾 = 当前标签 + 下个IFD指针.NumberOfTags();
	if (内存尾 < (char*)标签尾)
		throw Tiff异常(Tiff异常类型::文件太小, "读标签列表时意外地遇到文件尾");
	while (当前标签->Identifier != TagID::ImageDescription && ++当前标签 < 标签尾);
	if (当前标签 == 标签尾)
		throw Tiff异常(Tiff异常类型::OME规范, "首IFD不含ImageDescription标签");
	const UINT32 图像描述长度 = 当前标签->NoValues;
	const char* const 图像描述指针 = 当前标签->ASCII偏移.取指针(基地址);
	if (内存尾 < 图像描述指针 + 图像描述长度)
		throw Tiff异常(Tiff异常类型::文件太小, "读图像描述时意外地遇到文件尾");
	const pugi::xml_parse_result 解析结果 = XML文档.load_buffer(图像描述指针, 图像描述长度);
	if (!解析结果)
		throw 解析结果;
}
void OmeBigTiffReader::缓存到(UINT32 IFD)
{
	while (已缓存数 <= IFD)
		IFD像素指针[已缓存数++] = 读像素指针(下个IFD);
}