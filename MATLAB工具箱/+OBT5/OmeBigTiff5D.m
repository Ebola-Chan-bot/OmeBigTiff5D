classdef OmeBigTiff5D<OBT5.OmeTiffReader
	%读写符合5D规范的OME BigTiff文件
	%%
	%5D是在OME BigTiff规范基础上更进一步的严格格式要求，为了实现高性能的读写：
	%
	%标准文件头之后是一个字节79，然后紧跟OME XML图像描述文本。
	%
	%文本之后是首个IFD，两者之间可以有任意大空隙，作为以后扩展图像描述的预留空间
	%
	%除首个IFD具有额外的图像描述，共计12个标签以外，其余每个IFD都有且仅有Tiff规范灰度图像的11个必需标签。这些标签按照固定顺序排列：ImageDescription（仅首IFD有）,
	%StripOffsets, ImageWidth, ImageLength, BitsPerSample, RowsPerStrip,
	%StripByteCounts, Compression, PhotometricInterpretation, XResolution,
	%YResolution, ResolutionUnit
	%
	%所有IFD按顺序紧密排列在一起，中间不留任何空隙。
	% 
	% IFD之后是像素数据，两者之间可以有任意大空隙，作为以后扩展IFD的预留空间。
	% 
	% 所有像素数据也按照IFD顺序紧密排列在一起，中间不留任何空隙。像素数据结束到文件尾可以预留空隙。
	%
	%由于5D规范要求像素数据按顺序紧密排列，整个像素数据段可以看作一个完整的5D数组，进行高性能的5D索引操作和连续内存段对拷。但也因为对结构顺序要求严格，一旦某一结构成员超出预留空间，将不得不向后移动其后所有成员以扩展空间，并修改许多文件内部偏移指针数值，这种操作具有较大的开销。因此使用者应尽可能一次性指定好各维度尺寸信息，避免事后修改。
	%
	%虽然允许修改元数据，但这种修改可能会破坏原有像素值。除非那些像素值已经无用，否则请在修改前将其读出以免丢失。
	%
	%本类还继承了PixelPointer3D和PixelPointer5D两个返回像素指针的函数。在定义它们的基类中，返回的指针指向只读内存段，且不能用于跨IFD拷贝。但对本类对象使用这些函数时，返回的指针指向可写内存，可用于跨IFD拷贝。
	properties(Dependent)
		SizeX(1,1)uint16
		SizeY(1,1)uint16
		PixelType(1,1)OBT5.PixelType
		%OME-TIFF规范要求将文件名信息存储在图像描述中
		FileName(1,:)char
		SizeC(1,1)uint8
		SizeZ(1,1)uint8
		SizeT(1,1)uint8
		%像素数值在磁盘上实际的排列顺序，从低维度到高维度。
		DimensionOrder(1,1)OBT5.DimensionOrder
		ImageDescription(1,:)char
	end
	methods(Access=protected)
		function obj = OmeBigTiff5D(Pointer)
			obj@OBT5.OmeTiffReader(Pointer);
		end
		function ReleasePointer(obj)
			%子类重写此函数可避免重复释放指针，delete无法被重写。
			MexInterface(uint8(OBT5.Internal.ApiCode.DestroyOmeBigTiff5D),obj.Pointer);
		end
	end
	methods(Static)		
		function varargout = Create(FilePath,CD,ImageDescription,options)
			%创建类对象
			%% 语法（import OBT5.*）
			% obj=OmeBigTiff5D.Create(FilePath,CreationDisposition);
			% 使用上述语法时，CreationDisposition只能取OpenExisting或TryOpen
			% obj=OmeBigTiff5D.Create(FilePath,CreationDisposition,ImageDescription);
			% obj=OmeBigTiff5D.Create(FilePath,CreationDisposition,Name=Value);
			% 使用上述语法时，CreationDisposition只能取OpenOrCreate或Overwrite
			% [obj,Open]=OmeBigTiff5D.Create(FilePath,CreationDisposition.OpenOrCreate,ImageDescription);
			% [obj,Open]=OmeBigTiff5D.Create(FilePath,CreationDisposition.OpenOrCreate,Name=Value);
			%% 必需参数
			% FilePath(1,:)char，文件路径
			% CreationDisposition(1,1)OBT5.CreationDisposition，详见枚举类文档
			%% 可选参数
			% ImageDescription(1,:)char，OME XML 图像描述，通常从其它 OME TIFF 文件中读取，可以实现元数据的原封拷贝。
			% 如果指定了可选参数，名称值参数将被忽略。
			%% 名称值参数
			% SizeX, SizeY, SizeT(1,1)uint16=1，图像宽度、高度、时长
			% SizeC, SizeZ(1,1)uint8=1，图像颜色通道数、层数
			% DimensionOrder(1,1)OBT5.DimensionOrder=OBT5.DimensionOrder.XYCZT，维度顺序
			% PixelType(1,1)OBT5.PixelType=OBT5.PixelType.UINT16，像素类型
			% ChannelColors(:,1)int32，各通道颜色，每个通道用一个int32表示颜色，详见ChannelColor函数
			%% 返回值
			% obj(1,1)OBT5.OmeBigTiff5D
			% Open(1,1)logical，使用OpenOrCreate时，返回逻辑值，指示图像是否被打开而非新建
			arguments
				FilePath(1,:)char
				CD(1,1)OBT5.CreationDisposition
				ImageDescription(1,:)char=''
				options.SizeX(1,1)uint16=1
				options.SizeY(1,1)uint16=1
				options.SizeC(1,1)uint8=1
				options.SizeZ(1,1)uint8=1
				options.SizeT(1,1)uint16=1
				options.DimensionOrder(1,1)OBT5.DimensionOrder=OBT5.DimensionOrder.XYCZT
				options.PixelType(1,1)OBT5.PixelType=OBT5.PixelType.UINT16
				options.ChannelColors(:,1)int32=[]
			end
			varargout=cell(1,(CD==OBT5.CreationDisposition.OpenOrCreate)+1);
			if isempty(ImageDescription)
				[varargout{:}]=MexInterface(uint8(OBT5.Internal.ApiCode.CreateOmeBigTiff5D),FilePath,uint8(CD),options.SizeX,options.SizeY,options.SizeC,options.SizeZ,options.SizeT,uint8(options.DimensionOrder),uint8(options.PixelType),options.ChannelColors);
			else
				[varargout{:}]=MexInterface(uint8(OBT5.Internal.ApiCode.CreateOmeBigTiff5D),FilePath,uint8(CD),ImageDescription);
			end
			varargout{1}=OBT5.OmeBigTiff5D(CheckPos(varargout{1}));
		end
	end
	methods
		function Size=get.SizeX(obj)
			Size=MexInterface(uint8(OBT5.Internal.ApiCode.SizeX),obj.Pointer);
		end
		function set.SizeX(obj,Size)
			MexInterface(uint8(OBT5.Internal.ApiCode.SizeX),obj.Pointer,Size);
		end
		function Size=get.SizeY(obj)
			Size=MexInterface(uint8(OBT5.Internal.ApiCode.SizeY),obj.Pointer);
		end
		function set.SizeY(obj,Size)
			MexInterface(uint8(OBT5.Internal.ApiCode.SizeY),obj.Pointer,Size);
		end
		function Type=get.PixelType(obj)
			Type=OBT5.PixelType(MexInterface(uint8(OBT5.Internal.ApiCode.PixelType),obj.Pointer));
		end
		function set.PixelType(obj,Type)
			MexInterface(uint8(OBT5.Internal.ApiCode.PixelType),obj.Pointer,uint8(Type));
		end
		function Name=get.FileName(obj)
			Name=MexInterface(uint8(OBT5.Internal.ApiCode.FileName),obj.Pointer);
		end
		function set.FileName(obj,Name)
			MexInterface(uint8(OBT5.Internal.ApiCode.FileName),obj.Pointer,Name);
		end
		function Size=get.SizeC(obj)
			Size=MexInterface(uint8(OBT5.Internal.ApiCode.SizeC),obj.Pointer);
		end
		function set.SizeC(obj,Size)
			MexInterface(uint8(OBT5.Internal.ApiCode.SizeC),obj.Pointer,Size);
		end
		function Size=get.SizeZ(obj)
			Size=MexInterface(uint8(OBT5.Internal.ApiCode.SizeZ),obj.Pointer);
		end
		function set.SizeZ(obj,Size)
			MexInterface(uint8(OBT5.Internal.ApiCode.SizeZ),obj.Pointer,Size);
		end
		function Size=get.SizeT(obj)
			Size=MexInterface(uint8(OBT5.Internal.ApiCode.SizeT),obj.Pointer);
		end
		function set.SizeT(obj,Size)
			MexInterface(uint8(OBT5.Internal.ApiCode.SizeT),obj.Pointer,Size);
		end
		function Order=get.DimensionOrder(obj)
			Order=OBT5.DimensionOrder(MexInterface(uint8(OBT5.Internal.ApiCode.DimensionOrder),obj.Pointer));
		end
		function set.DimensionOrder(obj,Order)
			MexInterface(uint8(OBT5.Internal.ApiCode.SizeX),obj.Pointer,uint8(Order));
		end
		function Description=get.ImageDescription(obj)
			Description=MexInterface(uint8(OBT5.Internal.ApiCode.ImageDescription),obj.Pointer);
		end
		function set.ImageDescription(obj,Description)
			MexInterface(uint8(OBT5.Internal.ApiCode.SizeX),obj.Pointer,Description);
		end
		function varargout=ChannelColor(obj,varargin)
			%获取或设置全部或指定通道的颜色
			%返回的每个颜色值为int32类型，可通过typecast函数转换为ABGR分量。例如如果返回颜色值16711935(0xff00ff)，则可计算如下：
			% >>typecast(0xff00ff,'uint8')
			% ans =
			%  1×4 uint8 行向量
			%   255     0   255     0
			% 4个uint8值依次为该颜色的不透明度(Alpha)=255、蓝色分量(Blue)=0、绿色分量(Green)=255、红色分量(Red)=0
			%设置的颜色值也同样需要排列成ABGR向量，然后用typecast转换成int32类型后交付给本函数进行设置：
			% >>typecast([0xff 0x00 0xff 0x00],'int32')
			% ans =
			%  int32
			%   16711935
			%% 语法
			% Colors=obj.ChannelColor; %获取所有通道的颜色
			% Color=obj.ChannelColor(Channel); %获取指定通道的颜色
			% obj.ChannelColor(Colors); %设置所有通道的颜色
			% obj.ChannelColor(Channel,Color); %设置指定通道的颜色
			%% 可选参数
			% Channel(1,1)uint8，要获取/指定哪个通道的颜色？注意索引是从0开始，不同于MATLAB的一般索引规范
			% Colors(:,1)int32，依次设置每个通道的颜色
			% Color(1,1)int32，要设置的单个通道的颜色
			%% 返回值
			% Color(:,1)int32，如果指定了Channel参数则为标量，表示指定通道的颜色；否则为列向量，依次排列所有通道的颜色。
			varargout=cell(1,nargout);
			[varargout{:}]=MexInterface(uint8(OBT5.Internal.ApiCode.ChannelColor),obj.Pointer,varargin{:});
		end
		function WritePixels5D(obj,PixelPointer,varargin)
			%向文件写出5D像素数据
			%% 语法
			%以下涉及所有索引参数，均从0开始，而不同于MATLAB标准索引规范。索引为[]或不指定，表示写满该维度：此时数组在该维度的尺寸必须等于文件在该维度的尺寸。

			%# 写出与文件维度顺序相同的数组
			%obj.WritePixels5D(Pixels[,XRange][,YRange][,Range3][,Range4][,Range5]);
			%5个范围参数可选，且顺序与文件维度顺序一致。不指定范围或指定为[]，均表示顺序写满该维度。例如：
			%obj.WritePixels5D(Pixels,0:511,511:-1:0,[]);
			%上述代码写X轴0:511，Y轴反转，其它维度写满

			%#写出与文件维度顺序不同的数组
			%obj.WritePixels5D(Pixels,Dimension1=Range1,Dimension2=Range2,…);
			%例如：
			%obj.WritePixels5D(Pixels,C=0,Z=[],T=0:65535,Y=0:511,X=[]);
			%上述代码指定Pixels的维度顺序为CZTYX，可以与文件不一致。

			%#指定要写出的数据存储在特定内存指针位置
			%obj.WritePixels5D(FromPointer[,XRange][,YRange][,Range3][,Range4][,Range5]);

			%% 参数说明
			%Pixels(:,:,:,:,:)，要写出的5D像素数据，数据类型必须与文件一致。
			%XRange, YRange(1,:)uint16，要写出到的XY轴范围。向量长度必须与Pixels对应维度长度一致。
			%Range3, Range4, Range5(1,:)uint16，要写出到的CZT轴范围。维度顺序与文件一致，向量长度与数组对应维度一致。
			%Dimension1, Dimension2, …，可用的维度包括XYCZT。不同于一般的名称值参数，此处指定名称值参数的维度顺序规定了Pixels数组的维度顺序
			%Range1, Range2, …(1,:)uint16，对应维度的写出范围，向量长度必须与数组对应维度一致。不指定范围或指定为[]，均表示顺序写满该维度。
			%FromPointer(1,1)uint64，从给定的内存指针读取要写出的数据，维度顺序必须和目标文件的DimensionOrder一致。如果指针来源于OmeBigTiff5D的PixelPointer3D或PixelPointer5D方法，可用于跨IFD拷贝。
			Ranges=cell(1,5);
			if ~isempty(varargin)
				if isnumeric(varargin{1})
					Ranges(1:numel(varargin))=varargin;
				else
					[~,Reorder]=ismember(char(strcat(varargin{1:2:end})),char(obj.DimensionOrder));
					Ranges(Reorder)=varargin(2:2:end);
					PixelPointer=ipermute(PixelPointer,[Reorder setdiff(1:5,Reorder)]);
				end
			end
			Ranges=cellfun(@uint64,Ranges,UniformOutput=false);
			MexInterface(uint8(OBT5.Internal.ApiCode.WritePixels5D),obj.Pointer,Ranges{:},PixelPointer);
		end
		function ModifyMultiParameters(obj,options)
			%一次性修改多个基本参数
			%每次修改基本参数，底层都要对文件结构作调整，性能开销较大。建议用本函数一次性修改多个基本参数，然后仅作一次调整。
			%% 名称值参数（可以同时修改的参数。不指定的参数不会被修改。）
			% SizeX(1,1)uint16
			% SizeY(1,1)uint16
			% SizeC(1,1)uint8
			% SizeZ(1,1)uint8
			% SizeT(1,1)uint16
			% DimensionOrder(1,1)OBT5.DimensionOrder
			% PixelType(1,1)OBT5.PixelType
			% Colors(:,1)int32，每个通道的颜色。此项要么不指定不修改，要么一口气设定所有通道
			% FileName(1,:)char，只修改OME-XML中记录的内部文件名，不会改变面向操作系统的外部文件名
			arguments
				obj(1,1)OBT5.OmeBigTiff5D
				options.SizeX(1,1)uint16=0
				options.SizeY(1,1)uint16=0
				options.SizeC(1,1)uint8=0
				options.SizeZ(1,1)uint8=0
				options.SizeT(1,1)uint16=0
				options.DimensionOrder(1,1)OBT5.DimensionOrder=OBT5.DimensionOrder.DEFAULT
				options.PixelType(1,1)OBT5.PixelType=OBT5.PixelType.DEFAULT
				options.Colors(:,1)int32=uint32.empty
				options.FileName(1,:)char=''
			end
			MexInterface(uint8(OBT5.Internal.ApiCode.ModifyMultiParameters),obj.Pointer,options.SizeX,options.SizeY,options.SizeC,options.SizeZ,options.SizeT,uint8(options.DimensionOrder),uint8(options.PixelType),options.Colors,options.FileName);
		end
	end
end