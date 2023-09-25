
#include "EpsonScan2.h"
#include "Engine.hpp"
#include "TransferEvent.hpp"
#include "FFManager.hpp"
#include <memory>
#include <vector>
#include <string>

namespace epsonscan {

class Scanner;
class ModelInfo;
class TransferMgr;
class KeyMgr;
class Image;
class Controller {
		public:
				Controller(const SDIDeviceInfo& devInfo, SDIScannerDriver* driver); 

				virtual ~Controller();

				SDIError Open();
				void Close();

				void Reset();
				void SetValue(const std::string& key, SDIValueType valType, void* value, SDIInt size);
				void GetValue(const std::string& key, SDIValueType valType, void* value, SDIInt size); 
				void GetDefault(const std::string& key, SDIValueType valType, void* value, SDIInt size); 
				void GetCapability(const std::string& key, SDICapability& capability);			

				SDIError Scan();	

				SDIError StartAFM(bool continuous);
				SDIError StopAFM(bool continuous);

					
				void Cancel();
	
				boost::optional<TransferEvent>	DequeueEvent() ;
				bool CheckNextEvent(bool waitUntilReceived);

				void SetInterruptEventCallBackProc(InterruptEventCallBackProc interruptEventCallBack, void* interruptEventCallBackUserData)
				{
					interruptEventCallBack_ = interruptEventCallBack;
					interruptEventCallBackUserData_ = interruptEventCallBackUserData;
				}

				void SetInterruptionEnabled(bool interruptionEnabled)
				{
					interruptionEnabled_ = interruptionEnabled;
				}

				void CheckGlassDirtStatus();


				// Multi page API
				bool SDIMultipage_Start(const char* destPath,  SDIImageFormat format)
				{
					return FFManager::GetInstance().MultipageStart(destPath, format, keyMgr_);
				}

				bool SDIMultipage_Add(const char* sourcePath)
				{
					return FFManager::GetInstance().MultipageAdd(sourcePath);
				}

				bool SDIMultipage_Fin()
				{
					return FFManager::GetInstance().MultipageFin();
				}

			    SDIError UnlockAdministratorLock();

			   SDIError LockAdministratorLock();

		private:
				void ScannerCallBackReceiver(EngineEventType type, Image* image, SDIError error);
				std::shared_ptr<ModelInfo> 	modelInfo_;
				std::shared_ptr<Scanner> 	scanner_;
				std::shared_ptr<TransferMgr> transferMgr_;
				std::shared_ptr<KeyMgr>     keyMgr_;
				bool opened_;
				bool interruptionEnabled_;

				SDIScannerDriver* driver_;
				InterruptEventCallBackProc interruptEventCallBack_;
				void* interruptEventCallBackUserData_;
};

}//namespae epsonscan
