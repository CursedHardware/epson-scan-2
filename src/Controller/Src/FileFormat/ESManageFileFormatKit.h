//----------------------------------------=----------------------------------------┐
/*!@file
@brief	モジュール管理クラス
@par		Copyright:
				Copyright (C) SEIKO EPSON CORPORATION 2014. All rights reserver. <BR>
@date		2014.10.30
*/

#pragma once

#include <ES2FileFormat/FileFormatKit.h>
#include <ES2FileFormat/FKPAPI.h>
//----------------------------------------=----------------------------------------┐
/*!@brief	TBD
*/
class CESManageFileFormatKit
{
	HMODULE m_hModule;

public:
	CESManageFileFormatKit() ;
	virtual ~CESManageFileFormatKit(void);


	virtual bool	LoadModule(const TCHAR* pszModulePath) ;
	virtual BOOL	_FreeModule() ;

	IFKSource* Call_CreateAndInitFKSource(FKSourceType eSourceType, ES_CMN_FUNCS::BUFFER::IESBuffer& cSource, BOOL bIsFull)
	{
		if (m_pPFNCreateAndInitFKSourceInstance) {

			return m_pPFNCreateAndInitFKSourceInstance(eSourceType, cSource, bIsFull);
		}
		return NULL ;
	}

	IFKDestination* Call_CreateAndInitFKDestination(FKDestinationType eType, ES_CMN_FUNCS::BUFFER::IESBuffer& cSource)
	{
		if (m_pPFNCreateAndInitFKDestinationInstance) {
			return m_pPFNCreateAndInitFKDestinationInstance(eType, cSource) ;
		}
		return NULL ;
	}

	IFKReader* Call_CreateAndInitFKReaderInstance( FKReaderType eReaderType)
	{
		if (m_pPFNCreateAndInitFKReaderInstance) {
			return m_pPFNCreateAndInitFKReaderInstance(eReaderType) ;
		}
		return NULL ;
	}

	IFKWriter* Call_CreateAndInitFKWriterInstance( FKWriterType eWriterType, ES_CHAR_CPTR pszOptionAsJson)
	{
		if (m_pPFNCreateAndInitFKWriterInstance) {
			return m_pPFNCreateAndInitFKWriterInstance(eWriterType, pszOptionAsJson) ;
		}
		return NULL ;
	}

	BOOL Call_DecodeJpegWithResize(const ES_CMN_FUNCS::BUFFER::IESBuffer& cSrcJpegData,const ES_CMN_FUNCS::BUFFER::IESBuffer& cOutResizedRawData,const ST_ES_SIZE_UN32& stRequestSize,const IESResultString& strOutImageInfoAsJson)
	{
		if (m_pPFNDecodeJpegWithResize) {
			return m_pPFNDecodeJpegWithResize(cSrcJpegData, cOutResizedRawData,stRequestSize,strOutImageInfoAsJson) ;
		}
		return FALSE ;
	}

private:
	PFN_INIT_FKSOURCE_INSTANCE			m_pPFNCreateAndInitFKSourceInstance ;
	PFN_INIT_FKDESTINATION_INSTANCE		m_pPFNCreateAndInitFKDestinationInstance ;
	PFN_INIT_FKREADER_INSTANCE			m_pPFNCreateAndInitFKReaderInstance ;
	PFN_INIT_FKWRITER_INSTANCE			m_pPFNCreateAndInitFKWriterInstance ;
	PFN_DECODE_JPEG_WITH_RESIZE			m_pPFNDecodeJpegWithResize;
};
