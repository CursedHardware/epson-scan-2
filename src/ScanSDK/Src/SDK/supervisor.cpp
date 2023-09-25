#include "supervisor.h"
#include "PathUtils.h"
#include "Config.h"
#include "saveimagepathlist.h"
#include <memory>


Supervisor::Supervisor():
    SDIDeviceFinder_CreatePtr_(NULL),
    SDIDeviceFinder_StartDiscoveryPtr_(NULL),
    SDIDeviceFinder_StopDiscoveryPtr_(NULL),
    SDIDeviceFinder_GetDevicesPtr_(NULL),
    SDIDeviceFinder_DisposePtr_(NULL),
    SDIScannerDriver_CreatePtr_(NULL),
    SDIScannerDriver_OpenPtr_(NULL),
    SDIScannerDriver_ClosePtr_(NULL),
    SDIScannerDriver_DisposePtr_(NULL),
    SDIScannerDriver_GetValuePtr_(NULL),
    SDIScannerDriver_SetValuePtr_(NULL),
    SDIScannerDriver_GetCapability_(NULL),
    SDIScannerDriver_DoScanJobPtr_(NULL),
    SDIScannerDriver_CheckNextTransferEventPtr_(NULL),
    SDIScannerDriver_GetNextTransferEventPtr_(NULL),
    SDIImage_CreatePtr_(NULL),
    SDIImage_GetPathPtr_(NULL),
    SDIImage_DisposePtr_(NULL),
    SDIImage_GetWidthPtr_(NULL),
    SDIImage_GetHeightPtr_(NULL),
    SDIImage_GetSamplesPerPixelPtr_(NULL),
    SDIImage_GetBitsPerSamplePtr_(NULL),
    SDIMultipage_StartPtr_(NULL),
    SDIMultipage_AddPtr_(NULL),
    SDIMultipage_FinPtr_(NULL),
    SDIDeviceInfo_ResolvePtr_(NULL),
    SDIImage_CheckImageIsBlankPtr_(NULL),
    es2lib_(nullptr),
    outEventType(kSDITrasnferEventTypeImage),
    scanning_status(SAVE),
    bAllReset(false),
    edit_mode(false),
    outImageData(NULL),
    output_path(nullptr),
    m_DoneBlanckPage(false),
    m_ExistsPage(false),
    m_pCbStFunc(NULL),
    m_pCbImgFunc(NULL),
    m_pPrivate(NULL),
    m_fileNumStartCount(0),
    m_numPageStart(0),
    m_numPageEnd(0),
    m_ScanCounter(0)
{
    driver = nullptr;
    m_LastError = kSDIErrorNone;
}

void Supervisor::SetUp() {

#ifdef AKBMODE
  std::string strDst = EPSON_INSTALL_PATH;
  std::string Controller_name = "lib";
  Controller_name = Controller_name + DRIVER_NAME;
  Controller_name = Controller_name + ".so";
  ES_CMN_FUNCS::PATH::ES_CombinePath(strDst, strDst, Controller_name.c_str());
#else
  std::string strDst = EPSON_INSTALL_PATH;
  ES_CMN_FUNCS::PATH::ES_CombinePath(strDst, strDst, "libepsonscan2.so");
#endif

  //std::cout << strDst.c_str() << std::endl;

  es2lib_ = dlopen(strDst.c_str(), RTLD_LAZY);
  if(es2lib_ == nullptr){
      printf("dlerro = %s\n", dlerror());
      assert(false);
  }

  SDIDeviceFinder_CreatePtr_ = (SDIDeviceFinder_CreatePtr)dlsym(es2lib_, "SDIDeviceFinder_Create");

  SDIDeviceFinder_StartDiscoveryPtr_ = (SDIDeviceFinder_StartDiscoveryPtr)dlsym(es2lib_, "SDIDeviceFinder_StartDiscovery");

  SDIDeviceFinder_StopDiscoveryPtr_ = (SDIDeviceFinder_StopDiscoveryPtr)dlsym(es2lib_, "SDIDeviceFinder_StopDiscovery");

  SDIDeviceFinder_GetDevicesPtr_ = (SDIDeviceFinder_GetDevicesPtr)dlsym(es2lib_, "SDIDeviceFinder_GetDevices");

  SDIDeviceFinder_DisposePtr_  = (SDIDeviceFinder_DisposePtr)dlsym(es2lib_, "SDIDeviceFinder_Dispose");

  SDIScannerDriver_CreatePtr_ = (SDIScannerDriver_CreatePtr)dlsym(es2lib_, "SDIScannerDriver_Create");

  SDIScannerDriver_OpenPtr_  = (SDIScannerDriver_OpenPtr)dlsym(es2lib_, "SDIScannerDriver_Open");

  SDIScannerDriver_ClosePtr_ = (SDIScannerDriver_ClosePtr)dlsym(es2lib_, "SDIScannerDriver_Close");

  SDIScannerDriver_DisposePtr_ = (SDIScannerDriver_DisposePtr)dlsym(es2lib_, "SDIScannerDriver_Dispose");

  SDIScannerDriver_GetValuePtr_ = (SDIScannerDriver_GetValuePtr)dlsym(es2lib_, "SDIScannerDriver_GetValue");

  SDIScannerDriver_SetValuePtr_ = (SDIScannerDriver_GetValuePtr)dlsym(es2lib_, "SDIScannerDriver_SetValue");

  SDIScannerDriver_GetCapability_ = (SDIScannerDriver_GetCapabilityPtr)dlsym(es2lib_, "SDIScannerDriver_GetCapability");

  SDIScannerDriver_DoScanJobPtr_ = (SDIScannerDriver_DoScanJobPtr)dlsym(es2lib_, "SDIScannerDrive_DoScanJob");

  SDIScannerDriver_CheckNextTransferEventPtr_ = (SDIScannerDriver_CheckNextTransferEventPtr)dlsym(es2lib_, "SDIScannerDriver_CheckNextTransferEvent");

  SDIScannerDriver_GetNextTransferEventPtr_ = (SDIScannerDriver_GetNextTransferEventPtr)dlsym(es2lib_, "SDIScannerDriver_GetNextTransferEvent");

  SDIImage_CreatePtr_ = (SDIImage_CreatePtr)dlsym(es2lib_, "SDIImage_Create");

  SDIImage_GetPathPtr_ = (SDIImage_GetPathPtr)dlsym(es2lib_, "SDIImage_GetPath");

  SDIImage_DisposePtr_ = (SDIImage_DisposePtr)dlsym(es2lib_, "SDIImage_Dispose");

  SDIImage_GetWidthPtr_ = (SDIImage_GetWidthPtr)dlsym(es2lib_, "SDIImage_GetWidth");

  SDIImage_GetHeightPtr_ = (SDIImage_GetWidthPtr)dlsym(es2lib_, "SDIImage_GetHeight");

  SDIImage_GetSamplesPerPixelPtr_ = (SDIImage_GetSamplesPerPixelPtr)dlsym(es2lib_, "SDIImage_GetSamplesPerPixel");

  SDIImage_GetBitsPerSamplePtr_ = (SDIImage_GetBitsPerSamplePtr)dlsym(es2lib_, "SDIImage_GetBitsPerSample");

  SDIMultipage_StartPtr_ = (SDIMultipage_StartPtr)dlsym(es2lib_, "SDIMultipage_Start");

  SDIMultipage_AddPtr_ = (SDIMultipage_AddPtr)dlsym(es2lib_, "SDIMultipage_Add");

  SDIMultipage_FinPtr_ = (SDIMultipage_FinPtr)dlsym(es2lib_, "SDIMultipage_Fin");

  SDIDeviceInfo_ResolvePtr_ = (SDIDeviceInfo_ResolvePtr)dlsym(es2lib_, "SDIDeviceInfo_Resolve");

  SDIImage_CheckImageIsBlankPtr_ = (SDIImage_CheckImageIsBlankPtr)dlsym(es2lib_, "SDIImage_CheckImageIsBlank");

  SDIScannerDriver_CheckCautionStatusPtr_ = (SDIScannerDriver_CheckCautionStatusPtr)dlsym(es2lib_, "SDIScannerDriver_CheckCautionStatus");
}

void Supervisor::Terminate()
{
    if(SaveImagePathList::save_image_path_list.size() != 0){
        auto save_path = SaveImagePathList::save_image_path_list.begin();
        int count = 0;
        while(count < SaveImagePathList::save_image_path_list.size()){
            if(*save_path){
                free(*save_path);
                *save_path = nullptr;
            }

            ++save_path;
            count++;
        }
        SaveImagePathList::save_image_path_list.clear();
    }else if(this->output_path){
        free(this->output_path);
        this->output_path = nullptr;
    }
    m_pCbStFunc = NULL;
    m_pCbImgFunc = NULL;
    m_pPrivate = NULL;
    if(es2lib_ != nullptr){
        dlclose(es2lib_);
        es2lib_ = nullptr;
    }
}

SDIError Supervisor::Connecting_Scanner(SDIDeviceInfo devInfo, InterruptEventCallBackProc AlertFunc)
{
    SDIScannerDriver_CreatePtr_(&driver, &devInfo, AlertFunc, nullptr);
    //std::cout << " Create OK " << std::endl;

    if(driver == nullptr){
        assert(false);
    }

    return SDIScannerDriver_OpenPtr_(driver);
    //std::cout << " Open OK " << std::endl;
}

void Supervisor::Disconnecting_Scanner(void)
{

    if(driver){
        SDIScannerDriver_ClosePtr_(driver);
        //std::cout << " Close OK " << std::endl;
    }
}

void Supervisor::Dispose_Scanner(void)
{
    if(driver){
        SDIScannerDriver_DisposePtr_(driver);
        //std::cout << "Dispose OK" << std::endl;
        driver = nullptr;
    }
}

void Supervisor::Cancel_Scanning(void)
{
    SDIScannerDriver_DoScanJobPtr_(driver, kSDIOperationTypeCancel);
}

void Supervisor::Continue_Scanning(void)
{
    SDIScannerDriver_DoScanJobPtr_(driver, kSDIOperationTypeNew);
}

bool Supervisor::Scan_Start(SDIOperationType operation_type)
{
    this->m_ExistsPage = false;
    outEventType = kSDITrasnferEventTypeImage;
    SDIError outError = kSDIErrorNone;

    this->Set_All_Value();

    {
        outError = SDIScannerDriver_DoScanJobPtr_(driver, operation_type);
    }
    return CheckScanningError(outError);
}

bool Supervisor::Scan_Terminate()
{
    SDIError outError = kSDIErrorNone;

    {
        outError = SDIScannerDriver_DoScanJobPtr_(driver, kSDIOperationTypeStopAFM);
    }
    outEventType = kSDITransferEventTypeComplete;

    if(outError != kSDIErrorNone){
        m_LastError = outError;
    }
    return m_LastError;
}


bool Supervisor::GetPaperLoadedStatus(void){
    SDIInt out     = (SDIInt)-1;
    SDIScannerDriver_GetValuePtr_(driver, kSDIPaperLoadedKey, kSDIValueTypeInt, &out, sizeof(out));

    return (bool)out;
}

bool Supervisor::GetUnscannebleScanParameterStatus(void){
    SDIInt out     = (SDIInt)0;
    SDIScannerDriver_GetValuePtr_(driver, kSDIUnscannebleScanParameterStatusAllKey, kSDIValueTypeInt, &out, sizeof(out));

    return (bool)out;
}

bool Supervisor::GetScannerPosition(void){
    SDIInt out     = (SDIInt)-1;
    SDIScannerDriver_GetValuePtr_(driver, kSDIScannerPositionKey, kSDIValueTypeInt, &out, sizeof(out));

    if(out == kSDIScannerPositionInvalid){
        return true;
    }else {
        return false;
    }
}

bool Supervisor::CheckNextTransferEvent(bool wait)
{
    return SDIScannerDriver_CheckNextTransferEventPtr_(driver, wait);
}

bool Supervisor::Scanning()
{
    SDIError outError = kSDIErrorNone;

    if(outEventType != kSDITransferEventTypeComplete)
    {
        SDIImage_CreatePtr_(&outImageData);
        SDIScannerDriver_GetNextTransferEventPtr_(driver,
                                                &outEventType, //kSDITrasnferEventTypeImage -> kSDITransferEventTypeComplete
                                                outImageData,
                                                &outError);
    }

    if (outEventType == kSDITransferEventTypeComplete)
    {
        SDIScannerDriver_CheckCautionStatusPtr_(driver);
    }

    return CheckScanningError(outError);
}

#include "devicelist.h"
#include "loadnetworksetting.h"

bool Supervisor::SANEManuNetfinder()
{
    DeviceList::select_manu_ip_address = -1;
    GetNetworkDevcicesFromIni(DeviceList::manu_network_device_list);
    return true;
}

bool Supervisor::DeviceInfoResolve(SDIDeviceInfo* devInfo)
{
    char *old_display_name = (char *)malloc(MAX_DISPLAYNAME+1);
    memcpy_s(old_display_name, MAX_DISPLAYNAME+1, devInfo->displayName, MAX_DISPLAYNAME);

    if(SDIDeviceInfo_ResolvePtr_(devInfo)){
        //手動追加ネットワーク
        auto tmp_device = DeviceList::manu_network_device_list.begin();
        int list_size = DeviceList::manu_network_device_list.size();
        int i = 0;
        while(i < list_size){
            if(strcmp(devInfo->ipAddress, (const char*)tmp_device->ipAddress) == 0 ){
                memcpy(tmp_device->displayName, devInfo->displayName, strlen(devInfo->displayName)+1);
                memcpy(tmp_device->modelID, devInfo->modelID, strlen(devInfo->modelID)+1);

                free(old_display_name);
                return true;
            }
            ++tmp_device;
            i++;
        }

        //自動検索ネットワーク
        tmp_device = DeviceList::device_list.begin();
        list_size = DeviceList::device_list.size();
        i = 0;
        while(i < list_size){
            //USBデバイスはスキップする
            if(tmp_device->ipAddress[0] != '\0'){
                if(strcmp(devInfo->ipAddress, (const char*)tmp_device->ipAddress) == 0 ){
                    //自動検索でみつかった機種を名前解決するとPIDになってしまうため元の名前に戻す
                    memcpy_s(tmp_device->displayName, 50, old_display_name, strlen(old_display_name)+1);
                    memcpy_s(tmp_device->modelID, 20, devInfo->modelID, strlen(devInfo->modelID)+1);

                    free(old_display_name);
                    return true;
                }
            }
            ++tmp_device;
            i++;
        }
    }
    free(old_display_name);
    return false;
}

bool Supervisor::CheckScanningError(SDIError outError)
{
    if(outEventType == /* kSDITransferEventTypeInterrupt*/ 10 ){
        //紙ジャムもしくは重送検知エラー時
        if(outError != kSDIErrorNone){
            m_LastError = outError;
            return false;
        }
    }else if(outEventType == kSDITransferEventTypeComplete){
        if(outError != kSDIErrorNone){
            m_LastError = outError;
            return false;
        }
    }else if(outEventType == kSDITransferEventTypeCancel){
        return false;
    }else {
        if(outError != kSDIErrorNone){
            m_LastError = outError;
            return false;
        }
    }
    m_LastError = kSDIErrorNone;
    return true;
}

SDIError Supervisor::CheckDeviceErrorStatus()
{
    SDIInt out     = (SDIInt)-1;
    SDIScannerDriver_GetValuePtr_(driver, kSDIErrorStatusKey, kSDIValueTypeInt, &out, sizeof(out));

    return (SDIError)out;
}

void Supervisor::Get_Image()
{
    unsigned int nSize = 100;
    this->output_path = (unsigned char*)::malloc(nSize);
    if(this->output_path){
        memset(this->output_path, 0, nSize);
    }

    if (outEventType == kSDITrasnferEventTypeImage)
    {
        SDIImage_GetPathPtr_(outImageData, (SDIChar*)this->output_path, 0);
    }
}

void Supervisor::Save_Path(void)
{
    this->m_DoneBlanckPage = this->CheckImageIsBlank();

    if(!SDIImage_CheckImageIsBlankPtr_(outImageData)){
        SaveImagePathList::save_image_path_list.push_back(this->output_path);
        this->m_ExistsPage = true;
    }
}

bool Supervisor::CheckImageIsBlank()
{
    return SDIImage_CheckImageIsBlankPtr_(outImageData);
}

void Supervisor::Dispose_Ptr(void)
{
    if(outImageData){
        SDIImage_DisposePtr_(outImageData);
    }
}

