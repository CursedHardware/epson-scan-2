#pragma once

#include "../ESCANLIB.h"
#include "supervisor.h"
#include "devicelist.h"
#include "PathUtils.h"

#include <string.h>

// Storage callback status definitions
#define ST_STATUS_IMAGE_FILE		0x00000001
#define ST_STATUS_IMAGE_MEMORY		0x00000002

BOOL CALLBACK scanstatusCallback(EslCBDataScanStatus* scanstatus, PVOID pPrivate);
BOOL CALLBACK imagefileCallback(EslCBDImageFile* imagefile, PVOID pPrivate);

class CScanMgr
{
public:
	CScanMgr(void);
	~CScanMgr(void);

	// Public functions for termination
	void		free(void);

	// Public functions for scanner open/close
	DWORD		openScanner(const ConnectInfo* pScanner);
	void		closeScanner(void);

	// Public functions for set/get parameters
	DWORD		ctrlParameter(const eParamOpType OpType, EslParamHdr* pParam);

	// Public functions for execute
	DWORD		execute(const eExecType ExecType, peslCallback pCbFunc, PVOID pPrivate);

	// Callback process function from StorageMgr class
	//DWORD		storageCallbackProcess(StorageCBData* pData);

	DWORD 		getEslDeviceStatus(void);

	BOOL			sendAppCallbackScanStatus(EslCBDataScanStatus* scanstatus);
	BOOL			sendAppCallbackImageFile(EslCBDImageFile* imagefile);

private:

	// Control class pointers 
	Supervisor* m_sv;

	// Chached Parameters
	EslParamScanExV1	m_ParamScanDefault;
	EslParamScanExV1	m_ParamScanCurrent;
	BOOL				m_ShowIndicators;
	BOOL				m_ColorSlipReduction;

	EslParamSaveBasic	m_ParamSaveDefault;
	EslParamSaveBasic	m_ParamSaveCurrent;
	EslParamSaveJPEG	m_ParamJpegDefault;
	EslParamSaveJPEG	m_ParamJpegCurrent;
	EslParamSaveTIFF	m_ParamTiffDefault;
	EslParamSaveTIFF	m_ParamTiffCurrent;

	// Default Chached parameter functions
	void			setSDKDefault(void);
	void			loadScanDefault(void);
	void			loadDefault(void);

	// Private functions for paramters
	DWORD			setParameter(EslParamHdr* pParam);
	DWORD			getCurrentParameter(EslParamHdr* pParam);
	DWORD			getDefaultParameter(EslParamHdr* pParam);

	// Private functions for scan paramters
	DWORD			setParamScan(const EslParamHdr* pParam);
	DWORD			getCurrentParamScan(EslParamHdr* pParam);
	DWORD			getDefaultParamScan(EslParamHdr* pParam);

	// Private functions for storage parameters
	DWORD			setParamSave(const EslParamHdr* pParam);
	DWORD			getCurrentParamSave(EslParamHdr* pParam);
	DWORD			getDefaultParamSave(EslParamHdr* pParam);

	// Private functions for info parameters
	DWORD			getParamInfo(EslParamHdr* pParam);

	// Application callback function pointer to notify status
	peslCallback	m_pCbFunc;
	PVOID			m_pPrivate;

	// Internal functions for scan parameters
	DWORD			setScanCapabilities(const EslParamHdr* pParam);
	DWORD			conversionSaveCapSDKtoSV(const EslParamHdr* pParam);
	DWORD			conversionScanCapSDKtoSV(const EslParamHdr* pParam);
	DWORD			conversionJpegCapSDKtoSV(const EslParamHdr* pParam);
	DWORD			conversionTiffCapSDKtoSV(const EslParamHdr* pParam);
	void			StandardSizeToPixel(eDocSize docSize, eSizeUnit sizeUnit, RECT sizeUser);
	double			calculate_pixel_size(double value);

	// Internal functions for info parameters
	DWORD			getInfoCapabilities(EslParamHdr* pParam);

	// Internal functions to start operations
	DWORD			execScan(eExecType ExecType);
	DWORD			execSave(eExecType ExecType);

	void			execScanAndSave(void);
	void			execScanAndStore(void);
	void			execSaveStored(void);

	// Send scanned image to StorageMgr
	//DWORD			sendToStorage(IMG_HEADER imghdr, PBYTE buffer, bool LastImage);

	static	void	showEslError(DWORD Code);
	static	void	showEslDeviceStatus(DWORD Code);

	DWORD			setParam(const EslParamHdr* pParam);
};
