#pragma once
#include "TiffReader.h"
class SmallTiffReader :public TiffReader
{
private:
	IFD偏移<UINT16, UINT32> 下个IFD;
protected:
	UINT32 缓存全部()override;
	void 缓存到(UINT32 IFD)override;
public:
	void 加载文件(HANDLE 文件句柄)override;
	void 尝试加载(HANDLE 文件句柄)override;
};