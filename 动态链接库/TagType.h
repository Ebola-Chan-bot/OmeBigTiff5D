#pragma once
enum class TagType :UINT16
{
	BYTE = 1,
	ASCII = 2,
	SHORT = 3,
	LONG = 4,
	RATIONAL = 5,
	SBYTE = 6,
	UNDEFINED = 7,
	SSHORT = 8,
	SLONG = 9,
	SRATIONAL = 10,
	FLOAT = 11,
	DOUBLE = 12,
	LONG8 = 16,
	SLONG8 = 17,
	IFD8 = 18
};