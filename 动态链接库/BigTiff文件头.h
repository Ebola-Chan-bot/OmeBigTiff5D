#pragma once
#include "BigTiff文件头Base.h"
#include "IFD.h"
struct BigTiff文件头 :public BigTiff文件头Base
{
	IFD偏移<UINT64, UINT64> FirstIFD;
};