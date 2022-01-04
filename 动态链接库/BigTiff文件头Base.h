#pragma once
#include "Tiff文件头.h"=
struct BigTiff文件头Base :public Tiff文件头
{
	UINT16 OffsetSize = 8;
	UINT16 留空 = 0;
	BigTiff文件头Base() { VersionNumber = 43; }
};