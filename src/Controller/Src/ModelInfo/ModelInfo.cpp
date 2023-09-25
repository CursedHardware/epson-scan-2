#include "Utility.hpp"
#include "ModelInfo.hpp"
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <boost/foreach.hpp>
#include <ESJsonUtils.h>
#include <utils/PathUtils.h>
#include "sha1.h"

namespace epsonscan {

ModelInfo::ModelInfo(const SDIDeviceInfo& devInfo) {

	std::string modelInfoPath;
	GetModelInfoDirPath(devInfo, modelInfoPath);
	
	ESDictionary outDict;
    ES_CMN_FUNCS::JSON::JSONFiletoDictionary(modelInfoPath, outDict);

	hardwareInfo_ = boost::any_cast<ESDictionary>(outDict["HardwareInfo"]); 

 	if (ES_CMN_FUNCS::PATH::ES_IsExistFile(modelInfoPath)) {
		ESDictionary outDict;
		ES_CMN_FUNCS::JSON::JSONFiletoDictionary(modelInfoPath, outDict);
		hardwareInfo_ = boost::any_cast<ESDictionary>(outDict["HardwareInfo"]);
		modelID_ =  "ES" + boost::any_cast<ESString>(hardwareInfo_[kPrvHRD_ModelID]);
	}
}

ModelInfo::~ModelInfo()
{
	SDI_TRACE_LOG("ModelInfo dispose");
}

bool ModelInfo::GetConvertTable(ESDictionary& dict) {
	std::string path = EPSON_INSTALL_PATH + std::string(kBaseResourceDirPath) + "Models/"+ modelID_ + "/ConvertTable.json" ;
	SDI_TRACE_LOG("%s", path.c_str());
	ES_CMN_FUNCS::JSON::JSONFiletoDictionary(path , dict);

	return true;
}

//static
bool  ModelInfo::GetModelIDFromProductID(int32_t productID, std::string hrdDir, std::string& outModelID)
{
        if (ES_CMN_FUNCS::PATH::ES_IsDirectory(hrdDir)) {
			if (ES_CMN_FUNCS::PATH::ES_IsExistFile(hrdDir))
			{
					auto files = ES_CMN_FUNCS::PATH::EnumFilesInDir("", hrdDir);
					for(auto file : files)
					{
						ESString ext;
						ES_CMN_FUNCS::PATH::ES_GetExtension(ext, file);
					    if (ext == "json") {
							ESDictionary dict;   
							ES_CMN_FUNCS::JSON::JSONFiletoDictionary(file.c_str(), dict);	
						    ESDictionary hard = boost::any_cast<ESDictionary>(dict["HardwareInfo"]);
							ESString productIDAsString = boost::any_cast<ESString>(hard[kPrvHRD_ProductID]);
							int32_t modelProductID = strtol(productIDAsString.c_str(), nullptr, 16);
							SDI_TRACE_LOG("modelProductID = %d productIDAsString = %s ", modelProductID, productIDAsString.c_str());

							if (productID == modelProductID) {
								outModelID = "ES" + boost::any_cast<ESString>(hard[kPrvHRD_ModelID]);	
								return true;
							}
						}	
					}
			}
		}

		return false;
}

// static
bool ModelInfo::GetModelIDFromProductID(int32_t productID, std::string& outModelID)
{
	const std::string path( EPSON_INSTALL_PATH + std::string(kBaseResourceDirPath) + "Models/");
	auto files = ES_CMN_FUNCS::PATH::EnumFilesInDir("", path);

	for(auto file : files)
	{
        if (ES_CMN_FUNCS::PATH::ES_IsDirectory(file)) {
		  	std::string allPath = file;
			  
			ES_CMN_FUNCS::PATH::ES_CombinePath(allPath, allPath, "ALL");
			if(GetModelIDFromProductID(productID, allPath.c_str(), outModelID))
			{
				return true;
			}
	
			std::string jpPath = file;
			ES_CMN_FUNCS::PATH::ES_CombinePath(jpPath, jpPath, "JP");
			if(GetModelIDFromProductID(productID, jpPath.c_str(), outModelID))
			{
				return true;
			}
			
			std::string wwPath = file;
			ES_CMN_FUNCS::PATH::ES_CombinePath(wwPath, wwPath, "WW");
			if(GetModelIDFromProductID(productID, wwPath.c_str(), outModelID))
			{
				return true;
			}

			std::string usPath = file;
			ES_CMN_FUNCS::PATH::ES_CombinePath(usPath, usPath, "US");
			if(GetModelIDFromProductID(productID, usPath.c_str(), outModelID))
			{
				return true;
			}


			std::string us2Path = file;
			ES_CMN_FUNCS::PATH::ES_CombinePath(us2Path, us2Path, "US2");
			if(GetModelIDFromProductID(productID, us2Path.c_str(), outModelID))
			{
				return true;
			}
		}
    }

	return false;
}

#define SHA1HashSize 20
//static
void ModelInfo::GetSha1Hash(const std::string& inValue, std::string& outValue)
{
    SHA1Context sha;
    int  err = 0;
    uint8_t Message_Digest[SHA1HashSize];
  	char tmp[10] = "";
	outValue = "";
	std::stringstream ss;

    err = SHA1Reset(&sha);
    if (err) {
		return;
	}
    err = SHA1Input(&sha,(const unsigned char *) inValue.c_str() ,strlen(inValue.c_str()));
    if (err) {
       	return;
    }
    err = SHA1Result(&sha, Message_Digest);
    if (err) {
		return;
    } else {
		for (int i = 0; i < SHA1HashSize ; ++i) {
            sprintf(tmp, "%02x", Message_Digest[i] );
            tmp[2] = '\0';
			ss << std::string(tmp);
        }
    }
	outValue = ss.str();
}

//static
void ModelInfo::GetModelIDFromProductName(const std::string& productName, std::string hrdDir, std::string& outModelID)
{
    if (ES_CMN_FUNCS::PATH::ES_IsDirectory(hrdDir)) {
			if (ES_CMN_FUNCS::PATH::ES_IsExistFile(hrdDir)) {

					auto files = ES_CMN_FUNCS::PATH::EnumFilesInDir("", hrdDir);
					for(auto file : files)
					{
						ESString ext;
						ES_CMN_FUNCS::PATH::ES_GetExtension(ext, file);
					    if (ext== "json") {
							ESDictionary dict;   
							ES_CMN_FUNCS::JSON::JSONFiletoDictionary(file.c_str(), dict);	
						    ESDictionary hard = boost::any_cast<ESDictionary>(dict["HardwareInfo"]);

							if (hard[kPrvHRD_ProductID].empty() == false) {
								ESStringArray productNamesInHRD = boost::any_cast<ESStringArray>(hard[kPrvHRD_ProductName]);								
								for (ESString productNameInHRD : productNamesInHRD) {
									if (productName == productNameInHRD) {
										outModelID = "ES" + boost::any_cast<ESString>(hard[kPrvHRD_ModelID]);	
										return;
									}	
								}
							}

							if (hard[kPrvHRD_DeviceID].empty() == false) {
								ESStringArray deviceIDsInHRD = boost::any_cast<ESStringArray>(hard[kPrvHRD_DeviceID]);								
								for (ESString  deviceIDInHRD : deviceIDsInHRD) {
									if (productName == deviceIDInHRD) {
										outModelID = "ES" + boost::any_cast<ESString>(hard[kPrvHRD_ModelID]);	
										return;
									}	
								}
							}
						}	
					}
			}
		}
}	


// static
bool ModelInfo::GetModelIDFromProductName(const std::string& productName, std::string& outModelID)
{
	ESString hashName;
	GetSha1Hash(productName, hashName);

	const std::string path( EPSON_INSTALL_PATH + std::string(kBaseResourceDirPath) + "Models/");
	auto files = ES_CMN_FUNCS::PATH::EnumFilesInDir("", path);

	for(auto file : files)
	{
        if (ES_CMN_FUNCS::PATH::ES_IsExistFile(file)) 
		{
					
			std::string allPath = file;  
			ES_CMN_FUNCS::PATH::ES_CombinePath(allPath, allPath, "ALL");
			GetModelIDFromProductName(productName, allPath.c_str(), outModelID);
			GetModelIDFromProductName(hashName, allPath.c_str(), outModelID);


			std::string jpPath = file;  
			ES_CMN_FUNCS::PATH::ES_CombinePath(jpPath, jpPath, "JP");
			GetModelIDFromProductName(productName, jpPath.c_str(), outModelID);
			GetModelIDFromProductName(hashName, jpPath.c_str(), outModelID);

			std::string wwPath = file;  
			ES_CMN_FUNCS::PATH::ES_CombinePath(wwPath, wwPath, "WW");
			GetModelIDFromProductName(productName, wwPath.c_str(), outModelID);
			GetModelIDFromProductName(hashName, wwPath.c_str(), outModelID);

			std::string usPath = file;  
			ES_CMN_FUNCS::PATH::ES_CombinePath(usPath, usPath, "US");
			GetModelIDFromProductName(productName, usPath.c_str(), outModelID);
			GetModelIDFromProductName(hashName, usPath.c_str(), outModelID);		

			std::string us2Path = file;  
			ES_CMN_FUNCS::PATH::ES_CombinePath(us2Path, us2Path, "US2");
			GetModelIDFromProductName(productName, us2Path.c_str(), outModelID);
			GetModelIDFromProductName(hashName, us2Path.c_str(), outModelID);
        }
	}
    
	if(outModelID == ""){
               return false;
    }

	return true;
}

// static
bool  ModelInfo::GetModelInfoDirPath(const SDIDeviceInfo& devInfo, std::string& outPath)
{
	const std::string path( EPSON_INSTALL_PATH + std::string(kBaseResourceDirPath) + "Models/");
	auto files = ES_CMN_FUNCS::PATH::EnumFilesInDir("", path);
	for(auto file : files)
	{
        if (ES_CMN_FUNCS::PATH::ES_IsExistFile(file))
		{ 
			std::string allPath = file;  
			ES_CMN_FUNCS::PATH::ES_CombinePath(allPath, allPath, "ALL");
			ES_CMN_FUNCS::PATH::ES_CombinePath(allPath, allPath, std::string(devInfo.modelID));
			allPath += ".json";

			if (ES_CMN_FUNCS::PATH::ES_IsExistFile(allPath)) {
				outPath = allPath.c_str();
				return true;
			}


			std::string jpPath = file;  
			ES_CMN_FUNCS::PATH::ES_CombinePath(jpPath, jpPath, "JP");
			ES_CMN_FUNCS::PATH::ES_CombinePath(jpPath, jpPath, std::string(devInfo.modelID));
			jpPath += ".json";

			if (ES_CMN_FUNCS::PATH::ES_IsExistFile(jpPath)) {
				outPath = jpPath.c_str();
				return true;
			}
				

			std::string wwPath = file;  
			ES_CMN_FUNCS::PATH::ES_CombinePath(wwPath, wwPath, "WW");
			ES_CMN_FUNCS::PATH::ES_CombinePath(wwPath, wwPath, std::string(devInfo.modelID));
			wwPath += ".json";

			if (ES_CMN_FUNCS::PATH::ES_IsExistFile(wwPath)) {
				outPath = wwPath.c_str();
				return true;
			}
		}
	}
	return false;
}

} // nemespace epsonscan
