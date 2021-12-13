#include "pch.h"
#include "Tiff文件头.h"
#include "SmallTiffReader.h"
#include "BigTiffReader.h"
OmeBigTiff5D导出(ITiffReader*) 创建TiffReader(LPCWSTR 文件路径)
{
	const HANDLE File = CreateFileW(文件路径, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	Tiff文件头 文件头;
	DWORD NumberOfBytesRead;
	ReadFile(File, &文件头, sizeof(文件头), &NumberOfBytesRead, NULL);
	TiffReader* 对象;
	switch (文件头.VersionNumber)
	{
	case 42:
		对象 = new SmallTiffReader;
		break;
	case 43:
		对象 = new BigTiffReader;
	}
	对象->加载文件(File);
	return 对象;
}
OmeBigTiff5D导出(void) 销毁TiffReader(ITiffReader* 对象)
{
	delete (TiffReader*)对象;
}