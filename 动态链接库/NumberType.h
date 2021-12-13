#pragma once
#include <type_traits>
template <typename T>
concept NumberType = std::is_arithmetic<T>::value;