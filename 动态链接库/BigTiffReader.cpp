#include "pch.h"
#include "BigTiffReader.h"
#include "BigTiff文件头.h"
#include "像素类型尺寸.h"
void BigTiffReader::加载文件(HANDLE 文件句柄)
{
	LARGE_INTEGER 文件大小{ .QuadPart = 0 };
	SetFilePointerEx(文件句柄, 文件大小, &文件大小, FILE_END);
	ReaderBase::加载文件(文件句柄);
	const char* 内存尾 = 基地址 + 文件大小.QuadPart;
	if (内存尾 < 基地址 + sizeof(BigTiff文件头))
		throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::文件太小,.错误消息 = "读文件头时意外地遇到文件尾" };
	下个IFD = ((BigTiff文件头*)基地址)->FirstIFD;
	const IFD指针<UINT64, UINT64> 下个IFD指针 = 下个IFD.取指针(基地址);
	Tag<UINT64>* 当前标签 = 下个IFD指针.Tags();
	if (内存尾 < 下个IFD指针.指针 + sizeof(UINT64))
		throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::文件太小,.错误消息 = "读标签个数时意外地遇到文件尾"  };
	const Tag<UINT64>* 标签尾 = 当前标签 + 下个IFD指针.NumberOfTags();
	if (内存尾 < (char*)标签尾)
		throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::文件太小,.错误消息 = "读标签列表时意外地遇到文件尾" };
	UINT8 完成进度 = 0;
	const UINT8* 像素指针;
	bool 像素偏移未设置 = true;
	bool 像素字节未设置 = true;
	bool 图像长度未设置 = true;
	bool 图像宽度未设置 = true;
	while (完成进度 < 4 && 当前标签 < 标签尾)
	{
		switch (当前标签->Identifier)
		{
		case TagID::StripOffsets:
			像素指针 = 当前标签->BYTE偏移.取指针(基地址);
			if (内存尾 < (char*)像素指针)
				throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::文件太小,.错误消息 = "像素偏移超出文件范围" };
			IFD像素指针.push_back(像素指针);
			完成进度++;
			像素偏移未设置 = false;
			break;
		case TagID::BitsPerSample:
			iSizeP = 当前标签->BYTE值 / 8;
			for (UINT8 P = 0; P < 像素类型个数; ++P)
				if (像素类型尺寸[P] == iSizeP)
				{
					iPixelType = 像素类型(P);
					break;
				}
			完成进度++;
			像素字节未设置 = false;
			break;
		case TagID::ImageLength:
			iSizeX = 当前标签->SHORT值;
			完成进度++;
			图像长度未设置 = false;
			break;
		case TagID::ImageWidth:
			iSizeY = 当前标签->SHORT值;
			完成进度++;
			图像宽度未设置 = false;
			break;
		}
		当前标签++;
	}
	if (完成进度 < 4)
	{
		if (像素偏移未设置)
			throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::缺少标签,.错误消息 = "未设置StripOffsets标签" };
		if (像素字节未设置)
			throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::缺少标签,.错误消息 = "未设置BitsPerSample标签" };
		if (图像长度未设置)
			throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::缺少标签,.错误消息 = "未设置ImageLength标签" };
		if (图像宽度未设置)
			throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::缺少标签,.错误消息 = "未设置ImageWidth标签" };
	}

	if (内存尾 < (char*)像素指针 + UINT32(iSizeX) * iSizeY * iSizeP)
		throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::文件太小,.错误消息 = "像素数据超出文件尾" };
	下个IFD = 下个IFD指针.NextIFD();
	if (文件大小.QuadPart < 下个IFD.偏移)
		throw 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::文件太小,.错误消息 = "第2个IFD超出文件尾" };
}
UINT32 BigTiffReader::缓存全部()
{
	while (下个IFD.偏移)
		IFD像素指针.push_back(读像素指针(下个IFD));
	return IFD像素指针.size();
}
void BigTiffReader::缓存到(UINT32 IFD)
{
	while (IFD像素指针.size() <= IFD)
		IFD像素指针.push_back(读像素指针(下个IFD));
}