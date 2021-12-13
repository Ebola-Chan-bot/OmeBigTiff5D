#pragma once
#include "BigTiff文件头.h"
#pragma pack(1)
struct OmeBigTiff5D文件头 :public BigTiff文件头
{
	UINT8 规范5D = 79;
};