bool Supervisor::Save_Image2(void)
{
    //std::cout << "[INFO]Save_Image2" << std::endl;
    auto save_path = SaveImagePathList::save_image_path_list.begin();
    bool save_success = true;
    bool exactly_make_file = false;
    int pageStart = m_numPageStart;
    int pageEnd = m_numPageEnd;
    int fileNameCount = m_fileNumStartCount;

    //std::cout << "[INFO]pageStart : " << pageStart << std::endl;
    //std::cout << "[INFO]pageEnd : " << pageEnd << std::endl;
    //std::cout << "[INFO]fileNameCount : " << fileNameCount << std::endl;
    //std::cout << "[INFO]path list size : " << SaveImagePathList::save_image_path_list.size() << std::endl;
    
    if(pageStart > SaveImagePathList::save_image_path_list.size()){
        std::cout << "[ERROR]Parameter error" << std::endl;
        return false;
    }
    //page skip
    int count = 1;
    while(count < pageStart){
        //std::cout << "count = " << count << *save_path << std::endl;
        free(*save_path);
        *save_path = nullptr;

        ++save_path;
        count++;
    }
    int endcount = 0;
    if(pageEnd > SaveImagePathList::save_image_path_list.size()){
        pageEnd = SaveImagePathList::save_image_path_list.size();
    }
    if(pageEnd != 0){
        endcount = SaveImagePathList::save_image_path_list.size() - pageEnd;
    }
    int i = 0;
    int headPage = 0;
    if(pageStart != 0){
        i = pageStart - 1;
        headPage = i;
        if(pageEnd == 0){
            pageEnd = SaveImagePathList::save_image_path_list.size();
        }
    }
    while(i < SaveImagePathList::save_image_path_list.size() - endcount){
        SDIInt kSaveFormat   = device_data.ImageFormatSettings.ImageFormat.select;
        std::string new_filename;
        std::string ext = "jpg";

        m_ScanCounter++;
        //std::cout << "new_filename : " << new_filename << std::endl;
        if(!this->sendAppCallbackScanStatus(CBDT_STATUS_SCAN, STATUS_SCAN_PAGE_END, m_ScanCounter, 100)){
            //Cancel
            free(*save_path);
            *save_path = nullptr;
            scanning_status = CANCEL;
            save_success = false;
            goto BAIL;
        }

        if (outEventType == kSDITransferEventTypeComplete)
        {
            if (kSaveFormat == kSDIImageFormatJPEG)
            {
                ext = "jpg";
            }else if (kSaveFormat == ImageFormatColorJpegMonoRaw) //PDF
            {
                ext = "pdf";
                kSaveFormat = kSDIImageFormatPDF;
            }else if (kSaveFormat == kSDIImageFormatPNM && !device_data.ImageFormatSettings.MultiTiffEnabled.select)
            {
                ext = "pnm";
            }else if (kSaveFormat == kSDIImageFormatTIFF)
            {
                ext = "tiff";
            }else if (kSaveFormat == kSDIImageFormatPNG)
            {
                ext = "png";
            }else if (kSaveFormat == kSDIImageFormatPNM && device_data.ImageFormatSettings.MultiTiffEnabled.select) //Multi-TIFF
            {
                ext = "tiff";
                kSaveFormat = kSDIImageFormatTIFF;
            }

            std::string str_counter;
            //ファイル名カウンターあり
            if(device_data.FileName_Counter){
                fileNameCount++;
                //同一ファイル名が存在する場合カウントを増やして再度確認する
                while(ES_CMN_FUNCS::PATH::ES_IsExistFile((boost::format("%1%/%2%%4%.%3%") % device_data.SelectPath.c_str() % device_data.Prefix.c_str() % ext.c_str() % fileNameCount).str().c_str())){
                    fileNameCount++;
                }
                str_counter = std::to_string(fileNameCount);
            }else {
                //ファイル名カウンターなし
                if(fileNameCount == 0){
                    str_counter = "";
                }else {
                    str_counter = std::to_string(fileNameCount);
                }
                if(!device_data.FileName_OverWrite){
                    //同一ファイル名が存在する場合カウントを増やして再度確認する
                    while(ES_CMN_FUNCS::PATH::ES_IsExistFile((boost::format("%1%/%2%%4%.%3%") % device_data.SelectPath.c_str() % device_data.Prefix.c_str() % ext.c_str() % str_counter).str().c_str())){
                        fileNameCount++;
                        str_counter = std::to_string(fileNameCount);
                    }
                }
            }
            //std::cout << "str_counter : " << str_counter << std::endl;

            //PDF or Multi-Tiff scan
            if(device_data.ImageFormatSettings.ImageFormat.select == ImageFormatColorJpegMonoRaw ||
                    (device_data.ImageFormatSettings.ImageFormat.select == ImageFormatPNM && device_data.ImageFormatSettings.MultiTiffEnabled.select)){
                if(device_data.ImageFormatSettings.PDFImgFormatOption.AllPage || (device_data.ImageFormatSettings.ImageFormat.select == ImageFormatPNM && device_data.ImageFormatSettings.MultiTiffEnabled.select)){
                    if(i == headPage){
                        new_filename = (boost::format("%1%/%2%%4%.%3%") % device_data.SelectPath.c_str() % device_data.Prefix.c_str() % ext.c_str() % str_counter.c_str()).str().c_str();
                        SDIMultipage_StartPtr_(driver, new_filename.c_str(), (SDIImageFormat)kSaveFormat);
                    }
                    SDIMultipage_AddPtr_(driver, (const char*)*save_path);
                }else {
                    if(i == headPage || (i+1)%device_data.ImageFormatSettings.PDFImgFormatOption.page != 0 ||
                            device_data.ImageFormatSettings.PDFImgFormatOption.page == 1){
                        new_filename = (boost::format("%1%/%2%%4%.%3%") % device_data.SelectPath.c_str() % device_data.Prefix.c_str() % ext.c_str() % str_counter.c_str()).str().c_str();
                        SDIMultipage_StartPtr_(driver, new_filename.c_str(), (SDIImageFormat)kSaveFormat);
                    }
                    SDIMultipage_AddPtr_(driver, (const char*)*save_path);
                    if((i+1)%device_data.ImageFormatSettings.PDFImgFormatOption.page == 0){
                       SDIMultipage_FinPtr_(driver);
                       exactly_make_file = true;
                    }else if(i+1 == SaveImagePathList::save_image_path_list.size()){
                        //PDFページ区切りが悪い時の処理
                        SDIMultipage_FinPtr_(driver);
                        exactly_make_file = true;
                    }
                }
            }else {
            //Other format
                new_filename = (boost::format("%1%/%2%%4%.%3%") % device_data.SelectPath.c_str() % device_data.Prefix.c_str() % ext.c_str() % str_counter.c_str()).str().c_str();
                if (rename((const char*)*save_path, new_filename.c_str()) != 0)
                {
                    // try again using mv command
                    int result = system((boost::format("mv %1% %2%") % (const char*)*save_path %  new_filename.c_str()).str().c_str());
                    //systemが異常終了した場合(画像の移動に失敗した)
                    if(result != 0){
                        m_LastError = kSDIErrorWriteFile;
                        save_success = false;
                        goto BAIL;
                    }
                }
            }
            this->sendAppCallbackImageFile(CBDT_IMAGE_FILE, STATUS_IMAGE_FILE_INFO, new_filename.c_str());
        }

        free(*save_path);
        *save_path = nullptr;

        ++save_path;
        i++;
    }
    //PDF or Multi-Tiff scan 結合処理
    if(!exactly_make_file){
        if(SaveImagePathList::save_image_path_list.size() != 0){
            if(device_data.ImageFormatSettings.ImageFormat.select == ImageFormatColorJpegMonoRaw ||
               (device_data.ImageFormatSettings.ImageFormat.select == ImageFormatPNM && device_data.ImageFormatSettings.MultiTiffEnabled.select)){
                if(!SDIMultipage_FinPtr_(driver)){
                    m_LastError = kSDIErrorWriteFile;
                    save_success = false;
                }
            }
        }else {
            save_success = false;
        }
    }

BAIL:
    SaveImagePathList::save_image_path_list.clear();
    this->output_path = nullptr;
    return save_success;
}

#define CHECK_ERROR(err) (err == 0) ? true : throw(false)

