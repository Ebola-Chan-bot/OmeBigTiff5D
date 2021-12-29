#include "pch.h"
#include "Tiff文件头.h"
#include "SmallTiffReader.h"
#include "BigTiffReader.h"
OmeBigTiff5D导出(尝试结果) 创建TiffReader(LPCWSTR 文件路径,ITiffReader*& 文件指针)
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
		文件指针 = new SmallTiffReader;
		break;
	case 43:
		文件指针 = new BigTiffReader;
		break;
	default:
		return 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::不支持的格式,.错误消息 = "不支持的Tiff版本" };
	}
	try
	{
		((TiffReader*)文件指针)->加载文件(File);
	}
	catch( 尝试结果 ex)
	{
		delete 文件指针;
		return ex;
	}
	return 尝试结果{ .结果 = 结果分类::成功 };
}
OmeBigTiff5D导出(void) 销毁TiffReader(ITiffReader* 对象)
{
	delete (TiffReader*)对象;
}