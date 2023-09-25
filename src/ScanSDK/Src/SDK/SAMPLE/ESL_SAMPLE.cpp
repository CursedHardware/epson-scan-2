// ESL_SAMPLE.cpp : Defines the entry point for the application.
//

#include "ESL_SAMPLE.h"
#include "EslStub.h"
#include <iostream> // for std::cout log

#define LICENSE_KEY	"EPSON"
//#define	DEVICE_NAME	"PX-M780F Series"
#define	DEVICE_NAME	"DS-535II"
/*///////////////////////////////////////////////////////////////////////
Individual identification is possible by entering the information 
obtained as a result of entering "epsonscan2 -l" in the device name. 
$ epsonscan2 -l
 === List of available devices ==
device ID :DS-730N:001:003
ModelID:ES021C
->#define	DEVICE_NAME	"DS-730N:001:003"

The following method connects to the first device found. 
#define	DEVICE_NAME	"DS-730N"
*////////////////////////////////////////////////////////////////////////
#define IP_ADDRESS "192.168.xx.x"

BOOL CALLBACK eslCallback(EslCBDataHdr* pData, PVOID pPrivate)
{
	std::cout << "[INFO]eslCallback" << std::endl;

	// Check if the parameter is valid
	if (pData == NULL) {
		std::cout << "[INFO]pData is NULL" << std::endl;
		return FALSE;
	}

	BOOL cancelReqfromApp = FALSE;
	switch (pData->type) {
		case CBDT_STATUS_SCAN:
		{
			EslCBDataScanStatus* pstatus = (EslCBDataScanStatus*)pData;
			switch (pstatus->status) {
			case STATUS_SCAN_UPDATE_PROGRESS:
				break;
			case STATUS_SCAN_PAGE_END:
				std::cout << "[INFO]PageCount: " << pstatus->count << std::endl;
				std::cout << "[INFO]Proguress: " << pstatus->progress << std::endl;
				break;
			default:
				std::cout << "[ERROR]Invalid CallbackData Status\n" << std::endl;
			}

			// If application wants to cancel scanning, return FALSE
			//cancelReqfromApp = TRUE;
			if (cancelReqfromApp) {
				return FALSE;
			}

			break;
		}
		case CBDT_IMAGE_FILE:
		{
			// Get saved image file information
			EslCBDImageFile* pstatus = (EslCBDImageFile*)pData;
			std::cout << "[INFO]FileName: " << pstatus->filename << std::endl;
			switch (pstatus->status) {
			case STATUS_IMAGE_FILE_INFO:
				break;
			default:
				std::cout << "[ERROR]Invalid CallbackData Status\n" << std::endl;
			}
			break;
		}
		default:
			std::cout << "[ERROR]Invalid CallbackData Status\n" << std::endl;
	}

	return TRUE;
}

DWORD SCAN_SAVE_Default(const char* device_name)
{
	std::cout << "[INFO]SCAN_SAVE_Default" << std::endl;
	DWORD instance = 0;
	DWORD err = 0;

	CEslStub eslib(NULL);

	//
	// Initialize Library
	//
	BOOL ret = eslib.eslInit(&instance, LICENSE_KEY, &err);
	if (ret) {

		//
		// Open Scanner
		//
		ConnectInfo scanner = {CT_DEFAULT}; // specify connection type and device name
		memcpy(scanner.deviceName, device_name, strlen(device_name)+1);
		//ConnectInfo scanner = {CT_NET_IPV4, "", IP_ADDRESS};
		ret = eslib.eslOpenScanner(instance, &scanner, &err);
		if (ret) {
			if (ret) {
				// Scan start(Scan and Save)
				ret = eslib.eslExecute(instance, ET_SCAN_AND_SAVE, eslCallback, NULL, &err);
			}

			// Close scanner
			ret = eslib.eslCloseScanner(instance, &err);
		}

		// Free Library
		eslib.eslFree(instance, &err);
	}

	return err;
}

