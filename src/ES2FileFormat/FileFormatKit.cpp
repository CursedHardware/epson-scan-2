// FileFormatKit.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//

#include "stdafx.h"
#include "FKPAPI.h"
#include "FileFormatKit.h"
#include "FKSource.h"
#include "FKDestination.h"
#include "FKWriter.h"
#include "FKReader.h"
#include "Source/Reader/FKJpegReader.h"
#include "Source/Writer/FKJpegWriter.h"
#include "Source/JpegUtil/FKJpegDecodeUtil.h"

FILEFORMATKIT_API IFKSource* CreateAndInitFKSourceInstance(FKSourceType eSourceType, ES_CMN_FUNCS::BUFFER::IESBuffer& cSource, BOOL bIsFull)
{
	return CFKSource::CreateAndInitInstanceWithType(eSourceType,cSource,bIsFull);

}

FILEFORMATKIT_API IFKDestination* CreateAndInitFKDestinationInstance(FKDestinationType eType, ES_CMN_FUNCS::BUFFER::IESBuffer& cSource)
{
	return CFKDestination::CreateAndInitInstanceWithType(eType,cSource);

}

FILEFORMATKIT_API IFKReader* CreateAndInitFKReaderInstance( FKReaderType eReaderType)
{

	return 	CFKReader::CreateFKReaderWithType(eReaderType);

}

FILEFORMATKIT_API IFKWriter* CreateAndInitFKWriterInstance(FKWriterType eWriterType,  ES_CHAR_CPTR pszOptionAsJson)
{

	ESDictionary dictOption ;
	if(pszOptionAsJson){
		ES_CMN_FUNCS::JSON::JSONtoDictionary(ESString(pszOptionAsJson),dictOption);
	}
	return CFKWriter::CreateFKWriterWithType(eWriterType,dictOption);

}

FILEFORMATKIT_API BOOL DecodeJpegWithResize(const ES_CMN_FUNCS::BUFFER::IESBuffer& cSrcJpegData,ES_CMN_FUNCS::BUFFER::IESBuffer& cOutResizedRawData,ST_ES_SIZE_UN32& stRequestSize,IESResultString& strOutImageInfoAsJson)
{
	ESString esstrOutImageInfoAsJson;
	CFKJpegDecodeUtil cUtil;

	BOOL bRet = cUtil.DecodeJpegDataWithResize(cSrcJpegData,cOutResizedRawData,stRequestSize,esstrOutImageInfoAsJson);

	strOutImageInfoAsJson.Set(esstrOutImageInfoAsJson.c_str());

	return bRet;
}
