#include "EpsonScan2.h"
#include "Finder.hpp"
#include "Error.hpp"
#include "Controller.hpp"
#include "Image.hpp"
#include "FFManager.hpp"
#include "DeviceResolver.hpp"

using namespace epsonscan;

static const int32_t kMaxDevices = 50;

struct SDIScannerDriver
{
	Controller* controller;
};

struct SDIDeviceFinder 
{
	Finder* instanceOfFinder;	
};

struct SDIImage
{
	Image* instanceOfImage;
};

SDIError SDIDeviceFinder_Create(SDIDeviceFinter** outFinder)
{
  try {
	 auto finder  = new SDIDeviceFinder();
	 finder->instanceOfFinder = new Finder(); 
	 if (outFinder) {
		 *outFinder = finder;
	 }else{
		 delete finder;
	 }
  }catch(std::bad_alloc& e) {
	return kSDIErrorNoMemory;
  }catch(RuntimeError& e) {
 	return kSDIErrorUnknownError;
  }	  
  return kSDIErrorNone;
}

SDIError SDIDeviceFinder_StartDiscovery(SDIDeviceFinder* finder,DeviceFinderCallBackProc callBack,  void* param)
{
	try {
	   finder->instanceOfFinder->Start();
	}catch(std::bad_alloc& e) {
		return kSDIErrorNoMemory;
  	}catch (RuntimeError& e) {
		return kSDIErrorUnknownError;
	}
	return kSDIErrorNone;
}

SDIError SDIDeviceFinder_StopDiscovery(SDIDeviceFinder* finder)
{
	try {
	   finder->instanceOfFinder->Stop();
	}catch(std::bad_alloc& e) {
		return kSDIErrorNoMemory;
  	}catch (RuntimeError& e) {
		return kSDIErrorUnknownError;
	}
	return kSDIErrorNone;
}

SDIError SDIDeviceFinder_SetTimeout(SDIDeviceFinder* finder, SDIInt seconds)
{
	return kSDIErrorNone;
}

SDIError SDIDeviceFinder_GetDevices(SDIDeviceFinder* finder, SDIDeviceInfo** outDevices, SDIInt* outCount)
{
	try {
			Finder* instanceOfFinder  = finder->instanceOfFinder;

			if (outDevices) {
				*outDevices = instanceOfFinder->GetDevicesPointer();	
			}
			if (outCount) {
			   *outCount = instanceOfFinder->GetDevicesCount();			
			}
	}catch(std::bad_alloc& e) {
		return kSDIErrorNoMemory;
	 }catch(RuntimeError& error) {
		return kSDIErrorUnknownError;
	}
	return kSDIErrorNone;
}

SDIError SDIDeviceFinder_Dispose(SDIDeviceFinder* finder)
{
	delete finder->instanceOfFinder;
	finder->instanceOfFinder = nullptr;

	delete finder;
	finder = nullptr;

	return kSDIErrorNone;
}

//////////////////////////////////////////////////////
SDIError SDIScannerDriver_Create(SDIScannerDriver** outDriver, 
   						 	 	 const SDIDeviceInfo* deviceInfo,
							   	 InterruptEventCallBackProc callBack,
							  	 void *userdata)
{
 try {
	 auto driver  = new SDIScannerDriver();
	 if(driver == nullptr){
		 return kSDIErrorUnknownError;
	 }

	 driver->controller = new Controller(*deviceInfo, driver);
	 if(driver->controller == nullptr){
		 return kSDIErrorUnknownError;
	 }

	 driver->controller->SetInterruptEventCallBackProc(callBack,userdata);

	 if (outDriver) {
		 *outDriver = driver;
	 }
  }catch (std::bad_alloc& e) {
	return kSDIErrorNoMemory;
  }catch(RuntimeError& e) {
 	return kSDIErrorUnknownError;
  }	

  return kSDIErrorNone;
}