DWORD SCAN_STORE_SAVE_Default(const char* device_name)
{
	DWORD instance = 0;
	DWORD err = 0;

	CEslStub eslib(NULL);

	//
	// Initialize Library
	//
	BOOL ret = eslib.eslInit(&instance, LICENSE_KEY, &err);
	if (ret) {

		//
		// Open Scanner
		//
		ConnectInfo scanner = {CT_DEFAULT}; // specify connection type and device name
		memcpy(scanner.deviceName, device_name, strlen(device_name)+1);	
		ret = eslib.eslOpenScanner(instance, &scanner, &err);
		if (ret) {

			EslParamInfoBasic info_params;
			info_params.type = ESL_PARAM_INFO_BASIC;
			info_params.size = sizeof(info_params);

			// Get default parameters
			ret = eslib.eslCtrlParameter(instance, POT_GET_DEFAULT, (pEslParamHdr)&info_params, &err);
			std::cout << "[INFO]Serial No: " << info_params.serialNumber << std::endl;

			if (ret) {
				// Scan start(Scan and Store)
				ret = eslib.eslExecute(instance, ET_SCAN_AND_STORE, eslCallback, NULL, &err);
			}

			if (ret) {
				// Scan start(Save stored images)
				ret = eslib.eslExecute(instance, ET_SAVE_STORED_IMAGE, eslCallback, NULL, &err);
			}

			//status = Device status
			DWORD status = 0;
			ret = eslib.eslGetDeviceStatus(instance, &status, &err);
			DISPLAY_DEVICE_STATUS(status);

			// Close scanner
			ret = eslib.eslCloseScanner(instance, &err);
		}

		// Free Library
		eslib.eslFree(instance, &err);
	}

	return err;
}

DWORD SCAN_SAVE_JPEG(const char* device_name)
{
	DWORD instance = 0;
	DWORD err = 0;

	CEslStub eslib(NULL);

	//
	// Initialize Library
	//
	BOOL ret = eslib.eslInit(&instance, LICENSE_KEY, &err);
	if (ret) {

		//
		// Open Scanner
		//
		ConnectInfo scanner = {CT_DEFAULT}; // specify connection type and device name
		memcpy(scanner.deviceName, device_name, strlen(device_name)+1);
		ret = eslib.eslOpenScanner(instance, &scanner, &err);
		if (ret) {
			//
			// Update Scan Basic parameters
			//
			EslParamScanExV1 scan_params;
			scan_params.type = ESL_PARAM_SCAN_EX_V1;
			scan_params.size = sizeof(scan_params);

			// Get default parameters
			ret = eslib.eslCtrlParameter(instance, POT_GET_DEFAULT, (pEslParamHdr)&scan_params, &err);

			// Change some parameters...
			if (ret) {
				scan_params.docSource = SRC_FLATBED;
				scan_params.imgType = IT_COLOR;
				scan_params.resolution = 300;
				scan_params.docSize = DS_A4;
				//scan_params.sizeUnit = SU_CENTIMETER;
				//scan_params.sizeUser.left = 0;
				//scan_params.sizeUser.top = 0;
				//scan_params.sizeUser.right = 21000;
				//scan_params.sizeUser.bottom = 29700;
				scan_params.docRotate = DR_NONE;
				scan_params.brightness = 0;
				scan_params.contrast = 0;
				scan_params.threshold = 128;
				scan_params.colorFilter = CF_NONE;
				scan_params.numScan = 0;

				scan_params.optDoubleFeedDetect = DFD_NONE;
				scan_params.optBlankPageSkip = BPS_NONE;
				scan_params.skewCorrect = SC_EDGE;

				// Set parameters
				ret = eslib.eslCtrlParameter(instance, POT_SET, (pEslParamHdr)&scan_params, &err);
			}

			//
			// Update Save Basic parameters
			//
			EslParamSaveBasic save_params;
			save_params.type = ESL_PARAM_SAVE_BASIC;
			save_params.size = sizeof(save_params);

			// Get default parameters
			if (ret) {
				ret = eslib.eslCtrlParameter(instance, POT_GET_DEFAULT, (pEslParamHdr)&save_params, &err);
			}

			// Change some parameters...
			if (ret) {
				save_params.fileFormat = FF_JPEG;
				strncpy((char *)save_params.filePath, ".", sizeof(save_params.filePath));
				strncpy((char *)save_params.fileNamePrefix, "img", sizeof(save_params.fileNamePrefix));
				save_params.fileNumStartCount = 1;
				save_params.numPageStart = 0;
				save_params.numPageEnd = 0;

				// Set parameters
				ret = eslib.eslCtrlParameter(instance, POT_SET, (pEslParamHdr)&save_params, &err);
			}

			//
			// Update JPEG file parameters
			//
			EslParamSaveJPEG jpeg_params;
			jpeg_params.type = ESL_PARAM_SAVE_JPEG;
			jpeg_params.size = sizeof(jpeg_params);

			// Get default parameters
			if (ret) {
				ret = eslib.eslCtrlParameter(instance, POT_GET_DEFAULT, (pEslParamHdr)&jpeg_params, &err);
			}

			// Change some parameters...
			if (ret) {
				jpeg_params.imgQuality = 90;
				jpeg_params.encProgressive = FALSE;

				// Set parameters
				ret = eslib.eslCtrlParameter(instance, POT_SET, (pEslParamHdr)&jpeg_params, &err);
			}

			//
			// Update TIFF file parameters
			//
			EslParamSaveTIFF tiff_params;
			tiff_params.type = ESL_PARAM_SAVE_TIFF;
			tiff_params.size = sizeof(tiff_params);

			// Get default parameters
			if (ret) {
				ret = eslib.eslCtrlParameter(instance, POT_GET_DEFAULT, (pEslParamHdr)&tiff_params, &err);
			}

			// Change some parameters...
			if (ret) {
				tiff_params.compMono = TC_NONE;

				// Set parameters
				ret = eslib.eslCtrlParameter(instance, POT_SET, (pEslParamHdr)&tiff_params, &err);
			}

			if (ret) {
				// Scan start(Scan and Store)
				ret = eslib.eslExecute(instance, ET_SCAN_AND_STORE, eslCallback, NULL, &err);
				//ret = eslib.eslExecute(instance, ET_SCAN_AND_STORE, eslCallback, NULL, &err);
				//ret = eslib.eslExecute(instance, ET_SCAN_AND_STORE, eslCallback, NULL, &err);
				//ret = eslib.eslExecute(instance, ET_SCAN_AND_STORE, eslCallback, NULL, &err);
				//ret = eslib.eslExecute(instance, ET_SCAN_AND_STORE, eslCallback, NULL, &err);
			}

			if (ret) {
				// Scan start(Save stored images)
				ret = eslib.eslExecute(instance, ET_SAVE_STORED_IMAGE, eslCallback, NULL, &err);
			}

			// Close scanner
			ret = eslib.eslCloseScanner(instance, &err);
		}

		// Free Library
		eslib.eslFree(instance, &err);
	}

	return err;
}

