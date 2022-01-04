#pragma once
#include "TagID.h"
#include "TagType.h"
#include "文件偏移.h"
#include "Rational.h"
#include "SRational.h"
#include "SampleFormat.h"
#include "Compression.h"
#include "PhotometricInterpretation.h"
#include "ResolutionUnit.h"
#pragma pack(4)
template <NumberType DWord>
struct Tag
{
	TagID Identifier;
	TagType DataType;
	DWord NoValues = 1;
	union
	{
		UINT8 BYTE值;
		char ASCII值;
		UINT16 SHORT值;
		UINT32 LONG值;
		Rational RATIONAL值;
		INT8 SBYTE值;
		DWord UNDEFINED值;
		INT16 SSHORT值;
		INT32 SLONG值;
		SRational SRATIONAL值;
		float FLOAT值;
		double DOUBLE值;
		UINT64 LONG8值;
		INT64 SLONG8值;
		UINT64 IFD8值;
		SampleFormat SampleFormat枚举;
		Compression Compression枚举;
		PhotometricInterpretation PhotometricInterpretation枚举;
		ResolutionUnit ResolutionUnit枚举;
		文件偏移<DWord, UINT8> BYTE偏移;
		文件偏移<DWord, char> ASCII偏移;
		文件偏移<DWord, UINT16> SHORT偏移;
		文件偏移<DWord, UINT32> LONG偏移;
		文件偏移<DWord, Rational> RATIONAL偏移;
		文件偏移<DWord, INT8> SBYTE偏移;
		文件偏移<DWord, DWord> UNDEFINED偏移;
		文件偏移<DWord, INT16> SSHORT偏移;
		文件偏移<DWord, INT32> SLONG偏移;
		文件偏移<DWord, SRational> SRATIONAL偏移;
		文件偏移<DWord, float> FLOAT偏移;
		文件偏移<DWord, double> DOUBLE偏移;
		文件偏移<DWord, UINT64> LONG8偏移;
		文件偏移<DWord, INT64> SLONG8偏移;
		文件偏移<DWord, UINT64> IFD8偏移;
	};
};
#pragma pack()
extern template struct Tag<UINT32>;
extern template struct Tag<UINT64>;