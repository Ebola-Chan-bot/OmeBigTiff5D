#pragma once
#include "mex.hpp"
#include "mexAdapter.hpp"
using namespace matlab::data;
class MexFunction : public matlab::mex::Function
{
public:
	void operator()(matlab::mex::ArgumentList outputs, matlab::mex::ArgumentList inputs)override;
};