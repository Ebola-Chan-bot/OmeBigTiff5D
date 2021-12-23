#pragma once
union 颜色
{
	UINT32 整数值;
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