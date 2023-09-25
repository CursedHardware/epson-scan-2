
#ifdef __APPLE__
#include "TargetConditionals.h"
#endif

#if defined(__ANDROID__) || TARGET_OS_IPHONE || defined(TARGET_OS_EMBEDDED)
#else
#include "Include/Config.h"
#endif

#include <memory>
#include <cassert>
#include "ModelInfo.h"
#include "CommonUtility/utils/ESAnyCastUtils.h"
#include "CommonUtility/utils/ESJsonUtils.h"
#include "CommonUtility/utils/PathUtils.h"

using namespace ES_CMN_FUNCS::JSON;
using namespace ES_CMN_FUNCS::PATH;

namespace epsonscan2 {
namespace es2command {

#if  !defined(__ANDROID__) && !TARGET_OS_IPHONE && !defined(TARGET_OS_EMBEDDED)
STATIC_CONST_KEY kModelInfoFile = ES_STRING("Resources/Models/ModelInfo.json");
#endif
    
namespace {

#if  !defined(__ANDROID__) && !TARGET_OS_IPHONE && !defined(TARGET_OS_EMBEDDED)
ESString ModelInfoFilePath()
{

    ESString path(EPSON_INSTALL_PATH);
    ES_CombinePath(path, path, kModelInfoFile);
    return path;
}
#endif


bool IsTargetModel(const ESDictionary& dic, const ESString& key, const ESString& model)
{
    const ESStringArray* models = SAFE_KEYS_DATA_CPTR(dic, key, ESStringArray);

    if (models){
        auto it = std::find(models->begin(), models->end(), model);
        if (it != models->end()){
            return true;
        }
    }
    return false;
}
ESString ESCILogFunctionKeyString(ESCILogFunctionKey key)
{
    switch(key)
    {
    case ESCILogFunctionLamp1Counter:
        return ES_STRING("Lamp1Counter");
    case ESCILogFunctionLamp2Counter:
        return ES_STRING("Lamp2Counter");
    case ESCILogFunctionFlatbedScanCounter:
        return ES_STRING("FlatbedScanCounter");
    case ESCILogFunctionSimplexScanCounter:
        return ES_STRING("SimplexScanCounter");
    case ESCILogFunctionDuplexScanCounter:
        return ES_STRING("DuplexScanCounter");
    case ESCILogFunctionPickupRollerCounter:
        return ES_STRING("PickupRollerCounter");
    case ESCILogFunctionPowerOffTime:
        return ES_STRING("PowerOffTime");
    default:
        return ES_STRING("");
    }
}
} // namespace

std::mutex ModelInfo::sm_mtx;

ModelInfo& ModelInfo::Instance()
{
    std::lock_guard<std::mutex> lock(sm_mtx);
    static std::unique_ptr<ModelInfo> instance(new ModelInfo());
    return *instance;
}


ModelInfo::ModelInfo()
{
#if defined(__ANDROID__) || TARGET_OS_IPHONE || defined(TARGET_OS_EMBEDDED)

    const char* json = R"({
            "shouldKeepControlMode" : {
                "array_str" : [
                "DS-50000",
                        "DS-60000",
                        "DS-70000",
                        "DS-5500",
                        "DS-6500",
                        "DS-7500"
                ]
            },
            "shouldSyncFunctionalUnit" : {
                "array_str" : [
                "DS-50000",
                        "DS-60000",
                        "DS-70000",
                        "DS-5500",
                        "DS-6500",
                        "DS-7500"
                ]
            },
            "shouldZeroClearEdgeFillForFB" : {
                "array_str" : [
                "DS-50000",
                        "DS-60000",
                        "DS-70000",
                        "DS-5500",
                        "DS-6500",
                        "DS-7500"
                ]
            },
            "shouldIgnoreDateAndTime" : {
                "array_str" : [
                "DS-860",
                        "DS-760",
                        "DS-510",
                        "DS-520",
                        "DS-560",
                        "DS-50000",
                        "DS-60000",
                        "DS-70000",
                        "DS-5500",
                        "DS-6500",
                        "DS-7500"
                ]
            },
            "shouldObserveButtonWithCommand" : {
                "array_str" : [
                "DS-30"
                ]
            },
            "shouldAlwaysWaitAtIMGRequest" : {
                "array_str" : [
                "DS-770",
                        "DS-780N"
                ]
            },
            "customButtonObservationInterval" : [
            {
                "DS-30" : {
                    "int" : 250
                }
            }
            ],
            "shouldIgnoreCancelResponse" : {
                "array_str" : [
                "DS-30"
                ]
            },
            "supportedLogFunctions" : [
            {
                "DS-30" : [
                {
                    "Lamp1Counter" : {
                        "boolean" : false
                    },
                    "Lamp2Counter" : {
                        "boolean" : false
                    },
                    "FlatbedScanCounter" : {
                        "boolean" : false
                    },
                    "SimplexScanCounter" : {
                        "boolean" : true
                    },
                    "DuplexScanCounter" : {
                        "boolean" : false
                    },
                    "PickupRollerCounter" : {
                        "boolean" : false
                    },
                    "PowerOffTime" : {
                        "boolean" : false
                    }
                }
                ]
            }
            ]
    })";

