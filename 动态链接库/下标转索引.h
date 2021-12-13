#pragma once
#include "pch.h"
#define EIGEN_USE_MKL_ALL
#include "Eigen/Dense"
using namespace Eigen;
typedef Matrix<UINT64, Dynamic, 1> 偏移向量;
void 下标转索引(UINT64 基础权重, UINT8 维数, UINT32* 各维尺寸, UINT32* 下标长度, UINT64** 下标, 偏移向量& 索引);