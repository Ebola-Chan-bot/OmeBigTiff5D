#pragma once
#include "BigTiff文件头Base.h"
#include "IFD5D.h"
#pragma pack(1)
struct OmeBigTiff5D文件头:public BigTiff文件头Base
{
	文件偏移<UINT64,IFD5D> FirstIFD;
	UINT8 规范5D = 79;
};
#pragma pack()