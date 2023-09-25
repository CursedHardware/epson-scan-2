#include "Filter.hpp"
#include "Utility.hpp"
#include <utils/PathUtils.h>

namespace epsonscan
{
    void Filter::Process(ESImageInfo& imageInfo, 
					     ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf)
    {
                SDI_TRACE_LOG("enter %s for page = %d", Name().c_str(), ES_IMAGE_INFO::GetESImagePageCount(imageInfo));
                DoProcess(imageInfo, inDataBuf);
				DumpImage(imageInfo, inDataBuf,  Name().c_str());
                SDI_TRACE_LOG("leave %s for page = %d",  Name().c_str(), ES_IMAGE_INFO::GetESImagePageCount(imageInfo));
     }

    std::string Filter::GetPluginRootDir()
    {
	  std::string strModulePath = EPSON_INSTALL_PATH;
      ES_CMN_FUNCS::PATH::ES_CombinePath(strModulePath, strModulePath, ES_STRING("non-free-exec"));
	  return strModulePath;
    }
}