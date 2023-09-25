//#include <algorithm>
//#include <csignal>
//#include <cstdio>
//#include <cstdlib>
//#include <locale>
//#include <new>
//#include <set>
//#include <stdexcept>
//#include <string>

//#include <string.h>
//#include <dlfcn.h>

//#include <boost/preprocessor/stringize.hpp>
//#include <boost/static_assert.hpp>

#include "backend.h"
#include "version.hpp"
#include "assert.h"

#include "epsonscan.h"
#include "list.h"
#include "loadnetworksetting.h"

#include <sstream>

//#include "guard.hpp"
//#include "handle.hpp"
//#include "device.hpp"

/* Deprecated API calls */
extern SANE_Status epsonscan_open (const char *, SANE_Handle *);

typedef struct
{
  void  *cfg;
  void  *net;
  list  *sane_dev;
  void **dev_list;

} backend_type;

static backend_type *epson_backend = NULL;

static void be_sane_dev_dtor (void *);

//! Communicates backend constructor check results to sane_init()
static bool backend_is_usable = true;

//! Remembers the authorization callback passed to sane_init()
static SANE_Auth_Callback auth_cb = NULL;


extern "C" {

SANE_Status check_error (SDIError inError);

/*! \defgroup SANE_API  SANE API Entry Points
 *
 *  The SANE API entry points make up the \e full API available to the
 *  SANE frontend application programmer.  Users of this API should be
 *  careful \e never to assume \e anything about a backend's behaviour
 *  beyond what is required by the SANE standard.  The standard can be
 *  retrieved via http://sane.alioth.debian.org/docs.html on the SANE
 *  project's web site.
 *
 *  Whatever documentation may be provided here serves to document the
 *  implementation, if anything.  In case of discrepancy with the SANE
 *  specification, the SANE specification is correct.
 *
 *  @{
 */

//! Prepares the backend for use by a SANE frontend
/*! \remarks
 *  This function \e must be called before any other SANE API entry is
 *  called.  It is the only SANE function that may be called after the
 *  sane_exit() function has been called.
 */
SANE_Status
sane_init (SANE_Int *version_code, SANE_Auth_Callback authorize)
{
  SANE_Status status = SANE_STATUS_GOOD;
  msg_init ();
  log_call("--------------sane_init--------------");

  /*! \todo  Decide whether to return a version code and/or reset the
   *         authorization callback when called without an intervening
   *         call to sane_exit().
   */

  if(epson_backend)
  {
      return status;
  }

  if (version_code)
    {
      *version_code = SANE_VERSION_CODE (BACKEND_MAJOR, BACKEND_MINOR,
                                             BACKEND_BUILD);
    }

  epson_backend = t_calloc (1, backend_type);
  if (!epson_backend)
    {
      return SANE_STATUS_NO_MEM;
    }

  auth_cb = authorize;

  return SANE_STATUS_GOOD;
}

//! Releases all resources held by the backend
/*! \remarks
 *  Applications \e must call this function to terminate use of the
 *  backend.  After it has been called, sane_init() has to be called
 *  before other SANE API can be used.  The function needs to close
 *  any open handles.
 *
 *  \remarks
 *  The implementation must be able to deal properly with a partially
 *  initialised backend so that sane_init() can use this function for
 *  its error recovery.
 */
void
sane_exit (void)
{
  log_call("--------------sane_exit--------------");

  if (!epson_backend)
    {
      return;
    }

  //! \todo  Add bit flipping support to log::category
  // log::matching &= ~log::SANE_BACKEND;

  delete (epson_backend->dev_list);
  list_destroy (epson_backend->sane_dev, be_sane_dev_dtor);

  if(epson_backend){
    delete (epson_backend);
    epson_backend = NULL;
  }

  return;
}

//! Creates a list of devices available through the backend
/*! \remarks
 *  The returned \a device_list \e must remain unchanged and valid
 *  until this function is called again or sane_exit() is called.
 *
 *  \remarks
 *  When returning successfully, the \a device_list points to a \c
 *  NULL terminated list of \c SANE_Device pointers.
 *
 *  \remarks
 *  Applications are \e not required to call this function before
 *  they call sane_open().
 */
#include <iostream>

SANE_Status
sane_get_devices (const SANE_Device ***device_list, SANE_Bool local_only)
{
  log_call("--------------sane_get_devices--------------");
  SANE_Status status = SANE_STATUS_GOOD;
  list *sane_dev = NULL;

  sane_dev = list_create ();

  Supervisor* sv = new Supervisor();
  sv->SetUp();
/*
typedef struct {
	SDIInt version;			     	  	   // out  version of this struct definition
	char modelID[MAX_MODEL_ID];   	   // out ES00xx..
	SDIInt  productID;   				   // out usb pid
	char ipAddress[MAX_IP_ADDR];  	   // out 192..
	char displayName[MAX_DISPLAYNAME]; // displayname
}SDIDeviceInfo;
*/
  if (sane_dev)
  {
    SDIInt count = 0;

    SDIDeviceFinder* finder = nullptr;
    sv->SDIDeviceFinder_CreatePtr_(&finder);

    sv->SDIDeviceFinder_StartDiscoveryPtr_(finder, nullptr, nullptr);
    sv->SDIDeviceFinder_StopDiscoveryPtr_(finder);

    SDIDeviceInfo* sdidevices = nullptr;
    sv->SDIDeviceFinder_GetDevicesPtr_(finder, &sdidevices, &count);


     std::list<SDIDeviceInfo> netdevs;
     GetNetworkDevcicesFromIni(netdevs);
    
    // add USB Devices
    for (int i = 0; i < count; i++)
    {
      SANE_Device* dev = (SANE_Device*)malloc(sizeof(SANE_Device));
      if(!dev){
        status = SANE_STATUS_NO_MEM;
        goto DELETE;
      }
      { // setup name
        std::stringstream nameStream;
        nameStream << sdidevices[i].displayName;
        nameStream << ":esci2:usb:";
        nameStream << sdidevices[i].modelID;
        nameStream << ":";
        nameStream << sdidevices[i].productID;
        dev->name = (SANE_String)malloc(nameStream.str().length() + 1);
        memset((void*)dev->name, 0, nameStream.str().length() + 1);
        strcpy((char*)dev->name, nameStream.str().c_str() );
      }
      {
#ifdef AKBMODE
        std::string vender = V_NAME;
#else
        std::string vender = "EPSON";
#endif
         dev->vendor = (SANE_String)malloc(vender.length() + 1);
         memset((void*)dev->vendor, 0, vender.length() + 1);
         strcpy((char*)dev->vendor, vender.c_str() );
      }
      {
         std::string model = sdidevices[i].displayName;;
         dev->model = (SANE_String)malloc(model.length() + 1);
         memset((void*)dev->model, 0, model.length() + 1);
         strcpy((char*)dev->model, model.c_str() );
      }
      {
         std::string type = "flatbed scanner";
         dev->type = (SANE_String)malloc(type.length() + 1);
         memset((void*)dev->type, 0, type.length() + 1);
         strcpy((char*)dev->type, type.c_str() );
      }
     list_append(sane_dev, dev);
    }

    // add net devices
    for (auto netdev : netdevs)
    {
      SANE_Device* dev = (SANE_Device*)malloc(sizeof(SANE_Device));
      if(!dev){
        status = SANE_STATUS_NO_MEM;
        goto DELETE;
      } 
      { // setup name
        std::stringstream nameStream;
        nameStream << "networkscanner:esci2:network:";
        nameStream << netdev.ipAddress;
        dev->name = (SANE_String)malloc(nameStream.str().length() + 1);
        memset((void*)dev->name, 0, nameStream.str().length() + 1);
        strcpy((char*)dev->name, nameStream.str().c_str() );
      }
      {
#ifdef AKBMODE
        std::string vender = V_NAME;
#else
        std::string vender = "EPSON";
#endif
         dev->vendor = (SANE_String)malloc(vender.length() + 1);
         memset((void*)dev->vendor, 0, vender.length() + 1);
         strcpy((char*)dev->vendor, vender.c_str() );
      }
      {
         std::string model = "network scanner";;
         dev->model = (SANE_String)malloc(model.length() + 1);
         memset((void*)dev->model, 0, model.length() + 1);
         strcpy((char*)dev->model, model.c_str() );
      }
      {
         std::string type = "flatbed scanner";
         dev->type = (SANE_String)malloc(type.length() + 1);
         memset((void*)dev->type, 0, type.length() + 1);
         strcpy((char*)dev->type, type.c_str() );
      }
      list_append(sane_dev, dev);
    }

    if (epson_backend->sane_dev)
    {
      delete (epson_backend->dev_list);
      list_destroy (epson_backend->sane_dev, be_sane_dev_dtor);
    }
    epson_backend->sane_dev = sane_dev;

     
    sv->SDIDeviceFinder_DisposePtr_(finder);
    finder = nullptr;
  }
  
  epson_backend->dev_list = list_normalize (epson_backend->sane_dev); //リストの最後をNULLにしなければならない
  *device_list = (const SANE_Device **)epson_backend->dev_list;
  if (!*device_list)
  {
    status = SANE_STATUS_NO_MEM;
  }

  delete sv;
  sv = nullptr;

  return status; 
  
DELETE:

  list_destroy (sane_dev, be_sane_dev_dtor);
  sane_dev = NULL;

  delete sv;
  sv = nullptr;

  return status; 
}

//! Establishes a connection to a named device
/*! \remarks
 *  Applications are allowed to call this function directly, without a
 *  call to sane_get_devices() first.  An empty string may be used for
 *  the \a device_name to request the first available device.
 *
 *  \remarks
 *  The SANE specification says nothing about required behaviour when
 *  the frontend passes a \c NULL \a device_name, only when the empty
 *  string is passed do we have to do something special.  We degrade
 *  gracefully anyway and treat \c NULL as if it were an empty string.
 */
SANE_Status
sane_open (SANE_String_Const device_name, SANE_Handle *handle)
{
  log_call("--------------sane_open--------------");
  log_call("%s", device_name);

  const SANE_Device *sane_dev = NULL;

  if (!epson_backend)
  {
    return SANE_STATUS_ACCESS_DENIED;
  }

  if (!handle)
  {
    return SANE_STATUS_INVAL;
  }

  return  epsonscan_open(device_name, handle);
}

void
dip_flip_bits (SANE_Byte *buf, SANE_Int total_size)
{
  SANE_Byte *p;

  int i = 0;
  p = buf;
  while (i < total_size)
    {
      *p = ~*p;
      ++p;
      i++;
    }
}

SANE_Status
check_error (SDIError inError)
{
  SANE_Status status = SANE_STATUS_GOOD;
  log_call("Error Code : %d", inError);

  if(inError != kSDIErrorNone){
    if(inError == kSDIErrorPaperEmpty){
      //原稿なしエラー
      log_call("ERROR : Load the originals in the ADF.");
      status = SANE_STATUS_NO_DOCS;
    }else if(inError == kSDIErrorPaperJam){
      //紙ジャムエラー時
      log_call("ERROR : A paper jam has occurred. See the documentation for instructions on removing your originals.");
      status = SANE_STATUS_JAMMED;
    }else if(inError == kSDIErrorPaperDoubleFeed){
      //重送エラー時
      log_call("ERROR : Double feed detected. See the documentation for instructions on removing your originals.");
      status = SANE_STATUS_JAMMED;
    }else if(inError == kSDIErrorPaperDoubleFeed2){
      //重送エラー(Mode2)時
      log_call("ERROR : Double feed detected.");
      status = SANE_STATUS_JAMMED;
    }else if(inError == kSDIErrorPaperProtect){
      //原稿保護エラー時
      log_call("ERROR : A paper jam has occurred. See the documentation for instructions on removing your originals.");
      status = SANE_STATUS_JAMMED;
    }else  if(inError == kSDIErrorDeviceInBusy){
      log_call("ERROR : kSDIErrorDeviceInBusy");
      status = SANE_STATUS_DEVICE_BUSY;
    }else  if(inError == kSDIErrorCoverOpen){
      //ADFカバーオープンエラー
      log_call("ERROR : ADF or ADF cover is open. Close it and reload the originals.");
      status = SANE_STATUS_COVER_OPEN;
    }else  if(inError == kSDIErrorTrayClose){
      //給紙トレークローズエラー
      log_call("ERROR : The input tray is closed. Open the input tray.");
      status = SANE_STATUS_INVAL;
    }else if(inError == kSDIErrorDataSend){
      //データ送信エラー
      log_call("ERROR : Unable to send data. Check the connection to the scanner and try again.");
      status = SANE_STATUS_IO_ERROR;
    }else if(inError == kSDIErrorDataReceive){
      //データ受信エラー
      log_call("ERROR : Unable to receive data. Check the connection to the scanner and try again.");
      status = SANE_STATUS_IO_ERROR;
    }else if(inError == kSDIErrorDeviceInUse){
      //スキャナーが使用中または使用できないエラー
      log_call("ERROR : The scanner is in use or unavailable. Please wait.");
      status = SANE_STATUS_DEVICE_BUSY;
    }else  if(inError == kSDIErrorNoMemory){
      //メモリ不足エラー
#ifdef AKBMODE
      log_call("ERROR : Not enough memory for Scanner Driver. Close other applications and try again.");
#else
      log_call("ERROR : Not enough memory for Epson Scan 2. Close other applications and try again.");
#endif
      status = SANE_STATUS_NO_MEM;
    }else if(inError == kSDIErrorUnknownError){
      //予期せぬエラー・不明なエラー
#ifdef AKBMODE
      log_call("ERROR : An unexpected error occurred. Scanner Driver will close.");
#else
      log_call("ERROR : An unexpected error occurred. Epson Scan 2 will close.");
#endif
      status = SANE_STATUS_INVAL;
    }else if(inError == kSDIErrorDeviceFormChangedInterruptedly){
      //デバイスの姿勢がスキャン中に変更されたエラー
      log_call("ERROR : DeviceFormChangedInterruptedly");
      status = SANE_STATUS_INVAL;
    }else if(inError == kSDIErrorDeviceFormUnstable){
      //デバイスの姿勢が不定状態エラー
      log_call("ERROR : DeviceFormUnstable");
      status = SANE_STATUS_INVAL;
    }else if(inError == kSDIErrorSepLeverChangedInterruptedly){
      //原稿分離レバーがスキャン中に変更されたエラー
      log_call("ERROR : SepLeverChangedInterruptedly");
      status = SANE_STATUS_INVAL;
    }else if(inError == kSDIErrorUnscannableDeviceConfig1){
      //スキャナの姿勢と分離レバー組み合わせエラー
      log_call("ERROR : UnscannableDeviceConfig");
      status = SANE_STATUS_INVAL;
    }else if(inError == kSDIErrorETSensorError){
      //ET Sensor エラー
      log_call("ERROR : ETSensor");
      status = SANE_STATUS_INVAL;
    }else if(inError == kSDIErrorUserAuthEnabled){
      //本体認証モードエラー
      log_call("ERROR : UserAuthEnabled");
      status = SANE_STATUS_INVAL;
    }else {
      //予期せぬエラー・不明なエラー
#ifdef AKBMODE
      log_call("ERROR : An unexpected error occurred. Scanner Driver will close.");
#else
      log_call("ERROR : An unexpected error occurred. Epson Scan 2 will close.");
#endif
      status = SANE_STATUS_INVAL;
    }
  }

  return status;
}

double calculate_pixel_size(int base_res, double value)
{
    //mm -> pixel
    double mpp = 25.4 / base_res;
    return value / mpp;
}

#define BASE_RESOLUTION		100	/* dpi */

bool handle_scan_area (Epson_Scanner *s, double* current_pixel_x, double* current_pixel_y)
{
  int base_res = s->hw->sv->device_data.Resolution.select;
  *current_pixel_x = s->hw->sv->device_data.ScanArea.ScanAreaWidth.select  / BASE_RESOLUTION * base_res;
  *current_pixel_y = s->hw->sv->device_data.ScanArea.ScanAreaHeight.select  / BASE_RESOLUTION * base_res;

  log_call("doc size : %s", s->hw->cur_doc_size);
  log_call("base res : %d", s->hw->sv->device_data.Resolution.select);

  if(s->hw->sv->device_data.ScanArea.FixedSize == A3){
    *current_pixel_x = calculate_pixel_size(base_res, 297);
    *current_pixel_y = calculate_pixel_size(base_res, 420);
  }else if(s->hw->sv->device_data.ScanArea.FixedSize == A4){
    *current_pixel_x = calculate_pixel_size(base_res, 210);
    *current_pixel_y = calculate_pixel_size(base_res, 297);
  }else if(s->hw->sv->device_data.ScanArea.FixedSize == A4_Landscape){
    *current_pixel_x = calculate_pixel_size(base_res, 297);
    *current_pixel_y = calculate_pixel_size(base_res, 210);
  }else if(s->hw->sv->device_data.ScanArea.FixedSize == A5){
    *current_pixel_x = calculate_pixel_size(base_res, 148);
    *current_pixel_y = calculate_pixel_size(base_res, 210);
  }else if(s->hw->sv->device_data.ScanArea.FixedSize == A5_Landscape){
    *current_pixel_x = calculate_pixel_size(base_res, 210);
    *current_pixel_y = calculate_pixel_size(base_res, 148);
  }else if(s->hw->sv->device_data.ScanArea.FixedSize == A6){
    *current_pixel_x = calculate_pixel_size(base_res, 105);
    *current_pixel_y = calculate_pixel_size(base_res, 148);
  }else if(s->hw->sv->device_data.ScanArea.FixedSize == A6_Landscape){
    *current_pixel_x = calculate_pixel_size(base_res, 148);
    *current_pixel_y = calculate_pixel_size(base_res, 105);
  }else if(s->hw->sv->device_data.ScanArea.FixedSize == A8){
    *current_pixel_x = calculate_pixel_size(base_res, 52);
    *current_pixel_y = calculate_pixel_size(base_res, 74);
  }else if(s->hw->sv->device_data.ScanArea.FixedSize == A8_Landscape){
    *current_pixel_x = calculate_pixel_size(base_res, 74);
    *current_pixel_y = calculate_pixel_size(base_res, 52);
  }else if(s->hw->sv->device_data.ScanArea.FixedSize == B4_JIS){
    *current_pixel_x = calculate_pixel_size(base_res, 257);
    *current_pixel_y = calculate_pixel_size(base_res, 364);
  }else if(s->hw->sv->device_data.ScanArea.FixedSize == B4_JIS_Landscape){
    *current_pixel_x = calculate_pixel_size(base_res, 364);
    *current_pixel_y = calculate_pixel_size(base_res, 257);
 }else if(s->hw->sv->device_data.ScanArea.FixedSize == B5_JIS){
    *current_pixel_x = calculate_pixel_size(base_res, 182);
    *current_pixel_y = calculate_pixel_size(base_res, 257);
  }else if(s->hw->sv->device_data.ScanArea.FixedSize == B5_JIS_Landscape){
    *current_pixel_x = calculate_pixel_size(base_res, 257);
    *current_pixel_y = calculate_pixel_size(base_res, 182);
  }else if(s->hw->sv->device_data.ScanArea.FixedSize == Letter){
    *current_pixel_x = calculate_pixel_size(base_res, 216);
    *current_pixel_y = calculate_pixel_size(base_res, 279);
  }else if(s->hw->sv->device_data.ScanArea.FixedSize == Letter_Landscape){
    *current_pixel_x = calculate_pixel_size(base_res, 279);
    *current_pixel_y = calculate_pixel_size(base_res, 216);
  }else if(s->hw->sv->device_data.ScanArea.FixedSize == Legal){
    *current_pixel_x = calculate_pixel_size(base_res, 216);
    *current_pixel_y = calculate_pixel_size(base_res, 356);
  }else if(s->hw->sv->device_data.ScanArea.FixedSize == Postcard){
    *current_pixel_x = calculate_pixel_size(base_res, 100);
    *current_pixel_y = calculate_pixel_size(base_res, 148);
  }else if(s->hw->sv->device_data.ScanArea.FixedSize == Postcard_Landscape){
    *current_pixel_x = calculate_pixel_size(base_res, 148);
    *current_pixel_y = calculate_pixel_size(base_res, 100);
  }else if(s->hw->sv->device_data.ScanArea.FixedSize == PlasticCard){
    *current_pixel_x = calculate_pixel_size(base_res, 85.6);
    *current_pixel_y = calculate_pixel_size(base_res, 54);
  }else if(s->hw->sv->device_data.ScanArea.FixedSize == Tabloid){
    *current_pixel_x = calculate_pixel_size(base_res, 279);
    *current_pixel_y = calculate_pixel_size(base_res, 432);
  }else if(s->hw->sv->device_data.ScanArea.FixedSize == Maximum_Size){
    *current_pixel_x = (double)s->hw->sv->device_data.ScanArea.MaxScanAreaWidth.select / (double)BASE_RESOLUTION * (double)base_res;
    *current_pixel_y = (double)s->hw->sv->device_data.ScanArea.MaxScanAreaHeight.select / (double)BASE_RESOLUTION * (double)base_res;
  }else if(s->hw->sv->device_data.ScanArea.FixedSize == Manual){
    s->hw->sv->device_data.ScanArea.AutoSize.select = 0;
    return true;
  }else {
    return false;
  }
  log_call ("x, y :(%f , %f)", *current_pixel_x, *current_pixel_y);
  s->hw->sv->device_data.ScanArea.AutoSize.select = 0;
  return true;
}

#include "epsonsane.h"

//! Obtains the current scan parameters for a device
/*! \remarks
 *  The parameters are only guaranteed to be accurate between a call
 *  to sane_start() and the completion of that request.  Outside of
 *  that scope the parameters are a best effort only and the backend
 *  is at liberty to change them.
 */
SANE_Status
sane_get_parameters (SANE_Handle handle, SANE_Parameters *parameters)
{
  log_call("--------------sane_get_parameters--------------");

  Epson_Scanner *s = (Epson_Scanner *) handle;
  SANE_Status status = SANE_STATUS_GOOD;

  int width = 0;
  int height = 0;

  if(s->scan_ready){

    SDITransferEventType outType = kSDITrasnferEventTypeImage;
    unsigned int nSize = 100;
    unsigned char* tmp_buf = (unsigned char*)::malloc(nSize);
    SDIChar* output_path;
    if(tmp_buf){
      memset(tmp_buf, 0, nSize);
    }else {
      log_call("Status is SANE_STATUS_NO_MEM.");
      status = SANE_STATUS_NO_MEM;
      goto ERROR_2;
    }
    output_path = (SDIChar*)tmp_buf;

    SDIError outError;


    
    while (true)
    {    
      s->hw->sv->SDIImage_CreatePtr_(&s->outImageData);
      s->hw->sv->SDIScannerDriver_GetNextTransferEventPtr_(s->hw->sv->driver,
                                             &outType, //kSDITrasnferEventTypeImage -> kSDITransferEventTypeCompletekSDIDoubleFeedDetectionKey
                                             s->outImageData,
                                             &outError);
    
      if((outType == kSDITrasnferEventTypeImage) && s->hw->sv->SDIImage_CheckImageIsBlankPtr_(s->outImageData))
      {
        log_call("blankptr.");
        s->hw->sv->SDIImage_DisposePtr_(s->outImageData);
      }else{
        break;
      }
    }


    //Check Error code
    status = check_error(outError);
    if(status != SANE_STATUS_GOOD){
      log_call("Status is no SANE_STATUS_GOOD. %d", outError);
      goto ERROR;
    }

    if (outType == kSDITrasnferEventTypeImage)
    {
      log_call("kSDITrasnferEventTypeImage -> SDIImage_GetPathPtr_");
      s->hw->sv->SDIImage_GetPathPtr_(s->outImageData, output_path, 0);
    }else if (outType == kSDITransferEventTypeCancel){
      log_call("Button Cancel");
      s->cancel_requested = true;
      s->scan_complete = true;
    }else {
      log_call("Other EventType");
    }

    //CheckNextEvent
    /*
    if(!s->hw->sv->SDIScannerDriver_CheckNextTransferEventPtr_(s->hw->sv->driver, true)){
      s->hw->sv->SDIScannerDriver_GetNextTransferEventPtr_(s->hw->sv->driver,
                                            &outType, //kSDITrasnferEventTypeImage -> kSDITransferEventTypeCompletekSDIDoubleFeedDetectionKey
                                            s->outImageData,
                                            &outError);

      if (outType == kSDITransferEventTypeComplete) {
        log_call("kSDITransferEventTypeComplete");
      }
    }
    */
    //format
    if(s->hw->sv->device_data.ColorType.select == kSDIColorTypeRGB24)
    {
      log_call("SANE_FRAME_RGB");
      parameters->format = SANE_FRAME_RGB;
    }else if(s->hw->sv->device_data.ColorType.select == kSDIColorTypeMono8 ||
            s->hw->sv->device_data.ColorType.select == kSDIColorTypeMono1)
    {
      log_call("SANE_FRAME_GRAY");
      parameters->format = SANE_FRAME_GRAY;
    }
    width = s->hw->sv->SDIImage_GetWidthPtr_(s->outImageData);
    height = s->hw->sv->SDIImage_GetHeightPtr_(s->outImageData);

    //lines
    parameters->lines = height;

    //depth
    parameters->depth = s->hw->sv->SDIImage_GetBitsPerSamplePtr_(s->outImageData);
    s->hw->ChannelPerPixel = s->hw->sv->SDIImage_GetSamplesPerPixelPtr_(s->outImageData);

    //bytes_per_line
    parameters->bytes_per_line = (width * s->hw->ChannelPerPixel * parameters->depth + 7) / 8;
    //pixels_per_line
    parameters->pixels_per_line = width;

    s->total_datasize = parameters->bytes_per_line * height;
    
    log_call("width : %d", width);
    log_call("height : %d", height);

    log_call("parameters->last_frame : %d", parameters->last_frame);
    log_call("parameters->format : %d", parameters->format);
    log_call("parameters->bytes_per_line : %d", parameters->bytes_per_line);
    log_call("parameters->pixels_per_line : %d", parameters->pixels_per_line);
    log_call("parameters->lines : %d", parameters->lines);
    log_call("parameters->depth : %d", parameters->depth);
    log_call("s->hw->ChannelPerPixel : %d", s->hw->ChannelPerPixel);
    log_call("total_datasize : %d", s->total_datasize);

    if (s->cancel_requested == true){
      log_call("Cancel requested");
      goto ERROR;
    }

    //8 pixel
    /*s->pixels_per_line2 = parameters->pixels_per_line;
    s->pixels_per_line2 &= ~7;
    if (s->hw->sv->device_data.ColorType.select == kSDIColorTypeMono1)
    {
      s->pixels_per_line2 &= ~31;
    }
    log_call("s->pixels_per_line2 : %d", s->pixels_per_line2);

    s->bytes_per_line2 = (s->pixels_per_line2 * s->hw->ChannelPerPixel * parameters->depth + 7) / 8;
    log_call("s->bytes_per_line2 : %d", s->bytes_per_line2);
    s->skip = parameters->bytes_per_line - s->bytes_per_line2;*/

    s->cur_pos = 0;

    if(output_path){
      FILE *fp = nullptr;

      log_call("output_path : %s", output_path);  

      if(s->out_buffer){
        free(s->out_buffer);
        s->out_buffer = NULL;
      }
      s->out_buffer = (SANE_Byte *)malloc(s->total_datasize);
      if(!s->out_buffer){
        status = SANE_STATUS_NO_MEM;
        goto ERROR;
      }
      if((fp = fopen(output_path, "rb")) != nullptr){
        if(fread(s->out_buffer, 1, s->total_datasize, fp) != s->total_datasize){
          status = SANE_STATUS_INVAL;
          if(fp != nullptr){
            fclose(fp);
          }
          goto ERROR;
        }else {
          if(fp != nullptr){
            fclose(fp);
          }
          parameters->last_frame = SANE_TRUE;
        }
      }else {
        status = SANE_STATUS_INVAL;
        goto ERROR;
      }
      if(s->hw->sv->device_data.ColorType.select == kSDIColorTypeMono1){
        dip_flip_bits(s->out_buffer, s->total_datasize);
      }
    }

  ERROR:
    s->hw->sv->SDIImage_DisposePtr_(s->outImageData);
  ERROR_2:
    if(tmp_buf){
      free(tmp_buf);
      tmp_buf = nullptr;
    }

    if(s->cancel_requested == true){
      log_call("Cancel requested2");
      status = SANE_STATUS_CANCELLED;
    }
}else {

  //format
  if(s->hw->sv->device_data.ColorType.select == ColorTypeRGB24)
  {
    log_call("SANE_FRAME_RGB");
    parameters->format = SANE_FRAME_RGB;
  }else if(s->hw->sv->device_data.ColorType.select == ColorTypeMono8 ||
          s->hw->sv->device_data.ColorType.select == ColorTypeMono1)
  {
    log_call("SANE_FRAME_GRAY");
    parameters->format = SANE_FRAME_GRAY;
  }

  double current_pixel_x = 0;
  double current_pixel_y = 0;
  if(!handle_scan_area(s, &current_pixel_x, &current_pixel_y)){
    select_current_doc_size(s, s->hw->sv->device_data);
    handle_scan_area(s, &current_pixel_x, &current_pixel_y);
  }
  
  log_call("device_data.ScanArea.FixedSize : %d", s->hw->sv->device_data.ScanArea.FixedSize);
  log_call("current_pixel_x : %f", current_pixel_x);
  log_call("current_pixel_y : %f", current_pixel_y);
  log_call("s->val[OPT_SCAN_AREA].w : %d", s->val[OPT_SCAN_AREA].w);
  log_call("s->val[OPT_TL_X].w : %f", s->val[OPT_TL_X].w);
  log_call("s->val[OPT_TL_Y].w : %f", s->val[OPT_TL_Y].w);
  log_call("s->val[OPT_BR_X].w : %f", s->val[OPT_BR_X].w);
  log_call("s->val[OPT_BR_Y].w : %f", s->val[OPT_BR_Y].w);

  width = current_pixel_x;
  height = current_pixel_y;

  //lines
  parameters->lines = height;

  //depth
  if(s->hw->sv->device_data.ColorType.select == ColorTypeRGB24){
    parameters->depth = 8;
    s->hw->ChannelPerPixel = 3;
  }else if(s->hw->sv->device_data.ColorType.select == ColorTypeMono8){
    parameters->depth = 8;
    s->hw->ChannelPerPixel = 1;
  }else if(s->hw->sv->device_data.ColorType.select == ColorTypeMono1){
    parameters->depth = 1;
    s->hw->ChannelPerPixel = 1;
  }

  //bytes_per_line
  parameters->bytes_per_line = (width * s->hw->ChannelPerPixel * parameters->depth + 7) / 8;
  //pixels_per_line
  parameters->pixels_per_line = width;

  s->total_datasize = parameters->bytes_per_line * height;

    log_call("parameters->last_frame : %d", parameters->last_frame);
    log_call("parameters->format : %d", parameters->format);
    log_call("parameters->bytes_per_line : %d", parameters->bytes_per_line);
    log_call("parameters->pixels_per_line : %d", parameters->pixels_per_line);
    log_call("parameters->lines : %d", parameters->lines);
    log_call("parameters->depth : %d", parameters->depth);
    log_call("total_datasize : %d", s->total_datasize);
}

  return status;
}

//! Acquires up to \a max_length bytes of new image data
/*! \remarks
 *  The \a length is guaranteed to be zero in case of an unsuccessful
 *  request.
 */
SANE_Status
sane_read (SANE_Handle handle, SANE_Byte *buffer, SANE_Int max_length,
           SANE_Int *length)
{
  log_call("--------------sane_read--------------");
  SANE_Status status = SANE_STATUS_GOOD;
  Epson_Scanner *s;

  if (length) *length = 0;

  if (!handle)
    {
      return SANE_STATUS_INVAL;
    }

  s = (Epson_Scanner *) handle;
  
  if(s->out_buffer == NULL)
  {
    *length = 0;
    return SANE_STATUS_GOOD;
  }
  if(s->total_datasize == 0){
    log_call("image data -> EOF");
    s->image_count++;
    s->scan_complete = true;
    if(s->out_buffer){
      free(s->out_buffer);
      s->out_buffer = NULL;
    }
    return SANE_STATUS_EOF;
  }else if(length){
    if((s->total_datasize - max_length) < 0){
      log_call("Last data size : %d", s->total_datasize);
      *length = s->total_datasize;
    }else {
      *length = max_length;
    }
    memcpy(buffer, s->out_buffer + s->cur_pos, *length);
    //*length += s->skip;
    s->total_datasize -= *length;
    s->cur_pos += *length;

    log_call("length : %d", *length);
  }
  log_call("cur_pos : %d", s->cur_pos);

  return status;
}

//! Initiates cancellation of the currently pending operation
/*! \remarks
 *  As per "4.4 Code Flow", this function \e must be called when all
 *  frames or images have been acquired.  If a SANE frontend expects
 *  additional frames \e or images the function should not be called
 *  until the last frame or image has been acquired.
 *
 *  \remarks
 *  It is safe to call this function asynchronously (e.g. from signal
 *  handlers).  Its completion only guarantees that cancellation of a
 *  long-running operation has been initiated, not that cancellation
 *  of that operation has completed.  The long-running operation will
 *  typically be acquisition of an image but other operations such as
 *  initiating a scan with sane_start() or performing calibration via
 *  a call to sane_control_option() for a \c SANE_TYPE_BUTTON option
 *  could be subject to cancellation as well.  Note, though, that only
 *  sane_read() lends itself to straightforward cancellation support
 *  (due to its intended use in a loop construct).
 */
void
sane_cancel (SANE_Handle handle)
{
  log_call("--------------sane_cancel--------------");
  SANE_Status status = SANE_STATUS_GOOD;
  Epson_Scanner *s;

  s = (Epson_Scanner *) handle;

  s->image_count = 0;
  s->scan_ready = false;

  if(s->scan_complete == false){
    if(s->cancel_requested == false){
      s->cancel_requested = true;
    }else if(s->cancel_requested == true){
      log_call("Scan cancel terminated.");
      s->hw->sv->SDIScannerDriver_DoScanJobPtr_(s->hw->sv->driver, kSDIOperationTypeCancel);
    }
  }else {
    s->hw->sv->SDIImage_CreatePtr_(&s->outImageData);
    s->hw->sv->SDIScannerDriver_GetNextTransferEventPtr_(s->hw->sv->driver,
                                           0, //kSDITrasnferEventTypeImage -> kSDITransferEventTypeCompletekSDIDoubleFeedDetectionKey
                                           s->outImageData,
                                           0);
    s->hw->sv->SDIImage_DisposePtr_(s->outImageData);
  }
  log_call("--------------sane_cancel--------------");

  return;
}

//! Controls whether device I/O is (non-)blocking
/*! \remarks
 *  Blocking I/O is the default I/O mode and \e must be supported.
 *  Support for non-blocking I/O is optional.
 *
 *  \remarks
 *  This function may only be called after a call to sane_start().
 */
SANE_Status
sane_set_io_mode (SANE_Handle handle, SANE_Bool non_blocking)
{
  log_call("--------------sane_set_io_mode--------------");
  /* get rid of compiler warning */
	handle = handle;
	non_blocking = non_blocking;

	return SANE_STATUS_UNSUPPORTED;
}

//! Obtains a file descriptor if image data is available
/*! \remarks
 *  Support for file descriptors is optional.  The file descriptor is
 *  guaranteed to remain valid for the duration of the current image
 *  acquisition.  That is, until sane_read() returns SANE_STATUS_EOF
 *  or the frontend calls one of sane_cancel() or sane_start().
 *
 *  \remarks
 *  This function may only be called after a call to sane_start().
 */
SANE_Status
sane_get_select_fd (SANE_Handle handle, SANE_Int *fdp)
{
  log_call("--------------sane_get_select_fd--------------");
 
  /* get rid of compiler warnings */
	handle = handle;

	return SANE_STATUS_UNSUPPORTED;
}

}//extern "C"

/*! Releases the resources held by a SANE_Device.

    This function is primarily useful to maintain the \c sane_dev
    member of a backend_type object.
 */
static void
be_sane_dev_dtor (void *p)
{
  SANE_Device *sd = (SANE_Device *) p;
  if (!sd) return;

  free((void*)sd->name);
  free((void*)sd->vendor);
  free((void*)sd->type);
  free((void*)sd->model);

  free (sd);
}
