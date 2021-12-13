#include "pch.h"
#include "CZT重排.h"
void CZT重排(UINT32* 各维尺寸, UINT32* 下标长度, UINT64** 下标, const UINT16* CZTSize, UINT64** CZTRange, const UINT16* SizeCZT,const char* 维度顺序)
{
	for (UINT8 D = 0; D < 3; ++D)
		switch (维度顺序[D])
		{
		case 'C':
			各维尺寸[D] = SizeCZT[0];
			下标长度[D] = CZTSize[0];
			下标[D] = CZTRange[0];
			break;
		case 'Z':
			各维尺寸[D] = SizeCZT[1];
			下标长度[D] = CZTSize[1];
			下标[D] = CZTRange[1];
			break;
		case 'T':
			各维尺寸[D] = SizeCZT[2];
			下标长度[D] = CZTSize[2];
			下标[D] = CZTRange[2];
			break;
		}
}