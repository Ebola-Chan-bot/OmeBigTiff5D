#pragma once
enum class 像素类型:UINT8
{
	UINT8,
	UINT16,
	UINT32,
	//请注意，ImageJ不支持有符号整数型的TIFF文件；但它仍能正确读取，只是会弹出警告。
	INT8,
	INT16,
	INT32,
	FLOAT,
	DOUBLE,
	缺省
};