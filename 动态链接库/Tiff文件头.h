#pragma once
#pragma pack(2)
struct Tiff文件头
{
	UINT16 ByteOrder = 0x4949;
	UINT16 VersionNumber;
};