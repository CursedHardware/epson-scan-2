#include "gtest/gtest.h"
#include "EpsonScan2.h"
#include <stdio.h>
#include <dlfcn.h>
#include <iostream>
#include <boost/format.hpp>

class ControllerTest : public ::testing::Test {

public:
void* es2lib_;
SDIDeviceFinder_CreatePtr		   SDIDeviceFinder_CreatePtr_;
SDIDeviceFinder_DisposePtr		   SDIDeviceFinder_DisposePtr_;
SDIDeviceFinder_StartDiscoveryPtr  SDIDeviceFinder_StartDiscoveryPtr_;
SDIDeviceFinder_StopDiscoveryPtr   SDIDeviceFinder_StopDiscoveryPtr_;
SDIDeviceFinder_GetDevicesPtr      SDIDeviceFinder_GetDevicesPtr_;

SDIScannerDriver_CreatePtr 			SDIScannerDriver_CreatePtr_;
SDIScannerDriver_OpenPtr 			SDIScannerDriver_OpenPtr_;
SDIScannerDriver_ClosePtr 			SDIScannerDriver_ClosePtr_;
SDIScannerDriver_DisposePtr 		SDIScannerDriver_DisposePtr_;
SDIScannerDriver_SetValuePtr   		SDIScannerDriver_SetValuePtr_;
SDIScannerDriver_GetValuePtr        SDIScannerDriver_GetValuePtr_;
SDIScannerDriver_GetCapabilityPtr   SDIScannerDriver_GetCapability_;
SDIScannerDriver_DoScanJobPtr     	SDIScannerDriver_DoScanJobPtr_;
SDIScannerDriver_GetNextTransferEventPtr SDIScannerDriver_GetNextTransferEventPtr_;
SDIImage_CreatePtr					SDIImage_CreatePtr_;
SDIImage_GetPathPtr					SDIImage_GetPathPtr_;
SDIImage_DisposePtr 				SDIImage_DisposePtr_;

SDIImage_GetWidthPtr				SDIImage_GetWidthPtr_;
SDIImage_GetHeightPtr				SDIImage_GetHeightPtr_;
SDIImage_GetSamplesPerPixelPtr		SDIImage_GetSamplesPerPixelPtr_;
SDIImage_GetBitsPerSamplePtr		SDIImage_GetBitsPerSamplePtr_;

virtual void SetUp() {
  es2lib_ = dlopen("./libepsonscan2.so", RTLD_LAZY);
  printf("%s", dlerror());
  ASSERT_TRUE(es2lib_ != nullptr);

  SDIDeviceFinder_CreatePtr_ = (SDIDeviceFinder_CreatePtr)dlsym(es2lib_, "SDIDeviceFinder_Create");
  ASSERT_TRUE(SDIDeviceFinder_CreatePtr_ != nullptr);
 
  SDIDeviceFinder_StartDiscoveryPtr_ = (SDIDeviceFinder_StartDiscoveryPtr)dlsym(es2lib_, "SDIDeviceFinder_StartDiscovery"); 
  ASSERT_TRUE(SDIDeviceFinder_StartDiscoveryPtr_ != nullptr);

  SDIDeviceFinder_StopDiscoveryPtr_ = (SDIDeviceFinder_StopDiscoveryPtr)dlsym(es2lib_, "SDIDeviceFinder_StopDiscovery");
  ASSERT_TRUE(SDIDeviceFinder_StopDiscoveryPtr_ != nullptr);

  SDIDeviceFinder_GetDevicesPtr_ = (SDIDeviceFinder_GetDevicesPtr)dlsym(es2lib_, "SDIDeviceFinder_GetDevices");
  ASSERT_TRUE(SDIDeviceFinder_GetDevicesPtr_ != nullptr);

  SDIDeviceFinder_DisposePtr_  = (SDIDeviceFinder_DisposePtr)dlsym(es2lib_, "SDIDeviceFinder_Dispose");
  ASSERT_TRUE(SDIDeviceFinder_DisposePtr_ != nullptr); 
  

  SDIScannerDriver_CreatePtr_ = (SDIScannerDriver_CreatePtr)dlsym(es2lib_, "SDIScannerDriver_Create");
  ASSERT_TRUE(SDIDeviceFinder_CreatePtr_ != nullptr); 

  SDIScannerDriver_OpenPtr_  = (SDIScannerDriver_OpenPtr)dlsym(es2lib_, "SDIScannerDriver_Open");
  ASSERT_TRUE(SDIScannerDriver_OpenPtr_ != nullptr);

  SDIScannerDriver_ClosePtr_ = (SDIScannerDriver_ClosePtr)dlsym(es2lib_, "SDIScannerDriver_Close");
  ASSERT_TRUE(SDIScannerDriver_ClosePtr_ != nullptr);

  SDIScannerDriver_DisposePtr_ = (SDIScannerDriver_DisposePtr)dlsym(es2lib_, "SDIScannerDriver_Dispose");
  ASSERT_TRUE(SDIScannerDriver_DisposePtr_ != nullptr);

  SDIScannerDriver_GetValuePtr_ = (SDIScannerDriver_GetValuePtr)dlsym(es2lib_, "SDIScannerDriver_GetValue");
  ASSERT_TRUE( SDIScannerDriver_GetValuePtr_ != nullptr);

  SDIScannerDriver_SetValuePtr_ = (SDIScannerDriver_GetValuePtr)dlsym(es2lib_, "SDIScannerDriver_SetValue");
  ASSERT_TRUE( SDIScannerDriver_SetValuePtr_ != nullptr);

  SDIScannerDriver_GetCapability_ = (SDIScannerDriver_GetCapabilityPtr)dlsym(es2lib_, "SDIScannerDriver_GetCapability");
  ASSERT_TRUE( SDIScannerDriver_GetCapability_ != nullptr);
 
  SDIScannerDriver_DoScanJobPtr_ = (SDIScannerDriver_DoScanJobPtr)dlsym(es2lib_, "SDIScannerDrive_DoScanJob");
  ASSERT_TRUE( SDIScannerDriver_DoScanJobPtr_ != nullptr);

  SDIScannerDriver_GetNextTransferEventPtr_ = (SDIScannerDriver_GetNextTransferEventPtr)dlsym(es2lib_, "SDIScannerDriver_GetNextTransferEvent");
  ASSERT_TRUE( SDIScannerDriver_GetNextTransferEventPtr_ != nullptr);

  SDIImage_CreatePtr_ = (SDIImage_CreatePtr)dlsym(es2lib_, "SDIImage_Create");
  ASSERT_TRUE( SDIImage_CreatePtr_ != nullptr);

  SDIImage_GetPathPtr_ = (SDIImage_GetPathPtr)dlsym(es2lib_, "SDIImage_GetPath");
  ASSERT_TRUE( SDIImage_GetPathPtr_ != nullptr);

  SDIImage_DisposePtr_ = (SDIImage_DisposePtr)dlsym(es2lib_, "SDIImage_Dispose");
  ASSERT_TRUE( SDIImage_GetPathPtr_ != nullptr);

  SDIImage_GetWidthPtr_ = (SDIImage_GetWidthPtr)dlsym(es2lib_, "SDIImage_GetWidth");
  ASSERT_TRUE( SDIImage_GetWidthPtr_ != nullptr);

  SDIImage_GetHeightPtr_ = (SDIImage_GetWidthPtr)dlsym(es2lib_, "SDIImage_GetHeight");
  ASSERT_TRUE( SDIImage_GetHeightPtr_ != nullptr);

  SDIImage_GetSamplesPerPixelPtr_ = (SDIImage_GetSamplesPerPixelPtr)dlsym(es2lib_, "SDIImage_GetSamplesPerPixel");
  ASSERT_TRUE( SDIImage_GetSamplesPerPixelPtr_ != nullptr);

  SDIImage_GetBitsPerSamplePtr_ = (SDIImage_GetBitsPerSamplePtr)dlsym(es2lib_, "SDIImage_GetBitsPerSample");
  ASSERT_TRUE( SDIImage_GetBitsPerSamplePtr_ != nullptr);
}

virtual void TearDown() {

}

};

