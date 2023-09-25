//----------------------------------------=----------------------------------------┐
/*!@file
@brief		モジュール管理クラス
@par		Copyright:
			Copyright (C) SEIKO EPSON CORPORATION 2014. All rights reserver. <BR>
@date		2015.1.30
*/

#include "stdafx.h"
#include "ESManageFileFormatKit.h"



CESManageFileFormatKit::CESManageFileFormatKit()
{
	m_hModule = NULL;

	m_pPFNCreateAndInitFKSourceInstance = 0 ;
	m_pPFNCreateAndInitFKDestinationInstance = 0 ;
	m_pPFNCreateAndInitFKReaderInstance = 0 ;
	m_pPFNCreateAndInitFKWriterInstance = 0 ;
	m_pPFNDecodeJpegWithResize = 0;

}

//----------------------------------------=----------------------------------------┐
/*!@brief	TBD
*/
CESManageFileFormatKit::~CESManageFileFormatKit(void)
{
	_FreeModule();
	m_pPFNCreateAndInitFKSourceInstance = 0 ;
	m_pPFNCreateAndInitFKDestinationInstance = 0 ;
	m_pPFNCreateAndInitFKReaderInstance = 0 ;
	m_pPFNCreateAndInitFKWriterInstance = 0 ;
	m_pPFNDecodeJpegWithResize = 0;
}

//----------------------------------------=----------------------------------------┐
/*!@brief	TBD
*/
bool	CESManageFileFormatKit::LoadModule(const TCHAR *pszModulePath)
{
	m_hModule = LoadLibraryEx(pszModulePath, NULL , LOAD_WITH_ALTERED_SEARCH_PATH) ;
	if (m_hModule) {
#ifdef WIN32
		m_pPFNCreateAndInitFKSourceInstance 		= (PFN_INIT_FKSOURCE_INSTANCE)::GetProcAddress(m_hModule,"CreateAndInitFKSourceInstance") ;
		m_pPFNCreateAndInitFKDestinationInstance = (PFN_INIT_FKDESTINATION_INSTANCE)::GetProcAddress(m_hModule,"CreateAndInitFKDestinationInstance") ;
		m_pPFNCreateAndInitFKReaderInstance 		= (PFN_INIT_FKREADER_INSTANCE)::GetProcAddress(m_hModule,"CreateAndInitFKReaderInstance") ;
		m_pPFNCreateAndInitFKWriterInstance 		= (PFN_INIT_FKWRITER_INSTANCE)::GetProcAddress(m_hModule,"CreateAndInitFKWriterInstance") ;
		m_pPFNDecodeJpegWithResize					= (PFN_DECODE_JPEG_WITH_RESIZE)::GetProcAddress(m_hModule,"DecodeJpegWithResize") ;
#else
		m_pPFNCreateAndInitFKSourceInstance 		= (PFN_INIT_FKSOURCE_INSTANCE)dlsym(m_hModule,"CreateAndInitFKSourceInstance") ;
		m_pPFNCreateAndInitFKDestinationInstance = (PFN_INIT_FKDESTINATION_INSTANCE)dlsym(m_hModule,"CreateAndInitFKDestinationInstance") ;
		m_pPFNCreateAndInitFKReaderInstance 		= (PFN_INIT_FKREADER_INSTANCE)dlsym(m_hModule,"CreateAndInitFKReaderInstance") ;
		m_pPFNCreateAndInitFKWriterInstance 		= (PFN_INIT_FKWRITER_INSTANCE)dlsym(m_hModule,"CreateAndInitFKWriterInstance") ;
		m_pPFNDecodeJpegWithResize					= (PFN_DECODE_JPEG_WITH_RESIZE)dlsym(m_hModule,"DecodeJpegWithResize") ;
#endif
		if (!m_pPFNCreateAndInitFKSourceInstance ||
			!m_pPFNCreateAndInitFKDestinationInstance ||
			!m_pPFNCreateAndInitFKReaderInstance ||
			!m_pPFNCreateAndInitFKWriterInstance ||
			!m_pPFNDecodeJpegWithResize) {
			return false ;
		}
	} else {
#ifdef WIN32
		ES_Error_Log(this,ES_STRING("Can not load library [%s] [%x]"),pszModulePath,GetLastError());
#endif
		return false ;
	}

	return true ;
}

//----------------------------------------=----------------------------------------┐
/*!@brief	TBD
*/
BOOL	CESManageFileFormatKit::_FreeModule()
{

	if(m_hModule){
		FreeLibrary(m_hModule) ;
		m_hModule = NULL;
	}

	return true;

}