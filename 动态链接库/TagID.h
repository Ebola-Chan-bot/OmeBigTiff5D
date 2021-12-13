#pragma once
enum class TagID :UINT16
{
	ImageWidth = 256,
	ImageLength = 257,
	BitsPerSample = 258,
	Compression = 259,
	PhotometricInterpretation = 262,
	ImageDescription = 270,
	StripOffsets = 273,
	RowsPerStrip = 278,
	StripByteCounts = 279,
	XResolution = 282,
	YResolution = 283,
	ResolutionUnit = 296
};