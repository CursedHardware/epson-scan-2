#pragma once

#include <cstdint>
#include <string>
#include <mutex>

#include "CommonUtility/ESCommonTypedef.h"
#include "Include/ES2Command/ESCommandProperties.h"

typedef enum {
    ESCILogFunctionLamp1Counter,
    ESCILogFunctionLamp2Counter,
    ESCILogFunctionFlatbedScanCounter,
    ESCILogFunctionSimplexScanCounter,
    ESCILogFunctionDuplexScanCounter,
    ESCILogFunctionPickupRollerCounter,
    ESCILogFunctionPowerOffTime,
} ESCILogFunctionKey;

namespace epsonscan2 {
namespace es2command {

class ModelInfo
{
private:
    ModelInfo();
public:
    static ModelInfo& Instance();
    ~ModelInfo();

    bool IsShouldKeepControlMode(const std::string& model_name);
    bool IsShouldSyncFunctionalUnit(const std::string& model_name);
    bool IsShouldZeroClearEdgeFillForFB(const std::string& model_name);
    bool IsShouldIgnoreDateAndTime(const std::string& model_name);
    bool IsShouldObserveButtonWithCommand(const std::string& model_name);
    bool IsShouldAlwaysWaitAtIMGRequest(const std::string& model_name);
    bool IsShouldIgnoreCancelResponse(const std::string& model_name);
    int32_t CustomButtonObservationInterval_ms(const std::string& model_name);
    bool IsSupportedLogFunctions(const std::string& model_name, ESCILogFunctionKey function);
    bool IsShouldSendStatAfterADFScan(const std::string& model_name);

private:
    static std::mutex sm_mtx;
    ESDictionary m_dicInfo;
};

} // namespace es2command
} // namespace epsonscan2