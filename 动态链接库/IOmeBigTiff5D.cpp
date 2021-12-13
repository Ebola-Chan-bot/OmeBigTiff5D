#include "pch.h"
#include "OmeBigTiff5D.h"
OmeBigTiff5D导出(IOmeBigTiff5D*) 创建OmeBigTiff5D()
{
	return new OmeBigTiff5D;
}
OmeBigTiff5D导出(void) 销毁OmeBigTiff5D(IOmeBigTiff5D* 对象指针)
{
	delete (OmeBigTiff5D*)对象指针;
}