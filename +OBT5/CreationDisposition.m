classdef CreationDisposition<uint8
	%创建OmeBigTiff5D对象时，如果指定的文件存在或不存在，应该执行何种操作？
	enumeration
		%文件一定存在且有效，只管打开就完了
		OpenExisting(0)
		%文件可能不存在，即使存在也可能有格式错误。如果无法打开，请返回详细错误信息。
		TryOpen(1)
		%文件可能不存在，即使存在也可能有格式错误。如果无法打开，请创建新文件。
		OpenOrCreate(2)
		%无论文件是否存在，直接覆盖创建新文件。
		Overwrite(3)
	end
end