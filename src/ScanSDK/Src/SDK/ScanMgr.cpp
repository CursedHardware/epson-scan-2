#include "ScanMgr.h"
#include <DbgLog.h>

#define SDI_TRACE_LOG(message, ...) \
        AfxGetLog()->MessageLog(ENUM_LOG_LEVEL::LogLevelTrace,__FUNCTION__,__FILE__,__LINE__, message, ##__VA_ARGS__ )

extern HMODULE g_hModule;

BOOL CALLBACK scanstatusCallback(EslCBDataScanStatus* scanstatus, PVOID pPrivate)
{
	// If application wants to cancel process, return ESL_CANCEL
	CScanMgr* pScanMgr = (CScanMgr*)pPrivate;
	if (pScanMgr) {
		return pScanMgr->sendAppCallbackScanStatus(scanstatus);
	}

	return FALSE;
}

BOOL CALLBACK imagefileCallback(EslCBDImageFile* imagefile, PVOID pPrivate)
{
	// If application wants to cancel process, return ESL_CANCEL
	CScanMgr* pScanMgr = (CScanMgr*)pPrivate;
	if (pScanMgr) {
		return pScanMgr->sendAppCallbackImageFile(imagefile);
	}

	return FALSE;
}

CScanMgr::CScanMgr(void) :
m_ShowIndicators(TRUE),
m_ColorSlipReduction(FALSE),
m_pCbFunc(NULL),
m_pPrivate(NULL),
m_sv(NULL)
{
}

CScanMgr::~CScanMgr(void)
{
	SDI_TRACE_LOG("CScanMgr::~CScanMgr");
	if(m_sv->driver){
        m_sv->SDIScannerDriver_DisposePtr_(m_sv->driver);
		SDI_TRACE_LOG("[INFO]Dispose OK");
        m_sv->driver = nullptr;
    }
	if(m_sv){
		m_sv->Terminate();

		delete m_sv;
        m_sv = NULL;
	}
}

void CScanMgr::free(void)
{
	DeviceList::device_list.clear();
    DeviceList::manu_network_device_list.clear();
}

DWORD CScanMgr::openScanner(const ConnectInfo* pScanner)
{
	SDI_TRACE_LOG("openScannerr");
	m_sv = new Supervisor();
	if(!m_sv){
		SDI_TRACE_LOG("[ERROR]m_sv is null");
        return ESL_ERR_GENERAL;
	}

	SDIDeviceInfo devInfo;
	DWORD err = ESL_SUCCESS;

	m_sv->SetUp();

	DeviceList* finder = new DeviceList();
    if(finder == NULL){
		SDI_TRACE_LOG("[ERROR]An unexpected error occurred. Epson Scan 2 will close.");
        return ESL_ERR_GENERAL;
    }

	SDI_TRACE_LOG("[INFO]Connect Device : %s", (const char*)pScanner->deviceName);
	switch (pScanner->connectType) {
	case CT_DEFAULT:
	case CT_LOCAL:
		SDI_TRACE_LOG("[INFO]ConnectType : CT_LOCAL");
		devInfo = finder->list((const char*)pScanner->deviceName);
		if(devInfo.displayName[0] == '\0'){
			//デバイスがみつからないとき
			m_sv->m_LastError = kSDIErrorDeviceNotFound;
			delete finder;
   			finder = NULL;
			return ESL_ERR_OPEN_FAIL;
		}else {
			SDI_TRACE_LOG("/////////USB DEVICE Information//////////");
			SDI_TRACE_LOG("[INFO]displayName : %s", devInfo.displayName);
			SDI_TRACE_LOG("[INFO]modelID : %d", devInfo.modelID);
			SDI_TRACE_LOG("[INFO]productID : %d", devInfo.productID);
		}
		break;
	case CT_NET_IPV4:
		SDI_TRACE_LOG("[INFO]ConnectType : CT_NET_IPV4");
		devInfo = finder->list((const char*)pScanner->networkAddress, true);
		if(devInfo.ipAddress[0] != '\0'){
			SDI_TRACE_LOG("/////////Network DEVICE Information//////////");
			SDI_TRACE_LOG("[INFO]ipAddress : %s", devInfo.ipAddress);
			SDI_TRACE_LOG("[INFO]modelID : %d", devInfo.modelID);
			SDI_TRACE_LOG("[INFO]productID : %d", devInfo.productID);
		}else {
			//デバイスがみつからないとき
			m_sv->m_LastError = kSDIErrorDeviceNotFound;
			delete finder;
   			finder = NULL;
			return ESL_ERR_OPEN_FAIL;
		}
		break;
	default:
		SDI_TRACE_LOG("[ERROR]ESL_ERR_INVALID_PARAM");
		delete finder;
   		finder = NULL;
		return ESL_ERR_INVALID_PARAM;
	}
	delete finder;
    finder = NULL;

    m_sv->SDIScannerDriver_CreatePtr_(&m_sv->driver, &devInfo, nullptr, nullptr);
    SDI_TRACE_LOG("[INFO]Create OK", devInfo.ipAddress);

    if(m_sv->driver == nullptr){
		SDI_TRACE_LOG("[ERROR]driver is null");
        assert(false);
    }

    m_sv->m_LastError = m_sv->SDIScannerDriver_OpenPtr_(m_sv->driver);
    if(m_sv->m_LastError != kSDIErrorNone){
		SDI_TRACE_LOG("[ERROR]ESL_ERR_OPEN_FAIL");
        err = ESL_ERR_OPEN_FAIL;
    }else {
        err = ESL_SUCCESS;
    }

	// Load default value
	this->loadScanDefault();
	this->loadDefault();

	return err;
}

void CScanMgr::closeScanner(void)
{
	SDI_TRACE_LOG("closeScanner");
	this->m_pCbFunc = NULL;
	this->m_pPrivate = NULL;

    if(m_sv->driver){
        m_sv->SDIScannerDriver_ClosePtr_(m_sv->driver);
		m_sv->SDIScannerDriver_DisposePtr_(m_sv->driver);
		m_sv->driver = nullptr;
		SDI_TRACE_LOG("[INFO]Close OK");
    }else {
		SDI_TRACE_LOG("[ERROR]driver is null");
	}
}

DWORD CScanMgr::ctrlParameter(const eParamOpType OpType, EslParamHdr* pParam)
{
	SDI_TRACE_LOG("ctrlParameter");

	DWORD err = ESL_SUCCESS;

	switch (OpType) {
	case POT_SET:
		err = this->setParameter(pParam);
		if (err != ESL_SUCCESS) {
			SDI_TRACE_LOG("[ERROR]setParameter failed");
		}
		break;
	case POT_GET_CURRENT:
		err = this->getCurrentParameter(pParam);
		if (err != ESL_SUCCESS) {
			SDI_TRACE_LOG("[ERROR]getCurrentParameter failed");
		}
		break;
	case POT_GET_DEFAULT:
		err = this->getDefaultParameter(pParam);
		if (err != ESL_SUCCESS) {
			SDI_TRACE_LOG("[ERROR]getDefaultParameter failed");
		}
		break;
	}
	return err;
}

DWORD CScanMgr::execute(const eExecType ExecType, peslCallback pCbFunc, PVOID pPrivate)
{
	SDI_TRACE_LOG("execute");

	DWORD err = ESL_SUCCESS;

	// Cache callback function pointer and private data
	m_pCbFunc = pCbFunc;
	SDI_TRACE_LOG("[INFO]Callback function pointer set");
	m_pPrivate = pPrivate;
	if (m_pCbFunc == NULL || m_pPrivate == NULL) {
		SDI_TRACE_LOG("[INFO]Callback function pointer is not set");
	}

	m_sv->setCallback(CBDT_STATUS_SCAN, (PVOID)scanstatusCallback, this);
	m_sv->setCallback(CBDT_IMAGE_FILE, (PVOID)imagefileCallback, this);

	// Execute specified operation
	switch (ExecType) {
	case ET_SCAN_AND_SAVE:
	case ET_SCAN_AND_STORE:
		// Start Scanning
		err = this->execScan(ExecType);
		break;
	case ET_SAVE_STORED_IMAGE:
	case ET_GET_STORED_IMAGE:
		// Start Storage Operation
		err = this->execSave(ExecType);
		break;
	default:
		SDI_TRACE_LOG("[ERROR]ESL_ERR_INVALID_PARAM");
		err = ESL_ERR_INVALID_PARAM;
	}
	this->showEslError(err);

	return err;
}

void CScanMgr::setSDKDefault(void)
{
	SDI_TRACE_LOG("setSDKDefault");
	//setting default value
	m_sv->device_data.AutomaticFeedingMode.select = 0;
	m_sv->device_data.AFMTimeout.select = 0;
	m_sv->device_data.Add_Pages = 0;
	m_sv->device_data.ScanArea.AutoSize.select = 1;
	m_sv->device_data.BackgroundColor.select = 1;
	m_sv->device_data.BackgroundRemoval.select = 0;
	m_sv->device_data.BlankPageSkipKey.select = 0;
	m_sv->device_data.BlankPageSkipLevelKey.select = 10;
	m_sv->device_data.Brightness.select = 0;
	m_sv->device_data.ColorType.select = 0;
	m_sv->device_data.Contrast.select = 0;
	m_sv->device_data.DNShow_LongPaperWarning_dialog = 0;
	m_sv->device_data.DNShow_PlasticCard_dialog = 0;
	m_sv->device_data.DoubleFeedDetectionValue.DoubleFeedDetection.select = 1;
	m_sv->device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaLength.select = 1550;
	m_sv->device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaMin.select = 0;
	m_sv->device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel.select = 0;
	m_sv->device_data.DropoutColor.select = 0;
	m_sv->device_data.DuplexType.select = 1;
	m_sv->device_data.FileName_Counter = 0;
	m_sv->device_data.FileName_OverWrite = 0;
	m_sv->device_data.Prefix = "img";
	m_sv->device_data.ScanArea.FixedSize = (DocumentSize)0;
    m_sv->device_data.Folder = 101;
	m_sv->device_data.FunctionalUnit.select = 0;
	m_sv->device_data.FunctionalUnit_Auto = 0;
	m_sv->device_data.Gamma.select = 22;
    m_sv->device_data.ImageFormatSettings.ImageFormat.select = 6;
	m_sv->device_data.ImageOption = 0;
	m_sv->device_data.ImageFormatSettings.JpegQuality.select = 80;
	m_sv->device_data.ImageFormatSettings.JpegQuality_forJPEG = 80;
	m_sv->device_data.ImageFormatSettings.JpegQuality_forPDF = 30;
	m_sv->device_data.ImageFormatSettings.TiffCompression.select = 0;
	m_sv->device_data.ImageFormatSettings.MultiTiffEnabled.select = 0;
	m_sv->device_data.NearDurationAlert = 0;
	m_sv->device_data.Orientation.select = 0;
	m_sv->device_data.OverDurationAlert = 0;
    m_sv->device_data.ImageFormatSettings.PDFImgFormatOption.AllPage = 1;
    m_sv->device_data.ImageFormatSettings.PDFImgFormatOption.page = 1;
	m_sv->device_data.PagesTobeScanned.select = 0;
	m_sv->device_data.PaperDeskew.select = 1;
	m_sv->device_data.ScanArea.PaperEndDetection.select = 0;
	m_sv->device_data.ScanArea.PaperEndDetectionMemory = 0;
	m_sv->device_data.Resolution.select = 200;
	m_sv->device_data.ScanArea.ScanAreaHeight.select = 2280;
	m_sv->device_data.ScanArea.ScanAreaOffsetX.select = 0;
	m_sv->device_data.ScanArea.ScanAreaOffsetY.select = 0;
	m_sv->device_data.ScanArea.ScanAreaWidth.select = 2280;
	m_sv->device_data.Threshold.select = 0;
	m_sv->device_data.TransferCompression.select = 1;
    m_sv->device_data.UserDefinePath = ".";
	m_sv->device_data.ScanArea.UserDefine_Width = 0;
	m_sv->device_data.ScanArea.UserDefine_Length = 0;
	m_sv->device_data.ImageFormatSettings.JpegProgressive.select = 0;

	m_sv->device_data.textEnhance.select = 0;

	if(m_sv->device_data.Folder == USER_DEFINE){
        m_sv->device_data.SelectPath = m_sv->device_data.UserDefinePath;
    }
}

void CScanMgr::loadScanDefault(void)
{
	SDI_TRACE_LOG("loadScanDefault");
	memset(&m_ParamScanDefault, 0, sizeof(EslParamScanExV1));

	this->setSDKDefault();
	m_sv->Get_All_Value();

	//FunctionalUnit
	if(m_sv->device_data.FunctionalUnit.select == Auto){
		m_ParamScanDefault.docSource = SRC_AUTO;
	}else if(m_sv->device_data.FunctionalUnit.select == Flatbed){
		m_ParamScanDefault.docSource = SRC_FLATBED;
	}else if(m_sv->device_data.FunctionalUnit.select == ADF){
		if(m_sv->device_data.DuplexType.select == Single_Sided){
			m_ParamScanDefault.docSource = SRC_FEEDER_SIMPLEX;
		}else if(m_sv->device_data.DuplexType.select == Double_Sided){
			m_ParamScanDefault.docSource = SRC_FEEDER_DUPLEX;
		}
	}
	//ColorType
	if(m_sv->device_data.ColorType.select == ColorTypeAuto){
		m_ParamScanDefault.imgType = IT_AUTO;
	}else if(m_sv->device_data.ColorType.select == ColorTypeRGB24){
		m_ParamScanDefault.imgType = IT_COLOR;
	}else if(m_sv->device_data.ColorType.select == ColorTypeMono8){
		m_ParamScanDefault.imgType = IT_GRAY;
	}else if(m_sv->device_data.ColorType.select == ColorTypeMono1){
		m_ParamScanDefault.imgType = IT_MONO;
	}

	//Resolution
	m_ParamScanDefault.resolution = m_sv->device_data.Resolution.select;

	//ScanArea
	m_ParamScanDefault.docSize = DS_A4;
	m_ParamScanDefault.sizeUnit = SU_INCH;
	m_ParamScanDefault.sizeUser.bottom = 0;
	m_ParamScanDefault.sizeUser.left = 0;
	m_ParamScanDefault.sizeUser.right = 0;
	m_ParamScanDefault.sizeUser.top = 0;

	//Orientation
	if(m_sv->device_data.Orientation.select == OrientationAuto){
		m_ParamScanDefault.docRotate = DR_AUTO;
	}else if(m_sv->device_data.Orientation.select == OrientationNone){
		m_ParamScanDefault.docRotate = DR_NONE;
	}else if(m_sv->device_data.Orientation.select == Orientation90){
		m_ParamScanDefault.docRotate = DR_R90;
	}else if(m_sv->device_data.Orientation.select == Orientation180){
		m_ParamScanDefault.docRotate = DR_R180;
	}else if(m_sv->device_data.Orientation.select == Orientation270){
		m_ParamScanDefault.docRotate = DR_R270;
	}

	//Brightness
	m_ParamScanDefault.brightness = m_sv->device_data.Brightness.select * 10;

	//Contrast
	m_ParamScanDefault.contrast = m_sv->device_data.Contrast.select * 10;

	//Threshold
	m_ParamScanDefault.threshold = m_sv->device_data.Threshold.select;

	//DropoutColor
	if(m_sv->device_data.DropoutColor.select == DropoutColorNone){
		m_ParamScanDefault.colorFilter = CF_NONE;
	}else if(m_sv->device_data.DropoutColor.select == DropoutColorRed){
		m_ParamScanDefault.colorFilter = CF_DROPOUT_RED;
	}else if(m_sv->device_data.DropoutColor.select == DropoutColorGreen){
		m_ParamScanDefault.colorFilter = CF_DROPOUT_GREEN;
	}else if(m_sv->device_data.DropoutColor.select == DropoutColorBlue){
		m_ParamScanDefault.colorFilter = CF_DROPOUT_BLUE;
	}

	//PagesTobeScanned
	m_ParamScanDefault.numScan = m_sv->device_data.PagesTobeScanned.select;

	//DoubleFeedDetection
	if(m_sv->device_data.DoubleFeedDetectionValue.DoubleFeedDetection.select == false){
		m_ParamScanDefault.optDoubleFeedDetect = DFD_NONE;
	}else if(m_sv->device_data.DoubleFeedDetectionValue.DoubleFeedDetection.select == true){
		if(m_sv->device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel.select == DoubleFeedDetectionLow){
			m_ParamScanDefault.optDoubleFeedDetect = DFD_LOW;
		}else if(m_sv->device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel.select == DoubleFeedDetectionHigh){
			m_ParamScanDefault.optDoubleFeedDetect = DFD_HIGH;
		}
	}

	//BlankPageSkip
	m_ParamScanDefault.optBlankPageSkip = BPS_NONE; //TBD

	//PaperDeskew
	if(m_sv->device_data.PaperDeskew.select == false){
		m_ParamScanDefault.skewCorrect = SC_NONE;
	}else if(m_sv->device_data.PaperDeskew.select == true){
		m_ParamScanDefault.skewCorrect = SC_EDGE;
	}

	//textEnhance
	if(m_sv->device_data.textEnhance.select == TextEnhanceNone){
		m_ParamScanDefault.docEnhance = DE_NONE;
	}else if(m_sv->device_data.textEnhance.select == TextEnhanceNormal){
		m_ParamScanDefault.docEnhance = DE_TET_STD;
	}else if(m_sv->device_data.textEnhance.select == TextEnhanceHigh){
		m_ParamScanDefault.docEnhance = DE_TET_HIGH;
	}

/*
	std::cout << "[INFO]---------------------------" << std::endl;
	std::cout << "[INFO]type: " << m_ParamScanDefault.type << std::endl;
	std::cout << "[INFO]size: " << m_ParamScanDefault.size << std::endl;
	std::cout << "[INFO]docSource: " << m_ParamScanDefault.docSource << std::endl;
	std::cout << "[INFO]imgType: " << m_ParamScanDefault.imgType << std::endl;
	std::cout << "[INFO]resolution: " << m_ParamScanDefault.resolution << std::endl;
	std::cout << "[INFO]docSize: " << m_ParamScanDefault.docSize << std::endl;
	std::cout << "[INFO]sizeUnit: " << m_ParamScanDefault.sizeUnit << std::endl;
	std::cout << "[INFO]sizeUser.left: " << m_ParamScanDefault.sizeUser.left << std::endl;
	std::cout << "[INFO]sizeUser.top: " << m_ParamScanDefault.sizeUser.top << std::endl;
	std::cout << "[INFO]sizeUser.right: " << m_ParamScanDefault.sizeUser.right << std::endl;
	std::cout << "[INFO]sizeUser.bottom: " << m_ParamScanDefault.sizeUser.bottom << std::endl;
	std::cout << "[INFO]docRotate: " << m_ParamScanDefault.docRotate << std::endl;
	std::cout << "[INFO]brightness: " << m_ParamScanDefault.brightness << std::endl;
	std::cout << "[INFO]contrast: " << m_ParamScanDefault.contrast << std::endl;
	std::cout << "[INFO]threshold: " << m_ParamScanDefault.threshold << std::endl;
	std::cout << "[INFO]colorFilter: " << m_ParamScanDefault.colorFilter << std::endl;
	std::cout << "[INFO]numScan: " << m_ParamScanDefault.numScan << std::endl;
	std::cout << "[INFO]optDoubleFeedDetect: " << m_ParamScanDefault.optDoubleFeedDetect << std::endl;
	std::cout << "[INFO]optBlankPageSkip: " << m_ParamScanDefault.optBlankPageSkip << std::endl;
	std::cout << "[INFO]skewCorrect: " << m_ParamScanDefault.skewCorrect << std::endl;
	std::cout << "[INFO]---------------------------" << std::endl;
*/
	// Copy to Scan Current parameters
	memcpy_s(&m_ParamScanCurrent, sizeof(EslParamScanExV1), &m_ParamScanDefault, sizeof(EslParamScanExV1));
}

void CScanMgr::loadDefault(void)
{
	SDI_TRACE_LOG("loadDefault");

	memset(&m_ParamSaveDefault, 0, sizeof(EslParamSaveBasic));
	memset(&m_ParamJpegDefault, 0, sizeof(EslParamSaveJPEG));
	memset(&m_ParamTiffDefault, 0, sizeof(EslParamSaveTIFF));

	//ParamSave
	m_ParamSaveDefault.type = ESL_PARAM_SAVE_BASIC;
	m_ParamSaveDefault.size = sizeof(EslParamSaveBasic);
	m_ParamSaveDefault.numPageStart = 0;
	m_ParamSaveDefault.numPageEnd = 0;
	m_ParamSaveDefault.fileFormat = FF_JPEG;
	memcpy_s(m_ParamSaveDefault.filePath, MAX_PATH, ".", strlen(".")+1);
	m_ParamSaveDefault.fileNameType = FN_PREFIX;
	memcpy_s(m_ParamSaveDefault.fileNamePrefix, 40, "image", strlen("image")+1);
	m_ParamSaveDefault.fileNumStartCount = 0;
/*
	std::cout << "[INFO]----------BASIC-----------------" << std::endl;
	std::cout << "[INFO]type: " << m_ParamSaveDefault.type << std::endl;
	std::cout << "[INFO]size: " << m_ParamSaveDefault.size << std::endl;
	std::cout << "[INFO]numPageStart: " << m_ParamSaveDefault.numPageStart << std::endl;
	std::cout << "[INFO]numPageEnd: " << m_ParamSaveDefault.numPageEnd << std::endl;
	std::cout << "[INFO]fileFormat: " << m_ParamSaveDefault.fileFormat << std::endl;
	std::cout << "[INFO]filePath: " << m_ParamSaveDefault.filePath << std::endl;
	std::cout << "[INFO]fileNameType: " << m_ParamSaveDefault.fileNameType << std::endl;
	std::cout << "[INFO]fileNamePrefix: " << m_ParamSaveDefault.fileNamePrefix << std::endl;
	std::cout << "[INFO]fileNumStartCount: " << m_ParamSaveDefault.fileNumStartCount << std::endl;
*/
	memcpy_s(&m_ParamSaveCurrent, sizeof(EslParamSaveBasic), &m_ParamSaveDefault, sizeof(EslParamSaveBasic));

	//ParamJpeg
	m_ParamJpegDefault.type = ESL_PARAM_SAVE_JPEG;
	m_ParamJpegDefault.size = sizeof(EslParamSaveJPEG);
	m_ParamJpegDefault.imgQuality = 80;
	m_ParamJpegDefault.encProgressive = FALSE;
/*
	std::cout << "[INFO]------------JPEG---------------" << std::endl;
	std::cout << "[INFO]type: " << m_ParamJpegDefault.type << std::endl;
	std::cout << "[INFO]size: " << m_ParamJpegDefault.size << std::endl;
	std::cout << "[INFO]imgQuality: " << m_ParamJpegDefault.imgQuality << std::endl;
	std::cout << "[INFO]encProgressive: " << m_ParamJpegDefault.encProgressive << std::endl;
*/
	memcpy_s(&m_ParamJpegCurrent, sizeof(EslParamSaveJPEG), &m_ParamJpegDefault, sizeof(EslParamSaveJPEG));

	//ParamTiff
	m_ParamTiffDefault.type = ESL_PARAM_SAVE_TIFF;
	m_ParamTiffDefault.size = sizeof(EslParamSaveTIFF);
	m_ParamTiffDefault.compMono = TC_NONE;
/*
	std::cout << "[INFO]------------TIFF---------------" << std::endl;
	std::cout << "[INFO]type: " << m_ParamTiffDefault.type << std::endl;
	std::cout << "[INFO]size: " << m_ParamTiffDefault.size << std::endl;
	std::cout << "[INFO]compMono: " << m_ParamTiffDefault.compMono << std::endl;
	std::cout << "[INFO]---------------------------" << std::endl;
*/
	memcpy_s(&m_ParamTiffCurrent, sizeof(EslParamSaveTIFF), &m_ParamTiffDefault, sizeof(EslParamSaveTIFF));
}

DWORD CScanMgr::setParameter(EslParamHdr* pParam)
{
	SDI_TRACE_LOG("setParameter");

	DWORD err = ESL_SUCCESS;

	switch (pParam->type) {
	case ESL_PARAM_SCAN_BASIC:
	case ESL_PARAM_SCAN_EX_V1:
		err = this->setParamScan(pParam);
		if (err != ESL_SUCCESS) {
			SDI_TRACE_LOG("[ERROR]this->setParamScan failed");
		}
		break;
	case ESL_PARAM_SAVE_BASIC:
	case ESL_PARAM_SAVE_JPEG:
	case ESL_PARAM_SAVE_TIFF:
		err = this->setParamSave(pParam);
		if (err != ESL_SUCCESS) {
			SDI_TRACE_LOG("[ERROR]this->setParamSave failed");
		}
		break;
	default:
		SDI_TRACE_LOG("[ERROR]Invalid operation for this type of parameter struct");
		err = ESL_ERR_INVALID_OPERATION;
	}

	return err;
}

DWORD CScanMgr::getCurrentParameter(EslParamHdr* pParam)
{
	SDI_TRACE_LOG("getCurrentParameter");
	DWORD err = ESL_SUCCESS;

	switch (pParam->type) {
	case ESL_PARAM_SCAN_BASIC:
	case ESL_PARAM_SCAN_EX_V1:
		err = this->getCurrentParamScan(pParam);
		if (err != ESL_SUCCESS) {
			SDI_TRACE_LOG("[ERROR]this->getCurrentParamScan failed");
		}
		break;
	case ESL_PARAM_SAVE_BASIC:
	case ESL_PARAM_SAVE_JPEG:
	case ESL_PARAM_SAVE_TIFF:
		err = this->getCurrentParamSave(pParam);
		if (err != ESL_SUCCESS) {
			SDI_TRACE_LOG("[ERROR]this->getCurrentParamSave failed");
		}
		break;
	case ESL_PARAM_INFO_BASIC:
		err = this->getParamInfo(pParam);
		if (err != ESL_SUCCESS) {
			SDI_TRACE_LOG("[ERROR]this->getParamInfo failed");
		}
		break;
	}

	return err;
}

DWORD CScanMgr::getDefaultParameter(EslParamHdr* pParam)
{
	SDI_TRACE_LOG("getDefaultParameter");

	DWORD err = ESL_SUCCESS;

	switch (pParam->type) {
	case ESL_PARAM_SCAN_BASIC:
	case ESL_PARAM_SCAN_EX_V1:
		err = this->getDefaultParamScan(pParam);
		if (err != ESL_SUCCESS) {
			SDI_TRACE_LOG("[ERROR]this->getDefaultParamScan failed");
		}
		break;
	case ESL_PARAM_SAVE_BASIC:
	case ESL_PARAM_SAVE_JPEG:
	case ESL_PARAM_SAVE_TIFF:
		err = this->getDefaultParamSave(pParam);
		if (err != ESL_SUCCESS) {
			SDI_TRACE_LOG("[ERROR]this->getDefaultParamSaveBasic failed");
		}
		break;
	case ESL_PARAM_INFO_BASIC:
		err = this->getParamInfo(pParam);
		if (err != ESL_SUCCESS) {
			SDI_TRACE_LOG("[ERROR]this->getParamInfo failed");
		}
		break;
	}

	return err;
}

DWORD CScanMgr::setParamScan(const EslParamHdr* pParam)
{
	SDI_TRACE_LOG("setParamScan");

	DWORD err = ESL_SUCCESS;

	if (err != ESL_SUCCESS) {
		SDI_TRACE_LOG("[ERROR]ESL_ERR_INVALID_PARAM");
		return ESL_ERR_INVALID_PARAM;
	}

	err = this->setScanCapabilities(pParam);
	if (err != ESL_SUCCESS) {
		SDI_TRACE_LOG("[ERROR]ESL_ERR_INVALID_PARAM");
		return ESL_ERR_INVALID_PARAM;
	}

	return err;
}

DWORD CScanMgr::getCurrentParamScan(EslParamHdr* pParam)
{
	SDI_TRACE_LOG("getCurrentParamScan");

	switch (pParam->type) {
	case ESL_PARAM_SCAN_BASIC:
		SDI_TRACE_LOG("[INFO]ESL_PARAM_SCAN_BASIC get");
		memcpy_s(pParam, sizeof(EslParamScanBasic), &m_ParamScanCurrent, sizeof(EslParamScanBasic));
		pParam->type = ESL_PARAM_SCAN_BASIC;
		pParam->size = sizeof(EslParamScanBasic);
		break;
	case ESL_PARAM_SCAN_EX_V1:
		SDI_TRACE_LOG("[INFO]ESL_PARAM_SCAN_EX_V1 get");
		memcpy_s(pParam, sizeof(EslParamScanExV1), &m_ParamScanCurrent, sizeof(EslParamScanExV1));
		pParam->type = ESL_PARAM_SCAN_EX_V1;
		pParam->size = sizeof(EslParamScanExV1);
		break;
	}

	return ESL_SUCCESS;
}

DWORD CScanMgr::getDefaultParamScan(EslParamHdr* pParam)
{
	SDI_TRACE_LOG("getDefaultParamScan");

	switch (pParam->type) {
	case ESL_PARAM_SCAN_BASIC:
		SDI_TRACE_LOG("[INFO]ESL_PARAM_SCAN_BASIC get");
		memcpy_s(pParam, sizeof(EslParamScanBasic), &m_ParamScanDefault, sizeof(EslParamScanBasic));
		pParam->type = ESL_PARAM_SCAN_BASIC;
		pParam->size = sizeof(EslParamScanBasic);
		break;
	case ESL_PARAM_SCAN_EX_V1:
		SDI_TRACE_LOG("[INFO]ESL_PARAM_SCAN_EX_V1 get");
		memcpy_s(pParam, sizeof(EslParamScanExV1), &m_ParamScanDefault, sizeof(EslParamScanExV1));
		pParam->type = ESL_PARAM_SCAN_EX_V1;
		pParam->size = sizeof(EslParamScanExV1);
		break;
	}

	return ESL_SUCCESS;
}

DWORD CScanMgr::setParamSave(const EslParamHdr* pParam)
{
	SDI_TRACE_LOG("getDefaultParamScan");

	DWORD err = ESL_SUCCESS;

	err = this->setParam(pParam);
	if (err != ESL_SUCCESS) {
		SDI_TRACE_LOG("[ERROR]ESL_ERR_INVALID_PARAM");
		return err;
	}

	return ESL_SUCCESS;
}

DWORD CScanMgr::getCurrentParamSave(EslParamHdr* pParam)
{
	SDI_TRACE_LOG("getCurrentParamSave");

	switch (pParam->type) {
	case ESL_PARAM_SAVE_BASIC:
		SDI_TRACE_LOG("[INFO]ESL_PARAM_SAVE_BASIC get");
		memcpy_s(pParam, sizeof(EslParamSaveBasic), &m_ParamSaveCurrent, sizeof(EslParamSaveBasic));
		pParam->type = ESL_PARAM_SAVE_BASIC;
		pParam->size = sizeof(EslParamSaveBasic);
		break;
	case ESL_PARAM_SAVE_JPEG:
		SDI_TRACE_LOG("[INFO]ESL_PARAM_SAVE_JPEG get");
		memcpy_s(pParam, sizeof(EslParamSaveJPEG), &m_ParamJpegCurrent, sizeof(EslParamSaveJPEG));
		break;
	case ESL_PARAM_SAVE_TIFF:
		SDI_TRACE_LOG("[INFO]ESL_PARAM_SAVE_TIFF get");
		memcpy_s(pParam, sizeof(EslParamSaveTIFF), &m_ParamTiffCurrent, sizeof(EslParamSaveTIFF));
		break;
	default:
		SDI_TRACE_LOG("[ERROR]ESL_ERR_INVALID_PARAM");
		return ESL_ERR_INVALID_PARAM;
	}

	return ESL_SUCCESS;
}

DWORD CScanMgr::getDefaultParamSave(EslParamHdr* pParam)
{
	SDI_TRACE_LOG("getDefaultParamSave");

	switch (pParam->type) {
	case ESL_PARAM_SAVE_BASIC:
		SDI_TRACE_LOG("[INFO]ESL_PARAM_SAVE_BASIC get");
		memcpy_s(pParam, sizeof(EslParamSaveBasic), &m_ParamSaveDefault, sizeof(EslParamSaveBasic));
		pParam->type = ESL_PARAM_SAVE_BASIC;
		pParam->size = sizeof(EslParamSaveBasic);
		break;
	case ESL_PARAM_SAVE_JPEG:
		SDI_TRACE_LOG("[INFO]ESL_PARAM_SAVE_JPEG get");
		memcpy_s(pParam, sizeof(EslParamSaveJPEG), &m_ParamJpegDefault, sizeof(EslParamSaveJPEG));
		break;
	case ESL_PARAM_SAVE_TIFF:
		SDI_TRACE_LOG("[INFO]ESL_PARAM_SAVE_TIFF get");
		memcpy_s(pParam, sizeof(EslParamSaveTIFF), &m_ParamTiffDefault, sizeof(EslParamSaveTIFF));
		break;
	default:
		SDI_TRACE_LOG("[ERROR]ESL_ERR_INVALID_PARAM");
		return ESL_ERR_INVALID_PARAM;
	}

	return ESL_SUCCESS;
}

DWORD CScanMgr::getParamInfo(EslParamHdr* pParam)
{
	SDI_TRACE_LOG("getParamInfo");
	DWORD err = ESL_SUCCESS;

	switch (pParam->type) {
	case ESL_PARAM_INFO_BASIC:
		SDI_TRACE_LOG("[INFO]ESL_PARAM_INFO_BASIC get");
		err = this->getInfoCapabilities(pParam);
		if (err != ESL_SUCCESS) {
			SDI_TRACE_LOG("[ERROR]getInfoCapabilities is failed");
			return err;
		}
		break;
	default:
		SDI_TRACE_LOG("[ERROR]ESL_ERR_INVALID_PARAM");
		return ESL_ERR_INVALID_PARAM;
	}

	return ESL_SUCCESS;
}

DWORD CScanMgr::setScanCapabilities(const EslParamHdr* pParam)
{	
	SDI_TRACE_LOG("setScanCapabilities");

	DWORD err = ESL_SUCCESS;
	EslParamScanExV1 param;

	// Get Current Full parameters
	memcpy_s(&param, sizeof(EslParamScanExV1), &m_ParamScanCurrent, sizeof(EslParamScanExV1));

	// Overwrite parameter structure depends on the parameter type
	if (pParam->type == ESL_PARAM_SCAN_BASIC) {
		memcpy_s(&param, sizeof(EslParamScanExV1), pParam, sizeof(EslParamScanBasic));
	} else if (pParam->type == ESL_PARAM_SCAN_EX_V1) {
		memcpy_s(&param, sizeof(EslParamScanExV1), pParam, sizeof(EslParamScanExV1));
	} else {
		SDI_TRACE_LOG("[ERROR]ESL_ERR_INVALID_PARAM");
		return ESL_ERR_INVALID_PARAM;
	}

/*
	std::cout << "[INFO]---------------------------" << std::endl;
	std::cout << "[INFO]type: " << param.type << std::endl;
	std::cout << "[INFO]size: " << param.size << std::endl;
	std::cout << "[INFO]docSource: " << param.docSource << std::endl;
	std::cout << "[INFO]imgType: " << param.imgType << std::endl;
	std::cout << "[INFO]resolution: " << param.resolution << std::endl;
	std::cout << "[INFO]docSize: " << param.docSize << std::endl;
	std::cout << "[INFO]sizeUnit: " << param.sizeUnit << std::endl;
	std::cout << "[INFO]sizeUser.left: " << param.sizeUser.left << std::endl;
	std::cout << "[INFO]sizeUser.top: " << param.sizeUser.top << std::endl;
	std::cout << "[INFO]sizeUser.right: " << param.sizeUser.right << std::endl;
	std::cout << "[INFO]sizeUser.bottom: " << param.sizeUser.bottom << std::endl;
	std::cout << "[INFO]docRotate: " << param.docRotate << std::endl;
	std::cout << "[INFO]brightness: " << param.brightness << std::endl;
	std::cout << "[INFO]contrast: " << param.contrast << std::endl;
	std::cout << "[INFO]threshold: " << param.threshold << std::endl;
	std::cout << "[INFO]colorFilter: " << param.colorFilter << std::endl;
	std::cout << "[INFO]numScan: " << param.numScan << std::endl;
	std::cout << "[INFO]optDoubleFeedDetect: " << param.optDoubleFeedDetect << std::endl;
	std::cout << "[INFO]optBlankPageSkip: " << param.optBlankPageSkip << std::endl;
	std::cout << "[INFO]skewCorrect: " << param.skewCorrect << std::endl;
	std::cout << "[INFO]---------------------------" << std::endl;
*/
	if (err == ESL_SUCCESS) {
		memcpy_s(&m_ParamScanCurrent, sizeof(EslParamScanExV1), &param, sizeof(EslParamScanExV1));
	}

	return ESL_SUCCESS;
}

DWORD CScanMgr::conversionSaveCapSDKtoSV(const EslParamHdr* pParam)
{
	SDI_TRACE_LOG("conversionSaveCapSDKtoSV");

	DWORD err = ESL_SUCCESS;
	EslParamSaveBasic param;

	// Get Current Full parameters
	memcpy_s(&param, sizeof(EslParamSaveBasic), &m_ParamSaveCurrent, sizeof(EslParamSaveBasic));

	// Overwrite parameter structure depends on the parameter type
	if (pParam->type == ESL_PARAM_SAVE_BASIC) {
		memcpy_s(&param, sizeof(EslParamSaveBasic), pParam, sizeof(EslParamSaveBasic));
	} else {
		SDI_TRACE_LOG("[ERROR]ESL_ERR_INVALID_PARAM");
		return ESL_ERR_INVALID_PARAM;
	}

	SDI_TRACE_LOG("[INFO]---------------------------");
	SDI_TRACE_LOG("[INFO]type: %d", param.type);
	SDI_TRACE_LOG("[INFO]size: %d", param.size);
	SDI_TRACE_LOG("[INFO]numPageStart: %d", param.numPageStart);
	SDI_TRACE_LOG("[INFO]numPageEnd: %d", param.numPageEnd);
	SDI_TRACE_LOG("[INFO]fileFormat: %d", param.fileFormat);
	SDI_TRACE_LOG("[INFO]filePath: %s", param.filePath);
	SDI_TRACE_LOG("[INFO]fileNameType: %d", param.fileNameType);
	SDI_TRACE_LOG("[INFO]fileNamePrefix: %s", param.fileNamePrefix);
	SDI_TRACE_LOG("[INFO]fileNumStartCount: %d", param.fileNumStartCount);
	SDI_TRACE_LOG("[INFO]---------------------------");

	//numPageStart
	m_sv->m_numPageStart = param.numPageStart;

	//numPageEnd
	m_sv->m_numPageEnd = param.numPageEnd;
	
	//ImageFormat
	if(param.fileFormat == FF_BMP){
		m_sv->device_data.ImageFormatSettings.ImageFormat.select = ImageFormatPNM;
	}else if(param.fileFormat == FF_JPEG){
		m_sv->device_data.ImageFormatSettings.ImageFormat.select = ImageFormatJPEG;
	}else if(param.fileFormat == FF_TIFF){
		m_sv->device_data.ImageFormatSettings.ImageFormat.select = ImageFormatTIFF;
	}else if(param.fileFormat == FF_MULTITIFF){
		m_sv->device_data.ImageFormatSettings.ImageFormat.select = ImageFormatPNM;
        m_sv->device_data.ImageFormatSettings.MultiTiffEnabled.select = true;
	}else if(param.fileFormat == FF_PDF){
		m_sv->device_data.ImageFormatSettings.ImageFormat.select = ImageFormatColorJpegMonoRaw;
	}else if(param.fileFormat == FF_PNG){
		m_sv->device_data.ImageFormatSettings.ImageFormat.select = ImageFormatPNG;
	}

	//UserDefinePath
	m_sv->device_data.SelectPath = m_sv->device_data.UserDefinePath = param.filePath;
	SDI_TRACE_LOG("[INFO]UserDefinePath: %s", m_sv->device_data.UserDefinePath.c_str());

	//Prefix
	m_sv->device_data.Prefix = param.fileNamePrefix;

	//fileNumStartCount
	m_sv->m_fileNumStartCount = param.fileNumStartCount;

	return ESL_SUCCESS;
}

DWORD CScanMgr::conversionJpegCapSDKtoSV(const EslParamHdr* pParam)
{
	SDI_TRACE_LOG("conversionJpegCapSDKtoSV");

	DWORD err = ESL_SUCCESS;
	EslParamSaveJPEG param;

	// Get Current Full parameters
	memcpy_s(&param, sizeof(EslParamSaveJPEG), &m_ParamJpegCurrent, sizeof(EslParamSaveJPEG));

	// Overwrite parameter structure depends on the parameter type
	if (pParam->type == ESL_PARAM_SAVE_JPEG) {
		memcpy_s(&param, sizeof(EslParamSaveJPEG), pParam, sizeof(EslParamSaveJPEG));
	} else {
		SDI_TRACE_LOG("[ERROR]ESL_ERR_INVALID_PARAM");
		return ESL_ERR_INVALID_PARAM;
	}

	SDI_TRACE_LOG("[INFO]-----------JPEG----------------");
	SDI_TRACE_LOG("[INFO]type: %d", param.type);
	SDI_TRACE_LOG("[INFO]size: %d", param.size);
	SDI_TRACE_LOG("[INFO]imgQuality: %d", param.imgQuality);
	SDI_TRACE_LOG("[INFO]encProgressive: %d", param.encProgressive);
	SDI_TRACE_LOG("[INFO]---------------------------");
	
	//ImageQuality
	m_sv->device_data.ImageFormatSettings.JpegQuality_forJPEG = m_sv->device_data.ImageFormatSettings.JpegQuality.select = param.imgQuality;

	//JpegProgressive
	m_sv->device_data.ImageFormatSettings.JpegProgressive.select = param.encProgressive;

	return ESL_SUCCESS;
}

DWORD CScanMgr::conversionTiffCapSDKtoSV(const EslParamHdr* pParam)
{
	SDI_TRACE_LOG("conversionTiffCapSDKtoSV");

	DWORD err = ESL_SUCCESS;
	EslParamSaveTIFF param;

	// Get Current Full parameters
	memcpy_s(&param, sizeof(EslParamSaveTIFF), &m_ParamTiffCurrent, sizeof(EslParamSaveTIFF));

	// Overwrite parameter structure depends on the parameter type
	if (pParam->type == ESL_PARAM_SAVE_TIFF) {
		memcpy_s(&param, sizeof(EslParamSaveTIFF), pParam, sizeof(EslParamSaveTIFF));
	} else {
		SDI_TRACE_LOG("[ERROR]ESL_ERR_INVALID_PARAM");
		return ESL_ERR_INVALID_PARAM;
	}

	SDI_TRACE_LOG("[INFO]-----------TIFF----------------");
	SDI_TRACE_LOG("[INFO]type: %d", param.type);
	SDI_TRACE_LOG("[INFO]size: %d", param.size);
	SDI_TRACE_LOG("[INFO]compMono: %d", param.compMono);
	SDI_TRACE_LOG("[INFO]---------------------------");
	
	//TiffCompression
	if(param.compMono == TC_NONE){
		m_sv->device_data.ImageFormatSettings.TiffCompression.select = TiffCompressionNone;
	}else if(param.compMono == TC_CCITT_G4){
		m_sv->device_data.ImageFormatSettings.TiffCompression.select = TiffCompressionCCITFAX4;
	}

	return ESL_SUCCESS;
}

DWORD CScanMgr::conversionScanCapSDKtoSV(const EslParamHdr* pParam)
{
	SDI_TRACE_LOG("conversionScanCapSDKtoSV");

	DWORD err = ESL_SUCCESS;
	EslParamScanExV1 param;

	// Get Current Full parameters
	memcpy_s(&param, sizeof(EslParamScanExV1), &m_ParamScanCurrent, sizeof(EslParamScanExV1));

	// Overwrite parameter structure depends on the parameter type
	if (pParam->type == ESL_PARAM_SCAN_BASIC) {
		memcpy_s(&param, sizeof(EslParamScanExV1), pParam, sizeof(EslParamScanBasic));
	} else if (pParam->type == ESL_PARAM_SCAN_EX_V1) {
		memcpy_s(&param, sizeof(EslParamScanExV1), pParam, sizeof(EslParamScanExV1));
	} else {
		SDI_TRACE_LOG("[ERROR]ESL_ERR_INVALID_PARAM");
		return ESL_ERR_INVALID_PARAM;
	}

	SDI_TRACE_LOG("[INFO]---------------------------");
	SDI_TRACE_LOG("[INFO]type: %d", param.type);
	SDI_TRACE_LOG("[INFO]size: %d", param.size);
	SDI_TRACE_LOG("[INFO]docSource: %d", param.docSource);
	SDI_TRACE_LOG("[INFO]imgType: %d", param.imgType);
	SDI_TRACE_LOG("[INFO]resolution: %d", param.resolution);
	SDI_TRACE_LOG("[INFO]docSize: %d", param.docSize);
	SDI_TRACE_LOG("[INFO]sizeUnit: %d", param.sizeUnit);
	SDI_TRACE_LOG("[INFO]sizeUser.left: %d", param.sizeUser.left);
	SDI_TRACE_LOG("[INFO]sizeUser.top: %d", param.sizeUser.top);
	SDI_TRACE_LOG("[INFO]sizeUser.right: %d", param.sizeUser.right);
	SDI_TRACE_LOG("[INFO]sizeUser.bottom: %d", param.sizeUser.bottom);
	SDI_TRACE_LOG("[INFO]docRotate: %d", param.docRotate);
	SDI_TRACE_LOG("[INFO]brightness: %d", param.brightness);
	SDI_TRACE_LOG("[INFO]contrast: %d", param.contrast);
	SDI_TRACE_LOG("[INFO]threshold: %d", param.threshold);
	SDI_TRACE_LOG("[INFO]colorFilter: %d", param.colorFilter);
	SDI_TRACE_LOG("[INFO]numScan: %d", param.numScan);
	SDI_TRACE_LOG("[INFO]optDoubleFeedDetect: %d", param.optDoubleFeedDetect);
	SDI_TRACE_LOG("[INFO]optBlankPageSkip: %d", param.optBlankPageSkip);
	SDI_TRACE_LOG("[INFO]skewCorrect: %d", param.skewCorrect);
	SDI_TRACE_LOG("[INFO]docEnhance: %d", param.docEnhance);
	SDI_TRACE_LOG("[INFO]---------------------------");

	//FunctionalUnit
	if(param.docSource == SRC_AUTO){
		m_sv->device_data.FunctionalUnit.select = Auto;
	}else if(param.docSource == SRC_FLATBED){
		m_sv->device_data.FunctionalUnit.select = Flatbed;
	}else if(param.docSource == SRC_FEEDER_SIMPLEX){
		m_sv->device_data.FunctionalUnit.select = ADF;
		m_sv->device_data.DuplexType.select = Single_Sided;
	}else if(param.docSource == SRC_FEEDER_DUPLEX){
		m_sv->device_data.FunctionalUnit.select = ADF;
		m_sv->device_data.DuplexType.select = Double_Sided;
	}

	//ColorType
	if(param.imgType == IT_AUTO){
		m_sv->device_data.ColorType.select = ColorTypeAuto;
	}else if(param.imgType == IT_COLOR){
		m_sv->device_data.ColorType.select = ColorTypeRGB24;
	}else if(param.imgType == IT_GRAY){
		m_sv->device_data.ColorType.select = ColorTypeMono8;
	}else if(param.imgType == IT_MONO){
		m_sv->device_data.ColorType.select = ColorTypeMono1;
	}

	//Resolution
	m_sv->device_data.Resolution.select = param.resolution;

	//ScanArea
	this->StandardSizeToPixel(param.docSize, param.sizeUnit, param.sizeUser);
	
	//Orientation
	if(param.docRotate == DR_AUTO){
		m_sv->device_data.Orientation.select = OrientationAuto;
	}else if(param.docRotate == DR_NONE){
		m_sv->device_data.Orientation.select = OrientationNone;
	}else if(param.docRotate == DR_R90){
		m_sv->device_data.Orientation.select = Orientation90;
	}else if(param.docRotate == DR_R180){
		m_sv->device_data.Orientation.select = Orientation180;
	}else if(param.docRotate == DR_R270){
		m_sv->device_data.Orientation.select = Orientation270;
	}

	//Brightness
	m_sv->device_data.Brightness.select = param.brightness / 10;

	//Contrast
	m_sv->device_data.Contrast.select = param.contrast / 10;

	//Threshold
	m_sv->device_data.Threshold.select = param.threshold;

	//DropoutColor
	if(param.colorFilter == CF_NONE){
		m_sv->device_data.DropoutColor.select = DropoutColorNone;
	}else if(param.colorFilter == CF_DROPOUT_RED){
		m_sv->device_data.DropoutColor.select = DropoutColorRed;
	}else if(param.colorFilter == CF_DROPOUT_GREEN){
		m_sv->device_data.DropoutColor.select = DropoutColorGreen;
	}else if(param.colorFilter == CF_DROPOUT_BLUE){
		m_sv->device_data.DropoutColor.select = DropoutColorBlue;
	}

	//PagesTobeScanned
	m_sv->device_data.PagesTobeScanned.select = param.numScan;
	
	//DoubleFeedDetection
	if(param.optDoubleFeedDetect == DFD_NONE){
		m_sv->device_data.DoubleFeedDetectionValue.DoubleFeedDetection.select = false;
	}else if(param.optDoubleFeedDetect == DFD_LOW){
		m_sv->device_data.DoubleFeedDetectionValue.DoubleFeedDetection.select = true;
		m_sv->device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel.select = DoubleFeedDetectionLow;
	}else if(param.optDoubleFeedDetect == DFD_HIGH){
		m_sv->device_data.DoubleFeedDetectionValue.DoubleFeedDetection.select = true;
		m_sv->device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel.select = DoubleFeedDetectionHigh;
	}

	//BlankPageSkip
	if(param.optBlankPageSkip == BPS_NONE){
		m_sv->device_data.BlankPageSkipKey.select = false;
	}else if(param.optBlankPageSkip == BPS_VERY_LOW){
		m_sv->device_data.BlankPageSkipKey.select = true;
		m_sv->device_data.BlankPageSkipLevelKey.select = 3; //TBD
	}else if(param.optBlankPageSkip == BPS_LOW){
		m_sv->device_data.BlankPageSkipKey.select = true;
		m_sv->device_data.BlankPageSkipLevelKey.select = 19; //TBD
	}else if(param.optBlankPageSkip == BPS_MEDIUM){
		m_sv->device_data.BlankPageSkipKey.select = true;
		m_sv->device_data.BlankPageSkipLevelKey.select = 22; //TBD
	}else if(param.optBlankPageSkip == BPS_HIGH){
		m_sv->device_data.BlankPageSkipKey.select = true;
		m_sv->device_data.BlankPageSkipLevelKey.select = 25; //TBD
	}
	
	//PaperDeskew
	if(param.skewCorrect == SC_NONE){
		m_sv->device_data.PaperDeskew.select = false;
	}else if(param.skewCorrect == SC_EDGE){
		m_sv->device_data.PaperDeskew.select = true;
	}

	//textEnhance
	if(param.docEnhance == DE_NONE){
		m_sv->device_data.textEnhance.select = TextEnhanceNone;
	}else if(param.docEnhance == DE_TET_STD){
		m_sv->device_data.textEnhance.select = TextEnhanceNormal;
	}else if(param.docEnhance == DE_TET_HIGH){
		m_sv->device_data.textEnhance.select = TextEnhanceHigh;
	}

	return ESL_SUCCESS;
}

double CScanMgr::calculate_pixel_size(double value)
{
	int res = m_sv->device_data.Resolution.select;
    //mm -> pixel
    double mpp = 25.4 / (double)res;
    return value / mpp;
}

void CScanMgr::StandardSizeToPixel(eDocSize docSize, eSizeUnit sizeUnit, RECT sizeUser)
{
	SDI_TRACE_LOG("StandardSizeToPixel");

	m_sv->device_data.ScanArea.AutoSize.select = AutoSizeNone;
    if(docSize == DS_AUTO){
        m_sv->device_data.ScanArea.AutoSize.select = AutoSizeStandard;
        //取り込み装置自動時のサイズ自動が非サポート時用にA4サイズを入れておく
        m_sv->device_data.ScanArea.ScanAreaWidth.select = calculate_pixel_size(210);
        m_sv->device_data.ScanArea.ScanAreaHeight.select = calculate_pixel_size(297);
    }else if(docSize == DS_A3){
        m_sv->device_data.ScanArea.ScanAreaWidth.select = calculate_pixel_size(297);
        m_sv->device_data.ScanArea.ScanAreaHeight.select = calculate_pixel_size(420);
    }else if(docSize == DS_A4){
        m_sv->device_data.ScanArea.ScanAreaWidth.select = calculate_pixel_size(210);
        m_sv->device_data.ScanArea.ScanAreaHeight.select = calculate_pixel_size(297);
    }else if(docSize == DS_A5){
        m_sv->device_data.ScanArea.ScanAreaWidth.select = calculate_pixel_size(148);
        m_sv->device_data.ScanArea.ScanAreaHeight.select = calculate_pixel_size(210);
    }else if(docSize == DS_B4){
        m_sv->device_data.ScanArea.ScanAreaWidth.select = calculate_pixel_size(257);
        m_sv->device_data.ScanArea.ScanAreaHeight.select = calculate_pixel_size(364);
    }else if(docSize == DS_B5){
        m_sv->device_data.ScanArea.ScanAreaWidth.select = calculate_pixel_size(182);
        m_sv->device_data.ScanArea.ScanAreaHeight.select = calculate_pixel_size(257);
    }else if(docSize == DS_US_LETTER){
        m_sv->device_data.ScanArea.ScanAreaWidth.select = calculate_pixel_size(216);
        m_sv->device_data.ScanArea.ScanAreaHeight.select = calculate_pixel_size(279);
    }else if(docSize == DS_US_LEGAL){
        m_sv->device_data.ScanArea.ScanAreaWidth.select = calculate_pixel_size(216);
        m_sv->device_data.ScanArea.ScanAreaHeight.select = calculate_pixel_size(356);
    }else if(docSize == DS_US_LEDGER){ //Tabloid
        m_sv->device_data.ScanArea.ScanAreaWidth.select = calculate_pixel_size(279);
        m_sv->device_data.ScanArea.ScanAreaHeight.select = calculate_pixel_size(432);
    }else if(docSize == DS_USER){ //TBD
		double left = sizeUser.left;
		double top = sizeUser.top;
		double right = sizeUser.right;
		double bottom = sizeUser.bottom;
		if (sizeUnit != SU_PIXEL) {
			left /= 100;
			top /= 100;
			right /= 100;
			bottom /= 100;
		}
		if(sizeUnit == SU_INCH){
			m_sv->device_data.ScanArea.ScanAreaOffsetX.select = left * m_sv->device_data.Resolution.select;
			m_sv->device_data.ScanArea.ScanAreaOffsetY.select = top * m_sv->device_data.Resolution.select;
            m_sv->device_data.ScanArea.ScanAreaWidth.select = (right - left) * m_sv->device_data.Resolution.select;
            m_sv->device_data.ScanArea.ScanAreaHeight.select = (bottom - top) * m_sv->device_data.Resolution.select;
		}else if(sizeUnit == SU_CENTIMETER){
			m_sv->device_data.ScanArea.ScanAreaOffsetX.select = calculate_pixel_size(left);
			m_sv->device_data.ScanArea.ScanAreaOffsetY.select = calculate_pixel_size(top);
            m_sv->device_data.ScanArea.ScanAreaWidth.select = calculate_pixel_size(right - left);
            m_sv->device_data.ScanArea.ScanAreaHeight.select = calculate_pixel_size(bottom - top);
        }else if(sizeUnit == SU_PIXEL){
			m_sv->device_data.ScanArea.ScanAreaOffsetX.select = left;
			m_sv->device_data.ScanArea.ScanAreaOffsetY.select = top;
            m_sv->device_data.ScanArea.ScanAreaWidth.select = right - left;
            m_sv->device_data.ScanArea.ScanAreaHeight.select = bottom - top;
        }
		SDI_TRACE_LOG("[INFO]User : ScanAreaOffsetX = %d", m_sv->device_data.ScanArea.ScanAreaOffsetX.select);
		SDI_TRACE_LOG("[INFO]User : ScanAreaOffsetY = %d", m_sv->device_data.ScanArea.ScanAreaOffsetY.select);
		SDI_TRACE_LOG("[INFO]User : ScanAreaWidth = %d", m_sv->device_data.ScanArea.ScanAreaWidth.select);
		SDI_TRACE_LOG("[INFO]User : ScanAreaHeight = %d", m_sv->device_data.ScanArea.ScanAreaHeight.select);
    }
}

DWORD CScanMgr::getInfoCapabilities(EslParamHdr* pParam)
{
	SDI_TRACE_LOG("getInfoCapabilities");

	DWORD err = ESL_SUCCESS;

	// Check if parameter structure is for Information type
	if (pParam->type != ESL_PARAM_INFO_BASIC) {
		SDI_TRACE_LOG("[ERROR]ESL_ERR_INVALID_PARAM");
		return ESL_ERR_INVALID_PARAM;
	}

	EslParamInfoBasic* pInfo = (EslParamInfoBasic*)pParam;

	strncpy((char *)pInfo->serialNumber, m_sv->device_data.Serial_Number_value.c_str(), sizeof(pInfo->serialNumber));

	memset(pInfo->reserved1, 0, sizeof(pInfo->reserved1));
	memset(pInfo->reserved2, 0, sizeof(pInfo->reserved2));

	return ESL_SUCCESS;
}

DWORD CScanMgr::execScan(eExecType ExecType)
{
	SDI_TRACE_LOG("execScan");

	DWORD err = ESL_SUCCESS;

	m_ParamScanCurrent.type = ESL_PARAM_SCAN_BASIC;
	//setting default value
	if(m_sv->device_data.Folder == USER_DEFINE){
        m_sv->device_data.SelectPath = m_sv->device_data.UserDefinePath;
    }

	err = this->conversionScanCapSDKtoSV((EslParamHdr*)&m_ParamScanCurrent);
	this->conversionSaveCapSDKtoSV((EslParamHdr*)&m_ParamSaveCurrent);
	if (err != ESL_SUCCESS) {
		SDI_TRACE_LOG("[ERROR]conversionScanCapSDKtoSV failed");
		return err;
	}
	this->conversionJpegCapSDKtoSV((EslParamHdr*)&m_ParamJpegCurrent);
	if (err != ESL_SUCCESS) {
		SDI_TRACE_LOG("[ERROR]conversionJpegCapSDKtoSV failed");
		return err;
	}
	this->conversionTiffCapSDKtoSV((EslParamHdr*)&m_ParamTiffCurrent);
	if (err != ESL_SUCCESS) {
		SDI_TRACE_LOG("[ERROR]conversionTiffCapSDKtoSV failed");
		return err;
	}

	//Functional Unit : Auto
	if(m_sv->device_data.FunctionalUnit.select == Auto){
		if(m_sv->GetPaperLoadedStatus()){
			//adf scan
			SDI_TRACE_LOG("[INFO]FunctionalUnit Auto : ADF");
			m_sv->device_data.FunctionalUnit.select = kSDIFunctionalUnitDocumentFeeder;
			m_sv->device_data.DuplexType.select = Single_Sided;
    	}else {
			SDI_TRACE_LOG("[INFO]FunctionalUnit Auto : Flatbed");
			m_sv->device_data.FunctionalUnit.select = kSDIFunctionalUnitFlatbed;
		}
	}
	// Initialize ScanCounter
	switch (ExecType) {
	case ET_SCAN_AND_SAVE:
		this->execScanAndSave();
		//m_pStorageMgr->setSaveFlag(true);
		break;
	case ET_SCAN_AND_STORE:
		this->execScanAndStore();
		break;
	default:
		SDI_TRACE_LOG("[ERROR]ESL_ERR_INVALID_PARAM");
		return ESL_ERR_INVALID_PARAM;
	}

	return err;
}

DWORD CScanMgr::execSave(eExecType ExecType)
{
	SDI_TRACE_LOG("execSave");

	DWORD err = ESL_SUCCESS;

	ExecType = ET_SAVE_STORED_IMAGE;

	// Initialize ScanCounter
	switch (ExecType) {
	case ET_SAVE_STORED_IMAGE:
		this->execSaveStored();

		break;
	case ET_GET_STORED_IMAGE:
		break;
	default:
		SDI_TRACE_LOG("[ERROR]ESL_ERR_INVALID_PARAM");
		return ESL_ERR_INVALID_PARAM;
	}

	return err;
}

void CScanMgr::execScanAndSave(void)
{
	SDI_TRACE_LOG("execScanAndSave");
	bool gs_cancel_do = false;
	//scanning roop
    m_sv->scanning_status = SAVE;
    if(m_sv->Scan_Start(kSDIOperationTypeNew)){
        while(m_sv->scanning_status == SAVE){
            if(gs_cancel_do){
                m_sv->scanning_status = CANCEL;
            }
            if(m_sv->scanning_status == CANCEL){
                m_sv->Cancel_Scanning();
                m_sv->outEventType = kSDITransferEventTypeCancel;
                break;
            }else {
                if(m_sv->CheckNextTransferEvent(false)){
                    if(m_sv->Scanning()){
                        if(m_sv->outEventType == kSDITrasnferEventTypeImage){
                            m_sv->Get_Image();
                            m_sv->Save_Path();
                        }
                    }else {//scanning error
                        m_sv->Get_Image();
						m_sv->Dispose_Ptr();
                        break;
                    }
					m_sv->Dispose_Ptr();
                }else if(m_sv->outEventType == kSDITransferEventTypeComplete){
                    break;
                }
            }
        }
    }

	this->execSaveStored();
}

void CScanMgr::execScanAndStore(void)
{
	SDI_TRACE_LOG("execScanAndStore");

	bool gs_cancel_do = false;
	//scanning roop
    m_sv->scanning_status = SAVE;
    if(m_sv->Scan_Start(kSDIOperationTypeNew)){
        while(m_sv->scanning_status == SAVE){
            if(gs_cancel_do){
                m_sv->scanning_status = CANCEL;
            }
            if(m_sv->scanning_status == CANCEL){
                m_sv->Cancel_Scanning();
                m_sv->outEventType = kSDITransferEventTypeCancel;
                break;
            }else {
                if(m_sv->CheckNextTransferEvent(false)){
                    if(m_sv->Scanning()){
                        if(m_sv->outEventType == kSDITrasnferEventTypeImage){
                            m_sv->Get_Image();
                            m_sv->Save_Path();
                        }
                    }else {//scanning error
                        m_sv->Get_Image();
						m_sv->Dispose_Ptr();
                        break;
                    }
					m_sv->Dispose_Ptr();
                }else if(m_sv->outEventType == kSDITransferEventTypeComplete){
                    break;
                }
            }
        }
    }
}

void CScanMgr::execSaveStored(void)
{
	SDI_TRACE_LOG("execSaveStored");

    if(m_sv->m_LastError == kSDIErrorNone){
        if(m_sv->scanning_status == SAVE){
            if(m_sv->Save_Image2()){
                //保存先のディレクトリが存在しているか確認する
                const bool result = ES_CMN_FUNCS::PATH::ES_IsExistFile(m_sv->device_data.SelectPath);
                if (!result) {
					SDI_TRACE_LOG("[ERROR]Unable to save scanned files.");
                }
            }else {
                if(m_sv->m_LastError == kSDIErrorWriteFile){
                    //書き込み失敗エラー
					SDI_TRACE_LOG("[ERROR]Unable to save scanned files.");
                }else if(m_sv->scanning_status == CANCEL){
					SDI_TRACE_LOG("[INFO]Scanning cancel.");
				}else {
                    //すべて白紙ページエラー
					SDI_TRACE_LOG("[ERROR]All scanned pages were blank.");
                }
            }
        }
    }
}

void CScanMgr::showEslError(DWORD Code)
{
	SDI_TRACE_LOG("showEslError");
	switch (Code) {
	case ESL_SUCCESS:
		SDI_TRACE_LOG("[INFO]ESL_SUCCESS");
		break;
	case ESL_CANCEL:
		SDI_TRACE_LOG("[INFO]ESL_CANCEL");
		break;
	case ESL_ERR_GENERAL:
		SDI_TRACE_LOG("[ERROR]ESL_ERR_GENERAL");
		break;
	case ESL_ERR_NOT_INITIALIZED:
		SDI_TRACE_LOG("[ERROR]ESL_ERR_NOT_INITIALIZED");
		break;
	case ESL_ERR_INVALID_PARAM:
		SDI_TRACE_LOG("[ERROR]ESL_ERR_INVALID_PARAM");
		break;
	case ESL_ERR_LOW_MEMORY:
		SDI_TRACE_LOG("[ERROR]ESL_ERR_LOW_MEMORY");
		break;
	case ESL_ERR_LOW_DISKSPACE:
		SDI_TRACE_LOG("[ERROR]ESL_ERR_LOW_DISKSPACE");
		break;
	case ESL_ERR_WRITE_FAIL:
		SDI_TRACE_LOG("[ERROR]ESL_ERR_WRITE_FAIL");
		break;
	case ESL_ERR_READ_FAIL:
		SDI_TRACE_LOG("[ERROR]ESL_ERR_READ_FAIL");
		break;
	case ESL_ERR_SCAN_GENERAL:
		SDI_TRACE_LOG("[ERROR]ESL_ERR_SCAN_GENERAL");
		break;
	case ESL_ERR_INVALID_KEY:
		SDI_TRACE_LOG("[ERROR]ESL_ERR_INVALID_KEY");
		break;
	case ESL_ERR_NOT_SUPPORTED:
		SDI_TRACE_LOG("[ERROR]ESL_ERR_NOT_SUPPORTED");
		break;
	case ESL_ERR_NO_DRIVER:
		SDI_TRACE_LOG("[ERROR]ESL_ERR_NO_DRIVER");
		break;
	case ESL_ERR_OPEN_FAIL:
		SDI_TRACE_LOG("[ERROR]ESL_ERR_OPEN_FAIL");
		break;
	default:
		SDI_TRACE_LOG("[ERROR]Unknown: %d", Code);
	}
}

void CScanMgr::showEslDeviceStatus(DWORD Code)
{
	SDI_TRACE_LOG("showEslDeviceStatus");
	switch (Code) {
	case ESL_STAT_READY:
		SDI_TRACE_LOG("[INFO]ESL_STAT_READY");
		break;
	case ESL_STAT_ERR_COVER_OPEN:
		SDI_TRACE_LOG("[ERROR]ESL_STAT_ERR_COVER_OPEN");
		break;
	case ESL_STAT_ERR_PAPER_JAM:
		SDI_TRACE_LOG("[ERROR]ESL_STAT_ERR_PAPER_JAM");
		break;
	case ESL_STAT_ERR_PAPER_EMPTY:
		SDI_TRACE_LOG("[ERROR]ESL_STAT_ERR_PAPER_EMPTY");
		break;
	case ESL_STAT_ERR_FATAL:
		SDI_TRACE_LOG("[ERROR]ESL_STAT_ERR_FATAL");
		break;
	case ESL_STAT_ERR_LAMP_TIME_FATAL:
		SDI_TRACE_LOG("[ERROR]ESL_STAT_ERR_LAMP_TIME_FATAL");
		break;
	case ESL_STAT_ERR_CR_LOCK:
		SDI_TRACE_LOG("[ERROR]ESL_STAT_ERR_CR_LOCK");
		break;
	case ESL_STAT_ERR_DOUBLE_FEED:
		SDI_TRACE_LOG("[ERROR]ESL_STAT_ERR_DOUBLE_FEED");
		break;
	case ESL_STAT_ERR_DOCUMENT_TRAY_CLOSE:
		SDI_TRACE_LOG("[ERROR]ESL_STAT_ERR_DOCUMENT_TRAY_CLOSE");
		break;
	case ESL_STAT_ERR_LOW_BATTERY:
		SDI_TRACE_LOG("[ERROR]ESL_STAT_ERR_LOW_BATTERY");
		break;
	case ESL_STAT_ERR_CARRIER_SHEET_SENSOR:
		SDI_TRACE_LOG("[ERROR]ESL_STAT_ERR_CARRIER_SHEET_SENSOR");
		break;
	case ESL_STAT_ERR_EDGE_TRACKING_SENSOR:
		SDI_TRACE_LOG("[ERROR]ESL_STAT_ERR_EDGE_TRACKING_SENSOR");
		break;
	case ESL_STAT_ERR_PAPER_PROTECTION:
		SDI_TRACE_LOG("[ERROR]ESL_STAT_ERR_PAPER_PROTECTION");
		break;
	case ESL_STAT_ERR_SEPARATION_LEVER:
		SDI_TRACE_LOG("[ERROR]ESL_STAT_ERR_SEPARATION_LEVER");
		break;
	case ESL_STAT_ERR_POSE:
		SDI_TRACE_LOG("[ERROR]ESL_STAT_ERR_POSE");
		break;
	case ESL_STAT_ERR_GAP_SENSOR:
		SDI_TRACE_LOG("[ERROR]ESL_STAT_ERR_GAP_SENSOR");
		break;
	case ESL_STAT_ERR_UNKNOWN:
		SDI_TRACE_LOG("[ERROR]ESL_STAT_ERR_UNKNOWN");
		break;
	default:
		SDI_TRACE_LOG("[ERROR]Unknown: %d", Code);
	}
}

DWORD CScanMgr::getEslDeviceStatus(void)
{
	SDI_TRACE_LOG("getEslDeviceStatus");
	DWORD status = ESL_STAT_READY;

	m_sv->m_LastError = m_sv->CheckDeviceErrorStatus();

	switch (m_sv->m_LastError) {
	case kSDIErrorNone:
		status = ESL_STAT_READY;
		break;
	case kSDIErrorCoverOpen:
		status = ESL_STAT_ERR_COVER_OPEN;
		break;
	case kSDIErrorPaperJam:
		status = ESL_STAT_ERR_PAPER_JAM;
		break;
	case kSDIErrorPaperEmpty:
		status = ESL_STAT_ERR_PAPER_EMPTY;
		break;
	case kSDIErrorDeviceFatalError:
		status = ESL_STAT_ERR_FATAL;
		break;
	case kSDIErrorLampTime:
		status = ESL_STAT_ERR_LAMP_TIME_FATAL;
		break;
	case kSDIErrorCRLock:
		status = ESL_STAT_ERR_CR_LOCK;
		break;
	case kSDIErrorPaperDoubleFeed:
	case kSDIErrorPaperDoubleFeed2:
		status = ESL_STAT_ERR_DOUBLE_FEED;
		break;
	case kSDIErrorTrayClose:
		status = ESL_STAT_ERR_DOCUMENT_TRAY_CLOSE;
		break;
	case kSDIErrorLowBattery:
		status = ESL_STAT_ERR_LOW_BATTERY;
		break;
	case kSDIErrorCRSheetSensorError:
		status = ESL_STAT_ERR_CARRIER_SHEET_SENSOR;
		break;
	case kSDIErrorETSensorError:
		status = ESL_STAT_ERR_EDGE_TRACKING_SENSOR;
		break;
	case kSDIErrorPaperProtect:
		status = ESL_STAT_ERR_PAPER_PROTECTION;
		break;
	case kSDIErrorSepLeverChangedInterruptedly:
		status = ESL_STAT_ERR_SEPARATION_LEVER;
		break;
	case kSDIErrorDeviceFormUnstable:
	case kSDIErrorDeviceFormChangedInterruptedly:
		status = ESL_STAT_ERR_POSE;
		break;
	case kSDIErrorGapSensorError:
		status = ESL_STAT_ERR_GAP_SENSOR;
		break;
	case kSDIErrorUnknownError:
		status = ESL_STAT_ERR_UNKNOWN;
		break;
	default:
		SDI_TRACE_LOG("[ERROR]Unknown: %d", m_sv->m_LastError);
	}

	this->showEslDeviceStatus(status);

	return status;
}

DWORD CScanMgr::setParam(const EslParamHdr* pParam)
{
	SDI_TRACE_LOG("setParam");

	switch (pParam->type) {
	case ESL_PARAM_SAVE_BASIC:
		SDI_TRACE_LOG("[INFO]setParam : ESL_PARAM_SAVE_BASIC");
		memcpy_s(&m_ParamSaveCurrent, sizeof(EslParamSaveBasic), pParam, sizeof(EslParamSaveBasic));
		break;
	case ESL_PARAM_SAVE_JPEG:
		SDI_TRACE_LOG("[INFO]setParam : ESL_PARAM_SAVE_JPEG");
		memcpy_s(&m_ParamJpegCurrent, sizeof(EslParamSaveJPEG), pParam, sizeof(EslParamSaveJPEG));
		break;
	case ESL_PARAM_SAVE_TIFF:
		SDI_TRACE_LOG("[INFO]setParam : ESL_PARAM_SAVE_TIFF");
		memcpy_s(&m_ParamTiffCurrent, sizeof(EslParamSaveTIFF), pParam, sizeof(EslParamSaveTIFF));
		break;
	default:
		SDI_TRACE_LOG("[ERROR]ESL_ERR_INVALID_PARAM");
		return ESL_ERR_INVALID_PARAM;
	}

	return ESL_SUCCESS;
}

BOOL CScanMgr::sendAppCallbackScanStatus(EslCBDataScanStatus* scanstatus)
{
	SDI_TRACE_LOG("CScanMgr::sendAppCallbackScanStatus");

	// Invoke callback function to notify end of page
	if (this->m_pCbFunc) {
		return this->m_pCbFunc((EslCBDataHdr*)scanstatus, this->m_pPrivate);
	}

	return TRUE;
}

BOOL CScanMgr::sendAppCallbackImageFile(EslCBDImageFile* imagefile)
{
	SDI_TRACE_LOG("CScanMgr::sendAppCallbackImageFile");

	// Invoke callback function to notify file name of image
	if (this->m_pCbFunc) {
		return this->m_pCbFunc((EslCBDataHdr*)imagefile, this->m_pPrivate);
	}

	return TRUE;
}
