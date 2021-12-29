classdef TiffReader<handle
	%基础Tiff读取器。此类不完全支持完整的TIFF规范文件，要求所有IFD都只有唯一的Strip，且所有IFD高度相等，宽度也相等，位深度也相等。
	properties(GetAccess=protected,SetAccess=immutable)
		Pointer(1,1)uint64
	end
	methods(Access=protected)
		function obj = TiffReader(Pointer)
			obj.Pointer=Pointer;
		end
		function ReleasePointer(obj)
			%子类重写此函数可避免重复释放指针，delete无法被重写。
			MexInterface(uint8(OBT5.Internal.ApiCode.DestroyTiffReader),obj.Pointer);
		end
	end
	methods(Static)
		function obj=Create(FilePath,varargin)
			% 创建类对象
			%% 语法
			% obj=TiffReader.Create(FilePath);
			% obj=TiffReader.Create(FilePath,Try);
			%% 参数说明
			% FilePath，文件路径
			% Try(1,1)logical=false，若设为true，将假定指定的文件可能不符合格式规范，如果读取出错，将输出更详细的调试信息，且不会导致MATLAB崩溃。
			%% 返回值
			% obj(1,1)OBT5.TiffReader
			%% 用例
			% obj=TiffReader.Create('D:\Image.tif');
			obj=OBT5.TiffReader(CheckPos(MexInterface(uint8(OBT5.Internal.ApiCode.CreateTiffReader),FilePath,varargin{:})));
		end
	end
	methods(Access=protected)
		function Pixels=ReadPixels(obj,VAI,DimensionOrder,API)
			Ranges=cell(1,numel(DimensionOrder));
			if ~isempty(VAI)&&isa(VAI{1},'uint64')
				ToPointer=VAI{1};
				VAI(1)=[];
			else
				ToPointer=0;
			end
			if ~isempty(VAI)
				if isnumeric(VAI{1})
					Ranges(1:numel(VAI))=VAI;
				else
					[~,ReorderIndex]=ismember(char(strcat(VAI{1:2:end})),DimensionOrder);
					Ranges(ReorderIndex)=VAI(2:2:end);
				end
			end
			Ranges=cellfun(@uint64,Ranges,UniformOutput=false);
			if ToPointer
				MexInterface(uint8(API),obj.Pointer,Ranges{:},ToPointer);
				Pixels=[];
			else
				Pixels=permute(MexInterface(uint8(API),obj.Pointer,Ranges{:}),[ReorderIndex setdiff(1:5,ReorderIndex)]);
			end
		end
	end
	methods
		function delete(obj)
			obj.ReleasePointer;
		end
		function Size=SizeX(obj)
			Size=MexInterface(uint8(OBT5.Internal.ApiCode.SizeX),obj.Pointer);
		end
		function Size=SizeY(obj)
			Size=MexInterface(uint8(OBT5.Internal.ApiCode.SizeY),obj.Pointer);
		end
		function Size=SizeI(obj)
			Size=MexInterface(uint8(OBT5.Internal.ApiCode.SizeI),obj.Pointer);
		end
		function Type=PixelType(obj)
			Type=OBT5.PixelType(MexInterface(uint8(OBT5.Internal.ApiCode.PixelType),obj.Pointer));
		end
		function Bytes=SizeP(obj)
			Bytes=MexInterface(uint8(OBT5.Internal.ApiCode.SizeP),obj.Pointer);
		end
		function Pointer=PixelPointer3D(obj,X,Y,I,options)
			%返回指定位置的只读像素指针
			%不能使用此方法返回的指针进行跨IFD对拷。只有OBT5对象返回的指针才支持跨IFD对拷。所有索引值从0开始，不同于MATLAB标准索引规则
			%% 语法
			%直接指定XYI坐标
			%obj.PixelPointer3D([X][,Y][,I])
			%未指定的坐标默认0，例如：
			%obj.PixelPointer3D(0,1)
			%取X0，Y1，I0位置像素指针

			%指定特定坐标
			%obj.PixelPointer3D(Dimension1=Position1,Dimension2=Position2,…)
			%未指定的坐标默认0，例如：
			%obj.PixelPointer3D(I=2)
			%取I2，X0，Y0位置像素指针
			%% 参数说明
			%X, Y(1,1)uint16=0，指针要指向的XY位置
			%I(1,1)uint32=0，指针要指向的IFD序号
			%Dimension1, Dimension2, …，可用维度包括XYI，任意顺序指定不影响结果。
			%Position1, Position2, …，对应维度位置
			%% 返回值
			%Pointer(1,1)uint64，指向指定像素位置的内存指针。此方法返回的像素指针指向只读内存段，请勿对其进行写入操作，可以用作OmeBigTiff5D.WritePixels5D的指针参数，实现直接拷贝。但因为标准Tiff的像素数据在IFD之间不是连续排列的，此方法不能可靠地连续拷贝多个IFD的像素值。
			arguments
				obj(1,1)OBT5.TiffReader
				X(1,1)uint16=0
				Y(1,1)uint16=0
				I(1,1)uint32=0
				options.X(1,1)uint16=X
				options.Y(1,1)uint16=Y
				options.I(1,1)uint32=I
			end
			Pointer=MexInterface(uint8(OBT5.Internal.ApiCode.PixelPointer3D),obj.Pointer,options.X,options.Y,options.I);
		end
		function Pixels=ReadPixels3D(obj,varargin)
			%根据指定的XYI范围读入像素值
			%% 语法

			%#像数组切片一样读入指定位置的像素值
			%Pixels=obj.ReadPixels3D([XRange][,YRange][,IRange]);
			%三个范围参数均可省略代表顺序读入该维度全长，或者用[]表示读入全长，例如
			%Pixels=obj.ReadPixels3D([],0:99);
			%上述代码读入X和I轴的全部以及Y轴的0~99。注意，不同于MATLAB标准索引方式，此处索引从0开始

			%#像素值不返回MATLAB，而是直接写出到指定的内存指针。可与上述语法组合使用
			%obj.ReadPixels3D(ToPointer,___);

			%#按照指定的维度顺序和切片方式返回数组
			%Pixels=obj.ReadPixels3D(Dimension1=Range1,Dimension2=Range2,…);
			%例如：
			%Pixels=obj.ReadPixels3D(I=[],X=10:-1:0);
			%上述代码读入I和Y轴全部，X轴从10开始递减到0的反转，输出数组的维度顺序是IXY。使用该语法时，不支持写出到内存指针。
			%% 参数说明
			%Pixels(:,:,:)，返回的像素数组，数据类型与PixelType一致。
			%XRange, YRange(1,:)uint16，XY像素值范围，用向量表示依次读入哪些位置。不指定或指定[]表示读入全部。
			%IRange(1,:)uint32，I像素值范围，用向量表示依次读入哪些位置。不指定或指定[]表示读入全部。
			%ToPointer(1,1)uint64，要写出到的内存指针，这个指针应当来自OmeBigTiff5D.PixelPointer5D函数。指定该参数时，将不返回Pixels。
			%Dimension1, Dimension2, …，可用的维度包括X, Y, I。不同于一般的无序名称值参数，此处指定名称值参数的顺序将决定返回数组的维度顺序。
			%Range1, Range2, …，对应维度的读入范围。可用[]表示读入该维度全部。
			Pixels=obj.ReadPixels(varargin,'XYI',OBT5.Internal.ApiCode.ReadPixels3D);
		end
	end
end