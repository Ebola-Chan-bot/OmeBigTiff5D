#pragma once
#include "Tiff文件头.h"
#include "IFD.h"
#pragma pack(2)
struct SmallTiff文件头 :public Tiff文件头
{
	IFD偏移<UINT16, UINT32> FirstIFD;
	SmallTiff文件头();
};