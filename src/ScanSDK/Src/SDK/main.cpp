// ESL_SAMPLE.cpp : Defines the entry point for the application.
//

#include <iostream>
#include "../ESCANLIB.h"

#define LICENSE_KEY	"EPSON"
#define	DEVICE_NAME	"PX-M780F Series"//"ES-500WII"

BOOL CALLBACK eslCallback(EslCBDataHdr* pData, PVOID pPrivate)
{
	// Check if the parameter is valid
	if (pData == NULL) {
		return FALSE;
	}

	switch (pData->type) {
		case CBDT_STATUS_SCAN:
		{
			EslCBDataScanStatus* pstatus = (EslCBDataScanStatus*)pData;
			switch (pstatus->status) {
			case STATUS_SCAN_UPDATE_PROGRESS:
				break;
			case STATUS_SCAN_PAGE_END:
				break;
			default:
				std::cout << "[ERROR]Invalid CallbackData Status\n" << std::endl;
			}

			// If application wants to cancel scanning, return FALSE
			BOOL cancelReqfromApp = FALSE;
			if (cancelReqfromApp) {
				return FALSE;
			}

			break;
		}
		case CBDT_IMAGE_FILE:
		{
			// Get saved image file information
			EslCBDImageFile* pstatus = (EslCBDImageFile*)pData;
			switch (pstatus->status) {
			case STATUS_IMAGE_FILE_INFO:
				break;
			default:
				std::cout << "[ERROR]Invalid CallbackData Status\n" << std::endl;
			}
			break;
		}
		/*case CBDT_IMAGE_MEMORY:
		{
			ES_TRACE(TEXT("[INFO]CBDT_IMAGE_MEMORY\n"));
			// Get image data from library
			EslCBDataImageMemory* pstatus = (EslCBDataImageMemory*)pData;
			switch (pstatus->status) {
			case STATUS_IMAGE_MEMORY_INFO:
				{
					ES_TRACE(TEXT("[INFO]STATUS_IMAGE_MEMORY_INFO\n"));
					ES_TRACE1(TEXT("[INFO]count:%d\n"), pstatus->count);
					ES_TRACE1(TEXT("[INFO]img_width:%d\n"), pstatus->img_width);
					ES_TRACE1(TEXT("[INFO]img_height:%d\n"), pstatus->img_height);
					ES_TRACE1(TEXT("[INFO]img_resolution:%d\n"), pstatus->img_resolution);
					ES_TRACE1(TEXT("[INFO]img_channel_per_pixel:%d\n"), pstatus->img_channel_per_pixel);
					ES_TRACE1(TEXT("[INFO]img_bits_per_channel:%d\n"), pstatus->img_bits_per_channel);
					ES_TRACE1(TEXT("[INFO]img_data_size:%d\n"), pstatus->img_data_size);
					ES_TRACE1(TEXT("[INFO]img_handle:%p\n"), pstatus->img_handle);
					CString filename;
					filename.Format(TEXT("C:\\SAMPLE%04d.BMP"), pstatus->count);
					debugSave(pstatus->img_handle, filename);
					break;
				}
			default:
				ES_TRACE1(TEXT("[ERROR]Invalid CallbackData Status(%x)\n"), pstatus->status);
			}
			break;
		}*/
		default:
			std::cout << "[ERROR]Invalid CallbackData Status\n" << std::endl;
	}

	return TRUE;
}

DWORD SCAN_SAVE_Default(void)
{
	std::cout << "[INFO]SCAN_SAVE_Default" << std::endl;
	DWORD instance = 0;
	DWORD err = 0;

	//
	// Initialize Library
	//
	BOOL ret = eslInit(&instance, LICENSE_KEY, &err);
	if (ret) {

		//
		// Open Scanner
		//
		ConnectInfo scanner = {CT_DEFAULT, DEVICE_NAME}; // specify connection type and device name
		ret = eslOpenScanner(instance, &scanner, &err);
		if (ret) {
			if (ret) {
				// Scan start(Scan and Save)
				ret = eslExecute(instance, ET_SCAN_AND_STORE, eslCallback, NULL, &err);
				ret = eslExecute(instance, ET_SCAN_AND_STORE, eslCallback, NULL, &err);
				ret = eslExecute(instance, ET_SCAN_AND_STORE, eslCallback, NULL, &err);
				ret = eslExecute(instance, ET_SAVE_STORED_IMAGE, eslCallback, NULL, &err);

			}

			// Close scanner
			ret = eslCloseScanner(instance, &err);
		}

		// Free Library
		eslFree(instance, &err);
	}

	return err;
}

int APIENTRY main()
{
	SCAN_SAVE_Default();
	std::cout << "[INFO]Last!!" << std::endl;
//	SCAN_STORE_SAVE_Default();
//	SCAN_SAVE_JPEG();
//	SCAN_SAVE_PDF();
//	SCAN_STORE_SAVE_PDF_SEPARATION();

	return 0;
}

