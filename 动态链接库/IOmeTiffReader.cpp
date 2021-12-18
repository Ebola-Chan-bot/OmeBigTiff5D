#include "pch.h"
#include "IOmeTiffReader.h"
#include "OmeBigTiffReader.h"
#include "OmeSmallTiffReader.h"
#include "Tiff文件头.h"
#include "Win32异常.h"
#include "Tiff异常.h"

OmeBigTiff5D导出(IOmeTiffReader*) 创建OmeTiffReader(LPCWSTR 文件路径)
{
	const HANDLE File = CreateFileW(文件路径, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	Tiff文件头 文件头;
	DWORD NumberOfBytesRead;
	ReadFile(File, &文件头, sizeof(文件头), &NumberOfBytesRead, NULL);
	OmeTiffReader* 对象;
	switch (文件头.VersionNumber)
	{
	case 42:
		对象 = new OmeSmallTiffReader;
		break;
	case 43:
		对象 = new OmeBigTiffReader;
		break;
	}
	对象->加载文件(File);
	return 对象;
}
OmeBigTiff5D导出(IOmeTiffReader*) 尝试创建OmeTiffReader(LPCWSTR 文件路径, 尝试结果& 结果)
{
	const HANDLE File = CreateFileW(文件路径, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!File)
	{
		结果 = 尝试结果{ .结果 = 结果分类::Win32异常,.错误代码 = GetLastError(),.错误消息 = "打开文件失败" };
		return nullptr;
	}
	Tiff文件头 文件头;
	DWORD NumberOfBytesRead;
	if (!ReadFile(File, &文件头, sizeof(文件头), &NumberOfBytesRead, NULL))
	{
		结果 = 尝试结果{ .结果 = 结果分类::Win32异常,.错误代码 = GetLastError(),.错误消息 = "读取文件头失败" };
		return nullptr;
	}
	OmeTiffReader* 返回指针;
	switch (文件头.VersionNumber)
	{
	case 42:
		返回指针 = new OmeSmallTiffReader();
		break;
	case 43:
		返回指针 = new OmeBigTiffReader();
		break;
	default:
		结果 = 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = Tiff异常类型::不支持的格式,.错误消息 = "文件头中的版本号信息无效" };
		return nullptr;
		break;
	}
	try
	{
		返回指针->尝试加载(File);
	}
	catch (Tiff异常 ex)
	{
		结果 = 尝试结果{ .结果 = 结果分类::Tiff异常,.异常类型 = ex.类型,.错误消息 = ex.错误消息 };
		delete 返回指针;
		return nullptr;
	}
	catch (Win32异常 ex)
	{
		结果 = 尝试结果{ .结果 = 结果分类::Win32异常,.错误代码 = ex.错误代码,.错误消息 = ex.错误消息 };
		delete 返回指针;
		return nullptr;
	}
	catch (pugi::xml_parse_result ex)
	{
		结果 = 尝试结果{ .结果 = 结果分类::XML异常,.XML异常 = XML解析结果(ex.status),.错误消息 = ex.description() };
		delete 返回指针;
		return nullptr;
	}
	结果 = 尝试结果{ .结果 = 结果分类::成功 };
	return 返回指针;
}
OmeBigTiff5D导出(void) 销毁OmeTiffReader(IOmeTiffReader* 对象)
{
	delete (OmeTiffReader*)对象;
}