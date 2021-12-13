#include "pch.h"
#include "IOmeTiffReader.h"
#include "OmeBigTiffReader.h"
#include "OmeSmallTiffReader.h"
#include "Tiff文件头.h"
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
	}
	对象->加载文件(File);
	return 对象;
}
OmeBigTiff5D导出(void) 销毁OmeTiffReader(IOmeTiffReader* 对象)
{
	delete (OmeTiffReader*)对象;
}