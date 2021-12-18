#pragma once
#include "OmeTiffReader.h"
class OmeSmallTiffReader :public OmeTiffReader
{
private:
	IFD偏移<UINT16, UINT32> 下个IFD;
protected:
	void 载入图像描述(pugi::xml_document& XML文档)override;
	void 尝试载入图像描述(pugi::xml_document& XML文档, const char* 尾指针)override;
	void 缓存到(UINT32 IFD)override;
};