bool Supervisor::Get_All_Value(void)
{
    if(!driver){
        return false;
    }
    try{
        // functionalunit
        {
            SDIFunctionalUnit out     = (SDIFunctionalUnit)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIFunctionalUnitKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.FunctionalUnit.select = (SDIInt)out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIFunctionalUnitKey, &capability));

            device_data.FunctionalUnit.capability.version = capability.version;
            device_data.FunctionalUnit.capability.supportLevel = capability.supportLevel;
            device_data.FunctionalUnit.capability.capabilityType = capability.capabilityType;

            device_data.FunctionalUnit.capability.minValue = capability.minValue;
            device_data.FunctionalUnit.capability.maxValue = capability.maxValue;

            device_data.FunctionalUnit.capability.allMinValue = capability.allMinValue;
            device_data.FunctionalUnit.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.FunctionalUnit.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.FunctionalUnit.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.FunctionalUnit.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.FunctionalUnit.capability.countOfAllList = capability.countOfAllList;
        }

        // GuidePosition
        {
            SDIGuidePosition out     = (SDIGuidePosition)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIGuidePositionKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.GuidePosition.select = (SDIInt)out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIGuidePositionKey, &capability));

            device_data.GuidePosition.capability.version = capability.version;
            device_data.GuidePosition.capability.supportLevel = capability.supportLevel;
            device_data.GuidePosition.capability.capabilityType = capability.capabilityType;

            device_data.GuidePosition.capability.minValue = capability.minValue;
            device_data.GuidePosition.capability.maxValue = capability.maxValue;

            device_data.GuidePosition.capability.allMinValue = capability.allMinValue;
            device_data.GuidePosition.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.GuidePosition.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.GuidePosition.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.GuidePosition.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.GuidePosition.capability.countOfAllList = capability.countOfAllList;
        }

        // ColorType
        {
            SDIColorType out     = (SDIColorType)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIColorTypeKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ColorType.select = (SDIInt)out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIColorTypeKey, &capability));

            device_data.ColorType.capability.version = capability.version;
            device_data.ColorType.capability.supportLevel = capability.supportLevel;
            device_data.ColorType.capability.capabilityType = capability.capabilityType;

            device_data.ColorType.capability.minValue = capability.minValue;
            device_data.ColorType.capability.maxValue = capability.maxValue;

            device_data.ColorType.capability.allMinValue = capability.allMinValue;
            device_data.ColorType.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ColorType.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ColorType.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ColorType.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ColorType.capability.countOfAllList = capability.countOfAllList;
        }

        // Resolution
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIResolutionKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.Resolution.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIResolutionKey, &capability));

            device_data.Resolution.capability.version = capability.version;
            device_data.Resolution.capability.supportLevel = capability.supportLevel;
            device_data.Resolution.capability.capabilityType = capability.capabilityType;

            device_data.Resolution.capability.minValue = capability.minValue;
            device_data.Resolution.capability.maxValue = capability.maxValue;

            device_data.Resolution.capability.allMinValue = capability.allMinValue;
            device_data.Resolution.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.Resolution.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.Resolution.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.Resolution.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.Resolution.capability.countOfAllList = capability.countOfAllList;
        }

        // ScanAreaWidth
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIScanAreaWidthKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ScanArea.ScanAreaWidth.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIScanAreaWidthKey, &capability));

            device_data.ScanArea.ScanAreaWidth.capability.version = capability.version;
            device_data.ScanArea.ScanAreaWidth.capability.supportLevel = capability.supportLevel;
            device_data.ScanArea.ScanAreaWidth.capability.capabilityType = capability.capabilityType;

            device_data.ScanArea.ScanAreaWidth.capability.minValue = capability.minValue;
            device_data.ScanArea.ScanAreaWidth.capability.maxValue = capability.maxValue;

            device_data.ScanArea.ScanAreaWidth.capability.allMinValue = capability.allMinValue;
            device_data.ScanArea.ScanAreaWidth.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ScanArea.ScanAreaWidth.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ScanArea.ScanAreaWidth.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ScanArea.ScanAreaWidth.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ScanArea.ScanAreaWidth.capability.countOfAllList = capability.countOfAllList;
        }

        // ScanAreaHeight
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIScanAreaHeightKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ScanArea.ScanAreaHeight.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIScanAreaHeightKey, &capability));

            device_data.ScanArea.ScanAreaHeight.capability.version = capability.version;
            device_data.ScanArea.ScanAreaHeight.capability.supportLevel = capability.supportLevel;
            device_data.ScanArea.ScanAreaHeight.capability.capabilityType = capability.capabilityType;

            device_data.ScanArea.ScanAreaHeight.capability.minValue = capability.minValue;
            device_data.ScanArea.ScanAreaHeight.capability.maxValue = capability.maxValue;

            device_data.ScanArea.ScanAreaHeight.capability.allMinValue = capability.allMinValue;
            device_data.ScanArea.ScanAreaHeight.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ScanArea.ScanAreaHeight.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ScanArea.ScanAreaHeight.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ScanArea.ScanAreaHeight.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ScanArea.ScanAreaHeight.capability.countOfAllList = capability.countOfAllList;
        }

        // ScanAreaOffsetX
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIScanAreaOffsetXKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ScanArea.ScanAreaOffsetX.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIScanAreaOffsetXKey, &capability));

            device_data.ScanArea.ScanAreaOffsetX.capability.version = capability.version;
            device_data.ScanArea.ScanAreaOffsetX.capability.supportLevel = capability.supportLevel;
            device_data.ScanArea.ScanAreaOffsetX.capability.capabilityType = capability.capabilityType;

            device_data.ScanArea.ScanAreaOffsetX.capability.minValue = capability.minValue;
            device_data.ScanArea.ScanAreaOffsetX.capability.maxValue = capability.maxValue;

            device_data.ScanArea.ScanAreaOffsetX.capability.allMinValue = capability.allMinValue;
            device_data.ScanArea.ScanAreaOffsetX.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ScanArea.ScanAreaOffsetX.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ScanArea.ScanAreaOffsetX.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ScanArea.ScanAreaOffsetX.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ScanArea.ScanAreaOffsetX.capability.countOfAllList = capability.countOfAllList;
        }

        // ScanAreaOffsetY
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIScanAreaOffsetYKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ScanArea.ScanAreaOffsetY.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIScanAreaOffsetYKey, &capability));

            device_data.ScanArea.ScanAreaOffsetY.capability.version = capability.version;
            device_data.ScanArea.ScanAreaOffsetY.capability.supportLevel = capability.supportLevel;
            device_data.ScanArea.ScanAreaOffsetY.capability.capabilityType = capability.capabilityType;

            device_data.ScanArea.ScanAreaOffsetY.capability.minValue = capability.minValue;
            device_data.ScanArea.ScanAreaOffsetY.capability.maxValue = capability.maxValue;

            device_data.ScanArea.ScanAreaOffsetY.capability.allMinValue = capability.allMinValue;
            device_data.ScanArea.ScanAreaOffsetY.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ScanArea.ScanAreaOffsetY.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ScanArea.ScanAreaOffsetY.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ScanArea.ScanAreaOffsetY.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ScanArea.ScanAreaOffsetY.capability.countOfAllList = capability.countOfAllList;
        }

        // MaxScanAreaWidth
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIMaxScanAreaWidthKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ScanArea.MaxScanAreaWidth.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIMaxScanAreaWidthKey, &capability));

            device_data.ScanArea.MaxScanAreaWidth.capability.version = capability.version;
            device_data.ScanArea.MaxScanAreaWidth.capability.supportLevel = capability.supportLevel;
            device_data.ScanArea.MaxScanAreaWidth.capability.capabilityType = capability.capabilityType;

            device_data.ScanArea.MaxScanAreaWidth.capability.minValue = capability.minValue;
            device_data.ScanArea.MaxScanAreaWidth.capability.maxValue = capability.maxValue;

            device_data.ScanArea.MaxScanAreaWidth.capability.allMinValue = capability.allMinValue;
            device_data.ScanArea.MaxScanAreaWidth.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ScanArea.MaxScanAreaWidth.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ScanArea.MaxScanAreaWidth.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ScanArea.MaxScanAreaWidth.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ScanArea.MaxScanAreaWidth.capability.countOfAllList = capability.countOfAllList;
        }

        // MaxScanAreaHeight
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIMaxScanAreaHeightKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ScanArea.MaxScanAreaHeight.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIMaxScanAreaHeightKey, &capability));

            device_data.ScanArea.MaxScanAreaHeight.capability.version = capability.version;
            device_data.ScanArea.MaxScanAreaHeight.capability.supportLevel = capability.supportLevel;
            device_data.ScanArea.MaxScanAreaHeight.capability.capabilityType = capability.capabilityType;

            device_data.ScanArea.MaxScanAreaHeight.capability.minValue = capability.minValue;
            device_data.ScanArea.MaxScanAreaHeight.capability.maxValue = capability.maxValue;

            device_data.ScanArea.MaxScanAreaHeight.capability.allMinValue = capability.allMinValue;
            device_data.ScanArea.MaxScanAreaHeight.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ScanArea.MaxScanAreaHeight.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ScanArea.MaxScanAreaHeight.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ScanArea.MaxScanAreaHeight.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ScanArea.MaxScanAreaHeight.capability.countOfAllList = capability.countOfAllList;
        }

        // MaxScanAreaHeightInLongPaper
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIMaxScanAreaHeightInLongPaperKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ScanArea.MaxScanAreaHeightInLongPaper.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIMaxScanAreaHeightInLongPaperKey, &capability));

            device_data.ScanArea.MaxScanAreaHeightInLongPaper.capability.version = capability.version;
            device_data.ScanArea.MaxScanAreaHeightInLongPaper.capability.supportLevel = capability.supportLevel;
            device_data.ScanArea.MaxScanAreaHeightInLongPaper.capability.capabilityType = capability.capabilityType;

            device_data.ScanArea.MaxScanAreaHeightInLongPaper.capability.minValue = capability.minValue;
            device_data.ScanArea.MaxScanAreaHeightInLongPaper.capability.maxValue = capability.maxValue;

            device_data.ScanArea.MaxScanAreaHeightInLongPaper.capability.allMinValue = capability.allMinValue;
            device_data.ScanArea.MaxScanAreaHeightInLongPaper.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ScanArea.MaxScanAreaHeightInLongPaper.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ScanArea.MaxScanAreaHeightInLongPaper.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ScanArea.MaxScanAreaHeightInLongPaper.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ScanArea.MaxScanAreaHeightInLongPaper.capability.countOfAllList = capability.countOfAllList;
        }

       // Max Scan Area Height in long paper table (JSON)
        {
            int kMaxJsonSize = 1000;
            std::unique_ptr<char[]>  out(new char[kMaxJsonSize]);
            memset((void*)out.get(), 0, kMaxJsonSize);
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIMaxScanAreaHeightInLongPaperResolutionTableKey, kSDIValueTypeString, (void*)out.get(), kMaxJsonSize));

            device_data.MaxScanArea_height_table = "";
            device_data.MaxScanArea_height_table = out.get();
        }

        // ADFHeightMin
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIADFHeightMinKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ScanArea.ADFHeightMin.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIADFHeightMinKey, &capability));

            device_data.ScanArea.ADFHeightMin.capability.version = capability.version;
            device_data.ScanArea.ADFHeightMin.capability.supportLevel = capability.supportLevel;
            device_data.ScanArea.ADFHeightMin.capability.capabilityType = capability.capabilityType;

            device_data.ScanArea.ADFHeightMin.capability.minValue = capability.minValue;
            device_data.ScanArea.ADFHeightMin.capability.maxValue = capability.maxValue;

            device_data.ScanArea.ADFHeightMin.capability.allMinValue = capability.allMinValue;
            device_data.ScanArea.ADFHeightMin.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ScanArea.ADFHeightMin.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ScanArea.ADFHeightMin.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ScanArea.ADFHeightMin.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ScanArea.ADFHeightMin.capability.countOfAllList = capability.countOfAllList;
        }

        // ADFDuplexHeightMin
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIADFDuplexHeightMinKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ScanArea.ADFDuplexHeightMin.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIADFDuplexHeightMinKey, &capability));

            device_data.ScanArea.ADFDuplexHeightMin.capability.version = capability.version;
            device_data.ScanArea.ADFDuplexHeightMin.capability.supportLevel = capability.supportLevel;
            device_data.ScanArea.ADFDuplexHeightMin.capability.capabilityType = capability.capabilityType;

            device_data.ScanArea.ADFDuplexHeightMin.capability.minValue = capability.minValue;
            device_data.ScanArea.ADFDuplexHeightMin.capability.maxValue = capability.maxValue;

            device_data.ScanArea.ADFDuplexHeightMin.capability.allMinValue = capability.allMinValue;
            device_data.ScanArea.ADFDuplexHeightMin.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ScanArea.ADFDuplexHeightMin.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ScanArea.ADFDuplexHeightMin.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ScanArea.ADFDuplexHeightMin.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ScanArea.ADFDuplexHeightMin.capability.countOfAllList = capability.countOfAllList;
        }

        // ADFWidthMin
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIADFWidthMinKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ScanArea.ADFWidthMin.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIADFWidthMinKey, &capability));

            device_data.ScanArea.ADFWidthMin.capability.version = capability.version;
            device_data.ScanArea.ADFWidthMin.capability.supportLevel = capability.supportLevel;
            device_data.ScanArea.ADFWidthMin.capability.capabilityType = capability.capabilityType;

            device_data.ScanArea.ADFWidthMin.capability.minValue = capability.minValue;
            device_data.ScanArea.ADFWidthMin.capability.maxValue = capability.maxValue;

            device_data.ScanArea.ADFWidthMin.capability.allMinValue = capability.allMinValue;
            device_data.ScanArea.ADFWidthMin.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ScanArea.ADFWidthMin.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ScanArea.ADFWidthMin.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ScanArea.ADFWidthMin.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ScanArea.ADFWidthMin.capability.countOfAllList = capability.countOfAllList;
        }

        // DropoutColor
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIDropoutColorKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.DropoutColor.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIDropoutColorKey, &capability));

            device_data.DropoutColor.capability.version = capability.version;
            device_data.DropoutColor.capability.supportLevel = capability.supportLevel;
            device_data.DropoutColor.capability.capabilityType = capability.capabilityType;

            device_data.DropoutColor.capability.minValue = capability.minValue;
            device_data.DropoutColor.capability.maxValue = capability.maxValue;

            device_data.DropoutColor.capability.allMinValue = capability.allMinValue;
            device_data.DropoutColor.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.DropoutColor.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.DropoutColor.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.DropoutColor.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.DropoutColor.capability.countOfAllList = capability.countOfAllList;
        }

        // Orientation
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIOrientationKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.Orientation.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIOrientationKey, &capability));

            device_data.Orientation.capability.version = capability.version;
            device_data.Orientation.capability.supportLevel = capability.supportLevel;
            device_data.Orientation.capability.capabilityType = capability.capabilityType;

            device_data.Orientation.capability.minValue = capability.minValue;
            device_data.Orientation.capability.maxValue = capability.maxValue;

            device_data.Orientation.capability.allMinValue = capability.allMinValue;
            device_data.Orientation.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.Orientation.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.Orientation.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.Orientation.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.Orientation.capability.countOfAllList = capability.countOfAllList;
        }

        // Gamma
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIGammaKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.Gamma.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIGammaKey, &capability));

            device_data.Gamma.capability.version = capability.version;
            device_data.Gamma.capability.supportLevel = capability.supportLevel;
            device_data.Gamma.capability.capabilityType = capability.capabilityType;

            device_data.Gamma.capability.minValue = capability.minValue;
            device_data.Gamma.capability.maxValue = capability.maxValue;

            device_data.Gamma.capability.allMinValue = capability.allMinValue;
            device_data.Gamma.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.Gamma.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.Gamma.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.Gamma.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.Gamma.capability.countOfAllList = capability.countOfAllList;
        }

        // ImageFormat
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIImageFormatKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ImageFormatSettings.ImageFormat.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIImageFormatKey, &capability));

            device_data.ImageFormatSettings.ImageFormat.capability.version = capability.version;
            device_data.ImageFormatSettings.ImageFormat.capability.supportLevel = capability.supportLevel;
            device_data.ImageFormatSettings.ImageFormat.capability.capabilityType = capability.capabilityType;

            device_data.ImageFormatSettings.ImageFormat.capability.minValue = capability.minValue;
            device_data.ImageFormatSettings.ImageFormat.capability.maxValue = capability.maxValue;

            device_data.ImageFormatSettings.ImageFormat.capability.allMinValue = capability.allMinValue;
            device_data.ImageFormatSettings.ImageFormat.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ImageFormatSettings.ImageFormat.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ImageFormatSettings.ImageFormat.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ImageFormatSettings.ImageFormat.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ImageFormatSettings.ImageFormat.capability.countOfAllList = capability.countOfAllList;
        }

        // MultiTiffEnabled
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIMultiTiffEnabledKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ImageFormatSettings.MultiTiffEnabled.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIMultiTiffEnabledKey, &capability));

            device_data.ImageFormatSettings.MultiTiffEnabled.capability.version = capability.version;
            device_data.ImageFormatSettings.MultiTiffEnabled.capability.supportLevel = capability.supportLevel;
            device_data.ImageFormatSettings.MultiTiffEnabled.capability.capabilityType = capability.capabilityType;

            device_data.ImageFormatSettings.MultiTiffEnabled.capability.minValue = capability.minValue;
            device_data.ImageFormatSettings.MultiTiffEnabled.capability.maxValue = capability.maxValue;

            device_data.ImageFormatSettings.MultiTiffEnabled.capability.allMinValue = capability.allMinValue;
            device_data.ImageFormatSettings.MultiTiffEnabled.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ImageFormatSettings.MultiTiffEnabled.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ImageFormatSettings.MultiTiffEnabled.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ImageFormatSettings.MultiTiffEnabled.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ImageFormatSettings.MultiTiffEnabled.capability.countOfAllList = capability.countOfAllList;
        }

        // TiffCompression
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDITiffCompressionKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ImageFormatSettings.TiffCompression.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDITiffCompressionKey, &capability));

            device_data.ImageFormatSettings.TiffCompression.capability.version = capability.version;
            device_data.ImageFormatSettings.TiffCompression.capability.supportLevel = capability.supportLevel;
            device_data.ImageFormatSettings.TiffCompression.capability.capabilityType = capability.capabilityType;

            device_data.ImageFormatSettings.TiffCompression.capability.minValue = capability.minValue;
            device_data.ImageFormatSettings.TiffCompression.capability.maxValue = capability.maxValue;

            device_data.ImageFormatSettings.TiffCompression.capability.allMinValue = capability.allMinValue;
            device_data.ImageFormatSettings.TiffCompression.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ImageFormatSettings.TiffCompression.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ImageFormatSettings.TiffCompression.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ImageFormatSettings.TiffCompression.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ImageFormatSettings.TiffCompression.capability.countOfAllList = capability.countOfAllList;
        }

        // Threshold
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIThresholdKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.Threshold.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIThresholdKey, &capability));

            device_data.Threshold.capability.version = capability.version;
            device_data.Threshold.capability.supportLevel = capability.supportLevel;
            device_data.Threshold.capability.capabilityType = capability.capabilityType;

            device_data.Threshold.capability.minValue = capability.minValue;
            device_data.Threshold.capability.maxValue = capability.maxValue;

            device_data.Threshold.capability.allMinValue = capability.allMinValue;
            device_data.Threshold.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.Threshold.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.Threshold.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.Threshold.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.Threshold.capability.countOfAllList = capability.countOfAllList;
        }

        // Brightness
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIBrightnessKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.Brightness.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIBrightnessKey, &capability));

            device_data.Brightness.capability.version = capability.version;
            device_data.Brightness.capability.supportLevel = capability.supportLevel;
            device_data.Brightness.capability.capabilityType = capability.capabilityType;

            device_data.Brightness.capability.minValue = capability.minValue;
            device_data.Brightness.capability.maxValue = capability.maxValue;

            device_data.Brightness.capability.allMinValue = capability.allMinValue;
            device_data.Brightness.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.Brightness.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.Brightness.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.Brightness.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.Brightness.capability.countOfAllList = capability.countOfAllList;
        }

        // Contrast
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIContrastKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.Contrast.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIContrastKey, &capability));

            device_data.Contrast.capability.version = capability.version;
            device_data.Contrast.capability.supportLevel = capability.supportLevel;
            device_data.Contrast.capability.capabilityType = capability.capabilityType;

            device_data.Contrast.capability.minValue = capability.minValue;
            device_data.Contrast.capability.maxValue = capability.maxValue;

            device_data.Contrast.capability.allMinValue = capability.allMinValue;
            device_data.Contrast.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.Contrast.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.Contrast.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.Contrast.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.Contrast.capability.countOfAllList = capability.countOfAllList;
        }

        // JpegQuality
        /*{
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIJpegQuality, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ImageFormatSettings.JpegQuality.select = out;
            device_data.ImageFormatSettings.JpegQuality_forJPEG = out;
            device_data.ImageFormatSettings.JpegQuality_forPDF = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIJpegQuality, &capability));

            device_data.ImageFormatSettings.JpegQuality.capability.version = capability.version;
            device_data.ImageFormatSettings.JpegQuality.capability.supportLevel = capability.supportLevel;
            device_data.ImageFormatSettings.JpegQuality.capability.capabilityType = capability.capabilityType;

            device_data.ImageFormatSettings.JpegQuality.capability.minValue = capability.minValue;
            device_data.ImageFormatSettings.JpegQuality.capability.maxValue = capability.maxValue;

            device_data.ImageFormatSettings.JpegQuality.capability.allMinValue = capability.allMinValue;
            device_data.ImageFormatSettings.JpegQuality.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ImageFormatSettings.JpegQuality.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ImageFormatSettings.JpegQuality.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ImageFormatSettings.JpegQuality.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ImageFormatSettings.JpegQuality.capability.countOfAllList = capability.countOfAllList;
        }*/

        // JpegProgressive
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIJpegProgressiveKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ImageFormatSettings.JpegProgressive.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIJpegProgressiveKey, &capability));

            device_data.ImageFormatSettings.JpegProgressive.capability.version = capability.version;
            device_data.ImageFormatSettings.JpegProgressive.capability.supportLevel = capability.supportLevel;
            device_data.ImageFormatSettings.JpegProgressive.capability.capabilityType = capability.capabilityType;

            device_data.ImageFormatSettings.JpegProgressive.capability.minValue = capability.minValue;
            device_data.ImageFormatSettings.JpegProgressive.capability.maxValue = capability.maxValue;

            device_data.ImageFormatSettings.JpegProgressive.capability.allMinValue = capability.allMinValue;
            device_data.ImageFormatSettings.JpegProgressive.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ImageFormatSettings.JpegProgressive.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ImageFormatSettings.JpegProgressive.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ImageFormatSettings.JpegProgressive.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ImageFormatSettings.JpegProgressive.capability.countOfAllList = capability.countOfAllList;
        }

        // DuplexType
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIDuplexTypeKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.DuplexType.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIDuplexTypeKey, &capability));

            device_data.DuplexType.capability.version = capability.version;
            device_data.DuplexType.capability.supportLevel = capability.supportLevel;
            device_data.DuplexType.capability.capabilityType = capability.capabilityType;

            device_data.DuplexType.capability.minValue = capability.minValue;
            device_data.DuplexType.capability.maxValue = capability.maxValue;

            device_data.DuplexType.capability.allMinValue = capability.allMinValue;
            device_data.DuplexType.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.DuplexType.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.DuplexType.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.DuplexType.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.DuplexType.capability.countOfAllList = capability.countOfAllList;
        }

        // PaperLoaded
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIPaperLoadedKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.PaperLoaded.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIPaperLoadedKey, &capability));

            device_data.PaperLoaded.capability.version = capability.version;
            device_data.PaperLoaded.capability.supportLevel = capability.supportLevel;
            device_data.PaperLoaded.capability.capabilityType = capability.capabilityType;

            device_data.PaperLoaded.capability.minValue = capability.minValue;
            device_data.PaperLoaded.capability.maxValue = capability.maxValue;

            device_data.PaperLoaded.capability.allMinValue = capability.allMinValue;
            device_data.PaperLoaded.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.PaperLoaded.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.PaperLoaded.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.PaperLoaded.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.PaperLoaded.capability.countOfAllList = capability.countOfAllList;
        }

        // DoubleFeedDetection
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIDoubleFeedDetectionKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.DoubleFeedDetectionValue.DoubleFeedDetection.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIDoubleFeedDetectionKey, &capability));

            device_data.DoubleFeedDetectionValue.DoubleFeedDetection.capability.version = capability.version;
            device_data.DoubleFeedDetectionValue.DoubleFeedDetection.capability.supportLevel = capability.supportLevel;
            device_data.DoubleFeedDetectionValue.DoubleFeedDetection.capability.capabilityType = capability.capabilityType;

            device_data.DoubleFeedDetectionValue.DoubleFeedDetection.capability.minValue = capability.minValue;
            device_data.DoubleFeedDetectionValue.DoubleFeedDetection.capability.maxValue = capability.maxValue;

            device_data.DoubleFeedDetectionValue.DoubleFeedDetection.capability.allMinValue = capability.allMinValue;
            device_data.DoubleFeedDetectionValue.DoubleFeedDetection.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.DoubleFeedDetectionValue.DoubleFeedDetection.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.DoubleFeedDetectionValue.DoubleFeedDetection.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.DoubleFeedDetectionValue.DoubleFeedDetection.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.DoubleFeedDetectionValue.DoubleFeedDetection.capability.countOfAllList = capability.countOfAllList;
        }

        // DoubleFeedDetectionLevel
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIDoubleFeedDetectionLevelKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIDoubleFeedDetectionLevelKey, &capability));

            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel.capability.version = capability.version;
            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel.capability.supportLevel = capability.supportLevel;
            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel.capability.capabilityType = capability.capabilityType;

            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel.capability.minValue = capability.minValue;
            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel.capability.maxValue = capability.maxValue;

            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel.capability.allMinValue = capability.allMinValue;
            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel.capability.countOfAllList = capability.countOfAllList;
        }

        // DoubleFeedDetectionAreaMin
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIDoubleFeedDetectionAreaMinKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaMin.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIDoubleFeedDetectionAreaMinKey, &capability));

            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaMin.capability.version = capability.version;
            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaMin.capability.supportLevel = capability.supportLevel;
            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaMin.capability.capabilityType = capability.capabilityType;

            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaMin.capability.minValue = capability.minValue;
            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaMin.capability.maxValue = capability.maxValue;

            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaMin.capability.allMinValue = capability.allMinValue;
            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaMin.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaMin.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaMin.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaMin.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaMin.capability.countOfAllList = capability.countOfAllList;
        }

        // DoubleFeedDetectionAreaLength
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIDoubleFeedDetectionAreaLengthKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaLength.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIDoubleFeedDetectionAreaLengthKey, &capability));

            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaLength.capability.version = capability.version;
            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaLength.capability.supportLevel = capability.supportLevel;
            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaLength.capability.capabilityType = capability.capabilityType;

            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaLength.capability.minValue = capability.minValue;
            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaLength.capability.maxValue = capability.maxValue;

            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaLength.capability.allMinValue = capability.allMinValue;
            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaLength.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaLength.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaLength.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaLength.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaLength.capability.countOfAllList = capability.countOfAllList;
        }

        // BlankPageSkip
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIBlankPageSkipKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.BlankPageSkipKey.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIBlankPageSkipKey, &capability));

            device_data.BlankPageSkipKey.capability.version = capability.version;
            device_data.BlankPageSkipKey.capability.supportLevel = capability.supportLevel;
            device_data.BlankPageSkipKey.capability.capabilityType = capability.capabilityType;

            device_data.BlankPageSkipKey.capability.minValue = capability.minValue;
            device_data.BlankPageSkipKey.capability.maxValue = capability.maxValue;

            device_data.BlankPageSkipKey.capability.allMinValue = capability.allMinValue;
            device_data.BlankPageSkipKey.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.BlankPageSkipKey.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.BlankPageSkipKey.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.BlankPageSkipKey.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.BlankPageSkipKey.capability.countOfAllList = capability.countOfAllList;
        }

        // BlankPageSkipLevelKey
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIBlankPageSkipLevelKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.BlankPageSkipLevelKey.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIBlankPageSkipLevelKey, &capability));

            device_data.BlankPageSkipLevelKey.capability.version = capability.version;
            device_data.BlankPageSkipLevelKey.capability.supportLevel = capability.supportLevel;
            device_data.BlankPageSkipLevelKey.capability.capabilityType = capability.capabilityType;

            device_data.BlankPageSkipLevelKey.capability.minValue = capability.minValue;
            device_data.BlankPageSkipLevelKey.capability.maxValue = capability.maxValue;

            device_data.BlankPageSkipLevelKey.capability.allMinValue = capability.allMinValue;
            device_data.BlankPageSkipLevelKey.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.BlankPageSkipLevelKey.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.BlankPageSkipLevelKey.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.BlankPageSkipLevelKey.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.BlankPageSkipLevelKey.capability.countOfAllList = capability.countOfAllList;
        }

        // PaperDeskew
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIPaperDeskewKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.PaperDeskew.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIPaperDeskewKey, &capability));

            device_data.PaperDeskew.capability.version = capability.version;
            device_data.PaperDeskew.capability.supportLevel = capability.supportLevel;
            device_data.PaperDeskew.capability.capabilityType = capability.capabilityType;

            device_data.PaperDeskew.capability.minValue = capability.minValue;
            device_data.PaperDeskew.capability.maxValue = capability.maxValue;

            device_data.PaperDeskew.capability.allMinValue = capability.allMinValue;
            device_data.PaperDeskew.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.PaperDeskew.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.PaperDeskew.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.PaperDeskew.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.PaperDeskew.capability.countOfAllList = capability.countOfAllList;
        }

        // BackgroundRemoval
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIBackgroundRemovalKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.BackgroundRemoval.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIBackgroundRemovalKey, &capability));

            device_data.BackgroundRemoval.capability.version = capability.version;
            device_data.BackgroundRemoval.capability.supportLevel = capability.supportLevel;
            device_data.BackgroundRemoval.capability.capabilityType = capability.capabilityType;

            device_data.BackgroundRemoval.capability.minValue = capability.minValue;
            device_data.BackgroundRemoval.capability.maxValue = capability.maxValue;

            device_data.BackgroundRemoval.capability.allMinValue = capability.allMinValue;
            device_data.BackgroundRemoval.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.BackgroundRemoval.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.BackgroundRemoval.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.BackgroundRemoval.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.BackgroundRemoval.capability.countOfAllList = capability.countOfAllList;
        }

        // AutoSize
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIAutoSizeKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ScanArea.AutoSize.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIAutoSizeKey, &capability));

            device_data.ScanArea.AutoSize.capability.version = capability.version;
            device_data.ScanArea.AutoSize.capability.supportLevel = capability.supportLevel;
            device_data.ScanArea.AutoSize.capability.capabilityType = capability.capabilityType;

            device_data.ScanArea.AutoSize.capability.minValue = capability.minValue;
            device_data.ScanArea.AutoSize.capability.maxValue = capability.maxValue;

            device_data.ScanArea.AutoSize.capability.allMinValue = capability.allMinValue;
            device_data.ScanArea.AutoSize.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ScanArea.AutoSize.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ScanArea.AutoSize.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ScanArea.AutoSize.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ScanArea.AutoSize.capability.countOfAllList = capability.countOfAllList;
        }

        // PaperEndDetection
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIPaperEndDetectionKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ScanArea.PaperEndDetection.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIPaperEndDetectionKey, &capability));

            device_data.ScanArea.PaperEndDetection.capability.version = capability.version;
            device_data.ScanArea.PaperEndDetection.capability.supportLevel = capability.supportLevel;
            device_data.ScanArea.PaperEndDetection.capability.capabilityType = capability.capabilityType;

            device_data.ScanArea.PaperEndDetection.capability.minValue = capability.minValue;
            device_data.ScanArea.PaperEndDetection.capability.maxValue = capability.maxValue;

            device_data.ScanArea.PaperEndDetection.capability.allMinValue = capability.allMinValue;
            device_data.ScanArea.PaperEndDetection.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ScanArea.PaperEndDetection.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ScanArea.PaperEndDetection.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ScanArea.PaperEndDetection.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ScanArea.PaperEndDetection.capability.countOfAllList = capability.countOfAllList;
        }

        // TransferCompression
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDITransferCompressionKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.TransferCompression.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDITransferCompressionKey, &capability));

            device_data.TransferCompression.capability.version = capability.version;
            device_data.TransferCompression.capability.supportLevel = capability.supportLevel;
            device_data.TransferCompression.capability.capabilityType = capability.capabilityType;

            device_data.TransferCompression.capability.minValue = capability.minValue;
            device_data.TransferCompression.capability.maxValue = capability.maxValue;

            device_data.TransferCompression.capability.allMinValue = capability.allMinValue;
            device_data.TransferCompression.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.TransferCompression.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.TransferCompression.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.TransferCompression.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.TransferCompression.capability.countOfAllList = capability.countOfAllList;
        }

        // SleepTime
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDISleepTimeKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.SleepTime.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDISleepTimeKey, &capability));

            device_data.SleepTime.capability.version = capability.version;
            device_data.SleepTime.capability.supportLevel = capability.supportLevel;
            device_data.SleepTime.capability.capabilityType = capability.capabilityType;

            device_data.SleepTime.capability.minValue = capability.minValue;
            device_data.SleepTime.capability.maxValue = capability.maxValue;

            device_data.SleepTime.capability.allMinValue = capability.allMinValue;
            device_data.SleepTime.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.SleepTime.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.SleepTime.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.SleepTime.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.SleepTime.capability.countOfAllList = capability.countOfAllList;
        }

        // AutoPowerOffTime
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIAutoPowerOffTimeKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.AutoPowerOffTime.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIAutoPowerOffTimeKey, &capability));

            device_data.AutoPowerOffTime.capability.version = capability.version;
            device_data.AutoPowerOffTime.capability.supportLevel = capability.supportLevel;
            device_data.AutoPowerOffTime.capability.capabilityType = capability.capabilityType;

            device_data.AutoPowerOffTime.capability.minValue = capability.minValue;
            device_data.AutoPowerOffTime.capability.maxValue = capability.maxValue;

            device_data.AutoPowerOffTime.capability.allMinValue = capability.allMinValue;
            device_data.AutoPowerOffTime.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.AutoPowerOffTime.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.AutoPowerOffTime.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.AutoPowerOffTime.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.AutoPowerOffTime.capability.countOfAllList = capability.countOfAllList;
        }

        // AutoPowerOffTimeBattery
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIAutoPowerOffTimeBatteryKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.AutoPowerOffTimeBattery.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIAutoPowerOffTimeBatteryKey, &capability));

            device_data.AutoPowerOffTimeBattery.capability.version = capability.version;
            device_data.AutoPowerOffTimeBattery.capability.supportLevel = capability.supportLevel;
            device_data.AutoPowerOffTimeBattery.capability.capabilityType = capability.capabilityType;

            device_data.AutoPowerOffTimeBattery.capability.minValue = capability.minValue;
            device_data.AutoPowerOffTimeBattery.capability.maxValue = capability.maxValue;

            device_data.AutoPowerOffTimeBattery.capability.allMinValue = capability.allMinValue;
            device_data.AutoPowerOffTimeBattery.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.AutoPowerOffTimeBattery.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.AutoPowerOffTimeBattery.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.AutoPowerOffTimeBattery.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.AutoPowerOffTimeBattery.capability.countOfAllList = capability.countOfAllList;
        }

        // 2in1Mode
        {
            SDIInt out     = false;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDI2in1ModeKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.Use2in1Mode.select = (SDIInt)out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDI2in1ModeKey, &capability));

            device_data.Use2in1Mode.capability.version = capability.version;
            device_data.Use2in1Mode.capability.supportLevel = capability.supportLevel;
            device_data.Use2in1Mode.capability.capabilityType = capability.capabilityType;

            device_data.Use2in1Mode.capability.minValue = capability.minValue;
            device_data.Use2in1Mode.capability.maxValue = capability.maxValue;

            device_data.Use2in1Mode.capability.allMinValue = capability.allMinValue;
            device_data.Use2in1Mode.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.Use2in1Mode.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.Use2in1Mode.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.Use2in1Mode.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.Use2in1Mode.capability.countOfAllList = capability.countOfAllList;
        }

        // BackgroundColor
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIBackgroundColorKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.BackgroundColor.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIBackgroundColorKey, &capability));

            device_data.BackgroundColor.capability.version = capability.version;
            device_data.BackgroundColor.capability.supportLevel = capability.supportLevel;
            device_data.BackgroundColor.capability.capabilityType = capability.capabilityType;

            device_data.BackgroundColor.capability.minValue = capability.minValue;
            device_data.BackgroundColor.capability.maxValue = capability.maxValue;

            device_data.BackgroundColor.capability.allMinValue = capability.allMinValue;
            device_data.BackgroundColor.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.BackgroundColor.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.BackgroundColor.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.BackgroundColor.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.BackgroundColor.capability.countOfAllList = capability.countOfAllList;
        }

        // AFMMode
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIAFMModeKey, kSDIValueTypeInt, &out, sizeof(out)));

            //device_data.AutomaticFeedingMode.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIAFMModeKey, &capability));

            device_data.AutomaticFeedingMode.capability.version = capability.version;
            device_data.AutomaticFeedingMode.capability.supportLevel = capability.supportLevel;
            device_data.AutomaticFeedingMode.capability.capabilityType = capability.capabilityType;

            device_data.AutomaticFeedingMode.capability.minValue = capability.minValue;
            device_data.AutomaticFeedingMode.capability.maxValue = capability.maxValue;

            device_data.AutomaticFeedingMode.capability.allMinValue = capability.allMinValue;
            device_data.AutomaticFeedingMode.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.AutomaticFeedingMode.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.AutomaticFeedingMode.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.AutomaticFeedingMode.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.AutomaticFeedingMode.capability.countOfAllList = capability.countOfAllList;
        }

        // AFMTimeout
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIAFMTimeoutKey, kSDIValueTypeInt, &out, sizeof(out)));

            //device_data.AFMTimeout.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIAFMTimeoutKey, &capability));

            device_data.AFMTimeout.capability.version = capability.version;
            device_data.AFMTimeout.capability.supportLevel = capability.supportLevel;
            device_data.AFMTimeout.capability.capabilityType = capability.capabilityType;

            device_data.AFMTimeout.capability.minValue = capability.minValue;
            device_data.AFMTimeout.capability.maxValue = capability.maxValue;

            device_data.AFMTimeout.capability.allMinValue = capability.allMinValue;
            device_data.AFMTimeout.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.AFMTimeout.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.AFMTimeout.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.AFMTimeout.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.AFMTimeout.capability.countOfAllList = capability.countOfAllList;
        }

        // ADFCard
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIADFCardKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ScanArea.ADFCard.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIADFCardKey, &capability));

            device_data.ScanArea.ADFCard.capability.version = capability.version;
            device_data.ScanArea.ADFCard.capability.supportLevel = capability.supportLevel;
            device_data.ScanArea.ADFCard.capability.capabilityType = capability.capabilityType;

            device_data.ScanArea.ADFCard.capability.minValue = capability.minValue;
            device_data.ScanArea.ADFCard.capability.maxValue = capability.maxValue;

            device_data.ScanArea.ADFCard.capability.allMinValue = capability.allMinValue;
            device_data.ScanArea.ADFCard.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ScanArea.ADFCard.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ScanArea.ADFCard.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ScanArea.ADFCard.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ScanArea.ADFCard.capability.countOfAllList = capability.countOfAllList;
        }

        // SensorGlassDirtSensitivity
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDISensorGlassDirtSensitivity , kSDIValueTypeInt, &out, sizeof(out)));

            device_data.SensorGlassDirtSensitivity.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDISensorGlassDirtSensitivity , &capability));

            device_data.SensorGlassDirtSensitivity.capability.version = capability.version;
            device_data.SensorGlassDirtSensitivity.capability.supportLevel = capability.supportLevel;
            device_data.SensorGlassDirtSensitivity.capability.capabilityType = capability.capabilityType;

            device_data.SensorGlassDirtSensitivity.capability.minValue = capability.minValue;
            device_data.SensorGlassDirtSensitivity.capability.maxValue = capability.maxValue;

            device_data.SensorGlassDirtSensitivity.capability.allMinValue = capability.allMinValue;
            device_data.SensorGlassDirtSensitivity.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.SensorGlassDirtSensitivity.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.SensorGlassDirtSensitivity.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.SensorGlassDirtSensitivity.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.SensorGlassDirtSensitivity.capability.countOfAllList = capability.countOfAllList;
        }


        // ADFPaperProtection
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIADFPaperProtectionKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ADFPaperProtection.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIADFPaperProtectionKey, &capability));

            device_data.ADFPaperProtection.capability.version = capability.version;
            device_data.ADFPaperProtection.capability.supportLevel = capability.supportLevel;
            device_data.ADFPaperProtection.capability.capabilityType = capability.capabilityType;

            device_data.ADFPaperProtection.capability.minValue = capability.minValue;
            device_data.ADFPaperProtection.capability.maxValue = capability.maxValue;

            device_data.ADFPaperProtection.capability.allMinValue = capability.allMinValue;
            device_data.ADFPaperProtection.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ADFPaperProtection.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ADFPaperProtection.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ADFPaperProtection.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ADFPaperProtection.capability.countOfAllList = capability.countOfAllList;
        }

        // BehaviorWhenDoubleFeed
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIBehaviorWhenDoubleFeedKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.BehaviorWhenDoubleFeed.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIBehaviorWhenDoubleFeedKey, &capability));

            device_data.BehaviorWhenDoubleFeed.capability.version = capability.version;
            device_data.BehaviorWhenDoubleFeed.capability.supportLevel = capability.supportLevel;
            device_data.BehaviorWhenDoubleFeed.capability.capabilityType = capability.capabilityType;

            device_data.BehaviorWhenDoubleFeed.capability.minValue = capability.minValue;
            device_data.BehaviorWhenDoubleFeed.capability.maxValue = capability.maxValue;

            device_data.BehaviorWhenDoubleFeed.capability.allMinValue = capability.allMinValue;
            device_data.BehaviorWhenDoubleFeed.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.BehaviorWhenDoubleFeed.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.BehaviorWhenDoubleFeed.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.BehaviorWhenDoubleFeed.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.BehaviorWhenDoubleFeed.capability.countOfAllList = capability.countOfAllList;
        }

        // DirectPowerOn
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIDirectPowerOnKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.DirectPowerOn.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIDirectPowerOnKey, &capability));

            device_data.DirectPowerOn.capability.version = capability.version;
            device_data.DirectPowerOn.capability.supportLevel = capability.supportLevel;
            device_data.DirectPowerOn.capability.capabilityType = capability.capabilityType;

            device_data.DirectPowerOn.capability.minValue = capability.minValue;
            device_data.DirectPowerOn.capability.maxValue = capability.maxValue;

            device_data.DirectPowerOn.capability.allMinValue = capability.allMinValue;
            device_data.DirectPowerOn.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.DirectPowerOn.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.DirectPowerOn.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.DirectPowerOn.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.DirectPowerOn.capability.countOfAllList = capability.countOfAllList;
        }

        // NonConnectPowerOff
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDINonConnectPowerOffKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.NonConnectPowerOff.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDINonConnectPowerOffKey, &capability));

            device_data.NonConnectPowerOff.capability.version = capability.version;
            device_data.NonConnectPowerOff.capability.supportLevel = capability.supportLevel;
            device_data.NonConnectPowerOff.capability.capabilityType = capability.capabilityType;

            device_data.NonConnectPowerOff.capability.minValue = capability.minValue;
            device_data.NonConnectPowerOff.capability.maxValue = capability.maxValue;

            device_data.NonConnectPowerOff.capability.allMinValue = capability.allMinValue;
            device_data.NonConnectPowerOff.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.NonConnectPowerOff.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.NonConnectPowerOff.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.NonConnectPowerOff.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.NonConnectPowerOff.capability.countOfAllList = capability.countOfAllList;
        }

        // ScannerKind
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, KSDIScannerKindKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ScannerKind.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, KSDIScannerKindKey, &capability));

            device_data.ScannerKind.capability.version = capability.version;
            device_data.ScannerKind.capability.supportLevel = capability.supportLevel;
            device_data.ScannerKind.capability.capabilityType = capability.capabilityType;

            device_data.ScannerKind.capability.minValue = capability.minValue;
            device_data.ScannerKind.capability.maxValue = capability.maxValue;

            device_data.ScannerKind.capability.allMinValue = capability.allMinValue;
            device_data.ScannerKind.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ScannerKind.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ScannerKind.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ScannerKind.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ScannerKind.capability.countOfAllList = capability.countOfAllList;
        }

        // AdfDuplexType
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIADFDuplexTypeKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.AdfDuplexType.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIADFDuplexTypeKey, &capability));

            device_data.AdfDuplexType.capability.version = capability.version;
            device_data.AdfDuplexType.capability.supportLevel = capability.supportLevel;
            device_data.AdfDuplexType.capability.capabilityType = capability.capabilityType;

            device_data.AdfDuplexType.capability.minValue = capability.minValue;
            device_data.AdfDuplexType.capability.maxValue = capability.maxValue;

            device_data.AdfDuplexType.capability.allMinValue = capability.allMinValue;
            device_data.AdfDuplexType.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.AdfDuplexType.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.AdfDuplexType.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.AdfDuplexType.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.AdfDuplexType.capability.countOfAllList = capability.countOfAllList;
        }

        // FirmwareVersion
        {
            int kMaxFirmwareVerSize = 50;
            std::unique_ptr<char[]>  out(new char[kMaxFirmwareVerSize]);
            memset((void*)out.get(), 0, kMaxFirmwareVerSize);
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIFirmwareVersionKey, kSDIValueTypeString, (void*)out.get(), kMaxFirmwareVerSize));

            device_data.Firmware_Version_value = out.get();

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIFirmwareVersionKey, &capability));

            device_data.FirmwareVersion.capability.version = capability.version;
            device_data.FirmwareVersion.capability.supportLevel = capability.supportLevel;
            device_data.FirmwareVersion.capability.capabilityType = capability.capabilityType;

            device_data.FirmwareVersion.capability.minValue = capability.minValue;
            device_data.FirmwareVersion.capability.maxValue = capability.maxValue;

            device_data.FirmwareVersion.capability.allMinValue = capability.allMinValue;
            device_data.FirmwareVersion.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.FirmwareVersion.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.FirmwareVersion.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.FirmwareVersion.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.FirmwareVersion.capability.countOfAllList = capability.countOfAllList;
        }

        // SerialNumber
        {
            int kMaxSerialNumSize = 50;
            std::unique_ptr<char[]>  out(new char[kMaxSerialNumSize]);
            memset((void*)out.get(), 0, kMaxSerialNumSize);
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDISerialNumberKey, kSDIValueTypeString, (void*)out.get(), kMaxSerialNumSize));

            device_data.Serial_Number_value = out.get();

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDISerialNumberKey, &capability));

            device_data.SerialNumber.capability.version = capability.version;
            device_data.SerialNumber.capability.supportLevel = capability.supportLevel;
            device_data.SerialNumber.capability.capabilityType = capability.capabilityType;

            device_data.SerialNumber.capability.minValue = capability.minValue;
            device_data.SerialNumber.capability.maxValue = capability.maxValue;

            device_data.SerialNumber.capability.allMinValue = capability.allMinValue;
            device_data.SerialNumber.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.SerialNumber.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.SerialNumber.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.SerialNumber.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.SerialNumber.capability.countOfAllList = capability.countOfAllList;
        }

        // textEnhance
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDITextEnhanceKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.textEnhance.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDITextEnhanceKey, &capability));

            device_data.textEnhance.capability.version = capability.version;
            device_data.textEnhance.capability.supportLevel = capability.supportLevel;
            device_data.textEnhance.capability.capabilityType = capability.capabilityType;

            device_data.textEnhance.capability.minValue = capability.minValue;
            device_data.textEnhance.capability.maxValue = capability.maxValue;

            device_data.textEnhance.capability.allMinValue = capability.allMinValue;
            device_data.textEnhance.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.textEnhance.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.textEnhance.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.textEnhance.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.textEnhance.capability.countOfAllList = capability.countOfAllList;
        }

        // textEnhanceNoiseReductionLevel
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDITextEnhanceNoiseReductionLevelKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.textEnhanceNoiseReductionLevel.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDITextEnhanceNoiseReductionLevelKey, &capability));

            device_data.textEnhanceNoiseReductionLevel.capability.version = capability.version;
            device_data.textEnhanceNoiseReductionLevel.capability.supportLevel = capability.supportLevel;
            device_data.textEnhanceNoiseReductionLevel.capability.capabilityType = capability.capabilityType;

            device_data.textEnhanceNoiseReductionLevel.capability.minValue = capability.minValue;
            device_data.textEnhanceNoiseReductionLevel.capability.maxValue = capability.maxValue;

            device_data.textEnhanceNoiseReductionLevel.capability.allMinValue = capability.allMinValue;
            device_data.textEnhanceNoiseReductionLevel.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.textEnhanceNoiseReductionLevel.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.textEnhanceNoiseReductionLevel.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.textEnhanceNoiseReductionLevel.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.textEnhanceNoiseReductionLevel.capability.countOfAllList = capability.countOfAllList;
        }

        // textEnhanceSensitivity
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDITextEnhanceSensitivityKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.textEnhanceSensitivity.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDITextEnhanceSensitivityKey, &capability));

            device_data.textEnhanceSensitivity.capability.version = capability.version;
            device_data.textEnhanceSensitivity.capability.supportLevel = capability.supportLevel;
            device_data.textEnhanceSensitivity.capability.capabilityType = capability.capabilityType;

            device_data.textEnhanceSensitivity.capability.minValue = capability.minValue;
            device_data.textEnhanceSensitivity.capability.maxValue = capability.maxValue;

            device_data.textEnhanceSensitivity.capability.allMinValue = capability.allMinValue;
            device_data.textEnhanceSensitivity.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.textEnhanceSensitivity.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.textEnhanceSensitivity.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.textEnhanceSensitivity.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.textEnhanceSensitivity.capability.countOfAllList = capability.countOfAllList;
        }

        // textEnhancePaperCreaseReduction
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDITextEnhancePaperCreaseReductionKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.textEnhancePaperCreaseReduction.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDITextEnhancePaperCreaseReductionKey, &capability));

            device_data.textEnhancePaperCreaseReduction.capability.version = capability.version;
            device_data.textEnhancePaperCreaseReduction.capability.supportLevel = capability.supportLevel;
            device_data.textEnhancePaperCreaseReduction.capability.capabilityType = capability.capabilityType;

            device_data.textEnhancePaperCreaseReduction.capability.minValue = capability.minValue;
            device_data.textEnhancePaperCreaseReduction.capability.maxValue = capability.maxValue;

            device_data.textEnhancePaperCreaseReduction.capability.allMinValue = capability.allMinValue;
            device_data.textEnhancePaperCreaseReduction.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.textEnhancePaperCreaseReduction.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.textEnhancePaperCreaseReduction.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.textEnhancePaperCreaseReduction.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.textEnhancePaperCreaseReduction.capability.countOfAllList = capability.countOfAllList;
        }

        // thresholdAdjustment
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDITextEnhanceThresholdAdjustmentKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.thresholdAdjustment.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDITextEnhanceThresholdAdjustmentKey, &capability));

            device_data.thresholdAdjustment.capability.version = capability.version;
            device_data.thresholdAdjustment.capability.supportLevel = capability.supportLevel;
            device_data.thresholdAdjustment.capability.capabilityType = capability.capabilityType;

            device_data.thresholdAdjustment.capability.minValue = capability.minValue;
            device_data.thresholdAdjustment.capability.maxValue = capability.maxValue;

            device_data.thresholdAdjustment.capability.allMinValue = capability.allMinValue;
            device_data.thresholdAdjustment.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.thresholdAdjustment.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.thresholdAdjustment.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.thresholdAdjustment.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.thresholdAdjustment.capability.countOfAllList = capability.countOfAllList;
        }

        this->Get_Counter_Value();
    }
    catch(bool error)
    {
        return error;
    }

    return true;
}

