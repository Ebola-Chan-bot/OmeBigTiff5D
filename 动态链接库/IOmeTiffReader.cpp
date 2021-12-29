#include "pch.h"
#include "IOmeTiffReader.h"
#include "OmeBigTiffReader.h"
#include "OmeSmallTiffReader.h"
#include "Tiff文件头.h"
OmeBigTiff5D导出(尝试结果) 创建OmeTiffReader(LPCWSTR 文件路径, IOmeTiffReader*& 文件指针)
{
	const HANDLE File = CreateFileW(文件路径, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (File == INVALID_HANDLE_VALUE)
		return 尝试结果{ .结果 = 结果分类::Win32异常,.错误代码 = GetLastError(),.错误消息 = "文件打开失败" };
	Tiff文件头 文件头;
	DWORD NumberOfBytesRead;
	ReadFile(File, &文件头, sizeof(文件头), &NumberOfBytesRead, NULL);
	switch (文件头.VersionNumber)
	{
	case 42:
		文件指针 = new OmeSmallTiffReader;
		break;
	case 43:
		文件指针 = new OmeBigTiffReader;
		break;
	default:
		return 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::不支持的格式,.错误消息 = "不支持的Tiff版本" };
	}
	try
	{
		((OmeTiffReader*)文件指针)->加载文件(File);
	}
	catch (尝试结果 ex)
	{
		return ex;
	}
	return 尝试结果{ .结果 = 结果分类::成功 };
}
OmeBigTiff5D导出(void) 销毁OmeTiffReader(IOmeTiffReader* 对象)
{
	delete (OmeTiffReader*)对象;
}