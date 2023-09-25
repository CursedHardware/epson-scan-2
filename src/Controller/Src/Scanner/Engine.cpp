#include "Engine.hpp"
#include "Image.hpp"
#include <dlfcn.h>

#include "PathUtils.h"

namespace epsonscan {

																									
void  Engine::ScannerWillBeginContinuousScanning(IESScanner* pScanner)
{
	 SDI_TRACE_LOG("Enter");

	if (isAfmEnabled_)
	{
		callBack_(kEngineEventTypeStartContinuousScanInAFM, nullptr, kSDIErrorNone);
	}

}

void  Engine::ScannerDidEndContinuousScanning(IESScanner* pScanenr)
{
	 SDI_TRACE_LOG("Enter");

	if (isAfmEnabled_)
	{
		engine_->ScheduleAutoFeedingModeTimeout();
		callBack_(kEngineEventTypeStopContinuousScanInAFM, nullptr, kSDIErrorNone);
	}
}

void  Engine::ScannerWillScanToScannedImage( IESScanner* pScanner, IESScannedImage* pESImage )
{
	 SDI_TRACE_LOG("Enter");

}

void  Engine::ScannerDidScanToScannedImage( IESScanner* pScanner, IESScannedImage* pESImage )
{
	 SDI_TRACE_LOG("Enter");

	pESImage->SetStorageType(kESStorageTypeFile);

	 if (callBack_) 
	 {
		 Image* image = new Image(pESImage);
		 callBack_(kEngineEventTypeDataReceive, image, kSDIErrorNone);
	 }
	 SDI_TRACE_LOG("Leave");
}

void  Engine::ScannerWillCancelScanning( IESScanner* pScanner )
{
	 SDI_TRACE_LOG("Enter");
}

void  Engine::ScannerDidCancelScanning( IESScanner* pScanner)
{
	 SDI_TRACE_LOG("Enter");

	callBack_(kEngineEventTypeCancel, nullptr, kSDIErrorNone);
}

void  Engine::ScannerDidCompleteScanningWithError( IESScanner* pScanner, ESErrorCode err)
{
	SDI_TRACE_LOG("Enter");

	callBack_(kEngineEventTypeScanEnd, nullptr, ExchangeError(err));
}

void  Engine::ScannerDidInterruptScanningWithError(IESScanner* pcScanner ,ESErrorCode err) 
{
	 SDI_TRACE_LOG("Enter");

}

void  Engine::ScannerDidEncounterDeviceCommunicationError( IESScanner* pScanner, ESErrorCode err ) 
{
	 SDI_TRACE_LOG("Enter");


	if (callBack_)
	{
 	 	callBack_(kEngineEventTypeDisconnect, nullptr, ExchangeError(err));
	}
}

void  Engine::ScannerWillWarmUp( IESScanner* pScanner )
{
	 SDI_TRACE_LOG("Enter");

}

void  Engine::ScannerDidWarmUp( IESScanner* pScanner ) 
{
	 SDI_TRACE_LOG("Enter");

}

void  Engine::NetworkScannerDidRequestStartScanning( IESScanner* pScanner ) 
{
	 SDI_TRACE_LOG("Enter");

   	 callBack_(kEngineEventTypePushScan, nullptr, kSDIErrorNone);

}

void  Engine::NetworkScannerDidRequestStopScanning( IESScanner* pScanner ) 
{
	 SDI_TRACE_LOG("Enter");

	if (pScanner->IsScanning())
	{
		 Cancel();
	}else{
 		callBack_(kEngineEventTypeDisconnect, nullptr, kSDIErrorNone);
	}
}

void Engine::ScannerDidNotifyStatusChange(IESScanner *pScanner)
{
	 SDI_TRACE_LOG("Enter");

}

void  Engine::ScannerDidDisconnect( IESScanner* pScanner ) 
{
	 SDI_TRACE_LOG("Enter");


	 callBack_(kEngineEventTypeDisconnect, nullptr, kSDIErrorNone);
}

void  Engine::NetworkScannerDidReceiveServerError(IESScanner* pScanner) 
{
	 SDI_TRACE_LOG("Enter");

  	 callBack_(kEngineEventTypeDisconnect, nullptr, kSDIErrorNone);
}

BOOL  Engine::NetworkScannerShouldPreventTimeout( IESScanner* pScanner ) 
{
	 SDI_TRACE_LOG("Enter");

	return TRUE;
}

void  Engine::NetworkScannerDidTimeout(  IESScanner* pScanner ) 
{
	 SDI_TRACE_LOG("Enter");
  	 callBack_(kEngineEventTypeDisconnect, nullptr, kSDIErrorNone);
}

void  Engine::ScannerIsReservedByHost( IESScanner* pScanner, const ES_CHAR* pszAddress ) 
{
	 SDI_TRACE_LOG("Enter");

}

void  Engine::ScannerDidPressButton( UInt8 un8ButtonNumber ) 
{
	 SDI_TRACE_LOG("Enter");

}

void  Engine::ScannerDidRequestStop(IESScanner *pScanner)
{
	SDI_TRACE_LOG("Enter");

}

void  Engine::ScannerDidRequestPushScanConnection(IESScanner *pScanner)
{
	SDI_TRACE_LOG("Enter");

}


Engine::Engine(ESCommandType comType) : module_(nullptr), engine_(nullptr), isAfmEnabled_(false)
{
	LoadSDModule(comType);
}

Engine::~Engine() 
{
	try{
		if (engine_) 
		{
			engine_->SetDelegate(nullptr);

			engine_->DestroyInstance();
			engine_ = nullptr;
		}

		dlclose(module_);

		module_ = nullptr;	
		engine_ = nullptr;
		SDI_TRACE_LOG("Engine destory");
	}catch(...)
	{

	}
}

void	Engine::call_GetValueForKey(const char *key, CESResultString &str) {
	if (engine_) {
			engine_->GetValueForKey(key, &str);
	}
}



void Engine::call_GetAvailableValuesForKey(const char *key, CESResultString &str) {
	if (engine_) {
		engine_->GetAvailableValuesForKey(key, &str);
	}
}

void Engine::SetValueForKey(const ESString& strKey, const ESAny& anyValue)
{
	if(engine_)
	{
		ESDictionary dictTmp;
		dictTmp[strKey] = anyValue;
		ESString strJson;
		unsigned int nRet = ES_CMN_FUNCS::JSON::DictionaryToJSON(dictTmp, strJson);
		if(!nRet)
		{
			SDI_TRACE_LOG("SetValueForKey  Key = %s value = %s", strKey.c_str(), strJson.c_str());
			engine_->SetValueForKey(strKey.c_str(), strJson.c_str());
		}
	}
}

void	Engine::SetValuesForKeysWithDictionary(const ESDictionary& dictionary) 
{
	SDI_TRACE_LOG("Enter");

	ESString jsonStr;

	ES_CMN_FUNCS::JSON::DictionaryToJSON(dictionary, jsonStr);
	if (engine_) {
		ES_Dump_Json(dictionary, "enineParam");
		engine_->SetValuesWithJSON(jsonStr.c_str());
	}
	SDI_TRACE_LOG("Leave");
}

bool Engine::InitWithDeviceInfoDict(const char *deviceInfo) {
	SDI_TRACE_LOG("Enter = %s", deviceInfo);
	if (engine_) {
		 engine_->SetConnection(deviceInfo);
	}
	SDI_TRACE_LOG("Leave");
	return true;
}

SDIError Engine::Open()
{
	SDI_TRACE_LOG("Enter");

	//kill es2netif
	//kill es2intif

	system("killall -9 -q es2netif > /dev/null");
	system("killall -9 -q es2intif > /dev/null");

	if (engine_) {
		return ExchangeError(engine_->Open());
	}
	SDI_TRACE_LOG("Leave");
	return kSDIErrorNone;
}

SDIError Engine::Close()
{
	SDI_TRACE_LOG("Enter");
	if (engine_) {
		return ExchangeError(engine_->Close());
	}
	SDI_TRACE_LOG("Leave");
	return kSDIErrorNone;
}

SDIError Engine::Cancel() 
{
	SDI_TRACE_LOG("Enter");
	if (engine_) {
			return ExchangeError(engine_->Cancel());
	}
	SDI_TRACE_LOG("Leave");
	return kSDIErrorNone;
}

SDIError Engine::ScanInBackground() 
{
	SDI_TRACE_LOG("Enter");
	if (engine_) {
		return ExchangeError(engine_->ScanInBackground());
	}	
	
	SDI_TRACE_LOG("Leave");
	return kSDIErrorNone;
}

SDIError Engine::StartJobInMode(ESJobMode eJobMode)
{
	SDI_TRACE_LOG("Enter");
	if (engine_) {

		if (eJobMode == kESJobModeAFM)
		{
			isAfmEnabled_ = true;
		}

		return ExchangeError(engine_->StartJobInMode(eJobMode));
	}	

	SDI_TRACE_LOG("Leave");
	return kSDIErrorNone;
}
					
SDIError Engine::StopJobInMode(ESJobMode eJobMode)
{
	SDI_TRACE_LOG("Enter");
	if (engine_) {

		if (eJobMode == kESJobModeAFM)
		{
			isAfmEnabled_ = false;
		}

		return ExchangeError(engine_->StopJobInMode(eJobMode));
	}	

	SDI_TRACE_LOG("Leave");
	return kSDIErrorNone;
}


SDIError Engine::RequestSavingPendingImages()
{
	SDI_TRACE_LOG("Enter");
	
	
	SDI_TRACE_LOG("Leave");
	return kSDIErrorNone;
}


void Engine::LoadSDModule(ESCommandType comType)
{
	SDI_TRACE_LOG("Enter");
	ESString strModulePath = EPSON_INSTALL_PATH;
 	ES_CMN_FUNCS::PATH::ES_CombinePath(strModulePath, strModulePath, ES_STRING("libes2command.so"));
	module_ = dlopen(strModulePath.c_str(), RTLD_LAZY) ;
	if (module_) {
		PFN_CREATE_SCANNER pfnCreateScanner =(PFN_CREATE_SCANNER) dlsym(module_,"ESCreateScanner");	  	
		ES_REQUIRE(pfnCreateScanner != nullptr, "CreateProc is nil");
		
		ESErrorCode eErrorCode =  pfnCreateScanner(comType,&engine_);
		ES_REQUIRE(eErrorCode == kESErrorNoError, "engine create fails");

		engine_->SetDelegate(this);

	} else {
	    SDI_TRACE_LOG("%s", dlerror());
		ES_REQUIRE(false, "module load fails");
	}
	SDI_TRACE_LOG("Leave");
}

} // namespace epsonscan
