#include "Utility.hpp"
#include "Controller.hpp"
#include "Scanner.hpp"
#include "ModelInfo.hpp"
#include "TransferMgr.hpp"
#include "KeyMgr.hpp"
#include "Error.hpp"
#include <utils/PathUtils.h>

namespace epsonscan{

	Controller::Controller(const SDIDeviceInfo& devInfo, SDIScannerDriver* driver) :  
	opened_(false),
	interruptionEnabled_(false),
	interruptEventCallBack_(nullptr), 
	interruptEventCallBackUserData_(nullptr),
	driver_(driver)
	{
		SDI_TRACE_LOG("Enter");

		modelInfo_ = std::make_shared<ModelInfo>(devInfo);
		ES_REQUIRE(modelInfo_, "modelInfo is kEventTypeImagenil");

	  	scanner_ = std::make_shared<Scanner>(devInfo, modelInfo_);
		ES_REQUIRE(modelInfo_, "scanner is nil");

		scanner_.get()->SetCallBackReceiver(std::bind(&Controller::ScannerCallBackReceiver,
													   this, 
													   std::placeholders::_1,
													   std::placeholders::_2,
													   std::placeholders::_3
													   ));

		keyMgr_ = std::make_shared<KeyMgr>(modelInfo_, scanner_);
		ES_REQUIRE(keyMgr_, "keyMgr is nil");

		transferMgr_ = std::make_shared<TransferMgr>();
		ES_REQUIRE(transferMgr_, "TransferMgr is nil");
	
		SDI_TRACE_LOG("Leave");
	}

	Controller::~Controller()																					
	{
		try{
			SDI_TRACE_LOG("Controller destoroy");
		}catch(...)
		{

		}
	}
			
	void Controller::ScannerCallBackReceiver(EngineEventType type, Image* image, SDIError error)
	{
		SDI_TRACE_LOG("Enter");

		switch(type)
		{
			case kEngineEventTypeDataReceive:			
			{
				SDI_TRACE_LOG("DataReceive");
				if (image)
				{
					image->applyImageProc(keyMgr_, modelInfo_);
				}
				transferMgr_.get()->EnqueEvent(TransferEvent(kEventTypeImage, image, error));
			}
			break;


			case kEngineEventTypeScanEnd:
			{
				SDI_TRACE_LOG("ScanEnd");

				transferMgr_.get()->EnqueEvent(TransferEvent(kEventTypeComplete, nullptr, error));
				transferMgr_.get()->Close();
			}
			break;

			case kEngineEventTypeCancel:
			{
				SDI_TRACE_LOG("Cancel");
				transferMgr_.get()->EnqueEvent(TransferEvent(kEventTypeCancel, nullptr, error));
			}
			break;

			case kEngineEventTypeStartContinuousScanInAFM:
			{
				SDI_TRACE_LOG("kEngineEventTypeStartContinuousScanInAFM");
				transferMgr_.get()->EnqueEvent(TransferEvent(kEventTypeAFMStartConituousScan, nullptr, error));
			}
			break;

			case kEngineEventTypeStopContinuousScanInAFM:
			{
				SDI_TRACE_LOG("kEngineEventTypeStopContinuousScanInAFM");
				transferMgr_.get()->EnqueEvent(TransferEvent(kEventTypeAFMStopContinuousScan, nullptr, error));
			}
			break;

			case kEngineEventTypeDisconnect:
			{
				SDI_TRACE_LOG("Disconnct");
				
				opened_ = false;

				if (interruptEventCallBack_)
				{
					interruptEventCallBack_(driver_, kSDIInterruptEventTypeDisconnect, interruptEventCallBackUserData_);
				}
			}
			break;

			case kEngineEventTypePushScan:
			{
				SDI_TRACE_LOG("PushScan");

				if (interruptEventCallBack_)
				{
					interruptEventCallBack_(driver_, kSDIInterruptEventTypePushScan, interruptEventCallBackUserData_);
				}
			}



			break;
		}

		SDI_TRACE_LOG("Leave");
	}

	boost::optional<TransferEvent> Controller::DequeueEvent()
	{
		auto event = transferMgr_.get()->DequeueEvent();
		if (event  && event->GetImage() != nullptr)
		{
			if(event->GetError() != kSDIErrorNone)
			{
				Cancel();
			}
		}

		return event;
	}

	SDIError Controller::Open()
	{
		SDI_TRACE_LOG("Enter");

		SDIError err = kSDIErrorNone;

		if (opened_) {
			return err;	
		}

		err = scanner_.get()->Open();
		if (err == kSDIErrorNone)
		{
			opened_ = true;

			Reset();

			CheckGlassDirtStatus();

		}
		SDI_TRACE_LOG("Leave");

		return err;
	}	
				
