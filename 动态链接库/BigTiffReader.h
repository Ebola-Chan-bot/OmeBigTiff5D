#pragma once
#include "TiffReader.h"
class BigTiffReader :public TiffReader
{
private:
	IFD偏移<UINT64, UINT64> 下个IFD;
protected:
	UINT32 缓存全部()override;
	void 缓存到(UINT32 IFD)override;
public:
	void 加载文件(HANDLE 文件句柄)override;
	void 尝试加载(HANDLE 文件句柄)override;
};