SDIError SDIScannerDriver_Open(SDIScannerDriver* driver)
{
		try {
	  		Controller* controller = driver->controller;
			if (controller) {
				return controller->Open();	
			}
		 }catch (std::bad_alloc& e) {
			return kSDIErrorNoMemory;
		}catch(RuntimeError& e) {
			return kSDIErrorUnknownError;
		}				

		return kSDIErrorNone;
}

SDIError SDIScannerDriver_Reset(SDIScannerDriver* driver)
{
		try {
	  		Controller* controller = driver->controller;
			if (controller) {
				controller->Reset();	
			}
		 }catch (std::bad_alloc& e) {
			return kSDIErrorNoMemory;
		}catch(RuntimeError& e) {
			return kSDIErrorUnknownError;
		}				

		return kSDIErrorNone;
}
SDIError SDIScannerDriver_GetAllKeys(SDIScannerDriver* driver, SDIChar **outKeys, SDIInt* outCount)
{
	   try {
	  		Controller* controller = driver->controller;
			if (controller) {

			}
	    }catch (std::bad_alloc& e) {
			return kSDIErrorNoMemory;
		}catch(RuntimeError& e) {
			return kSDIErrorUnknownError;
		}				

		return kSDIErrorNone;
}

SDIError SDIScannerDriver_SetValue(SDIScannerDriver* driver, SDIChar* key, SDIValueType valType, void* value, int size)
{
	   try {
	  		Controller* controller = driver->controller;
			if (controller) {
				controller->SetValue(std::string(key), valType, value, size);
			}
	    }catch (std::bad_alloc& e) {
			return kSDIErrorNoMemory;
		}catch(RuntimeError& e) {
			return kSDIErrorUnknownError;
		}				

		return kSDIErrorNone;
}

SDIError SDIScannerDriver_GetValue(SDIScannerDriver* driver, SDIChar* key, SDIValueType valType, void* value, int size)
{
  	  try {
	  		Controller* controller = driver->controller;
			if (controller) {
				controller->GetValue(std::string(key), valType, value, size);
			}
	    }catch (std::bad_alloc& e) {
			return kSDIErrorNoMemory;
		}catch(RuntimeError& e) {
			return kSDIErrorUnknownError;
		}				

		return kSDIErrorNone;
}

SDIError SDIScannerDriver_GetDefault(SDIScannerDriver* driver, SDIChar* key, SDIValueType valType, void* value)
{
		return kSDIErrorNone;
}

SDIError SDIScannerDriver_GetCapability(SDIScannerDriver* driver, SDIChar* key, SDICapability* outCapability)
{
  	  try {
	  		Controller* controller = driver->controller;
			if (controller) {
				controller->GetCapability(std::string(key), *outCapability);
			}
	    }catch (std::bad_alloc& e) {
			return kSDIErrorNoMemory;
		}catch(RuntimeError& e) {
			return kSDIErrorUnknownError;
		}				

		return kSDIErrorNone;

}

SDIError SDIScannerDrive_DoScanJob(SDIScannerDriver* driver, 
								   SDIOperationType operationType)
{
	   try {
	  		Controller* controller = driver->controller;
			if (controller) {
				switch(operationType)
				{
					case kSDIOperationTypeNew:
						return controller->Scan();
						break;
					case kSDIOperationTypeCancel:
						controller->Cancel();
						break;

					case kSDIOperationTypeStartAFM:
						controller->StartAFM(false);
						break;

					case kSDIOperationTypeStartAFMC:
						controller->StartAFM(true);
						break;

					case kSDIOperationTypeStopAFM:
						controller->StopAFM(false);
						break;					

					case kSDIOperationTypeStopAFMC:
						controller->StopAFM(true);
						break;		
				}
			}
	    }catch (std::bad_alloc& e) {
			return kSDIErrorNoMemory;
		}catch(RuntimeError& e) {
			return e.GetCode();
		}				

		return kSDIErrorNone;
}