	void Controller::Close()
	{
		SDI_TRACE_LOG("Enter");

		if (opened_ == false) {
			return;	
		}

		// deque all event before close
		while(DequeueEvent())
		{
		}

		scanner_.get()->Close();
		opened_ = false;

		// delete work folder
		ES_CMN_FUNCS::PATH::ES_DeleteFolder(ES_CMN_FUNCS::PATH::ES_GetWorkTempPath());


		SDI_TRACE_LOG("Leave");
	}

	void Controller::Reset()
	{
		SDI_TRACE_LOG("Enter");
		keyMgr_.get()->Reset();
		SDI_TRACE_LOG("Leave");
	}

	void Controller::SetValue(const std::string& key, SDIValueType valType, void* value, SDIInt size)
	{
		SDI_TRACE_LOG("Enter");
		keyMgr_.get()->SetValue(key, valType, value, size);
		SDI_TRACE_LOG("Leave");
	}
	
	void Controller::GetValue(const std::string& key, SDIValueType valType, void* value, SDIInt size) 
	{
		SDI_TRACE_LOG("Enter");
		keyMgr_.get()->GetValue(key, valType, value, size);
		SDI_TRACE_LOG("Leave");
	}

	void Controller::CheckGlassDirtStatus()
	{
		SDI_TRACE_LOG("Enter");

		if (scanner_->GetIsOpened() == false)
		{
			return;
		}

		ESNumber glassDirty = 0;
		if (scanner_->GetValueForKey(kESSensorGlassStatus, glassDirty))
		{
			if (glassDirty == kESSensorGlassDirty)
			{
				if (interruptEventCallBack_)
				{
					SDI_TRACE_LOG("Glass dirty found");
					interruptEventCallBack_(driver_, kSDIInterruptEventTypeGlassDirty, interruptEventCallBackUserData_);
				}
			}
		}

		ESNumber cleaningRequired = 0;
		if (scanner_->GetValueForKey(kESWarningStatus, cleaningRequired))
		{
			if (cleaningRequired == kESWarningNeedCleaning)
			{
				if (interruptEventCallBack_)
				{
					SDI_TRACE_LOG("Cleaning required");
					interruptEventCallBack_(driver_, kSDIInterruptEventTypeCleaningRequired, interruptEventCallBackUserData_);
				}
			}
		}

		SDI_TRACE_LOG("Leave");
	}
	SDIError Controller::UnlockAdministratorLock()
	{
		return scanner_.get()->UnlockAdministratorLock();
	}

	SDIError Controller::LockAdministratorLock() 
	{
		return scanner_.get()->LockAdministratorLock();
	}

	SDIError Controller::Scan()
	{
		SDI_TRACE_LOG("Enter");

		if (opened_ == false)
		{
			return kSDIErrorDataSend;
		}


		transferMgr_->Reset();
		transferMgr_->Open();
		scanner_->Reset();

		ESDictionary engineParams = keyMgr_.get()->GetEngineKeyValues();

		scanner_->SetValuesForKeysWithDictionary(engineParams);
		return scanner_->Scan();	

		SDI_TRACE_LOG("Leave");
	}

					
	SDIError Controller::StartAFM(bool continuous)
	{
		SDI_TRACE_LOG("Enter");
		if (opened_ == false)
		{
			return kSDIErrorDataSend;
		}


		transferMgr_->Reset();
		transferMgr_->Open();
		scanner_->Reset();

		ESDictionary engineParams = keyMgr_.get()->GetEngineKeyValues();

		scanner_->SetValuesForKeysWithDictionary(engineParams);
		if (continuous)
		{
			return scanner_->StartJobInMode(kESJobModeAFMC);	
		}else
		{
			return scanner_->StartJobInMode(kESJobModeAFM);	
		}
		SDI_TRACE_LOG("Leave");
	}
				
	SDIError Controller::StopAFM(bool continuous)
	{
		SDI_TRACE_LOG("Enter");

		if (continuous)
		{
			return scanner_->StopJobInMode(kESJobModeAFMC);	
		}else{
			return scanner_->StopJobInMode(kESJobModeAFM);	
		}
		SDI_TRACE_LOG("Leave");
	}


	void Controller::Cancel()
	{
		SDI_TRACE_LOG("Enter");

		scanner_.get()->Cancel();

		SDI_TRACE_LOG("Leave");
	}


	void Controller::GetCapability(const std::string& key, SDICapability& capability)
	{	
		SDI_TRACE_LOG("Enter key = %s", key.c_str());

		keyMgr_.get()->GetCapability(key, capability);

		SDI_TRACE_LOG("Leave");
	}

	void Controller::GetDefault(const std::string& key, SDIValueType valType, void* value, SDIInt size)
	{
		SDI_TRACE_LOG("Enter");
		keyMgr_.get()->GetDefault(key, valType, value, size);
		SDI_TRACE_LOG("Leave");
	}

	bool Controller::CheckNextEvent(bool waitUntilReceived)
	{
		if (waitUntilReceived)
		{
			return transferMgr_.get()->CanDequeue();	
		}

		return transferMgr_.get()->CanDequeueWithoutWait();
	}

}//namespace epsonscan
