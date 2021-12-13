#pragma once
#define EIGEN_USE_MKL_ALL
#include "Eigen/Dense"
using namespace Eigen;
typedef Matrix<UINT64, Dynamic, 1> 偏移向量;
void N维切片参数(UINT64 单位尺寸, UINT8 维数, UINT32* 各维尺寸, UINT32* 下标长度, UINT64** 下标, 偏移向量& 偏移, UINT64& 段长度);