    UInt32 n = JSONtoDictionary (json, m_dicInfo);
    assert(n == 0);

#else
    UInt32 n = JSONFiletoDictionary(ModelInfoFilePath(), m_dicInfo);
    assert(n == 0);
#endif
}
ModelInfo::~ModelInfo()
{
}

bool ModelInfo::IsShouldKeepControlMode(const std::string& model_name)
{
    return IsTargetModel(m_dicInfo, ES_STRING("shouldKeepControlMode"), model_name);
}
bool ModelInfo::IsShouldSyncFunctionalUnit(const std::string& model_name)
{
    return IsTargetModel(m_dicInfo, ES_STRING("shouldSyncFunctionalUnit"), model_name);
}
bool ModelInfo::IsShouldZeroClearEdgeFillForFB(const std::string& model_name)
{
    return IsTargetModel(m_dicInfo, ES_STRING("shouldZeroClearEdgeFillForFB"), model_name);
}
bool ModelInfo::IsShouldIgnoreDateAndTime(const std::string& model_name)
{
    return IsTargetModel(m_dicInfo, ES_STRING("shouldIgnoreDateAndTime"), model_name);
}
bool ModelInfo::IsShouldObserveButtonWithCommand(const std::string& model_name)
{
    return IsTargetModel(m_dicInfo, ES_STRING("shouldObserveButtonWithCommand"), model_name);
}
bool ModelInfo::IsShouldAlwaysWaitAtIMGRequest(const std::string& model_name)
{
    return IsTargetModel(m_dicInfo, ES_STRING("shouldAlwaysWaitAtIMGRequest"), model_name);
}
bool ModelInfo::IsShouldIgnoreCancelResponse(const std::string& model_name)
{
    return IsTargetModel(m_dicInfo, ES_STRING("shouldIgnoreCancelResponse"), model_name);
}
int32_t ModelInfo::CustomButtonObservationInterval_ms(const std::string& model_name)
{
    const ESDictionary* pDicIntervals = SafeKeysDataCPtr<ESDictionary>(m_dicInfo, ES_STRING("customButtonObservationInterval"));
    if (pDicIntervals){
        const ESNumber* pnInterval = SafeKeysDataCPtr<ESNumber>(*pDicIntervals, model_name);
        if (pnInterval){
            return static_cast<int32_t>(*pnInterval);
        }
    }
    return -1;
}

bool ModelInfo::IsSupportedLogFunctions(const std::string& model_name, ESCILogFunctionKey function)
{
    const ESDictionary* pDicSupportedLogFunctions = SafeKeysDataCPtr<ESDictionary>(m_dicInfo, ES_STRING("supportedLogFunctions"));
    if (pDicSupportedLogFunctions){
        const ESDictionary* pDicModelsLogFuncs = SafeKeysDataCPtr<ESDictionary>(*pDicSupportedLogFunctions, model_name);
        if (pDicModelsLogFuncs){
            const bool* pIsSupport = SafeKeysDataCPtr<bool>(*pDicModelsLogFuncs, ESCILogFunctionKeyString(function));
            if (pIsSupport){
                return *pIsSupport;
            } else {
                return false;
            }
        }
    }
    return true;
}

bool ModelInfo::IsShouldSendStatAfterADFScan(const std::string& model_name)
{
    return IsTargetModel(m_dicInfo, ES_STRING("shouldSendStatAfterADFScan"), model_name);
}

} // namespace es2command
} // namespace epsonscan2
