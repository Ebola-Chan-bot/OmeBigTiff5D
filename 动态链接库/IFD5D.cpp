#include "pch.h"
#include "IFD5D.h"
IFD5D::IFD5D(UINT64 ImageDescription长度, 文件偏移<UINT64, char>ImageDescription偏移, 文件偏移<UINT64, BYTE> StripOffsets, UINT8 SizeP, UINT16 SizeX, UINT16 SizeY, SampleFormat SampleFormat)noexcept :
	图像描述(Tag<UINT64>{.Identifier = TagID::ImageDescription, .DataType = TagType::ASCII, .NoValues = ImageDescription长度, .ASCII偏移 = ImageDescription偏移}),
	像素偏移(Tag<UINT64>{.Identifier = TagID::StripOffsets, .DataType = TagType::LONG8, .NoValues = 1, .BYTE偏移 = StripOffsets }),
	图像宽度(Tag<UINT64>{.Identifier = TagID::ImageWidth, .DataType = TagType::SHORT, .NoValues = 1, .SHORT值 = SizeX }),
	图像长度(Tag<UINT64>{.Identifier = TagID::ImageLength, .DataType = TagType::SHORT, .NoValues = 1, .SHORT值 = SizeY}),
	每个样本的位数(Tag<UINT64>{.Identifier = TagID::BitsPerSample, .DataType = TagType::SHORT, .NoValues = 1, .SHORT值 = UINT16(SizeP * 8) }),
	每条行数(Tag<UINT64>{.Identifier = TagID::RowsPerStrip, .DataType = TagType::SHORT, .NoValues = 1, .SHORT值 = SizeY }),
	像素字节数(Tag<UINT64>{.Identifier = TagID::StripByteCounts, .DataType = TagType::LONG, .NoValues = 1, .LONG值 = UINT32(SizeP * SizeX * SizeY) }),
	样本格式(Tag<UINT64>{.Identifier = TagID::SampleFormat, .DataType = TagType::SHORT, .NoValues = 1, .SampleFormat枚举 = SampleFormat}),
	压缩(Tag<UINT64>{.Identifier = TagID::Compression, .DataType = TagType::SHORT, .NoValues = 1, .Compression枚举 = Compression::NoCompression}),
	光度解释(Tag<UINT64>{.Identifier = TagID::PhotometricInterpretation, .DataType = TagType::SHORT, .NoValues = 1, .PhotometricInterpretation枚举 = PhotometricInterpretation::BlackIsZero}),
	X分辨率(Tag<UINT64>{.Identifier = TagID::XResolution, .DataType = TagType::RATIONAL, .NoValues = 1, .RATIONAL值 = Rational{ .Numerator = 1,.Denominator = 1 } }),
	Y分辨率(Tag<UINT64>{.Identifier = TagID::YResolution, .DataType = TagType::RATIONAL, .NoValues = 1, .RATIONAL值 = Rational{ .Numerator = 1,.Denominator = 1 } }),
	分辨率单位(Tag<UINT64>{.Identifier = TagID::ResolutionUnit, .DataType = TagType::SHORT, .NoValues = 1, .SHORT值 = UINT16(ResolutionUnit::NoUnit) })
{}