DWORD SCAN_SAVE_PDF(const char* device_name)
{
	DWORD instance = 0;
	DWORD err = 0;

	CEslStub eslib(NULL);

	//
	// Initialize Library
	//
	BOOL ret = eslib.eslInit(&instance, LICENSE_KEY, &err);
	if (ret) {

		//
		// Open Scanner
		//
		ConnectInfo scanner = {CT_DEFAULT}; // specify connection type and device name
		memcpy(scanner.deviceName, device_name, strlen(device_name)+1);
		ret = eslib.eslOpenScanner(instance, &scanner, &err);
		if (ret) {
			//
			// Update Scan Basic parameters
			//
			EslParamScanExV1 scan_params;
			scan_params.type = ESL_PARAM_SCAN_BASIC;
			scan_params.size = sizeof(scan_params);

			// Get default parameters
			ret = eslib.eslCtrlParameter(instance, POT_GET_DEFAULT, (pEslParamHdr)&scan_params, &err);

			// Change some parameters...
			if (ret) {
				scan_params.docSource = SRC_AUTO;
				scan_params.imgType = IT_COLOR;
				scan_params.resolution = 300;
				scan_params.docSize = DS_A4;
				//scan_params.sizeUnit = SU_INCH;
				//scan_params.sizeUser.left = 0;
				//scan_params.sizeUser.top = 0;
				//scan_params.sizeUser.right = 500;
				//scan_params.sizeUser.bottom = 500;
				scan_params.docRotate = DR_AUTO;
				scan_params.brightness = 0;
				scan_params.contrast = 0;
				scan_params.threshold = 110;
				scan_params.colorFilter = CF_NONE;
				scan_params.numScan = 5;

				// Set parameters
				ret = eslib.eslCtrlParameter(instance, POT_SET, (pEslParamHdr)&scan_params, &err);
			}

			//
			// Update Save Basic parameters
			//
			EslParamSaveBasic save_params;
			save_params.type = ESL_PARAM_SAVE_BASIC;
			save_params.size = sizeof(save_params);

			// Get default parameters
			if (ret) {
				ret = eslib.eslCtrlParameter(instance, POT_GET_DEFAULT, (pEslParamHdr)&save_params, &err);
			}

			// Change some parameters...
			if (ret) {
				save_params.fileFormat = FF_PDF;
				strncpy((char *)save_params.filePath, "./", sizeof(save_params.filePath));
				strncpy((char *)save_params.fileNamePrefix, "img", sizeof(save_params.fileNamePrefix));
				save_params.fileNumStartCount = 1;
				//Saved : page2 - page4
				save_params.numPageStart = 2;
				save_params.numPageEnd = 4;

				// Set parameters
				ret = eslib.eslCtrlParameter(instance, POT_SET, (pEslParamHdr)&save_params, &err);
			}

			if (ret) {
				// Scan start(Scan and Save)
				ret = eslib.eslExecute(instance, ET_SCAN_AND_STORE, eslCallback, NULL, &err);
				//ret = eslib.eslExecute(instance, ET_SCAN_AND_STORE, eslCallback, NULL, &err);
				//ret = eslib.eslExecute(instance, ET_SCAN_AND_STORE, eslCallback, NULL, &err);
				//ret = eslib.eslExecute(instance, ET_SCAN_AND_STORE, eslCallback, NULL, &err);
				//ret = eslib.eslExecute(instance, ET_SCAN_AND_STORE, eslCallback, NULL, &err);
				ret = eslib.eslExecute(instance, ET_SAVE_STORED_IMAGE, eslCallback, NULL, &err);
			}

			// Close scanner
			ret = eslib.eslCloseScanner(instance, &err);
		}

		// Free Library
		eslib.eslFree(instance, &err);
	}

	return err;
}