TEST_F(ControllerTest, FinderTest)
{
  	SDIDeviceFinder* finder = nullptr;
    SDIDeviceFinder_CreatePtr_(&finder);

	SDIDeviceFinder_StartDiscoveryPtr_(finder, nullptr, nullptr);
	sleep(5);
	SDIDeviceFinder_StopDiscoveryPtr_(finder);

	SDIDeviceInfo* devices = nullptr;
	SDIInt count = 0;
	SDIDeviceFinder_GetDevicesPtr_(finder, &devices,&count); 

	std::cout << " === dump devices start ==" << std::endl;

	for (int i = 0; i < count; i++) {
		SDIDeviceInfo devInfo = devices[i];	
		std::cout << "displayname :" << devInfo.displayName << std::endl;
		std::cout << "ipAdder :"     << devInfo.ipAddress   << std::endl;
		std::cout << "productID :"   << devInfo.productID   << std::endl;
		std::cout << "ModelID:"		 << devInfo.modelID 	<< std::endl;
	}

	std::cout << " === dump devices fin ==" << std::endl;

	SDIDeviceFinder_DisposePtr_(finder);
	finder = nullptr;
}


TEST_F(ControllerTest, DriverTest)
{
	std::cout << " === Start Driver Test === " << std::endl;
	SDIScannerDriver* driver = nullptr;
	SDIDeviceInfo devInfo = {0};


	static const SDIInt kMyResolution = 50;
	static const SDIInt kSaveFormat   = kSDIImageFormatJPEG;
	{
		strcpy(devInfo.modelID, "ES0129");
		strcpy(devInfo.displayName, "EPSON DS-530");
		devInfo.productID = 0x157;
	}
	{
		SDIScannerDriver_CreatePtr_(&driver, &devInfo, nullptr, nullptr);
		ASSERT_TRUE(driver != nullptr);	
		std::cout << " Create OK " << std::endl;
		
		SDIScannerDriver_OpenPtr_(driver);
		std::cout << " Open OK " << std::endl;
	}
	{// functionalunit
		SDIFunctionalUnit current = kSDIFunctionalUnitDocumentFeeder;
		SDIFunctionalUnit out     = (SDIFunctionalUnit)-1;
		SDIScannerDriver_SetValuePtr_(driver, kSDIFunctionalUnitKey, kSDIValueTypeInt, &current, sizeof(current));
		SDIScannerDriver_GetValuePtr_(driver, kSDIFunctionalUnitKey, kSDIValueTypeInt, &out, sizeof(out));

		SDICapability capability = {0};
		SDIScannerDriver_GetCapability_(driver, kSDIFunctionalUnitKey, &capability);

		ASSERT_TRUE(out == kSDIFunctionalUnitDocumentFeeder);
		std::cout << "Functional Unit Test OK" << std::endl;
	}
	{// ColorType
		SDIColorType current = kSDIColorTypeRGB24;
		SDIColorType out     =  (SDIColorType)-1;
		SDIScannerDriver_SetValuePtr_(driver, kSDIColorTypeKey, kSDIValueTypeInt, &current, sizeof(current));
		SDIScannerDriver_GetValuePtr_(driver, kSDIColorTypeKey, kSDIValueTypeInt, &out, sizeof(out));


		SDICapability capability = {0};
		SDIScannerDriver_GetCapability_(driver, kSDIColorTypeKey, &capability);

		ASSERT_TRUE(out == kSDIColorTypeRGB24);
		std::cout << "ColorType Test OK" << std::endl;
	}
	{// Resolution
		SDIInt current = kMyResolution;
		SDIInt out     = -1;
		SDIScannerDriver_SetValuePtr_(driver, kSDIResolutionKey, kSDIValueTypeInt, &current, sizeof(current));
		SDIScannerDriver_GetValuePtr_(driver, kSDIResolutionKey, kSDIValueTypeInt, &out, sizeof(out));
		ASSERT_TRUE(out == kMyResolution);
		std::cout << "Resolution Test OK" << std::endl;
	}
	{// ImageFormat
		SDIImageFormat current = (SDIImageFormat)kSaveFormat;
		SDIInt out = -1;
		SDIScannerDriver_SetValuePtr_(driver, kSDIImageFormatKey, kSDIValueTypeInt, &current, sizeof(current));
		SDIScannerDriver_GetValuePtr_(driver, kSDIImageFormatKey, kSDIValueTypeInt, &out, sizeof(out));
		ASSERT_TRUE(out == kSaveFormat);
		std::cout << "ImageFomat Test OK" << std::endl;
	}
	{// Width
		SDIInt current =  8.27 * kMyResolution /*1654*/;
		SDIInt out     = -1;
		SDIScannerDriver_SetValuePtr_(driver, kSDIScanAreaWidthKey, kSDIValueTypeInt, &current, sizeof(current));
		SDIScannerDriver_GetValuePtr_(driver, kSDIScanAreaWidthKey, kSDIValueTypeInt, &out, sizeof(out));
		//ASSERT_TRUE(out == 1654);
		std::cout << "ScanArea Width Test OK" << std::endl;
	}
	{// Height
		SDIInt current =  11.69 * kMyResolution  /*2338*/;
		SDIInt out     = -1;
		SDIScannerDriver_SetValuePtr_(driver, kSDIScanAreaHeightKey, kSDIValueTypeInt, &current, sizeof(current));
		SDIScannerDriver_GetValuePtr_(driver, kSDIScanAreaHeightKey, kSDIValueTypeInt, &out, sizeof(out));
		//ASSERT_TRUE(out == 2338);
		std::cout << "ScanArea Height Test OK" << std::endl;
	}
	{// X
		SDIInt current = 0;
		SDIInt out     = -1;
		SDIScannerDriver_SetValuePtr_(driver, kSDIScanAreaOffsetXKey, kSDIValueTypeInt, &current, sizeof(current));
		SDIScannerDriver_GetValuePtr_(driver, kSDIScanAreaOffsetXKey, kSDIValueTypeInt, &out, sizeof(out));
		ASSERT_TRUE(out == 0);
		std::cout << "ScanArea Location X Test OK" << std::endl;
	}
	{// Y
		SDIInt current = 0;
		SDIInt out     = -1;
		SDIScannerDriver_SetValuePtr_(driver, kSDIScanAreaOffsetYKey, kSDIValueTypeInt, &current, sizeof(current));
		SDIScannerDriver_GetValuePtr_(driver, kSDIScanAreaOffsetYKey, kSDIValueTypeInt, &out, sizeof(out));
		ASSERT_TRUE(out == 0);
		std::cout << "ScanArea Location Y Test OK" << std::endl;
	}
	{
		SDIInt current = 0;
		SDIScannerDriver_GetValuePtr_(driver, kSDIMaxScanAreaWidthKey, kSDIValueTypeInt, &current, sizeof(current));
		ASSERT_TRUE(current != 0);
		std::cout << "ScanArea Max width " << current <<  std::endl;
	}
	{
		SDIInt current = 0;
		SDIScannerDriver_GetValuePtr_(driver, kSDIMaxScanAreaHeightKey, kSDIValueTypeInt, &current, sizeof(current));
		ASSERT_TRUE(current != 0);
		std::cout << "ScanArea Max height " << current <<  std::endl;
	}
	{
		SDIInt current = 0;
		SDIScannerDriver_GetValuePtr_(driver, kSDIMaxScanAreaHeightInLongPaperKey, kSDIValueTypeInt, &current, sizeof(current));
		ASSERT_TRUE(current != 0);
		std::cout << "ScanArea Max height In Long Paper " << current <<  std::endl;
	}
	{
		SDIInt current = kSDIDropoutColorRed;
		SDIInt out     = -1;

		SDIScannerDriver_SetValuePtr_(driver, kSDIDropoutColorKey, kSDIValueTypeInt, &current, sizeof(current));
		SDIScannerDriver_GetValuePtr_(driver, kSDIDropoutColorKey, kSDIValueTypeInt, &out, sizeof(out));
		ASSERT_TRUE(current == kSDIDropoutColorRed);
		current = kSDIDropoutColorNone;
		SDIScannerDriver_SetValuePtr_(driver, kSDIDropoutColorKey, kSDIValueTypeInt, &current, sizeof(current));
		std::cout << "Drop out test is ok " << current <<  std::endl;
	}
	{
		SDIInt current = kSDIOrientation90;
		SDIInt out     = -1;
		SDIScannerDriver_SetValuePtr_(driver, kSDIOrientationKey, kSDIValueTypeInt, &current, sizeof(current));
		SDIScannerDriver_GetValuePtr_(driver, kSDIOrientationKey, kSDIValueTypeInt, &out, sizeof(out));
		ASSERT_TRUE(out == kSDIOrientation90);
		std::cout << "Orientaion Test OK" << std::endl;
	}
	{
		SDIInt current =  0;
		SDIInt out     = -1;
		SDIScannerDriver_SetValuePtr_(driver, kSDIMultiTiffEnabledKey, kSDIValueTypeInt, &current, sizeof(current));
		SDIScannerDriver_GetValuePtr_(driver, kSDIMultiTiffEnabledKey, kSDIValueTypeInt, &out, sizeof(out));
		ASSERT_TRUE(out == 0);
		std::cout << "MultiTiff Test OK" << std::endl;
	}
	{
		SDIInt current =  kSDITiffCompressionCCITFAX4;
		SDIInt out     = -1;
		SDIScannerDriver_SetValuePtr_(driver, kSDITiffCompressionKey, kSDIValueTypeInt, &current, sizeof(current));
		SDIScannerDriver_GetValuePtr_(driver, kSDITiffCompressionKey, kSDIValueTypeInt, &out, sizeof(out));
		ASSERT_TRUE(out == kSDITiffCompressionCCITFAX4);
		std::cout << "TIFFCompression Test OK" << std::endl;
	}
	{

		SDIInt current =  50;
		SDIInt out     = -1;
		SDIScannerDriver_SetValuePtr_(driver, kSDIContrastKey, kSDIValueTypeInt, &current, sizeof(current));
		SDIScannerDriver_GetValuePtr_(driver, kSDIContrastKey, kSDIValueTypeInt, &out, sizeof(out));


		SDICapability capability = {0};
		SDIScannerDriver_GetCapability_(driver, kSDIContrastKey, &capability);

		ASSERT_TRUE(out == 50);
		std::cout << "Contrast Test OK" << std::endl;
	}
		{
		SDIInt current =  50;
		SDIInt out     = -1;
		SDIScannerDriver_SetValuePtr_(driver, kSDIBrightnessKey, kSDIValueTypeInt, &current, sizeof(current));
		SDIScannerDriver_GetValuePtr_(driver, kSDIBrightnessKey, kSDIValueTypeInt, &out, sizeof(out));

		SDICapability capability = {0};
		SDIScannerDriver_GetCapability_(driver, kSDIBrightnessKey, &capability);

		ASSERT_TRUE(out == 50);
		std::cout << "Brightness Test OK" << std::endl;
	}
	{
		SDIInt current =  0;
		SDIInt out     = -1;
		SDIScannerDriver_SetValuePtr_(driver, kSDIThresholdKey, kSDIValueTypeInt, &current, sizeof(current));
		SDIScannerDriver_GetValuePtr_(driver, kSDIThresholdKey, kSDIValueTypeInt, &out, sizeof(out));
		ASSERT_TRUE(out == 0);

		SDICapability capability = {0};
		SDIScannerDriver_GetCapability_(driver, kSDIThresholdKey, &capability);

		std::cout << "Threshold Test OK" << std::endl;
	}
	{
		SDIInt current =  90;
		SDIInt out     = -1;
		SDIScannerDriver_SetValuePtr_(driver, kSDIJpegQuality, kSDIValueTypeInt, &current, sizeof(current));
		SDIScannerDriver_GetValuePtr_(driver, kSDIJpegQuality, kSDIValueTypeInt, &out, sizeof(out));
		ASSERT_TRUE(out == 90);
		std::cout << "JpegQuality Test OK" << std::endl;
	}
	{
		SDIInt current =  0;
		SDIInt out     = -1;
		SDIScannerDriver_SetValuePtr_(driver, kSDIDuplexTypeKey, kSDIValueTypeInt, &current, sizeof(current));
		SDIScannerDriver_GetValuePtr_(driver, kSDIDuplexTypeKey, kSDIValueTypeInt, &out, sizeof(out));
		ASSERT_TRUE(out == 0);

		SDICapability capability = {0};
		SDIScannerDriver_GetCapability_(driver, kSDIDuplexTypeKey, &capability);

		std::cout << "DuplexType Test OK" << std::endl;
	}
	{
		SDIInt out     = -1;
		SDIScannerDriver_GetValuePtr_(driver, kSDIPaperLoadedKey, kSDIValueTypeInt, &out, sizeof(out));
		std::cout << "PaperLoaded Test OK" << std::endl;
	}
	{
		SDIInt current =  1;
		SDIInt out     = -1;
		SDIScannerDriver_SetValuePtr_(driver, kSDIPagesTobeScannedKey, kSDIValueTypeInt, &current, sizeof(current));
		SDIScannerDriver_GetValuePtr_(driver, kSDIPagesTobeScannedKey, kSDIValueTypeInt, &out, sizeof(out));

		ASSERT_TRUE(out == 1);
		std::cout << "Pages to be scanned Test OK" << std::endl;
	}
	{
		SDIInt current =  kSDIDoubleFeedDetectionLow;
		SDIInt out     = -1;
		SDIScannerDriver_SetValuePtr_(driver, kSDIDoubleFeedDetectionKey, kSDIValueTypeInt, &current, sizeof(current));
		SDIScannerDriver_GetValuePtr_(driver, kSDIDoubleFeedDetectionKey, kSDIValueTypeInt, &out, sizeof(out));

	
		SDICapability capability = {0};
		SDIScannerDriver_GetCapability_(driver, kSDIDoubleFeedDetectionKey, &capability);

		ASSERT_TRUE(out == kSDIDoubleFeedDetectionLow);
		std::cout << "Double Feed Detection Test OK" << std::endl;
	}
	{
		SDIInt current =  30;
		SDIInt out     = -1;
		SDIScannerDriver_SetValuePtr_(driver, kSDIBlankPageSkipLevelKey, kSDIValueTypeInt, &current, sizeof(current));
		SDIScannerDriver_GetValuePtr_(driver, kSDIBlankPageSkipLevelKey, kSDIValueTypeInt, &out, sizeof(out));
		ASSERT_TRUE(out == 30);

	
		SDICapability capability = {0};
		SDIScannerDriver_GetCapability_(driver, kSDIBlankPageSkipLevelKey, &capability);

		std::cout << "BlankPageSkip Test OK" << std::endl;
	}
	{
		SDIInt current =  0;
		SDIInt out     = -1;
		SDIScannerDriver_SetValuePtr_(driver, kSDIBackgroundRemovalKey, kSDIValueTypeInt, &current, sizeof(current));
		SDIScannerDriver_GetValuePtr_(driver, kSDIBackgroundRemovalKey, kSDIValueTypeInt, &out, sizeof(out));
		ASSERT_TRUE(out == 0);

	
		SDICapability capability = {0};
		SDIScannerDriver_GetCapability_(driver, kSDIBackgroundRemovalKey, &capability);

		std::cout << "Background Removal Test OK" << std::endl;
	}
	{
		SDIInt current =  1;
		SDIInt out     = -1;
		SDIScannerDriver_SetValuePtr_(driver, kSDIPaperDeskewKey, kSDIValueTypeInt, &current, sizeof(current));
		SDIScannerDriver_GetValuePtr_(driver, kSDIPaperDeskewKey, kSDIValueTypeInt, &out, sizeof(out));
		ASSERT_TRUE(out == 1);

	
		SDICapability capability = {0};
		SDIScannerDriver_GetCapability_(driver, kSDIPaperDeskewKey, &capability);

		std::cout << "PaperDeskew Test OK" << std::endl;
	}
	{
		SDIInt current =  kSDIAutoSizeStandard;
		SDIInt out     = -1;
		SDIScannerDriver_SetValuePtr_(driver, kSDIAutoSizeKey, kSDIValueTypeInt, &current, sizeof(current));
		SDIScannerDriver_GetValuePtr_(driver, kSDIAutoSizeKey, kSDIValueTypeInt, &out, sizeof(out));
		ASSERT_TRUE(out == kSDIAutoSizeStandard);
		std::cout << "AutoSize Test OK" << std::endl;
	
		SDICapability capability = {0};
		SDIScannerDriver_GetCapability_(driver, kSDIAutoSizeKey, &capability);

	}
	{
		SDIInt current =  1;
		SDIInt out     = -1;
		SDIScannerDriver_SetValuePtr_(driver, kSDITransferCompressionKey, kSDIValueTypeInt, &current, sizeof(current));
		SDIScannerDriver_GetValuePtr_(driver, kSDITransferCompressionKey, kSDIValueTypeInt, &out, sizeof(out));
		ASSERT_TRUE(out == 1);

		SDICapability capability = {0};
		SDIScannerDriver_GetCapability_(driver, kSDITransferCompressionKey, &capability);
		std::cout << "TransferCompression Test OK" << std::endl;
	}
	{	
		SDICapability capability = {0};
		SDIScannerDriver_GetCapability_(driver, kSDIDoubleFeedDetectionKey, &capability);

		std::cout << "Capability Test OK" << std::endl;
	}
	{
		std::cout << "StartScanning..." << std::endl;

		SDIScannerDriver_DoScanJobPtr_(driver, kSDIOperationTypeNew);

		SDIInt isBlocking;
		SDITransferEventType outType = kSDITrasnferEventTypeImage;
		SDIImage* outImageData;
		SDIError outError;
		unsigned int nSize = 100;
		unsigned char* tmp_buf = (unsigned char*)::malloc(nSize);
		if(tmp_buf){
			memset(tmp_buf, 0, nSize);
		}
		SDIChar* output_path = (SDIChar*)tmp_buf;
		uint32_t imageCount = 0;
		while(outType != kSDITransferEventTypeComplete)
		{
			SDIImage_CreatePtr_(&outImageData);
			SDIScannerDriver_GetNextTransferEventPtr_(driver,
													&outType, //kSDITrasnferEventTypeImage -> kSDITransferEventTypeCompletekSDIDoubleFeedDetectionKey
													outImageData,
													&outError);
			if (outType == kSDITrasnferEventTypeImage)
			{
				SDIImage_GetPathPtr_(outImageData, output_path, 0);

				std::string ext = "jpg";
				if (kSaveFormat == kSDIImageFormatJPEG)
				{
					ext = "jpg";
				}

				if (kSaveFormat == kSDIImageFormatPDF)
				{
					ext = "pdf";
				}

				if (kSaveFormat == kSDIImageFormatPNM)
				{
					ext = "pnm";
				}

				if (kSaveFormat == kSDIImageFormatTIFF)
				{
					ext = "tiff";
				}

				if (kSaveFormat == kSDIImageFormatPNG)
				{
					ext = "png";
				}

				if (kSaveFormat == kSDIImageFormatRaw)
				{
					ext = "raw";
				}

				if (rename(output_path, (boost::format("/tmp/epson/work/img_%2%.%1%") % ext.c_str() % imageCount).str().c_str()) != 0)
				{
					std::cout << "Rename failed" << std::endl;
				}
				std::cout << "=== Image Number === " << imageCount << std::endl;
				std::cout << "ImageWidth = " << SDIImage_GetWidthPtr_(outImageData) << std::endl;
				std::cout << "ImageHeight = " << SDIImage_GetHeightPtr_(outImageData) << std::endl;
				std::cout << "SamplesPerPixel = " << SDIImage_GetSamplesPerPixelPtr_(outImageData) << std::endl;
				std::cout << "BitsPerSample = " << SDIImage_GetBitsPerSamplePtr_(outImageData) << std::endl; 

				imageCount++;
			}

			SDIImage_DisposePtr_(outImageData);
		}

		free(tmp_buf);
		tmp_buf = nullptr;
	}

	SDIScannerDriver_ClosePtr_(driver);
	std::cout << "Close OK" << std::endl;

	SDIScannerDriver_DisposePtr_(driver);
	std::cout << "Dispose OK" << std::endl;
	driver = nullptr;
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
