#pragma once
#include "Tiff文件头.h"
#include "IFD.h"
#pragma pack(2)
struct BigTiff文件头 :public Tiff文件头
{
	UINT16 OffsetSize = 8;
	UINT16 留空 = 0;
	IFD偏移<UINT64, UINT64> FirstIFD;
	BigTiff文件头();
};