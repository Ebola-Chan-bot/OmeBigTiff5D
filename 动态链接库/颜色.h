#pragma once
union 颜色
{
	UINT32 整数值;
#pragma pack(1)
	struct
	{
		UINT8 R;
		UINT8 G;
		UINT8 B;
		UINT8 A;
	}RGBA;
};