bool Supervisor::Get_Counter_Value(void)
{
    if(!driver){
        return false;
    }
    try{
        // ScanCounterKey
        {
            SDIInt out     = (SDIInt)0;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIScanCounterKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ConfigureCounter.ScanCounterKey.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIScanCounterKey, &capability));

            device_data.ConfigureCounter.ScanCounterKey.capability.version = capability.version;
            device_data.ConfigureCounter.ScanCounterKey.capability.supportLevel = capability.supportLevel;
            device_data.ConfigureCounter.ScanCounterKey.capability.capabilityType = capability.capabilityType;

            device_data.ConfigureCounter.ScanCounterKey.capability.minValue = capability.minValue;
            device_data.ConfigureCounter.ScanCounterKey.capability.maxValue = capability.maxValue;

            device_data.ConfigureCounter.ScanCounterKey.capability.allMinValue = capability.allMinValue;
            device_data.ConfigureCounter.ScanCounterKey.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ConfigureCounter.ScanCounterKey.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.ScanCounterKey.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ConfigureCounter.ScanCounterKey.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.ScanCounterKey.capability.countOfAllList = capability.countOfAllList;
        }

        // ADFScanCounterKey
        {
            SDIInt out     = (SDIInt)0;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIADFScanCounterKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ConfigureCounter.ADFScanCounterKey.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIADFScanCounterKey, &capability));

            device_data.ConfigureCounter.ADFScanCounterKey.capability.version = capability.version;
            device_data.ConfigureCounter.ADFScanCounterKey.capability.supportLevel = capability.supportLevel;
            device_data.ConfigureCounter.ADFScanCounterKey.capability.capabilityType = capability.capabilityType;

            device_data.ConfigureCounter.ADFScanCounterKey.capability.minValue = capability.minValue;
            device_data.ConfigureCounter.ADFScanCounterKey.capability.maxValue = capability.maxValue;

            device_data.ConfigureCounter.ADFScanCounterKey.capability.allMinValue = capability.allMinValue;
            device_data.ConfigureCounter.ADFScanCounterKey.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ConfigureCounter.ADFScanCounterKey.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.ADFScanCounterKey.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ConfigureCounter.ADFScanCounterKey.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.ADFScanCounterKey.capability.countOfAllList = capability.countOfAllList;
        }

        // ADFDuplexScanCounterKey
        {
            SDIInt out     = (SDIInt)0;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIADFDuplexCounterKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ConfigureCounter.ADFDuplexScanCounterKey.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIADFDuplexCounterKey, &capability));

            device_data.ConfigureCounter.ADFDuplexScanCounterKey.capability.version = capability.version;
            device_data.ConfigureCounter.ADFDuplexScanCounterKey.capability.supportLevel = capability.supportLevel;
            device_data.ConfigureCounter.ADFDuplexScanCounterKey.capability.capabilityType = capability.capabilityType;

            device_data.ConfigureCounter.ADFDuplexScanCounterKey.capability.minValue = capability.minValue;
            device_data.ConfigureCounter.ADFDuplexScanCounterKey.capability.maxValue = capability.maxValue;

            device_data.ConfigureCounter.ADFDuplexScanCounterKey.capability.allMinValue = capability.allMinValue;
            device_data.ConfigureCounter.ADFDuplexScanCounterKey.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ConfigureCounter.ADFDuplexScanCounterKey.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.ADFDuplexScanCounterKey.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ConfigureCounter.ADFDuplexScanCounterKey.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.ADFDuplexScanCounterKey.capability.countOfAllList = capability.countOfAllList;
        }

        // ADFCarrierSheetScanCounterKey
        {
            SDIInt out     = (SDIInt)0;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIADFCarrierSheetCounterKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ConfigureCounter.ADFCarrierSheetScanCounterKey.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIADFCarrierSheetCounterKey, &capability));

            device_data.ConfigureCounter.ADFCarrierSheetScanCounterKey.capability.version = capability.version;
            device_data.ConfigureCounter.ADFCarrierSheetScanCounterKey.capability.supportLevel = capability.supportLevel;
            device_data.ConfigureCounter.ADFCarrierSheetScanCounterKey.capability.capabilityType = capability.capabilityType;

            device_data.ConfigureCounter.ADFCarrierSheetScanCounterKey.capability.minValue = capability.minValue;
            device_data.ConfigureCounter.ADFCarrierSheetScanCounterKey.capability.maxValue = capability.maxValue;

            device_data.ConfigureCounter.ADFCarrierSheetScanCounterKey.capability.allMinValue = capability.allMinValue;
            device_data.ConfigureCounter.ADFCarrierSheetScanCounterKey.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ConfigureCounter.ADFCarrierSheetScanCounterKey.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.ADFCarrierSheetScanCounterKey.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ConfigureCounter.ADFCarrierSheetScanCounterKey.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.ADFCarrierSheetScanCounterKey.capability.countOfAllList = capability.countOfAllList;
        }

        // CardScanCounterKey
        {
            SDIInt out     = (SDIInt)0;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDICardScanCounterKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ConfigureCounter.CardScanCounterKey.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDICardScanCounterKey, &capability));

            device_data.ConfigureCounter.CardScanCounterKey.capability.version = capability.version;
            device_data.ConfigureCounter.CardScanCounterKey.capability.supportLevel = capability.supportLevel;
            device_data.ConfigureCounter.CardScanCounterKey.capability.capabilityType = capability.capabilityType;

            device_data.ConfigureCounter.CardScanCounterKey.capability.minValue = capability.minValue;
            device_data.ConfigureCounter.CardScanCounterKey.capability.maxValue = capability.maxValue;

            device_data.ConfigureCounter.CardScanCounterKey.capability.allMinValue = capability.allMinValue;
            device_data.ConfigureCounter.CardScanCounterKey.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ConfigureCounter.CardScanCounterKey.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.CardScanCounterKey.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ConfigureCounter.CardScanCounterKey.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.CardScanCounterKey.capability.countOfAllList = capability.countOfAllList;
        }

        // PaperJamCounter
        {
            SDIInt out     = (SDIInt)0;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIPaperJamCounterKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ConfigureCounter.PaperJamCounter.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIPaperJamCounterKey, &capability));

            device_data.ConfigureCounter.PaperJamCounter.capability.version = capability.version;
            device_data.ConfigureCounter.PaperJamCounter.capability.supportLevel = capability.supportLevel;
            device_data.ConfigureCounter.PaperJamCounter.capability.capabilityType = capability.capabilityType;

            device_data.ConfigureCounter.PaperJamCounter.capability.minValue = capability.minValue;
            device_data.ConfigureCounter.PaperJamCounter.capability.maxValue = capability.maxValue;

            device_data.ConfigureCounter.PaperJamCounter.capability.allMinValue = capability.allMinValue;
            device_data.ConfigureCounter.PaperJamCounter.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ConfigureCounter.PaperJamCounter.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.PaperJamCounter.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ConfigureCounter.PaperJamCounter.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.PaperJamCounter.capability.countOfAllList = capability.countOfAllList;
        }

        // DoubleFeedCounter
        {
            SDIInt out     = (SDIInt)0;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIDoubleFeedCounterKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ConfigureCounter.DoubleFeedCounter.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIDoubleFeedCounterKey, &capability));

            device_data.ConfigureCounter.DoubleFeedCounter.capability.version = capability.version;
            device_data.ConfigureCounter.DoubleFeedCounter.capability.supportLevel = capability.supportLevel;
            device_data.ConfigureCounter.DoubleFeedCounter.capability.capabilityType = capability.capabilityType;

            device_data.ConfigureCounter.DoubleFeedCounter.capability.minValue = capability.minValue;
            device_data.ConfigureCounter.DoubleFeedCounter.capability.maxValue = capability.maxValue;

            device_data.ConfigureCounter.DoubleFeedCounter.capability.allMinValue = capability.allMinValue;
            device_data.ConfigureCounter.DoubleFeedCounter.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ConfigureCounter.DoubleFeedCounter.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.DoubleFeedCounter.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ConfigureCounter.DoubleFeedCounter.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.DoubleFeedCounter.capability.countOfAllList = capability.countOfAllList;
        }

        // RollerKitCounter
        {
            SDIInt out     = (SDIInt)0;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIRollerKitCounterKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ConfigureCounter.RollerKitCounter.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIRollerKitCounterKey, &capability));

            device_data.ConfigureCounter.RollerKitCounter.capability.version = capability.version;
            device_data.ConfigureCounter.RollerKitCounter.capability.supportLevel = capability.supportLevel;
            device_data.ConfigureCounter.RollerKitCounter.capability.capabilityType = capability.capabilityType;

            device_data.ConfigureCounter.RollerKitCounter.capability.minValue = capability.minValue;
            device_data.ConfigureCounter.RollerKitCounter.capability.maxValue = capability.maxValue;

            device_data.ConfigureCounter.RollerKitCounter.capability.allMinValue = capability.allMinValue;
            device_data.ConfigureCounter.RollerKitCounter.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ConfigureCounter.RollerKitCounter.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.RollerKitCounter.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ConfigureCounter.RollerKitCounter.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.RollerKitCounter.capability.countOfAllList = capability.countOfAllList;
        }

        // RollerKitLifeLimit
        {
            SDIInt out     = (SDIInt)0;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIRollerKitLifeLimitKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ConfigureCounter.RollerKitLifeLimit.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIRollerKitLifeLimitKey, &capability));
            device_data.ConfigureCounter.RollerKitLifeLimit.capability.version = capability.version;
            device_data.ConfigureCounter.RollerKitLifeLimit.capability.supportLevel = capability.supportLevel;
            device_data.ConfigureCounter.RollerKitLifeLimit.capability.capabilityType = capability.capabilityType;

            device_data.ConfigureCounter.RollerKitLifeLimit.capability.minValue = capability.minValue;
            device_data.ConfigureCounter.RollerKitLifeLimit.capability.maxValue = capability.maxValue;

            device_data.ConfigureCounter.RollerKitLifeLimit.capability.allMinValue = capability.allMinValue;
            device_data.ConfigureCounter.RollerKitLifeLimit.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ConfigureCounter.RollerKitLifeLimit.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.RollerKitLifeLimit.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ConfigureCounter.RollerKitLifeLimit.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.RollerKitLifeLimit.capability.countOfAllList = capability.countOfAllList;
        }

        // RollerKitNearEnd
        {
            SDIInt out     = (SDIInt)0;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIRollerKitNearEndKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ConfigureCounter.RollerKitNearEnd.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIRollerKitNearEndKey, &capability));
            device_data.ConfigureCounter.RollerKitNearEnd.capability.version = capability.version;
            device_data.ConfigureCounter.RollerKitNearEnd.capability.supportLevel = capability.supportLevel;
            device_data.ConfigureCounter.RollerKitNearEnd.capability.capabilityType = capability.capabilityType;

            device_data.ConfigureCounter.RollerKitNearEnd.capability.minValue = capability.minValue;
            device_data.ConfigureCounter.RollerKitNearEnd.capability.maxValue = capability.maxValue;

            device_data.ConfigureCounter.RollerKitNearEnd.capability.allMinValue = capability.allMinValue;
            device_data.ConfigureCounter.RollerKitNearEnd.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ConfigureCounter.RollerKitNearEnd.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.RollerKitNearEnd.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ConfigureCounter.RollerKitNearEnd.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.RollerKitNearEnd.capability.countOfAllList = capability.countOfAllList;
        }

        // RollerCounter
        {
            SDIInt out     = (SDIInt)0;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIRollerCounterKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ConfigureCounter.RollerCounter.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIRollerCounterKey, &capability));

            device_data.ConfigureCounter.RollerCounter.capability.version = capability.version;
            device_data.ConfigureCounter.RollerCounter.capability.supportLevel = capability.supportLevel;
            device_data.ConfigureCounter.RollerCounter.capability.capabilityType = capability.capabilityType;

            device_data.ConfigureCounter.RollerCounter.capability.minValue = capability.minValue;
            device_data.ConfigureCounter.RollerCounter.capability.maxValue = capability.maxValue;

            device_data.ConfigureCounter.RollerCounter.capability.allMinValue = capability.allMinValue;
            device_data.ConfigureCounter.RollerCounter.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ConfigureCounter.RollerCounter.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.RollerCounter.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ConfigureCounter.RollerCounter.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.RollerCounter.capability.countOfAllList = capability.countOfAllList;
        }

        // RollerLifeLimit
        {
            SDIInt out     = (SDIInt)0;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIRollerLifeLimitKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ConfigureCounter.RollerLifeLimit.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIRollerLifeLimitKey, &capability));

            device_data.ConfigureCounter.RollerLifeLimit.capability.version = capability.version;
            device_data.ConfigureCounter.RollerLifeLimit.capability.supportLevel = capability.supportLevel;
            device_data.ConfigureCounter.RollerLifeLimit.capability.capabilityType = capability.capabilityType;

            device_data.ConfigureCounter.RollerLifeLimit.capability.minValue = capability.minValue;
            device_data.ConfigureCounter.RollerLifeLimit.capability.maxValue = capability.maxValue;

            device_data.ConfigureCounter.RollerLifeLimit.capability.allMinValue = capability.allMinValue;
            device_data.ConfigureCounter.RollerLifeLimit.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ConfigureCounter.RollerLifeLimit.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.RollerLifeLimit.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ConfigureCounter.RollerLifeLimit.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.RollerLifeLimit.capability.countOfAllList = capability.countOfAllList;
        }

        // RollerNearEnd
        {
            SDIInt out     = (SDIInt)0;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIRollerNearEndKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ConfigureCounter.RollerNearEnd.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIRollerNearEndKey, &capability));

            device_data.ConfigureCounter.RollerNearEnd.capability.version = capability.version;
            device_data.ConfigureCounter.RollerNearEnd.capability.supportLevel = capability.supportLevel;
            device_data.ConfigureCounter.RollerNearEnd.capability.capabilityType = capability.capabilityType;

            device_data.ConfigureCounter.RollerNearEnd.capability.minValue = capability.minValue;
            device_data.ConfigureCounter.RollerNearEnd.capability.maxValue = capability.maxValue;

            device_data.ConfigureCounter.RollerNearEnd.capability.allMinValue = capability.allMinValue;
            device_data.ConfigureCounter.RollerNearEnd.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ConfigureCounter.RollerNearEnd.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.RollerNearEnd.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ConfigureCounter.RollerNearEnd.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.RollerNearEnd.capability.countOfAllList = capability.countOfAllList;
        }


        // RetardRollerCounter
        {
            SDIInt out     = (SDIInt)0;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIRetardRollerCounterKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ConfigureCounter.RetardRollerCounter.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIRetardRollerCounterKey, &capability));

            device_data.ConfigureCounter.RetardRollerCounter.capability.version = capability.version;
            device_data.ConfigureCounter.RetardRollerCounter.capability.supportLevel = capability.supportLevel;
            device_data.ConfigureCounter.RetardRollerCounter.capability.capabilityType = capability.capabilityType;

            device_data.ConfigureCounter.RetardRollerCounter.capability.minValue = capability.minValue;
            device_data.ConfigureCounter.RetardRollerCounter.capability.maxValue = capability.maxValue;

            device_data.ConfigureCounter.RetardRollerCounter.capability.allMinValue = capability.allMinValue;
            device_data.ConfigureCounter.RetardRollerCounter.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ConfigureCounter.RetardRollerCounter.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.RetardRollerCounter.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ConfigureCounter.RetardRollerCounter.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.RetardRollerCounter.capability.countOfAllList = capability.countOfAllList;
        }

        // RetardRollerLifeLimit
        {
            SDIInt out     = (SDIInt)0;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIRetardRollerLifeLimitKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ConfigureCounter.RetardRollerLifeLimit.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIRetardRollerLifeLimitKey, &capability));

            device_data.ConfigureCounter.RetardRollerLifeLimit.capability.version = capability.version;
            device_data.ConfigureCounter.RetardRollerLifeLimit.capability.supportLevel = capability.supportLevel;
            device_data.ConfigureCounter.RetardRollerLifeLimit.capability.capabilityType = capability.capabilityType;

            device_data.ConfigureCounter.RetardRollerLifeLimit.capability.minValue = capability.minValue;
            device_data.ConfigureCounter.RetardRollerLifeLimit.capability.maxValue = capability.maxValue;

            device_data.ConfigureCounter.RetardRollerLifeLimit.capability.allMinValue = capability.allMinValue;
            device_data.ConfigureCounter.RetardRollerLifeLimit.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ConfigureCounter.RetardRollerLifeLimit.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.RetardRollerLifeLimit.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ConfigureCounter.RetardRollerLifeLimit.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.RetardRollerLifeLimit.capability.countOfAllList = capability.countOfAllList;
        }

        // RetardRollerNearEnd
        {
            SDIInt out     = (SDIInt)0;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIRetardRollerNearEndKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ConfigureCounter.RetardRollerNearEnd.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIRetardRollerNearEndKey, &capability));

            device_data.ConfigureCounter.RetardRollerNearEnd.capability.version = capability.version;
            device_data.ConfigureCounter.RetardRollerNearEnd.capability.supportLevel = capability.supportLevel;
            device_data.ConfigureCounter.RetardRollerNearEnd.capability.capabilityType = capability.capabilityType;

            device_data.ConfigureCounter.RetardRollerNearEnd.capability.minValue = capability.minValue;
            device_data.ConfigureCounter.RetardRollerNearEnd.capability.maxValue = capability.maxValue;

            device_data.ConfigureCounter.RetardRollerNearEnd.capability.allMinValue = capability.allMinValue;
            device_data.ConfigureCounter.RetardRollerNearEnd.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ConfigureCounter.RetardRollerNearEnd.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.RetardRollerNearEnd.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ConfigureCounter.RetardRollerNearEnd.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.RetardRollerNearEnd.capability.countOfAllList = capability.countOfAllList;
        }

        // RestrictFunction
        {
            SDIInt out     = (SDIInt)0;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIRestrictFunctionKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ConfigureCounter.RestrictFunction.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIRestrictFunctionKey, &capability));

            device_data.ConfigureCounter.RestrictFunction.capability.version = capability.version;
            device_data.ConfigureCounter.RestrictFunction.capability.supportLevel = capability.supportLevel;
            device_data.ConfigureCounter.RestrictFunction.capability.capabilityType = capability.capabilityType;

            device_data.ConfigureCounter.RestrictFunction.capability.minValue = capability.minValue;
            device_data.ConfigureCounter.RestrictFunction.capability.maxValue = capability.maxValue;

            device_data.ConfigureCounter.RestrictFunction.capability.allMinValue = capability.allMinValue;
            device_data.ConfigureCounter.RestrictFunction.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ConfigureCounter.RestrictFunction.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.RestrictFunction.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ConfigureCounter.RestrictFunction.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.RestrictFunction.capability.countOfAllList = capability.countOfAllList;
        }

        // CleaningAlertLimitCount
        {
            SDIInt out     = (SDIInt)0;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDICleaningAlertLimitCountKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ConfigureCounter.CleaningAlertLimitCount.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDICleaningAlertLimitCountKey, &capability));

            device_data.ConfigureCounter.CleaningAlertLimitCount.capability.version = capability.version;
            device_data.ConfigureCounter.CleaningAlertLimitCount.capability.supportLevel = capability.supportLevel;
            device_data.ConfigureCounter.CleaningAlertLimitCount.capability.capabilityType = capability.capabilityType;

            device_data.ConfigureCounter.CleaningAlertLimitCount.capability.minValue = capability.minValue;
            device_data.ConfigureCounter.CleaningAlertLimitCount.capability.maxValue = capability.maxValue;

            device_data.ConfigureCounter.CleaningAlertLimitCount.capability.allMinValue = capability.allMinValue;
            device_data.ConfigureCounter.CleaningAlertLimitCount.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ConfigureCounter.CleaningAlertLimitCount.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.CleaningAlertLimitCount.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ConfigureCounter.CleaningAlertLimitCount.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.CleaningAlertLimitCount.capability.countOfAllList = capability.countOfAllList;
        }

        // CleaningWarningNotifyCount
        {
            SDIInt out     = (SDIInt)0;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDICleaningWarningNotifyCountKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ConfigureCounter.CleaningWarningNotifyCount.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDICleaningWarningNotifyCountKey, &capability));

            device_data.ConfigureCounter.CleaningWarningNotifyCount.capability.version = capability.version;
            device_data.ConfigureCounter.CleaningWarningNotifyCount.capability.supportLevel = capability.supportLevel;
            device_data.ConfigureCounter.CleaningWarningNotifyCount.capability.capabilityType = capability.capabilityType;

            device_data.ConfigureCounter.CleaningWarningNotifyCount.capability.minValue = capability.minValue;
            device_data.ConfigureCounter.CleaningWarningNotifyCount.capability.maxValue = capability.maxValue;

            device_data.ConfigureCounter.CleaningWarningNotifyCount.capability.allMinValue = capability.allMinValue;
            device_data.ConfigureCounter.CleaningWarningNotifyCount.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ConfigureCounter.CleaningWarningNotifyCount.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.CleaningWarningNotifyCount.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ConfigureCounter.CleaningWarningNotifyCount.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.CleaningWarningNotifyCount.capability.countOfAllList = capability.countOfAllList;
        }

        // RollerKitNotifyCount
        {
            SDIInt out     = (SDIInt)0;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIRollerKitNotifyCountKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ConfigureCounter.RollerKitNotifyCount.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIRollerKitNotifyCountKey, &capability));

            device_data.ConfigureCounter.RollerKitNotifyCount.capability.version = capability.version;
            device_data.ConfigureCounter.RollerKitNotifyCount.capability.supportLevel = capability.supportLevel;
            device_data.ConfigureCounter.RollerKitNotifyCount.capability.capabilityType = capability.capabilityType;

            device_data.ConfigureCounter.RollerKitNotifyCount.capability.minValue = capability.minValue;
            device_data.ConfigureCounter.RollerKitNotifyCount.capability.maxValue = capability.maxValue;

            device_data.ConfigureCounter.RollerKitNotifyCount.capability.allMinValue = capability.allMinValue;
            device_data.ConfigureCounter.RollerKitNotifyCount.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ConfigureCounter.RollerKitNotifyCount.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.RollerKitNotifyCount.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ConfigureCounter.RollerKitNotifyCount.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.RollerKitNotifyCount.capability.countOfAllList = capability.countOfAllList;
        }

        // RetardRollerNotifyCount
        {
            SDIInt out     = (SDIInt)0;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIRetardRollerNotifyCountKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ConfigureCounter.RetardRollerNotifyCount.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIRetardRollerNotifyCountKey, &capability));

            device_data.ConfigureCounter.RetardRollerNotifyCount.capability.version = capability.version;
            device_data.ConfigureCounter.RetardRollerNotifyCount.capability.supportLevel = capability.supportLevel;
            device_data.ConfigureCounter.RetardRollerNotifyCount.capability.capabilityType = capability.capabilityType;

            device_data.ConfigureCounter.RetardRollerNotifyCount.capability.minValue = capability.minValue;
            device_data.ConfigureCounter.RetardRollerNotifyCount.capability.maxValue = capability.maxValue;

            device_data.ConfigureCounter.RetardRollerNotifyCount.capability.allMinValue = capability.allMinValue;
            device_data.ConfigureCounter.RetardRollerNotifyCount.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ConfigureCounter.RetardRollerNotifyCount.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.RetardRollerNotifyCount.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ConfigureCounter.RetardRollerNotifyCount.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.RetardRollerNotifyCount.capability.countOfAllList = capability.countOfAllList;
        }

        // SeparationPadNotifyCount
        {
            SDIInt out     = (SDIInt)0;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDISeparationPadNotifyCountKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ConfigureCounter.SeparationPadNotifyCount.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDISeparationPadNotifyCountKey, &capability));

            device_data.ConfigureCounter.SeparationPadNotifyCount.capability.version = capability.version;
            device_data.ConfigureCounter.SeparationPadNotifyCount.capability.supportLevel = capability.supportLevel;
            device_data.ConfigureCounter.SeparationPadNotifyCount.capability.capabilityType = capability.capabilityType;

            device_data.ConfigureCounter.SeparationPadNotifyCount.capability.minValue = capability.minValue;
            device_data.ConfigureCounter.SeparationPadNotifyCount.capability.maxValue = capability.maxValue;

            device_data.ConfigureCounter.SeparationPadNotifyCount.capability.allMinValue = capability.allMinValue;
            device_data.ConfigureCounter.SeparationPadNotifyCount.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ConfigureCounter.SeparationPadNotifyCount.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.SeparationPadNotifyCount.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ConfigureCounter.SeparationPadNotifyCount.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.SeparationPadNotifyCount.capability.countOfAllList = capability.countOfAllList;
        }

        // PickupRollerNotifyCount
        {
            SDIInt out     = (SDIInt)0;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIPickupRollerNotifyCountKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ConfigureCounter.PickupRollerNotifyCount.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIPickupRollerNotifyCountKey, &capability));

            device_data.ConfigureCounter.PickupRollerNotifyCount.capability.version = capability.version;
            device_data.ConfigureCounter.PickupRollerNotifyCount.capability.supportLevel = capability.supportLevel;
            device_data.ConfigureCounter.PickupRollerNotifyCount.capability.capabilityType = capability.capabilityType;

            device_data.ConfigureCounter.PickupRollerNotifyCount.capability.minValue = capability.minValue;
            device_data.ConfigureCounter.PickupRollerNotifyCount.capability.maxValue = capability.maxValue;

            device_data.ConfigureCounter.PickupRollerNotifyCount.capability.allMinValue = capability.allMinValue;
            device_data.ConfigureCounter.PickupRollerNotifyCount.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ConfigureCounter.PickupRollerNotifyCount.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.PickupRollerNotifyCount.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ConfigureCounter.PickupRollerNotifyCount.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.PickupRollerNotifyCount.capability.countOfAllList = capability.countOfAllList;
        }

        // CleaningWarningCounter
        {
            SDIInt out     = (SDIInt)0;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDICleaningWarningCounterKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ConfigureCounter.CleaningWarningCounter.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDICleaningWarningCounterKey, &capability));

            device_data.ConfigureCounter.CleaningWarningCounter.capability.version = capability.version;
            device_data.ConfigureCounter.CleaningWarningCounter.capability.supportLevel = capability.supportLevel;
            device_data.ConfigureCounter.CleaningWarningCounter.capability.capabilityType = capability.capabilityType;

            device_data.ConfigureCounter.CleaningWarningCounter.capability.minValue = capability.minValue;
            device_data.ConfigureCounter.CleaningWarningCounter.capability.maxValue = capability.maxValue;

            device_data.ConfigureCounter.CleaningWarningCounter.capability.allMinValue = capability.allMinValue;
            device_data.ConfigureCounter.CleaningWarningCounter.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ConfigureCounter.CleaningWarningCounter.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.CleaningWarningCounter.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ConfigureCounter.CleaningWarningCounter.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.CleaningWarningCounter.capability.countOfAllList = capability.countOfAllList;
        }

        // CleaningWarningNotify
        {
            SDIInt out     = (SDIInt)0;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDICleaningWarningNotifyKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ConfigureCounter.CleaningWarningNotify.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDICleaningWarningNotifyKey, &capability));

            device_data.ConfigureCounter.CleaningWarningNotify.capability.version = capability.version;
            device_data.ConfigureCounter.CleaningWarningNotify.capability.supportLevel = capability.supportLevel;
            device_data.ConfigureCounter.CleaningWarningNotify.capability.capabilityType = capability.capabilityType;

            device_data.ConfigureCounter.CleaningWarningNotify.capability.minValue = capability.minValue;
            device_data.ConfigureCounter.CleaningWarningNotify.capability.maxValue = capability.maxValue;

            device_data.ConfigureCounter.CleaningWarningNotify.capability.allMinValue = capability.allMinValue;
            device_data.ConfigureCounter.CleaningWarningNotify.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ConfigureCounter.CleaningWarningNotify.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.CleaningWarningNotify.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ConfigureCounter.CleaningWarningNotify.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.CleaningWarningNotify.capability.countOfAllList = capability.countOfAllList;
        }

        // PaperProtectionCounter
        {
            SDIInt out     = (SDIInt)0;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIPaperProtectionCounterKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ConfigureCounter.PaperProtectionCounter.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIPaperProtectionCounterKey, &capability));

            device_data.ConfigureCounter.PaperProtectionCounter.capability.version = capability.version;
            device_data.ConfigureCounter.PaperProtectionCounter.capability.supportLevel = capability.supportLevel;
            device_data.ConfigureCounter.PaperProtectionCounter.capability.capabilityType = capability.capabilityType;

            device_data.ConfigureCounter.PaperProtectionCounter.capability.minValue = capability.minValue;
            device_data.ConfigureCounter.PaperProtectionCounter.capability.maxValue = capability.maxValue;

            device_data.ConfigureCounter.PaperProtectionCounter.capability.allMinValue = capability.allMinValue;
            device_data.ConfigureCounter.PaperProtectionCounter.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ConfigureCounter.PaperProtectionCounter.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.PaperProtectionCounter.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ConfigureCounter.PaperProtectionCounter.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ConfigureCounter.PaperProtectionCounter.capability.countOfAllList = capability.countOfAllList;
        }
    }
    catch(bool error)
    {
        return error;
    }

    return true;
}