bool  SDIScannerDriver_CheckNextTransferEvent(SDIScannerDriver* driver, bool waitUntilReceived)
{
	try{
	  	Controller* controller = driver->controller;
		if (controller) {
			return controller->CheckNextEvent(waitUntilReceived);
		}
    }catch (std::bad_alloc& e) {
		return kSDIErrorNoMemory;
	}catch(RuntimeError& e) {
		return kSDIErrorUnknownError;
	}				

	return kSDIErrorNone;
}

void  SDIScannerDriver_CheckCautionStatus(SDIScannerDriver* driver)
{
	try{
		Controller* controller = driver->controller;
		if (controller) {
			controller->CheckGlassDirtStatus();
		}
	}catch(...)
	{
	}
}


SDIError SDIScannerDriver_GetNextTransferEvent(SDIScannerDriver* driver,
											   SDITransferEventType* outType,
											   SDIImage* outImageData,
											   SDIError* outError
											  ) {
	   try {
	  		Controller* controller = driver->controller;
			if (controller) {
				boost::optional<TransferEvent> transferEvent;
				transferEvent = controller->DequeueEvent();
				if (transferEvent)
				{
					switch(transferEvent->GetType())
					{
						case kEventTypeImage:
						{
							if (outImageData)
							{
								outImageData->instanceOfImage = transferEvent->GetImage();
							}

							if (outType)
							{
								*outType = kSDITrasnferEventTypeImage;
							}

							if (outError)
							{
						   		*outError = transferEvent->GetError();
							}
						}
						break;


						case kEventTypeComplete:
						{
							if (outType)
							{
						   		*outType = kSDITransferEventTypeComplete;
							}

							if (outError)
							{
						   		*outError = transferEvent->GetError();
							}
						}
						break;


						case kEventTypeCancel:
						{
							if (outType)
							{
						   		*outType = kSDITransferEventTypeCancel;
							}
						}
						break;

						case kEventTypeAFMStartConituousScan:
						{
							if (outType)
							{
						   		*outType = kSDITransferEventTypeStartContinuousScanInAFM;
							}
						}
						break;


						case kEventTypeAFMStopContinuousScan:
						{
							if (outType)
							{
						   		*outType = kSDITransferEventTypeStopContinuousScanInAFM;
							}
						}
						break;
						
					}

					if (outError)
					{
						*outError = transferEvent->GetError();
					}
					
				}
			}
	    }catch (std::bad_alloc& e) {
			return kSDIErrorNoMemory;
		}catch(RuntimeError& e) {
			return kSDIErrorUnknownError;
		}				

		return kSDIErrorNone;
}


SDIError SDIScannerDriver_Close(SDIScannerDriver* driver)
{
	   try {
	  		Controller* controller = driver->controller;
			if (controller) {
				controller->Close();
			}
	    }catch (std::bad_alloc& e) {
			return kSDIErrorNoMemory;
		}catch(RuntimeError& e) {
			return kSDIErrorUnknownError;
		}				

		return kSDIErrorNone;
}

SDIError SDIScannerDriver_Dispose(SDIScannerDriver* driver)
{	
		delete driver->controller;
		driver->controller = nullptr;

		delete driver;
		driver = nullptr;
		
		return kSDIErrorNone;
}

SDIError  SDIScannerDriver_UnlockAdministratorLock(SDIScannerDriver* driver)
{
	   try {
	  		Controller* controller = driver->controller;
			if (controller) {
				return controller->UnlockAdministratorLock();
			}
	    }catch (std::bad_alloc& e) {
			return kSDIErrorNoMemory;
		}catch(RuntimeError& e) {
			return kSDIErrorUnknownError;
		}				

		return kSDIErrorNone;
}

