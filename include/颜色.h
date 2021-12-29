#pragma once
union 颜色
{
	//OME标准是用Java实现，不支持无符号整数，就很坑
	INT32 整数值;
#pragma pack(1)
	struct
	{
		UINT8 A;
		UINT8 B;
		UINT8 G;
		UINT8 R;
	}RGBA;
#pragma pack()
};