bool Supervisor::Set_All_Value(void)
{
    if(!driver){
        return false;
    }
    try{
        {// functionalunit
            SDIInt current;
            /*if(device_data.FunctionalUnit_Auto){
                current = ADF;
            }else {*/
                current = device_data.FunctionalUnit.select;
            //}
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIFunctionalUnitKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// ColorType
            SDIInt current = device_data.ColorType.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIColorTypeKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// Resolution
            SDIInt current = device_data.Resolution.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIResolutionKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// ImageFormat
            SDIInt current = device_data.ImageFormatSettings.ImageFormat.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIImageFormatKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {//Gamma
            SDIInt current =  device_data.Gamma.select ;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIGammaKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// Width
            SDIInt current =  device_data.ScanArea.ScanAreaWidth.select ;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIScanAreaWidthKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// Height
            SDIInt current =  device_data.ScanArea.ScanAreaHeight.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIScanAreaHeightKey, kSDIValueTypeInt, &current, sizeof(current)));
        }

        {// X
            SDIInt current = device_data.ScanArea.ScanAreaOffsetX.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIScanAreaOffsetXKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// Y
            SDIInt current = device_data.ScanArea.ScanAreaOffsetY.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIScanAreaOffsetYKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        /*{// MaxScanAreaWidth
            SDIInt current = device_data.ScanArea.MaxScanAreaWidth.select;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIMaxScanAreaWidthKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// MaxScanAreaHeight
            SDIInt current = device_data.ScanArea.MaxScanAreaHeight.select;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIMaxScanAreaHeightKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// MaxScanAreaHeightInLongPaper
            SDIInt current = device_data.ScanArea.MaxScanAreaHeightInLongPaper.select;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIMaxScanAreaHeightInLongPaperKey, kSDIValueTypeInt, &current, sizeof(current)));
        }*/
        {// ADFHeightMin
            SDIInt current = device_data.ScanArea.ADFHeightMin.select;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIADFHeightMinKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// ADFDuplexHeightMin
            SDIInt current = device_data.ScanArea.ADFDuplexHeightMin.select;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIADFDuplexHeightMinKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// ADFWidthMin
            SDIInt current = device_data.ScanArea.ADFWidthMin.select;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIADFWidthMinKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// DropoutColor
            SDIInt current = device_data.DropoutColor.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIDropoutColorKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// Orientation
            SDIInt current = device_data.Orientation.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIOrientationKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// MultiTiffEnabled
            SDIInt current =  device_data.ImageFormatSettings.MultiTiffEnabled.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIMultiTiffEnabledKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// TiffCompression
            SDIInt current =  device_data.ImageFormatSettings.TiffCompression.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDITiffCompressionKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// Contrast
            SDIInt current =  device_data.Contrast.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIContrastKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// Brightness
            SDIInt current =  device_data.Brightness.select;
            CHECK_ERROR( SDIScannerDriver_SetValuePtr_(driver, kSDIBrightnessKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// Threshold
            SDIInt current =  device_data.Threshold.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIThresholdKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// JpegQuality
            if(device_data.ImageFormatSettings.ImageFormat.select == ImageFormatJPEG){
                device_data.ImageFormatSettings.JpegQuality.select = device_data.ImageFormatSettings.JpegQuality_forJPEG;
            }else if(device_data.ImageFormatSettings.ImageFormat.select == ImageFormatColorJpegMonoRaw){
                device_data.ImageFormatSettings.JpegQuality.select = device_data.ImageFormatSettings.JpegQuality_forPDF;
            }else {
                device_data.ImageFormatSettings.JpegQuality.select = 0;
            }
            SDIInt current =  device_data.ImageFormatSettings.JpegQuality.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIJpegQuality, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// jpegProgressive
            SDIInt current =  device_data.ImageFormatSettings.JpegProgressive.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIJpegProgressiveKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// DuplexType
            SDIInt current =  device_data.DuplexType.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIDuplexTypeKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// PagesTobeScanned
            SDIInt current =  device_data.PagesTobeScanned.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIPagesTobeScannedKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// DoubleFeedDetectionLevel
            SDIInt current =  device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIDoubleFeedDetectionLevelKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// DoubleFeedDetection
            SDIInt current =  device_data.DoubleFeedDetectionValue.DoubleFeedDetection.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIDoubleFeedDetectionKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// DoubleFeedDetectionAreaMin
            SDIInt current =  device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaMin.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIDoubleFeedDetectionAreaMinKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// DoubleFeedDetectionAreaLength
            SDIInt current =  device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaLength.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIDoubleFeedDetectionAreaLengthKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// BlankPageSkip
            SDIInt current =  device_data.BlankPageSkipKey.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIBlankPageSkipKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// BlankPageSkipLevel
            SDIInt current = device_data.BlankPageSkipLevelKey.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIBlankPageSkipLevelKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// BackgroundRemoval
            SDIInt current =  device_data.BackgroundRemoval.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIBackgroundRemovalKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// PaperDeskew
            SDIInt current =  device_data.PaperDeskew.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIPaperDeskewKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// AutoSize
            SDIInt current =  device_data.ScanArea.AutoSize.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIAutoSizeKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// PaperEndDetection
            SDIInt current;
            if(device_data.ScanArea.FixedSize == USER_DEFINE){
                current =  device_data.ScanArea.PaperEndDetection.select;
            }else {
                current = false;
            }
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIPaperEndDetectionKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// TransferCompression
            SDIInt current =  device_data.TransferCompression.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDITransferCompressionKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// BackgroundColor
            SDIInt current = device_data.BackgroundColor.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIBackgroundColorKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// AFMTimeout
            SDIInt current = device_data.AFMTimeout.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIAFMTimeoutKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// textEnhance
            SDIInt current = device_data.textEnhance.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDITextEnhanceKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// textEnhanceNoiseReductionLevel
            SDIInt current = device_data.textEnhanceNoiseReductionLevel.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDITextEnhanceNoiseReductionLevelKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// textEnhanceSensitivity
            SDIInt current = device_data.textEnhanceSensitivity.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDITextEnhanceSensitivityKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// textEnhancePaperCreaseReduction
            SDIInt current = device_data.textEnhancePaperCreaseReduction.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDITextEnhancePaperCreaseReductionKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// thresholdAdjustment
            SDIInt current = device_data.thresholdAdjustment.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDITextEnhanceThresholdAdjustmentKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        /*
        {// SensorGlassDirtSensitivity
            SDIInt current = device_data.SensorGlassDirtSensitivity.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDISensorGlassDirtSensitivity, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// ADFPaperProtection
            SDIInt current = device_data.ADFPaperProtection.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIADFPaperProtectionKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// BehaviorWhenDoubleFeed
            SDIInt current = device_data.BehaviorWhenDoubleFeed.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIBehaviorWhenDoubleFeedKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// DirectPowerOn
            SDIInt current = device_data.DirectPowerOn.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIDirectPowerOnKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// NonConnectPowerOff
            SDIInt current = device_data.NonConnectPowerOff.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDINonConnectPowerOffKey, kSDIValueTypeInt, &current, sizeof(current)));
        }*/
/*
#ifndef BUILDSANE
        Configuration& config = Configuration::GetConfiguration();
        {// SleepTime
            SDIInt current =  config.config_para.sleep_timer;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDISleepTimeKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// AutoPowerOffTime
            SDIInt current =  config.config_para.ac_power;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIAutoPowerOffTimeKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// AutoPowerOffTimeBattery
            SDIInt current =  config.config_para.usb_power;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIAutoPowerOffTimeBatteryKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
#endif
*/
    }
    catch(bool error)
    {
        return error;
    }

    return true;
}

/*
bool Supervisor::Reset_RollerCouunter_Value(void)
{
    if(!driver){
        return false;
    }
    try{
        {// RollerKitCounter
            SDIInt current =  device_data.ConfigureCounter.RollerKitCounter.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIRollerKitCounterKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// RollerCounter
            SDIInt current =  device_data.ConfigureCounter.RollerCounter.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIRollerCounterKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// RetardRollerCounter
            SDIInt current =  device_data.ConfigureCounter.RetardRollerCounter.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIRetardRollerCounterKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
#ifndef BUILDSANE
        Configuration& config = Configuration::GetConfiguration();
        {// CleaningWarningCounter
            SDIInt current = config.config_para.cleaning_warning_counter;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDICleaningWarningCounterKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
#endif
    }
    catch(bool error)
    {
        return error;
    }

    return true;
}

bool Supervisor::Set_RollerCouunter_Value(void)
{
    if(!driver){
        return false;
    }
    try{
#ifndef BUILDSANE
        Configuration& config = Configuration::GetConfiguration();
        {// CleaningWarningNotifyCount
            SDIInt current = config.config_para.cleaning_warning_notify_counter;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDICleaningWarningNotifyCountKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// RollerKitNotifyCount
            SDIInt current = config.config_para.roller_kit_notify_counter;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIRollerKitNotifyCountKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// CleaningWarningNotify
            SDIInt current = config.config_para.cleaning_warning_notify;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDICleaningWarningNotifyKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
#endif
    }
    catch(bool error)
    {
        return error;
    }

    return true;
}
*/

bool Supervisor::Get_Exclusion_Value(void)
{
    if(!driver){
        return false;
    }
    try{
        // Resolution
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIResolutionKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.Resolution.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIResolutionKey, &capability));

            device_data.Resolution.capability.version = capability.version;
            device_data.Resolution.capability.supportLevel = capability.supportLevel;
            device_data.Resolution.capability.capabilityType = capability.capabilityType;

            device_data.Resolution.capability.minValue = capability.minValue;
            device_data.Resolution.capability.maxValue = capability.maxValue;

            device_data.Resolution.capability.allMinValue = capability.allMinValue;
            device_data.Resolution.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.Resolution.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.Resolution.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.Resolution.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.Resolution.capability.countOfAllList = capability.countOfAllList;
        }

        // ScanAreaWidth
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIScanAreaWidthKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ScanArea.ScanAreaWidth.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIScanAreaWidthKey, &capability));

            device_data.ScanArea.ScanAreaWidth.capability.version = capability.version;
            device_data.ScanArea.ScanAreaWidth.capability.supportLevel = capability.supportLevel;
            device_data.ScanArea.ScanAreaWidth.capability.capabilityType = capability.capabilityType;

            device_data.ScanArea.ScanAreaWidth.capability.minValue = capability.minValue;
            device_data.ScanArea.ScanAreaWidth.capability.maxValue = capability.maxValue;

            device_data.ScanArea.ScanAreaWidth.capability.allMinValue = capability.allMinValue;
            device_data.ScanArea.ScanAreaWidth.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ScanArea.ScanAreaWidth.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ScanArea.ScanAreaWidth.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ScanArea.ScanAreaWidth.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ScanArea.ScanAreaWidth.capability.countOfAllList = capability.countOfAllList;
        }

        // ScanAreaHeight
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIScanAreaHeightKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ScanArea.ScanAreaHeight.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIScanAreaHeightKey, &capability));

            device_data.ScanArea.ScanAreaHeight.capability.version = capability.version;
            device_data.ScanArea.ScanAreaHeight.capability.supportLevel = capability.supportLevel;
            device_data.ScanArea.ScanAreaHeight.capability.capabilityType = capability.capabilityType;

            device_data.ScanArea.ScanAreaHeight.capability.minValue = capability.minValue;
            device_data.ScanArea.ScanAreaHeight.capability.maxValue = capability.maxValue;

            device_data.ScanArea.ScanAreaHeight.capability.allMinValue = capability.allMinValue;
            device_data.ScanArea.ScanAreaHeight.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ScanArea.ScanAreaHeight.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ScanArea.ScanAreaHeight.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ScanArea.ScanAreaHeight.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ScanArea.ScanAreaHeight.capability.countOfAllList = capability.countOfAllList;
        }

        // ScanAreaOffsetX
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIScanAreaOffsetXKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ScanArea.ScanAreaOffsetX.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIScanAreaOffsetXKey, &capability));

            device_data.ScanArea.ScanAreaOffsetX.capability.version = capability.version;
            device_data.ScanArea.ScanAreaOffsetX.capability.supportLevel = capability.supportLevel;
            device_data.ScanArea.ScanAreaOffsetX.capability.capabilityType = capability.capabilityType;

            device_data.ScanArea.ScanAreaOffsetX.capability.minValue = capability.minValue;
            device_data.ScanArea.ScanAreaOffsetX.capability.maxValue = capability.maxValue;

            device_data.ScanArea.ScanAreaOffsetX.capability.allMinValue = capability.allMinValue;
            device_data.ScanArea.ScanAreaOffsetX.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ScanArea.ScanAreaOffsetX.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ScanArea.ScanAreaOffsetX.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ScanArea.ScanAreaOffsetX.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ScanArea.ScanAreaOffsetX.capability.countOfAllList = capability.countOfAllList;
        }

        // ScanAreaOffsetY
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIScanAreaOffsetYKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ScanArea.ScanAreaOffsetY.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIScanAreaOffsetYKey, &capability));

            device_data.ScanArea.ScanAreaOffsetY.capability.version = capability.version;
            device_data.ScanArea.ScanAreaOffsetY.capability.supportLevel = capability.supportLevel;
            device_data.ScanArea.ScanAreaOffsetY.capability.capabilityType = capability.capabilityType;

            device_data.ScanArea.ScanAreaOffsetY.capability.minValue = capability.minValue;
            device_data.ScanArea.ScanAreaOffsetY.capability.maxValue = capability.maxValue;

            device_data.ScanArea.ScanAreaOffsetY.capability.allMinValue = capability.allMinValue;
            device_data.ScanArea.ScanAreaOffsetY.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ScanArea.ScanAreaOffsetY.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ScanArea.ScanAreaOffsetY.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ScanArea.ScanAreaOffsetY.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ScanArea.ScanAreaOffsetY.capability.countOfAllList = capability.countOfAllList;
        }

        // MaxScanAreaWidth
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIMaxScanAreaWidthKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ScanArea.MaxScanAreaWidth.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIMaxScanAreaWidthKey, &capability));

            device_data.ScanArea.MaxScanAreaWidth.capability.version = capability.version;
            device_data.ScanArea.MaxScanAreaWidth.capability.supportLevel = capability.supportLevel;
            device_data.ScanArea.MaxScanAreaWidth.capability.capabilityType = capability.capabilityType;

            device_data.ScanArea.MaxScanAreaWidth.capability.minValue = capability.minValue;
            device_data.ScanArea.MaxScanAreaWidth.capability.maxValue = capability.maxValue;

            device_data.ScanArea.MaxScanAreaWidth.capability.allMinValue = capability.allMinValue;
            device_data.ScanArea.MaxScanAreaWidth.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ScanArea.MaxScanAreaWidth.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ScanArea.MaxScanAreaWidth.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ScanArea.MaxScanAreaWidth.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ScanArea.MaxScanAreaWidth.capability.countOfAllList = capability.countOfAllList;
        }

        // MaxScanAreaHeight
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIMaxScanAreaHeightKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ScanArea.MaxScanAreaHeight.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIMaxScanAreaHeightKey, &capability));

            device_data.ScanArea.MaxScanAreaHeight.capability.version = capability.version;
            device_data.ScanArea.MaxScanAreaHeight.capability.supportLevel = capability.supportLevel;
            device_data.ScanArea.MaxScanAreaHeight.capability.capabilityType = capability.capabilityType;

            device_data.ScanArea.MaxScanAreaHeight.capability.minValue = capability.minValue;
            device_data.ScanArea.MaxScanAreaHeight.capability.maxValue = capability.maxValue;

            device_data.ScanArea.MaxScanAreaHeight.capability.allMinValue = capability.allMinValue;
            device_data.ScanArea.MaxScanAreaHeight.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ScanArea.MaxScanAreaHeight.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ScanArea.MaxScanAreaHeight.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ScanArea.MaxScanAreaHeight.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ScanArea.MaxScanAreaHeight.capability.countOfAllList = capability.countOfAllList;
        }

        // MaxScanAreaHeightInLongPaper
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIMaxScanAreaHeightInLongPaperKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ScanArea.MaxScanAreaHeightInLongPaper.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIMaxScanAreaHeightInLongPaperKey, &capability));

            device_data.ScanArea.MaxScanAreaHeightInLongPaper.capability.version = capability.version;
            device_data.ScanArea.MaxScanAreaHeightInLongPaper.capability.supportLevel = capability.supportLevel;
            device_data.ScanArea.MaxScanAreaHeightInLongPaper.capability.capabilityType = capability.capabilityType;

            device_data.ScanArea.MaxScanAreaHeightInLongPaper.capability.minValue = capability.minValue;
            device_data.ScanArea.MaxScanAreaHeightInLongPaper.capability.maxValue = capability.maxValue;

            device_data.ScanArea.MaxScanAreaHeightInLongPaper.capability.allMinValue = capability.allMinValue;
            device_data.ScanArea.MaxScanAreaHeightInLongPaper.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ScanArea.MaxScanAreaHeightInLongPaper.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ScanArea.MaxScanAreaHeightInLongPaper.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ScanArea.MaxScanAreaHeightInLongPaper.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ScanArea.MaxScanAreaHeightInLongPaper.capability.countOfAllList = capability.countOfAllList;
        }

        // DropoutColor
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIDropoutColorKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.DropoutColor.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIDropoutColorKey, &capability));

            device_data.DropoutColor.capability.version = capability.version;
            device_data.DropoutColor.capability.supportLevel = capability.supportLevel;
            device_data.DropoutColor.capability.capabilityType = capability.capabilityType;

            device_data.DropoutColor.capability.minValue = capability.minValue;
            device_data.DropoutColor.capability.maxValue = capability.maxValue;

            device_data.DropoutColor.capability.allMinValue = capability.allMinValue;
            device_data.DropoutColor.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.DropoutColor.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.DropoutColor.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.DropoutColor.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.DropoutColor.capability.countOfAllList = capability.countOfAllList;
        }

        // MultiTiffEnabled
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIMultiTiffEnabledKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ImageFormatSettings.MultiTiffEnabled.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIMultiTiffEnabledKey, &capability));

            device_data.ImageFormatSettings.MultiTiffEnabled.capability.version = capability.version;
            device_data.ImageFormatSettings.MultiTiffEnabled.capability.supportLevel = capability.supportLevel;
            device_data.ImageFormatSettings.MultiTiffEnabled.capability.capabilityType = capability.capabilityType;

            device_data.ImageFormatSettings.MultiTiffEnabled.capability.minValue = capability.minValue;
            device_data.ImageFormatSettings.MultiTiffEnabled.capability.maxValue = capability.maxValue;

            device_data.ImageFormatSettings.MultiTiffEnabled.capability.allMinValue = capability.allMinValue;
            device_data.ImageFormatSettings.MultiTiffEnabled.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ImageFormatSettings.MultiTiffEnabled.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ImageFormatSettings.MultiTiffEnabled.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ImageFormatSettings.MultiTiffEnabled.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ImageFormatSettings.MultiTiffEnabled.capability.countOfAllList = capability.countOfAllList;
        }

        // TiffCompression
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDITiffCompressionKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ImageFormatSettings.TiffCompression.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDITiffCompressionKey, &capability));

            device_data.ImageFormatSettings.TiffCompression.capability.version = capability.version;
            device_data.ImageFormatSettings.TiffCompression.capability.supportLevel = capability.supportLevel;
            device_data.ImageFormatSettings.TiffCompression.capability.capabilityType = capability.capabilityType;

            device_data.ImageFormatSettings.TiffCompression.capability.minValue = capability.minValue;
            device_data.ImageFormatSettings.TiffCompression.capability.maxValue = capability.maxValue;

            device_data.ImageFormatSettings.TiffCompression.capability.allMinValue = capability.allMinValue;
            device_data.ImageFormatSettings.TiffCompression.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ImageFormatSettings.TiffCompression.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ImageFormatSettings.TiffCompression.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ImageFormatSettings.TiffCompression.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ImageFormatSettings.TiffCompression.capability.countOfAllList = capability.countOfAllList;
        }

        // DuplexType
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIDuplexTypeKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.DuplexType.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIDuplexTypeKey, &capability));

            device_data.DuplexType.capability.version = capability.version;
            device_data.DuplexType.capability.supportLevel = capability.supportLevel;
            device_data.DuplexType.capability.capabilityType = capability.capabilityType;

            device_data.DuplexType.capability.minValue = capability.minValue;
            device_data.DuplexType.capability.maxValue = capability.maxValue;

            device_data.DuplexType.capability.allMinValue = capability.allMinValue;
            device_data.DuplexType.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.DuplexType.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.DuplexType.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.DuplexType.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.DuplexType.capability.countOfAllList = capability.countOfAllList;
        }

        // DoubleFeedDetection
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIDoubleFeedDetectionKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.DoubleFeedDetectionValue.DoubleFeedDetection.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIDoubleFeedDetectionKey, &capability));

            device_data.DoubleFeedDetectionValue.DoubleFeedDetection.capability.version = capability.version;
            device_data.DoubleFeedDetectionValue.DoubleFeedDetection.capability.supportLevel = capability.supportLevel;
            device_data.DoubleFeedDetectionValue.DoubleFeedDetection.capability.capabilityType = capability.capabilityType;

            device_data.DoubleFeedDetectionValue.DoubleFeedDetection.capability.minValue = capability.minValue;
            device_data.DoubleFeedDetectionValue.DoubleFeedDetection.capability.maxValue = capability.maxValue;

            device_data.DoubleFeedDetectionValue.DoubleFeedDetection.capability.allMinValue = capability.allMinValue;
            device_data.DoubleFeedDetectionValue.DoubleFeedDetection.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.DoubleFeedDetectionValue.DoubleFeedDetection.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.DoubleFeedDetectionValue.DoubleFeedDetection.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.DoubleFeedDetectionValue.DoubleFeedDetection.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.DoubleFeedDetectionValue.DoubleFeedDetection.capability.countOfAllList = capability.countOfAllList;
        }

        // DoubleFeedDetectionLevel
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIDoubleFeedDetectionLevelKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIDoubleFeedDetectionLevelKey, &capability));

            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel.capability.version = capability.version;
            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel.capability.supportLevel = capability.supportLevel;
            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel.capability.capabilityType = capability.capabilityType;

            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel.capability.minValue = capability.minValue;
            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel.capability.maxValue = capability.maxValue;

            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel.capability.allMinValue = capability.allMinValue;
            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel.capability.countOfAllList = capability.countOfAllList;
        }
        // DoubleFeedDetectionAreaMin
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIDoubleFeedDetectionAreaMinKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaMin.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIDoubleFeedDetectionAreaMinKey, &capability));

            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaMin.capability.version = capability.version;
            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaMin.capability.supportLevel = capability.supportLevel;
            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaMin.capability.capabilityType = capability.capabilityType;

            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaMin.capability.minValue = capability.minValue;
            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaMin.capability.maxValue = capability.maxValue;

            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaMin.capability.allMinValue = capability.allMinValue;
            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaMin.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaMin.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaMin.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaMin.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaMin.capability.countOfAllList = capability.countOfAllList;
        }

        // DoubleFeedDetectionAreaLength
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIDoubleFeedDetectionAreaLengthKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaLength.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIDoubleFeedDetectionAreaLengthKey, &capability));

            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaLength.capability.version = capability.version;
            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaLength.capability.supportLevel = capability.supportLevel;
            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaLength.capability.capabilityType = capability.capabilityType;

            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaLength.capability.minValue = capability.minValue;
            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaLength.capability.maxValue = capability.maxValue;

            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaLength.capability.allMinValue = capability.allMinValue;
            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaLength.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaLength.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaLength.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaLength.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaLength.capability.countOfAllList = capability.countOfAllList;
        }

        // BlankPageSkip
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIBlankPageSkipKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.BlankPageSkipKey.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIBlankPageSkipKey, &capability));

            device_data.BlankPageSkipKey.capability.version = capability.version;
            device_data.BlankPageSkipKey.capability.supportLevel = capability.supportLevel;
            device_data.BlankPageSkipKey.capability.capabilityType = capability.capabilityType;

            device_data.BlankPageSkipKey.capability.minValue = capability.minValue;
            device_data.BlankPageSkipKey.capability.maxValue = capability.maxValue;

            device_data.BlankPageSkipKey.capability.allMinValue = capability.allMinValue;
            device_data.BlankPageSkipKey.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.BlankPageSkipKey.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.BlankPageSkipKey.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.BlankPageSkipKey.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.BlankPageSkipKey.capability.countOfAllList = capability.countOfAllList;
        }

        // BlankPageSkipLevelKey
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIBlankPageSkipLevelKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.BlankPageSkipLevelKey.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIBlankPageSkipLevelKey, &capability));

            device_data.BlankPageSkipLevelKey.capability.version = capability.version;
            device_data.BlankPageSkipLevelKey.capability.supportLevel = capability.supportLevel;
            device_data.BlankPageSkipLevelKey.capability.capabilityType = capability.capabilityType;

            device_data.BlankPageSkipLevelKey.capability.minValue = capability.minValue;
            device_data.BlankPageSkipLevelKey.capability.maxValue = capability.maxValue;

            device_data.BlankPageSkipLevelKey.capability.allMinValue = capability.allMinValue;
            device_data.BlankPageSkipLevelKey.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.BlankPageSkipLevelKey.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.BlankPageSkipLevelKey.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.BlankPageSkipLevelKey.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.BlankPageSkipLevelKey.capability.countOfAllList = capability.countOfAllList;
        }

        // PaperDeskew
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIPaperDeskewKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.PaperDeskew.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIPaperDeskewKey, &capability));

            device_data.PaperDeskew.capability.version = capability.version;
            device_data.PaperDeskew.capability.supportLevel = capability.supportLevel;
            device_data.PaperDeskew.capability.capabilityType = capability.capabilityType;

            device_data.PaperDeskew.capability.minValue = capability.minValue;
            device_data.PaperDeskew.capability.maxValue = capability.maxValue;

            device_data.PaperDeskew.capability.allMinValue = capability.allMinValue;
            device_data.PaperDeskew.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.PaperDeskew.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.PaperDeskew.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.PaperDeskew.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.PaperDeskew.capability.countOfAllList = capability.countOfAllList;
        }

        // BackgroundRemoval
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIBackgroundRemovalKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.BackgroundRemoval.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIBackgroundRemovalKey, &capability));

            device_data.BackgroundRemoval.capability.version = capability.version;
            device_data.BackgroundRemoval.capability.supportLevel = capability.supportLevel;
            device_data.BackgroundRemoval.capability.capabilityType = capability.capabilityType;

            device_data.BackgroundRemoval.capability.minValue = capability.minValue;
            device_data.BackgroundRemoval.capability.maxValue = capability.maxValue;

            device_data.BackgroundRemoval.capability.allMinValue = capability.allMinValue;
            device_data.BackgroundRemoval.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.BackgroundRemoval.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.BackgroundRemoval.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.BackgroundRemoval.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.BackgroundRemoval.capability.countOfAllList = capability.countOfAllList;
        }

        // AutoSize
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIAutoSizeKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ScanArea.AutoSize.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIAutoSizeKey, &capability));

            device_data.ScanArea.AutoSize.capability.version = capability.version;
            device_data.ScanArea.AutoSize.capability.supportLevel = capability.supportLevel;
            device_data.ScanArea.AutoSize.capability.capabilityType = capability.capabilityType;

            device_data.ScanArea.AutoSize.capability.minValue = capability.minValue;
            device_data.ScanArea.AutoSize.capability.maxValue = capability.maxValue;

            device_data.ScanArea.AutoSize.capability.allMinValue = capability.allMinValue;
            device_data.ScanArea.AutoSize.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ScanArea.AutoSize.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ScanArea.AutoSize.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ScanArea.AutoSize.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ScanArea.AutoSize.capability.countOfAllList = capability.countOfAllList;
        }

        // PaperEndDetection
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIPaperEndDetectionKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ScanArea.PaperEndDetection.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIPaperEndDetectionKey, &capability));

            device_data.ScanArea.PaperEndDetection.capability.version = capability.version;
            device_data.ScanArea.PaperEndDetection.capability.supportLevel = capability.supportLevel;
            device_data.ScanArea.PaperEndDetection.capability.capabilityType = capability.capabilityType;

            device_data.ScanArea.PaperEndDetection.capability.minValue = capability.minValue;
            device_data.ScanArea.PaperEndDetection.capability.maxValue = capability.maxValue;

            device_data.ScanArea.PaperEndDetection.capability.allMinValue = capability.allMinValue;
            device_data.ScanArea.PaperEndDetection.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ScanArea.PaperEndDetection.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ScanArea.PaperEndDetection.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ScanArea.PaperEndDetection.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ScanArea.PaperEndDetection.capability.countOfAllList = capability.countOfAllList;
        }

        // TransferCompression
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDITransferCompressionKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.TransferCompression.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDITransferCompressionKey, &capability));

            device_data.TransferCompression.capability.version = capability.version;
            device_data.TransferCompression.capability.supportLevel = capability.supportLevel;
            device_data.TransferCompression.capability.capabilityType = capability.capabilityType;

            device_data.TransferCompression.capability.minValue = capability.minValue;
            device_data.TransferCompression.capability.maxValue = capability.maxValue;

            device_data.TransferCompression.capability.allMinValue = capability.allMinValue;
            device_data.TransferCompression.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.TransferCompression.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.TransferCompression.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.TransferCompression.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.TransferCompression.capability.countOfAllList = capability.countOfAllList;
        }

        // SleepTime
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDISleepTimeKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.SleepTime.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDISleepTimeKey, &capability));

            device_data.SleepTime.capability.version = capability.version;
            device_data.SleepTime.capability.supportLevel = capability.supportLevel;
            device_data.SleepTime.capability.capabilityType = capability.capabilityType;

            device_data.SleepTime.capability.minValue = capability.minValue;
            device_data.SleepTime.capability.maxValue = capability.maxValue;

            device_data.SleepTime.capability.allMinValue = capability.allMinValue;
            device_data.SleepTime.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.SleepTime.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.SleepTime.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.SleepTime.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.SleepTime.capability.countOfAllList = capability.countOfAllList;
        }

        // AutoPowerOffTime
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIAutoPowerOffTimeKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.AutoPowerOffTime.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIAutoPowerOffTimeKey, &capability));

            device_data.AutoPowerOffTime.capability.version = capability.version;
            device_data.AutoPowerOffTime.capability.supportLevel = capability.supportLevel;
            device_data.AutoPowerOffTime.capability.capabilityType = capability.capabilityType;

            device_data.AutoPowerOffTime.capability.minValue = capability.minValue;
            device_data.AutoPowerOffTime.capability.maxValue = capability.maxValue;

            device_data.AutoPowerOffTime.capability.allMinValue = capability.allMinValue;
            device_data.AutoPowerOffTime.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.AutoPowerOffTime.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.AutoPowerOffTime.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.AutoPowerOffTime.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.AutoPowerOffTime.capability.countOfAllList = capability.countOfAllList;
        }

        // AutoPowerOffTimeBattery
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIAutoPowerOffTimeBatteryKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.AutoPowerOffTimeBattery.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIAutoPowerOffTimeBatteryKey, &capability));

            device_data.AutoPowerOffTimeBattery.capability.version = capability.version;
            device_data.AutoPowerOffTimeBattery.capability.supportLevel = capability.supportLevel;
            device_data.AutoPowerOffTimeBattery.capability.capabilityType = capability.capabilityType;

            device_data.AutoPowerOffTimeBattery.capability.minValue = capability.minValue;
            device_data.AutoPowerOffTimeBattery.capability.maxValue = capability.maxValue;

            device_data.AutoPowerOffTimeBattery.capability.allMinValue = capability.allMinValue;
            device_data.AutoPowerOffTimeBattery.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.AutoPowerOffTimeBattery.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.AutoPowerOffTimeBattery.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.AutoPowerOffTimeBattery.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.AutoPowerOffTimeBattery.capability.countOfAllList = capability.countOfAllList;
        }

        // BackgroundColor
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIBackgroundColorKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.BackgroundColor.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIBackgroundColorKey, &capability));

            device_data.BackgroundColor.capability.version = capability.version;
            device_data.BackgroundColor.capability.supportLevel = capability.supportLevel;
            device_data.BackgroundColor.capability.capabilityType = capability.capabilityType;

            device_data.BackgroundColor.capability.minValue = capability.minValue;
            device_data.BackgroundColor.capability.maxValue = capability.maxValue;

            device_data.BackgroundColor.capability.allMinValue = capability.allMinValue;
            device_data.BackgroundColor.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.BackgroundColor.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.BackgroundColor.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.BackgroundColor.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.BackgroundColor.capability.countOfAllList = capability.countOfAllList;
        }

        // AFMTimeout
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIAFMTimeoutKey, kSDIValueTypeInt, &out, sizeof(out)));

            //device_data.AFMTimeout.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIAFMTimeoutKey, &capability));

            device_data.AFMTimeout.capability.version = capability.version;
            device_data.AFMTimeout.capability.supportLevel = capability.supportLevel;
            device_data.AFMTimeout.capability.capabilityType = capability.capabilityType;

            device_data.AFMTimeout.capability.minValue = capability.minValue;
            device_data.AFMTimeout.capability.maxValue = capability.maxValue;

            device_data.AFMTimeout.capability.allMinValue = capability.allMinValue;
            device_data.AFMTimeout.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.AFMTimeout.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.AFMTimeout.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.AFMTimeout.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.AFMTimeout.capability.countOfAllList = capability.countOfAllList;
        }

        // ADFCard
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIADFCardKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ScanArea.ADFCard.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIADFCardKey, &capability));

            device_data.ScanArea.ADFCard.capability.version = capability.version;
            device_data.ScanArea.ADFCard.capability.supportLevel = capability.supportLevel;
            device_data.ScanArea.ADFCard.capability.capabilityType = capability.capabilityType;

            device_data.ScanArea.ADFCard.capability.minValue = capability.minValue;
            device_data.ScanArea.ADFCard.capability.maxValue = capability.maxValue;

            device_data.ScanArea.ADFCard.capability.allMinValue = capability.allMinValue;
            device_data.ScanArea.ADFCard.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ScanArea.ADFCard.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ScanArea.ADFCard.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ScanArea.ADFCard.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ScanArea.ADFCard.capability.countOfAllList = capability.countOfAllList;
        }

        // SensorGlassDirtSensitivity
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDISensorGlassDirtSensitivity , kSDIValueTypeInt, &out, sizeof(out)));

            device_data.SensorGlassDirtSensitivity.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDISensorGlassDirtSensitivity , &capability));

            device_data.SensorGlassDirtSensitivity.capability.version = capability.version;
            device_data.SensorGlassDirtSensitivity.capability.supportLevel = capability.supportLevel;
            device_data.SensorGlassDirtSensitivity.capability.capabilityType = capability.capabilityType;

            device_data.SensorGlassDirtSensitivity.capability.minValue = capability.minValue;
            device_data.SensorGlassDirtSensitivity.capability.maxValue = capability.maxValue;

            device_data.SensorGlassDirtSensitivity.capability.allMinValue = capability.allMinValue;
            device_data.SensorGlassDirtSensitivity.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.SensorGlassDirtSensitivity.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.SensorGlassDirtSensitivity.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.SensorGlassDirtSensitivity.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.SensorGlassDirtSensitivity.capability.countOfAllList = capability.countOfAllList;
        }


        // ADFPaperProtection
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIADFPaperProtectionKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.ADFPaperProtection.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIADFPaperProtectionKey, &capability));

            device_data.ADFPaperProtection.capability.version = capability.version;
            device_data.ADFPaperProtection.capability.supportLevel = capability.supportLevel;
            device_data.ADFPaperProtection.capability.capabilityType = capability.capabilityType;

            device_data.ADFPaperProtection.capability.minValue = capability.minValue;
            device_data.ADFPaperProtection.capability.maxValue = capability.maxValue;

            device_data.ADFPaperProtection.capability.allMinValue = capability.allMinValue;
            device_data.ADFPaperProtection.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.ADFPaperProtection.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.ADFPaperProtection.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.ADFPaperProtection.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.ADFPaperProtection.capability.countOfAllList = capability.countOfAllList;
        }

        // BehaviorWhenDoubleFeed
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIBehaviorWhenDoubleFeedKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.BehaviorWhenDoubleFeed.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIBehaviorWhenDoubleFeedKey, &capability));

            device_data.BehaviorWhenDoubleFeed.capability.version = capability.version;
            device_data.BehaviorWhenDoubleFeed.capability.supportLevel = capability.supportLevel;
            device_data.BehaviorWhenDoubleFeed.capability.capabilityType = capability.capabilityType;

            device_data.BehaviorWhenDoubleFeed.capability.minValue = capability.minValue;
            device_data.BehaviorWhenDoubleFeed.capability.maxValue = capability.maxValue;

            device_data.BehaviorWhenDoubleFeed.capability.allMinValue = capability.allMinValue;
            device_data.BehaviorWhenDoubleFeed.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.BehaviorWhenDoubleFeed.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.BehaviorWhenDoubleFeed.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.BehaviorWhenDoubleFeed.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.BehaviorWhenDoubleFeed.capability.countOfAllList = capability.countOfAllList;
        }

        // DirectPowerOn
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIDirectPowerOnKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.DirectPowerOn.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDIDirectPowerOnKey, &capability));

            device_data.DirectPowerOn.capability.version = capability.version;
            device_data.DirectPowerOn.capability.supportLevel = capability.supportLevel;
            device_data.DirectPowerOn.capability.capabilityType = capability.capabilityType;

            device_data.DirectPowerOn.capability.minValue = capability.minValue;
            device_data.DirectPowerOn.capability.maxValue = capability.maxValue;

            device_data.DirectPowerOn.capability.allMinValue = capability.allMinValue;
            device_data.DirectPowerOn.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.DirectPowerOn.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.DirectPowerOn.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.DirectPowerOn.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.DirectPowerOn.capability.countOfAllList = capability.countOfAllList;
        }

        // NonConnectPowerOff
        {
            SDIInt out     = (SDIInt)-1;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDINonConnectPowerOffKey, kSDIValueTypeInt, &out, sizeof(out)));

            device_data.NonConnectPowerOff.select = out;

            SDICapability capability = {0};
            CHECK_ERROR(SDIScannerDriver_GetCapability_(driver, kSDINonConnectPowerOffKey, &capability));

            device_data.NonConnectPowerOff.capability.version = capability.version;
            device_data.NonConnectPowerOff.capability.supportLevel = capability.supportLevel;
            device_data.NonConnectPowerOff.capability.capabilityType = capability.capabilityType;

            device_data.NonConnectPowerOff.capability.minValue = capability.minValue;
            device_data.NonConnectPowerOff.capability.maxValue = capability.maxValue;

            device_data.NonConnectPowerOff.capability.allMinValue = capability.allMinValue;
            device_data.NonConnectPowerOff.capability.allMaxValue = capability.allMaxValue;

            if(capability.countOfList > 0){
                memcpy(device_data.NonConnectPowerOff.capability.list, capability.list, capability.countOfList * sizeof(SDIInt));
            }
            device_data.NonConnectPowerOff.capability.countOfList = capability.countOfList;

            if(capability.countOfAllList > 0){
                memcpy(device_data.NonConnectPowerOff.capability.allList, capability.allList, capability.countOfAllList * sizeof(SDIInt));
            }
            device_data.NonConnectPowerOff.capability.countOfAllList = capability.countOfAllList;
        }
    }
    catch(bool error)
    {
        return error;
    }

    return true;
}

