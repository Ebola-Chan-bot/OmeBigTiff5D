#pragma once
#ifdef OMEBIGTIFF5D_EXPORTS
#define OmeBigTiff5D导出(返回类型) extern "C"  __declspec(dllexport) 返回类型 __stdcall
#else
#define OmeBigTiff5D导出(返回类型) extern "C"  __declspec(dllimport) 返回类型 __stdcall
#endif