SDIError  SDIScannerDriver_LockAdministratorLock(SDIScannerDriver* driver)
{
	   try {
	  		Controller* controller = driver->controller;
			if (controller) {
				return controller->LockAdministratorLock();
			}
	    }catch (std::bad_alloc& e) {
			return kSDIErrorNoMemory;
		}catch(RuntimeError& e) {
			return kSDIErrorUnknownError;
		}				

		return kSDIErrorNone;
}

////////////////////////////  ImageData API  ////////////////////////////

SDIError SDIImage_Create(SDIImage** image)
{
		if (image)
		{
			*image = new SDIImage();
			if (image)
			{
				(*image)->instanceOfImage = nullptr;
			}
		}

		return kSDIErrorNone;
}

SDIInt SDIImage_GetWidth(SDIImage* image)
{
		if (image && image->instanceOfImage)
		{
			return image->instanceOfImage->GetWidth();
		}
		return kSDIErrorNone;
}

SDIInt SDIImage_GetHeight(SDIImage* image)
{

		if (image && image->instanceOfImage)
		{
			return image->instanceOfImage->GetHeight();
		}

		return kSDIErrorNone;
}

SDIInt SDIImage_GetSamplesPerPixel(SDIImage* image)
{
		if (image && image->instanceOfImage)
		{
			return image->instanceOfImage->GetSamplesPerPixel();
		}

		return kSDIErrorNone;
}

SDIInt SDIImage_GetBitsPerSample(SDIImage* image)
{
		if (image && image->instanceOfImage)
		{
			return image->instanceOfImage->GetBitsPerSample();
		}
		return kSDIErrorNone;
}

SDIInt SDIImage_GetPath(SDIImage* image, int8_t * buf, SDIInt bufSize)
{
		if (image && image->instanceOfImage)
		{
			const std::string path = image->instanceOfImage->GetPath();
			if (buf) {
				memcpy(buf, path.c_str(), path.length());
			}
		}
		return kSDIErrorNone;
}

SDIInt SDIImage_CheckImageIsBlank(SDIImage* image)
{
		if (image && image->instanceOfImage)
		{
			return image->instanceOfImage->IsBlankPage();
		}
		return false;
}

SDIInt SDIImage_Dispose(SDIImage* image)
{
		if (image && image->instanceOfImage)
		{
			image->instanceOfImage->Release();
			image->instanceOfImage = nullptr;
		}

		if (image){
			delete image;
			image = nullptr;
		}

		return kSDIErrorNone;
}


bool SDIDeviceInfo_Resolve(SDIDeviceInfo* resolveDevice)
{
	DeviceResolver resolver;
	return resolver.Resolve(*resolveDevice);
}	


bool SDIMultipage_Start(SDIScannerDriver* driver, const char* destPath,  SDIImageFormat format)
{
	   try {
	  		Controller* controller = driver->controller;
			if (controller) {
				return controller->SDIMultipage_Start(destPath, format);	
			}
		 }catch (std::bad_alloc& e) {
			return kSDIErrorNoMemory;
		}catch(RuntimeError& e) {
			return kSDIErrorUnknownError;
		}				

		return kSDIErrorNone;
}

bool SDIMultipage_Add(SDIScannerDriver* driver, const char* sourcePath)
{
	try {
	  		Controller* controller = driver->controller;
			if (controller) {
				return controller->SDIMultipage_Add(sourcePath);	
			}
	}catch (std::bad_alloc& e) {
			return kSDIErrorNoMemory;
	}catch(RuntimeError& e) {
			return kSDIErrorUnknownError;
	}				

	return kSDIErrorNone;
}

bool SDIMultipage_Fin(SDIScannerDriver* driver)
{
	try {
	  		Controller* controller = driver->controller;
			if (controller) {
				return controller->SDIMultipage_Fin();	
			}
	}catch (std::bad_alloc& e) {
			return kSDIErrorNoMemory;
	}catch(RuntimeError& e) {
			return kSDIErrorUnknownError;
	}				

	return kSDIErrorNone;
}