bool Supervisor::Set_Exclusion_Value(void)
{
    if(!driver){
        return false;
    }
    try{
        {// functionalunit
            SDIInt current;
            if(device_data.FunctionalUnit_Auto){
                current = ADF;
            }else {
                current = device_data.FunctionalUnit.select;
            }
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIFunctionalUnitKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// ColorType
            SDIInt current = device_data.ColorType.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIColorTypeKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// Resolution
            SDIInt current = device_data.Resolution.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIResolutionKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// Width
            SDIInt current =  device_data.ScanArea.ScanAreaWidth.select ;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIScanAreaWidthKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// Height
            SDIInt current =  device_data.ScanArea.ScanAreaHeight.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIScanAreaHeightKey, kSDIValueTypeInt, &current, sizeof(current)));
        }

        {// X
            SDIInt current = device_data.ScanArea.ScanAreaOffsetX.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIScanAreaOffsetXKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// Y
            SDIInt current = device_data.ScanArea.ScanAreaOffsetY.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIScanAreaOffsetYKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// ADFHeightMin
            SDIInt current = device_data.ScanArea.ADFHeightMin.select;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIADFHeightMinKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// ADFDuplexHeightMin
            SDIInt current = device_data.ScanArea.ADFDuplexHeightMin.select;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIADFDuplexHeightMinKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// ADFWidthMin
            SDIInt current = device_data.ScanArea.ADFWidthMin.select;
            CHECK_ERROR(SDIScannerDriver_GetValuePtr_(driver, kSDIADFWidthMinKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// DropoutColor
            SDIInt current = device_data.DropoutColor.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIDropoutColorKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// Orientation
            SDIInt current = device_data.Orientation.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIOrientationKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// MultiTiffEnabled
            SDIInt current =  device_data.ImageFormatSettings.MultiTiffEnabled.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIMultiTiffEnabledKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// TiffCompression
            SDIInt current =  device_data.ImageFormatSettings.TiffCompression.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDITiffCompressionKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// Contrast
            SDIInt current =  device_data.Contrast.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIContrastKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// Brightness
            SDIInt current =  device_data.Brightness.select;
            CHECK_ERROR( SDIScannerDriver_SetValuePtr_(driver, kSDIBrightnessKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// Threshold
            SDIInt current =  device_data.Threshold.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIThresholdKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// JpegQuality
            if(device_data.ImageFormatSettings.ImageFormat.select == ImageFormatJPEG){
                device_data.ImageFormatSettings.JpegQuality.select = device_data.ImageFormatSettings.JpegQuality_forJPEG;
            }else if(device_data.ImageFormatSettings.ImageFormat.select == ImageFormatColorJpegMonoRaw){
                device_data.ImageFormatSettings.JpegQuality.select = device_data.ImageFormatSettings.JpegQuality_forPDF;
            }else {
                device_data.ImageFormatSettings.JpegQuality.select = 0;
            }
            SDIInt current =  device_data.ImageFormatSettings.JpegQuality.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIJpegQuality, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// jpegProgressive
            SDIInt current =  device_data.ImageFormatSettings.JpegProgressive.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIJpegProgressiveKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// DuplexType
            SDIInt current =  device_data.DuplexType.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIDuplexTypeKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// PagesTobeScanned
            SDIInt current =  device_data.PagesTobeScanned.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIPagesTobeScannedKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// DoubleFeedDetectionLevel
            SDIInt current =  device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIDoubleFeedDetectionLevelKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// DoubleFeedDetection
            SDIInt current =  device_data.DoubleFeedDetectionValue.DoubleFeedDetection.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIDoubleFeedDetectionKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// DoubleFeedDetectionAreaMin
            SDIInt current =  device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaMin.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIDoubleFeedDetectionAreaMinKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// DoubleFeedDetectionAreaLength
            SDIInt current =  device_data.DoubleFeedDetectionValue.DoubleFeedDetectionAreaLength.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIDoubleFeedDetectionAreaLengthKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// BlankPageSkip
            SDIInt current =  device_data.BlankPageSkipKey.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIBlankPageSkipKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// BlankPageSkipLevel
            SDIInt current = device_data.BlankPageSkipLevelKey.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIBlankPageSkipLevelKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// BackgroundRemoval
            SDIInt current =  device_data.BackgroundRemoval.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIBackgroundRemovalKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// PaperDeskew
            SDIInt current =  device_data.PaperDeskew.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIPaperDeskewKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// AutoSize
            SDIInt current =  device_data.ScanArea.AutoSize.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIAutoSizeKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// PaperEndDetection
            SDIInt current;
            if(device_data.ScanArea.FixedSize == USER_DEFINE){
                current =  device_data.ScanArea.PaperEndDetection.select;
            }else {
                current = false;
            }
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIPaperEndDetectionKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// BackgroundColor
            SDIInt current = device_data.BackgroundColor.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIBackgroundColorKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// AFMTimeout
            SDIInt current = device_data.AFMTimeout.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIAFMTimeoutKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// TransferCompression
            SDIInt current =  device_data.TransferCompression.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDITransferCompressionKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// SensorGlassDirtSensitivity
            SDIInt current = device_data.SensorGlassDirtSensitivity.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDISensorGlassDirtSensitivity, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// ADFPaperProtection
            SDIInt current = device_data.ADFPaperProtection.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIADFPaperProtectionKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// BehaviorWhenDoubleFeed
            SDIInt current = device_data.BehaviorWhenDoubleFeed.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIBehaviorWhenDoubleFeedKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// DirectPowerOn
            SDIInt current = device_data.DirectPowerOn.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIDirectPowerOnKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// NonConnectPowerOff
            SDIInt current = device_data.NonConnectPowerOff.select;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDINonConnectPowerOffKey, kSDIValueTypeInt, &current, sizeof(current)));
        }

#ifndef BUILDSANE
        Configuration& config = Configuration::GetConfiguration();
        {// SleepTime
            SDIInt current =  config.config_para.sleep_timer;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDISleepTimeKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// AutoPowerOffTime
            SDIInt current =  config.config_para.ac_power;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIAutoPowerOffTimeKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
        {// AutoPowerOffTimeBattery
            SDIInt current =  config.config_para.usb_power;
            CHECK_ERROR(SDIScannerDriver_SetValuePtr_(driver, kSDIAutoPowerOffTimeBatteryKey, kSDIValueTypeInt, &current, sizeof(current)));
        }
#endif

    }
    catch(bool error)
    {
        return error;
    }

    return true;
}

void Supervisor::setCallback(DWORD Type, PVOID pCbFunc, PVOID pPrivate)
{
	// Cache callback function pointer and private data
    switch (Type) {
	case CBDT_STATUS_SCAN:
        m_pCbStFunc = (supervisorStatusCallback)pCbFunc;
        break;
    case CBDT_IMAGE_FILE:
        m_pCbImgFunc = (supervisorImageCallback)pCbFunc;
        break;
    default:
		return;
	}
	m_pPrivate = pPrivate;
}

BOOL Supervisor::sendAppCallbackScanStatus(DWORD Type, DWORD Status, DWORD Count, LONG Progress)
{
	// Invoke callback function to notify end of page
	if (this->m_pCbStFunc) {
		EslCBDataScanStatus status;
		status.type = Type;
		status.size = sizeof(status);
		status.status = Status;
		status.progress = Progress;
		status.count = Count;
		return this->m_pCbStFunc((EslCBDataScanStatus*)&status, m_pPrivate);
	}

	return TRUE;
}

BOOL Supervisor::sendAppCallbackImageFile(DWORD Type, DWORD Status, LPCTSTR FileName)
{
	// Invoke callback function to notify file name of image
	if (this->m_pCbImgFunc) {
		EslCBDImageFile status;
		status.type = Type;
		status.size = sizeof(status);
		status.status = Status;
		//StringCbCopy(status.filename, sizeof(status.filename), FileName);
		strncpy((char *)status.filename, FileName, sizeof(status.filename));
		return this->m_pCbImgFunc((EslCBDImageFile*)&status, m_pPrivate);
	}

	return TRUE;
}