int APIENTRY main(int argc, char *argv[])
{
	char * device_name;
	if(argc > 1){
		device_name = (char*)malloc(strlen(argv[1]) + 1);
		memcpy(device_name, argv[1], strlen(argv[1])+1);
	}else {
		device_name = (char*)malloc(strlen(DEVICE_NAME) + 1);
		memcpy(device_name, DEVICE_NAME, strlen(DEVICE_NAME)+1);
	}

//////////////////Scan Action///////////////////////////////////////////
	SCAN_SAVE_Default(device_name);

//	SCAN_STORE_SAVE_Default(device_name);
//	SCAN_SAVE_JPEG(device_name);
//	SCAN_SAVE_PDF(device_name);
//////////////////Scan Action///////////////////////////////////////////

	free(device_name);

	return 0;
}

void DISPLAY_DEVICE_STATUS(DWORD Code)
{
	switch (Code) {
	case ESL_STAT_READY:
		std::cout << "[INFO]ESL_STAT_READY" << std::endl;
		break;
	case ESL_STAT_ERR_COVER_OPEN:
		std::cout << "[ERROR]ESL_STAT_ERR_COVER_OPEN" << std::endl;
		break;
	case ESL_STAT_ERR_PAPER_JAM:
		std::cout << "[ERROR]ESL_STAT_ERR_PAPER_JAM" << std::endl;
		break;
	case ESL_STAT_ERR_PAPER_EMPTY:
		std::cout << "[ERROR]ESL_STAT_ERR_PAPER_EMPTY" << std::endl;
		break;
	case ESL_STAT_ERR_FATAL:
		std::cout << "[ERROR]ESL_STAT_ERR_FATAL" << std::endl;
		break;
	case ESL_STAT_ERR_LAMP_TIME_FATAL:
		std::cout << "[ERROR]ESL_STAT_ERR_LAMP_TIME_FATAL" << std::endl;
		break;
	case ESL_STAT_ERR_CR_LOCK:
		std::cout << "[ERROR]ESL_STAT_ERR_CR_LOCK" << std::endl;
		break;
	case ESL_STAT_ERR_DOUBLE_FEED:
		std::cout << "[ERROR]ESL_STAT_ERR_DOUBLE_FEED" << std::endl;
		break;
	case ESL_STAT_ERR_DOCUMENT_TRAY_CLOSE:
		std::cout << "[ERROR]ESL_STAT_ERR_DOCUMENT_TRAY_CLOSE" << std::endl;
		break;
	case ESL_STAT_ERR_LOW_BATTERY:
		std::cout << "[ERROR]ESL_STAT_ERR_LOW_BATTERY" << std::endl;
		break;
	case ESL_STAT_ERR_CARRIER_SHEET_SENSOR:
		std::cout << "[ERROR]ESL_STAT_ERR_CARRIER_SHEET_SENSOR" << std::endl;
		break;
	case ESL_STAT_ERR_EDGE_TRACKING_SENSOR:
		std::cout << "[ERROR]ESL_STAT_ERR_EDGE_TRACKING_SENSOR" << std::endl;
		break;
	case ESL_STAT_ERR_PAPER_PROTECTION:
		std::cout << "[ERROR]ESL_STAT_ERR_PAPER_PROTECTION" << std::endl;
		break;
	case ESL_STAT_ERR_SEPARATION_LEVER:
		std::cout << "[ERROR]ESL_STAT_ERR_SEPARATION_LEVER" << std::endl;
		break;
	case ESL_STAT_ERR_POSE:
		std::cout << "[ERROR]ESL_STAT_ERR_POSE" << std::endl;
		break;
	case ESL_STAT_ERR_GAP_SENSOR:
		std::cout << "[ERROR]ESL_STAT_ERR_GAP_SENSOR" << std::endl;
		break;
	case ESL_STAT_ERR_UNKNOWN:
		std::cout << "[ERROR]ESL_STAT_ERR_UNKNOWN" << std::endl;
		break;
	default:
		std::cout << "[ERROR]Unknown: " << Code << std::endl;
	}
}
