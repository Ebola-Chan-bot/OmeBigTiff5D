function POS = CheckPos(POS)
import OBT5.Internal.*
if isstruct(POS)
	switch ErrorType(POS.Type)
		case ErrorType.TiffException
			error(sprintf('TiffException:%s',TiffException(POS.Code)),POS.Message);
		case ErrorType.Win32Exception
			error(sprintf('Win32Exception:%s',Win32API.SystemErrorCodes(POS.Code)),POS.Message);
		case ErrorType.XmlException
			error(sprintf('XmlException:%s',XmlException(POS.Code)),POS.Message);
		otherwise
			error('OBT5:UnknownException','未指定的错误');
	end
end