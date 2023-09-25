#ifndef SANE_I18N
#define SANE_I18N(text) (text)
#endif

#ifdef HAVE_CONFIG_H
#include  <config.h>
#endif

#include  <limits.h>
#include  <string.h>
#include  <unistd.h>
#include  <errno.h>
#include  <math.h>

#include  "epsonopts.h"
#include "../Include/Config.h"


#include "backend.h"
#include "epsonscan.h"

#define DEFAULT_RESOLUTION		300	/* dpi */
#define BASE_RESOLUTION		100	/* dpi */
#define DEFAULT_X_RESOLUTION	DEFAULT_RESOLUTION
#define DEFAULT_Y_RESOLUTION	DEFAULT_RESOLUTION

static void create_functionalunit_list (device *dev, int32_t item_list[], int32_t list_size);
static void create_double_feed_detection_list (device *dev, int32_t item_list[], int32_t list_size);
static void create_document_size_list (device *dev);
static void set_fixed_size(Epson_Scanner * s, SANE_String value);
static SANE_Status set_epson_value (Epson_Scanner *s, SANE_Int option, void *value);

static void handle_dropout (Epson_Scanner *s, SANE_String value);
static void handle_resolution (Epson_Scanner *s, SANE_Int value);
static void handle_rotate (Epson_Scanner *s, SANE_String value);
static void handle_mode (Epson_Scanner *s, SANE_String value);
static void handle_duplex (Epson_Scanner *s, SANE_Bool value);
static void handle_double_feed_detection (Epson_Scanner *s, SANE_String value);
static void handle_remove_background (Epson_Scanner *s, SANE_String value);
static void handle_background_color (Epson_Scanner *s, SANE_String value);
static void handle_transfer_format (Epson_Scanner *s, SANE_String value);
static void handle_source (Epson_Scanner *s, SANE_String value);
static bool handle_scan_area (Epson_Scanner *s, SANE_String value);
static void handle_skip_blankpages (Epson_Scanner *s, SANE_Word value);
static void handle_brightness (Epson_Scanner *s, SANE_Word value);
static void handle_contrast (Epson_Scanner *s, SANE_Word value);
static void handle_threshold (Epson_Scanner *s, SANE_Word value);
static void handle_gamma (Epson_Scanner *s, SANE_Int value);
static void handle_jpeg_quality (Epson_Scanner *s, SANE_Word value);
static void handle_long_paper_mode (Epson_Scanner *s, SANE_Bool value);
static void handle_top_left_x (Epson_Scanner *s, SANE_Int value);
static void handle_top_left_y (Epson_Scanner *s, SANE_Int value);
static void handle_bottom_right_x (Epson_Scanner *s, SANE_Int value);
static void handle_bottom_right_y (Epson_Scanner *s, SANE_Int value);
static void handle_text_enhance (Epson_Scanner *s, SANE_String value);

static bool sane_app_compatible (SANE_String value);

double calculate_mm_size(int base_res, double value);
double calculate_pixel_size(int base_res, double value);
double calculate_pixel_size_inchi(int base_res, double value);

static int search_string_list_index (const SANE_String_Const * list, SANE_String value);


/* Some utility macros
 */

/*! Returns the larger of the arguments \a a and \a b.
 */
#define max(a, b)  ((a) < (b) ? (b) : (a))

/*! Returns the smaller of the arguments \a a and \a b.
 */
#define min(a, b)  ((a) < (b) ? (a) : (b))

static size_t
max_string_size (const SANE_String_Const strings[])
{
  size_t size, max_size = 0;
  int i;

  for (i = 0; strings[i]; i++)
  {
    size = strlen (strings[i]) + 1;
    if (size > max_size)
      max_size = size;
  }
  return max_size;
}

static SANE_Status create_epson_device (device **dev, const char *name);
static SANE_Status create_epson_scanner (device *dev, Epson_Scanner **scanner);
static SANE_Status create_sane_handle (Epson_Scanner **scanner, const char *name);
static SANE_Status init_options (Epson_Scanner * s);

#define max_val 100                 /* any integer value will do */
static const SANE_Range brightness_range = { -max_val, max_val, 1};
static const SANE_Range contrast_range   = { -max_val, max_val, 1};
static const SANE_Range u8_range = { 0, 255, 0 };
static const SANE_Range s8_range = { -127, 127, 0 };
static const SANE_Range skip_blankpages_range = { 0, 30, 1};
static const SANE_Range jpeg_quality_range = { 1, max_val, 1};

static const SANE_Range image_count_range = { 1, 999, 1};
static const SANE_Range gamma_range = { SANE_FIX(0.5), SANE_FIX(3), SANE_FIX(0.1)};


static Epson_Scanner *first_handle = NULL;

SANE_Status
epsonscan_open(const char *name, SANE_Handle *handle)
{
  SANE_Status status = SANE_STATUS_GOOD;
  Epson_Scanner *s = NULL;

  status = create_sane_handle (&s, name);
  if (SANE_STATUS_GOOD != status) return status;

  /* insert newly opened handle into list of open handles */
  s->next = first_handle;
  first_handle = s;

  *handle = (SANE_Handle) s;

  return SANE_STATUS_GOOD;
}

/*! \todo Release resources in error recovery.
 */
static SANE_Status
create_sane_handle (Epson_Scanner **handle, const char *name)
{
  SANE_Status status = SANE_STATUS_GOOD;
  device *dev = NULL;
  Epson_Scanner *s = NULL;

  status = create_epson_device (&dev, name);
  if (SANE_STATUS_GOOD != status)
    return status;

  status = create_epson_scanner (dev, &s);
  if (!s || SANE_STATUS_GOOD != status)
    return status;

  init_options (s);

  *handle = s;
  return SANE_STATUS_GOOD;
}

static SANE_Status
create_epson_device (device **devp, const char *name)
{
  SANE_Status status = SANE_STATUS_GOOD;
  device *dev = NULL;

  DefaultSettings* default_settings_file = nullptr;

  require (devp);

  dev = t_calloc (1, device);
  if (!dev)
  {
    return SANE_STATUS_NO_MEM;
  }
  
  dev->sv = new Supervisor();
  dev->sv->SetUp();
 
memset(&dev->devInfo, 0, sizeof(SDIDeviceInfo));
{
  char* nameToTok = (char*)malloc(strlen(name) + 1);
  strcpy(nameToTok, name);

  char* dislayname = strtok(nameToTok, ":");
  char* bus_no = nullptr;
  char* device_no = nullptr;
  char temp_displayName[50];

  memcpy(temp_displayName, dislayname, min(sizeof(temp_displayName), strlen(dislayname) + 1));

  if(strncmp(temp_displayName, "network", strlen("network")) != 0){
    bus_no = strtok(NULL, ":");
    device_no = strtok(NULL, ":");
  }
  char* esci2 = strtok(NULL, ":");
  char* usborNet = strtok(NULL, ":");
  if (strcmp(usborNet, "usb") == 0)
  {
      char* modelID = strtok(NULL, ":");
      memcpy(dev->devInfo.modelID, modelID, min(sizeof(dev->devInfo.modelID), strlen(modelID) + 1));

      char* pid = strtok(NULL, ":");
      dev->devInfo.productID = strtol(pid, NULL, 10);
  }

  if (strcmp(usborNet, "network") == 0)
  {
      char* ipAddress = strtok(NULL, ":");

      memcpy(dev->devInfo.ipAddress, ipAddress, min(sizeof(dev->devInfo.ipAddress), strlen(ipAddress) + 1));
      dev->sv->SANEManuNetfinder();
      if(dev->sv->DeviceInfoResolve(&dev->devInfo) == false)
      {
        free(dev);
        dev = NULL;
        free(nameToTok);
        nameToTok = NULL;
        return SANE_STATUS_IO_ERROR;
      }
  }

  if(bus_no != nullptr && device_no != nullptr){
    sprintf(dev->devInfo.displayName, "%s:%s:%s", temp_displayName, bus_no, device_no);
  }

  free(nameToTok);
  nameToTok = NULL;
  
}

if(dev->sv->Connecting_Scanner(dev->devInfo, nullptr) != kSDIErrorNone)
{
  free(dev);
  dev = NULL;
  return SANE_STATUS_IO_ERROR;
}

//デフォルト設定ファイルがある場合はそちらから設定パラメーターを取得する
default_settings_file = new DefaultSettings();
if(default_settings_file->CheckDefaultSettingsFile()){
  default_settings_file->ReadSettingsFile(&dev->sv->device_data);
  dev->sv->Set_All_Value();
}

log_call("autolong = %d", dev->sv->device_data.ScanArea.AutoSize.select);

//デフォルト設定ファイルがある場合は値をセットしてからクラスを削除
if(default_settings_file){
  delete default_settings_file;
  default_settings_file = nullptr;
}
dev->sv->Get_All_Value(); // Get default value

  //OPT_MODE
  {
    int i = 0;
    dev->scan_mode[i++] = COLOR_STR;
    dev->scan_mode[i++] = GRAYSCALE_STR;
    dev->scan_mode[i++] = MONOCHROME_STR;
    dev->scan_mode[i] = NULL;    /* terminator */
  }

  //OPT_SOURCE
  create_functionalunit_list(dev, dev->sv->device_data.FunctionalUnit.capability.list,
                            dev->sv->device_data.FunctionalUnit.capability.countOfList);
  
  //OPT_DUPLEX
  {
    int i = 0;
    dev->scanning_side[i++] = SINGLE_SIDED_STR;
    dev->scanning_side[i++] = DOUBLE_SIDED_STR;
    dev->scanning_side[i] = NULL;    /* terminator */
  }
  //OPT_RESOLUTION
  {
    dev->dpi_range.min = dev->sv->device_data.Resolution.capability.minValue;
    dev->dpi_range.max = dev->sv->device_data.Resolution.capability.maxValue;
    dev->dpi_range.quant = 1;
  }

  //OPT_SCAN_AREA
  create_document_size_list(dev);

  //OPT_BR_X
  {
    dev->x_range.min = 0;
    dev->x_range.max = SANE_FIX(calculate_mm_size(BASE_RESOLUTION, dev->sv->device_data.ScanArea.MaxScanAreaWidth.select));
    dev->x_range.quant = 0;
  }

  //OPT_BR_Y
  {
    dev->y_range.min = 0;
    dev->y_range.max = SANE_FIX(calculate_mm_size(BASE_RESOLUTION, dev->sv->device_data.ScanArea.MaxScanAreaHeight.select));
    dev->y_range.quant = 0;
  }

  //OPT_ROTATE
  {
    int i = 0;
    dev->rotate[i++] = ROTATE_0_STR;
    dev->rotate[i++] = ROTATE_90_STR;
    dev->rotate[i++] = ROTATE_180_STR;
    dev->rotate[i++] = ROTATE_270_STR;
    dev->rotate[i++] = ROTATE_AUTO_STR;
    dev->rotate[i] = NULL;        /* terminator */
  }

  //OPT_DROPOUT
  {
    int i = 0;
    dev->dropout_color[i++] = DROPOUT_NONE_STR;
    dev->dropout_color[i++] = DROPOUT_RED_STR;
    dev->dropout_color[i++] = DROPOUT_BLUE_STR;
    dev->dropout_color[i++] = DROPOUT_GREEN_STR;
    dev->dropout_color[i] = NULL;        /* terminator */
  }

  //OPT_TRANSFER_FORMAT //TBD -> List
  {
    int i = 0;
    if(dev->sv->device_data.TransferCompression.capability.supportLevel == kSDISupportLevelAvailable){
      dev->transfer_format[i++] = TRANSFER_JPEG_STR;
    }
    dev->transfer_format[i++] = TRANSFER_RAW_STR;
    dev->transfer_format[i] = NULL;        /* terminator */
  }

  //OPT_DOUBLE_FEED_DETECTION
  create_double_feed_detection_list(dev, dev->sv->device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel.capability.list,
                            dev->sv->device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel.capability.countOfList);

  //OPT_REMOVE_BACKGROUND
  {
    int i = 0;
    dev->remove_background[i++] = REMOVE_BACKGROUND_NONE_STR;
    dev->remove_background[i++] = REMOVE_BACKGROUND_STANDARD_STR;
    dev->remove_background[i++] = REMOVE_BACKGROUND_HIGH_STR;
    dev->remove_background[i] = NULL;        /* terminator */
  }

  //OPT_BACKGROUND_COLOR
  {
    int i = 0;
    dev->background_color[i++] = BACKGROUND_COLOR_BLACK_STR;
    dev->background_color[i++] = BACKGROUND_COLOR_WHITE_STR;
    dev->background_color[i] = NULL;        /* terminator */
  }

  //OPT_TEXTENHANCE
  {
    int i = 0;
    dev->text_enhance[i++] = TEXT_ENHANCE_NONE_STR;
    dev->text_enhance[i++] = TEXT_ENHANCE_NORMAL_STR;
    dev->text_enhance[i++] = TEXT_ENHANCE_HIGH_STR;
    dev->text_enhance[i] = NULL;        /* terminator */
  }

  *devp = dev;

  return status;
}

static SANE_Status
create_epson_scanner (device *dev, Epson_Scanner **scanner)
{
  Epson_Scanner *s = t_calloc (1, Epson_Scanner);
  
  if (!s)
  {
    return SANE_STATUS_NO_MEM;
  }

  s->hw  = dev;

  s->image_count = 0;

  *scanner = s;
  return SANE_STATUS_GOOD;
}

static SANE_Status
init_options (Epson_Scanner * s)
{
  log_call("--------------init_options--------------");
  int i;
  SANE_Bool reload;

  for (i = 0; i < NUM_OPTIONS; ++i)
  {
    s->opt[i].size = sizeof (SANE_Word);
    s->opt[i].cap = SANE_CAP_SOFT_SELECT | SANE_CAP_SOFT_DETECT;
  }

  s->opt[OPT_NUM_OPTS].title = SANE_TITLE_NUM_OPTIONS;
  s->opt[OPT_NUM_OPTS].desc = SANE_DESC_NUM_OPTIONS;
  s->opt[OPT_NUM_OPTS].cap = SANE_CAP_SOFT_DETECT;
  s->val[OPT_NUM_OPTS].w = NUM_OPTIONS;

  /* "General" group: */
  s->opt[OPT_DEVICE_GENERAL_GROUP].title = SANE_I18N ("General");
  s->opt[OPT_DEVICE_GENERAL_GROUP].desc = "";
  s->opt[OPT_DEVICE_GENERAL_GROUP].type = SANE_TYPE_GROUP;
  s->opt[OPT_DEVICE_GENERAL_GROUP].cap = 0;

  /* scan mode */
  s->opt[OPT_MODE].name = SANE_NAME_SCAN_MODE;
  s->opt[OPT_MODE].title = SANE_TITLE_SCAN_MODE;
  s->opt[OPT_MODE].desc = SANE_DESC_SCAN_MODE;
  s->opt[OPT_MODE].type = SANE_TYPE_STRING;
  s->opt[OPT_MODE].size = max_string_size (s->hw->scan_mode);
  s->opt[OPT_MODE].constraint_type = SANE_CONSTRAINT_STRING_LIST;
  s->opt[OPT_MODE].constraint.string_list = s->hw->scan_mode;
  s->val[OPT_MODE].w = 0;	/* Color */

  /* source */
  s->opt[OPT_SOURCE].name = SANE_NAME_SCAN_SOURCE;
  s->opt[OPT_SOURCE].title = SANE_TITLE_SCAN_SOURCE;
  s->opt[OPT_SOURCE].desc = SANE_DESC_SCAN_SOURCE;

  s->opt[OPT_SOURCE].type = SANE_TYPE_STRING;
  s->opt[OPT_SOURCE].size = max_string_size (s->hw->doc_sources);
  s->opt[OPT_SOURCE].constraint_type = SANE_CONSTRAINT_STRING_LIST;
  s->opt[OPT_SOURCE].constraint.string_list = s->hw->doc_sources;

  if (s->hw->sv->device_data.FunctionalUnit.capability.supportLevel == kSDISupportLevelNone)        /* two or more scan sources */
  {
    s->opt[OPT_SOURCE].cap |= SANE_CAP_INACTIVE;
  }
  s->val[OPT_SOURCE].w = 0;

  /* duplex mode */
  s->opt[OPT_DUPLEX].name  = "duplex";
  s->opt[OPT_DUPLEX].title = SANE_I18N ("ADF Duplex Mode");
  s->opt[OPT_DUPLEX].desc  = SANE_I18N ("Selects the ADF mode (yes:duplex/no:simplex)");
  s->opt[OPT_DUPLEX].type  = SANE_TYPE_BOOL;
  s->opt[OPT_DUPLEX].unit  = SANE_UNIT_NONE;
  s->opt[OPT_DUPLEX].size  = sizeof (SANE_Bool);
  s->opt[OPT_DUPLEX].cap  |= SANE_CAP_EMULATED;
  if ( s->hw->sv->device_data.DuplexType.capability.supportLevel == kSDISupportLevelAvailable)
    {
      s->opt[OPT_DUPLEX].cap &= ~SANE_CAP_INACTIVE; // Active
    }
  else
    {
      s->opt[OPT_DUPLEX].cap  |= SANE_CAP_INACTIVE; // Inactive
    }
  s->opt[OPT_DUPLEX].constraint_type  = SANE_CONSTRAINT_NONE;
  s->val[OPT_DUPLEX].w = SANE_FALSE;

  /* resolution */
  s->opt[OPT_RESOLUTION].name  = SANE_NAME_SCAN_RESOLUTION;
  s->opt[OPT_RESOLUTION].title = SANE_TITLE_SCAN_RESOLUTION;
  s->opt[OPT_RESOLUTION].desc  = SANE_DESC_SCAN_RESOLUTION;
  s->opt[OPT_RESOLUTION].type  = SANE_TYPE_INT;
  s->opt[OPT_RESOLUTION].unit  = SANE_UNIT_DPI;

  if (s->hw->sv->device_data.Resolution.capability.countOfList == 0)
  {
    log_call("SANE_CONSTRAINT_RANGE");
    s->opt[OPT_RESOLUTION].constraint_type  = SANE_CONSTRAINT_RANGE;
    s->opt[OPT_RESOLUTION].constraint.range = &s->hw->dpi_range;
  }
  else
  {
    log_call("SANE_CONSTRAINT_WORD_LIST");
    //not supported
  }
  log_call("default resolution : %d", s->hw->sv->device_data.Resolution.select);
  s->val[OPT_RESOLUTION].w = s->hw->sv->device_data.Resolution.select;

#undef  SANE_NAME_SCAN_X_RESOLUTION
#define SANE_NAME_SCAN_X_RESOLUTION "x-resolution"

  /* resolution in main scan direction */
  s->opt[OPT_X_RESOLUTION].name  = SANE_NAME_SCAN_X_RESOLUTION;
  s->opt[OPT_X_RESOLUTION].title = SANE_TITLE_SCAN_X_RESOLUTION;
  s->opt[OPT_X_RESOLUTION].desc  = SANE_DESC_SCAN_X_RESOLUTION;
  s->opt[OPT_X_RESOLUTION].type  = SANE_TYPE_INT;
  s->opt[OPT_X_RESOLUTION].unit  = SANE_UNIT_DPI;

  s->opt[OPT_X_RESOLUTION].constraint_type  = SANE_CONSTRAINT_RANGE;
  s->opt[OPT_X_RESOLUTION].constraint.range = &s->hw->dpi_range;
  s->opt[OPT_X_RESOLUTION].cap |= SANE_CAP_INACTIVE;

  /* resolution in sub scan direction */
  s->opt[OPT_Y_RESOLUTION].name  = SANE_NAME_SCAN_Y_RESOLUTION;
  s->opt[OPT_Y_RESOLUTION].title = SANE_TITLE_SCAN_Y_RESOLUTION;
  s->opt[OPT_Y_RESOLUTION].desc  = SANE_DESC_SCAN_Y_RESOLUTION;
  s->opt[OPT_Y_RESOLUTION].type  = SANE_TYPE_INT;
  s->opt[OPT_Y_RESOLUTION].unit  = SANE_UNIT_DPI;

  s->opt[OPT_Y_RESOLUTION].constraint_type  = SANE_CONSTRAINT_RANGE;
  s->opt[OPT_Y_RESOLUTION].constraint.range = &s->hw->dpi_range;
  s->opt[OPT_Y_RESOLUTION].cap |= SANE_CAP_INACTIVE;

  /* media size oriented scan area setting */
  s->opt[OPT_SCAN_AREA].name  = "scan-area";
  s->opt[OPT_SCAN_AREA].title = SANE_I18N ("Scan area");
  s->opt[OPT_SCAN_AREA].desc  =
    SANE_I18N ("Select an area to scan based on well-known media sizes.");
  s->opt[OPT_SCAN_AREA].type = SANE_TYPE_STRING;
  s->opt[OPT_SCAN_AREA].size = max_string_size (s->hw->document_size);
  s->opt[OPT_SCAN_AREA].cap |= SANE_CAP_EMULATED;
  s->opt[OPT_SCAN_AREA].constraint_type = SANE_CONSTRAINT_STRING_LIST;
  s->opt[OPT_SCAN_AREA].constraint.string_list = s->hw->document_size;
  s->val[OPT_SCAN_AREA].w = search_string_list_index(s->hw->document_size, (char *)DOC_MANUAL_STR);//0;

  //s->hw->cur_doc_size = DOC_MANUAL_STR;
  select_current_doc_size(s, s->hw->sv->device_data);

  /* "Geometry group: */
  s->opt[OPT_GEOMETRY_GROUP].title = SANE_I18N ("Geometry");
  s->opt[OPT_GEOMETRY_GROUP].desc = "";
  s->opt[OPT_GEOMETRY_GROUP].type = SANE_TYPE_GROUP;
  s->opt[OPT_GEOMETRY_GROUP].cap = SANE_CAP_ADVANCED;

  /* top-left x */
  s->opt[OPT_TL_X].name = SANE_NAME_SCAN_TL_X;
  s->opt[OPT_TL_X].title = SANE_TITLE_SCAN_TL_X;
  s->opt[OPT_TL_X].desc = SANE_DESC_SCAN_TL_X;

  s->opt[OPT_TL_X].type = SANE_TYPE_FIXED;
  s->opt[OPT_TL_X].unit = SANE_UNIT_MM;
  s->opt[OPT_TL_X].constraint_type = SANE_CONSTRAINT_RANGE;
  s->opt[OPT_TL_X].constraint.range = &(s->hw->x_range);
  s->val[OPT_TL_X].w = 0;

  /* top-left y */
  s->opt[OPT_TL_Y].name = SANE_NAME_SCAN_TL_Y;
  s->opt[OPT_TL_Y].title = SANE_TITLE_SCAN_TL_Y;
  s->opt[OPT_TL_Y].desc = SANE_DESC_SCAN_TL_Y;

  s->opt[OPT_TL_Y].type = SANE_TYPE_FIXED;
  s->opt[OPT_TL_Y].unit = SANE_UNIT_MM;
  s->opt[OPT_TL_Y].constraint_type = SANE_CONSTRAINT_RANGE;
  s->opt[OPT_TL_Y].constraint.range = &(s->hw->y_range);
  s->val[OPT_TL_Y].w = 0;

  /* bottom-right x */
  s->opt[OPT_BR_X].name = SANE_NAME_SCAN_BR_X;
  s->opt[OPT_BR_X].title = SANE_TITLE_SCAN_BR_X;
  s->opt[OPT_BR_X].desc = SANE_DESC_SCAN_BR_X;

  s->opt[OPT_BR_X].type = SANE_TYPE_FIXED;
  s->opt[OPT_BR_X].unit = SANE_UNIT_MM;
  s->opt[OPT_BR_X].constraint_type = SANE_CONSTRAINT_RANGE;
  s->opt[OPT_BR_X].constraint.range = &(s->hw->x_range);
  //log_call("koko %d", s->hw->sv->device_data.ScanArea.MaxScanAreaWidth.select);
  //log_call("koko %f", calculate_mm_size(100, s->hw->sv->device_data.ScanArea.MaxScanAreaWidth.select));
  s->val[OPT_BR_X].w = s->hw->x_range.max;

  /* bottom-right y */
  s->opt[OPT_BR_Y].name = SANE_NAME_SCAN_BR_Y;
  s->opt[OPT_BR_Y].title = SANE_TITLE_SCAN_BR_Y;
  s->opt[OPT_BR_Y].desc = SANE_DESC_SCAN_BR_Y;

  s->opt[OPT_BR_Y].type = SANE_TYPE_FIXED;
  s->opt[OPT_BR_Y].unit = SANE_UNIT_MM;
  s->opt[OPT_BR_Y].constraint_type = SANE_CONSTRAINT_RANGE;
  s->opt[OPT_BR_Y].constraint.range = &(s->hw->y_range);
  s->val[OPT_BR_Y].w = s->hw->y_range.max;

  /* "Optional equipment" group: */
  s->opt[OPT_ENHANCEMENT_GROUP].title = SANE_I18N ("Enhancement");
  s->opt[OPT_ENHANCEMENT_GROUP].desc = "";
  s->opt[OPT_ENHANCEMENT_GROUP].type = SANE_TYPE_GROUP;
  s->opt[OPT_ENHANCEMENT_GROUP].cap = SANE_CAP_ADVANCED;

  /* deskew */
  s->opt[OPT_DESKEW].name  = "deskew";
  s->opt[OPT_DESKEW].title = SANE_I18N ("Deskew");
  s->opt[OPT_DESKEW].desc  = SANE_I18N ("Select the option to correct the skew.");
  s->opt[OPT_DESKEW].type  = SANE_TYPE_BOOL;
  s->opt[OPT_DESKEW].unit  = SANE_UNIT_NONE;
  s->opt[OPT_DESKEW].size  = sizeof (SANE_Bool);
  s->opt[OPT_DESKEW].cap  |= SANE_CAP_EMULATED;
  if ( s->hw->sv->device_data.PaperDeskew.capability.supportLevel == kSDISupportLevelNone )
    {
      s->opt[OPT_DESKEW].cap  |= SANE_CAP_INACTIVE;
    }
  s->opt[OPT_DESKEW].constraint_type  = SANE_CONSTRAINT_NONE;
  s->val[OPT_DESKEW].w = SANE_TRUE;

  /* dropout */
  s->opt[OPT_DROPOUT].name = "dropout";
  s->opt[OPT_DROPOUT].title = SANE_I18N ("Dropout");
  s->opt[OPT_DROPOUT].desc = SANE_I18N ("Selects the dropout.");

  s->opt[OPT_DROPOUT].type = SANE_TYPE_STRING;
  s->opt[OPT_DROPOUT].size = max_string_size (s->hw->dropout_color);
  s->opt[OPT_DROPOUT].cap |= SANE_CAP_EMULATED;
  s->opt[OPT_DROPOUT].constraint_type = SANE_CONSTRAINT_STRING_LIST;
  s->opt[OPT_DROPOUT].constraint.string_list = s->hw->dropout_color;
  s->val[OPT_DROPOUT].w = 0;	/* None */

  /* rotate */
  s->opt[OPT_ROTATE].name = "rotate";
  s->opt[OPT_ROTATE].title = "Rotate";
  s->opt[OPT_ROTATE].desc = "Rotate";

  s->opt[OPT_ROTATE].type = SANE_TYPE_STRING;
  s->opt[OPT_ROTATE].size = max_string_size (s->hw->rotate);
  s->opt[OPT_ROTATE].constraint_type = SANE_CONSTRAINT_STRING_LIST;
  s->opt[OPT_ROTATE].constraint.string_list = s->hw->rotate;
  s->val[OPT_ROTATE].w = 0;

  /* skip blank pages */
  s->opt[OPT_SKIP_BLANKPAGES].name  = "skip-blankpages";
  s->opt[OPT_SKIP_BLANKPAGES].title = SANE_I18N ("Skip Blank Pages Settings");
  s->opt[OPT_SKIP_BLANKPAGES].desc  = SANE_I18N ("Skip Blank Pages Settings.");
  s->opt[OPT_SKIP_BLANKPAGES].type  = SANE_TYPE_INT;
  s->opt[OPT_SKIP_BLANKPAGES].unit  = SANE_UNIT_NONE;
  s->opt[OPT_SKIP_BLANKPAGES].size  = sizeof (SANE_Int);
  s->opt[OPT_SKIP_BLANKPAGES].constraint_type = SANE_CONSTRAINT_RANGE;
  s->val[OPT_SKIP_BLANKPAGES].w = 0; /* neutral */
  s->opt[OPT_SKIP_BLANKPAGES].cap |= SANE_CAP_EMULATED;
  s->opt[OPT_SKIP_BLANKPAGES].constraint.range = &skip_blankpages_range;
  if ( s->hw->sv->device_data.BlankPageSkipLevelKey.capability.supportLevel == kSDISupportLevelNone )
  {
      s->opt[OPT_SKIP_BLANKPAGES].cap  |= SANE_CAP_INACTIVE;
  }

  /* brightness */
  s->opt[OPT_BRIGHTNESS].name  = SANE_NAME_BRIGHTNESS;
  s->opt[OPT_BRIGHTNESS].title = SANE_TITLE_BRIGHTNESS;
  s->opt[OPT_BRIGHTNESS].desc  = SANE_DESC_BRIGHTNESS;
  s->opt[OPT_BRIGHTNESS].type  = SANE_TYPE_INT;
  s->opt[OPT_BRIGHTNESS].unit  = SANE_UNIT_NONE;
  s->opt[OPT_BRIGHTNESS].size  = sizeof (SANE_Int);
  s->opt[OPT_BRIGHTNESS].constraint_type = SANE_CONSTRAINT_RANGE;
  s->val[OPT_BRIGHTNESS].w = 0; /* neutral */
  s->opt[OPT_BRIGHTNESS].cap |= SANE_CAP_EMULATED;
  s->opt[OPT_BRIGHTNESS].constraint.range = &brightness_range;

  /* contrast */
  s->opt[OPT_CONTRAST].name  = SANE_NAME_CONTRAST;
  s->opt[OPT_CONTRAST].title = SANE_TITLE_CONTRAST;
  s->opt[OPT_CONTRAST].desc  = SANE_DESC_CONTRAST;
  s->opt[OPT_CONTRAST].type  = SANE_TYPE_INT;
  s->opt[OPT_CONTRAST].unit  = SANE_UNIT_NONE;
  s->opt[OPT_CONTRAST].size  = sizeof (SANE_Int);
  s->opt[OPT_CONTRAST].cap  |= SANE_CAP_EMULATED;
  s->opt[OPT_CONTRAST].constraint_type  = SANE_CONSTRAINT_RANGE;
  s->opt[OPT_CONTRAST].constraint.range = &contrast_range;
  s->val[OPT_CONTRAST].w = 0;   /* neutral */


  /* threshold */
  s->opt[OPT_THRESHOLD].name = SANE_NAME_THRESHOLD;
  s->opt[OPT_THRESHOLD].title = SANE_TITLE_THRESHOLD;
  s->opt[OPT_THRESHOLD].desc = SANE_DESC_THRESHOLD;

  s->opt[OPT_THRESHOLD].type = SANE_TYPE_INT;
  s->opt[OPT_THRESHOLD].unit = SANE_UNIT_NONE;
  s->opt[OPT_THRESHOLD].constraint_type = SANE_CONSTRAINT_RANGE;
  s->opt[OPT_THRESHOLD].constraint.range = &u8_range;
  s->val[OPT_THRESHOLD].w = 0x80;

  if (s->hw->sv->device_data.Threshold.capability.supportLevel == kSDISupportLevelNone)
  {
    s->opt[OPT_THRESHOLD].cap |= SANE_CAP_INACTIVE;
  }

  /* "Other" group: */
  s->opt[OPT_OTHER_GROUP].title = SANE_I18N ("Other");
  s->opt[OPT_OTHER_GROUP].desc = "";
  s->opt[OPT_OTHER_GROUP].type = SANE_TYPE_GROUP;
  s->opt[OPT_OTHER_GROUP].cap = SANE_CAP_ADVANCED;

  /* detect double feed */
  s->opt[OPT_DOUBLE_FEED_DETECTION].name = "double-feed-detection";
  s->opt[OPT_DOUBLE_FEED_DETECTION].title = "Detection Double Feed";
  s->opt[OPT_DOUBLE_FEED_DETECTION].desc = "Detection Double Feed";

  s->opt[OPT_DOUBLE_FEED_DETECTION].type = SANE_TYPE_STRING;
  s->opt[OPT_DOUBLE_FEED_DETECTION].size = max_string_size (s->hw->detect_double_feed);
  s->opt[OPT_DOUBLE_FEED_DETECTION].constraint_type = SANE_CONSTRAINT_STRING_LIST;
  s->opt[OPT_DOUBLE_FEED_DETECTION].constraint.string_list = s->hw->detect_double_feed;
  
  s->val[OPT_DOUBLE_FEED_DETECTION].w = 0;
  
  if (s->hw->sv->device_data.DoubleFeedDetectionValue.DoubleFeedDetection.capability.supportLevel == kSDISupportLevelNone)
  {
    s->opt[OPT_DOUBLE_FEED_DETECTION].cap |= SANE_CAP_INACTIVE;
  }

  /* gamma */
 
  s->opt[OPT_GAMMA].name = SANE_NAME_GAMMA_CORRECTION;
  s->opt[OPT_GAMMA].title = SANE_TITLE_GAMMA_CORRECTION;
  s->opt[OPT_GAMMA].desc = SANE_DESC_GAMMA_CORRECTION;

  s->opt[OPT_GAMMA].type = SANE_TYPE_FIXED;
  s->opt[OPT_GAMMA].unit = SANE_UNIT_NONE;
  s->opt[OPT_GAMMA].constraint_type = SANE_CONSTRAINT_RANGE;
  s->opt[OPT_GAMMA].constraint.range = &gamma_range;
  
  s->val[OPT_GAMMA].w = SANE_FIX(2.2);

  /* image count */
  s->opt[OPT_IMAGE_COUNT].name  = "image-count";
  s->opt[OPT_IMAGE_COUNT].title = SANE_I18N ("Image Count");
  s->opt[OPT_IMAGE_COUNT].desc  = SANE_I18N ("Image Count");
  s->opt[OPT_IMAGE_COUNT].type  = SANE_TYPE_INT;
  s->opt[OPT_IMAGE_COUNT].unit  = SANE_UNIT_NONE;
  s->opt[OPT_IMAGE_COUNT].size  = sizeof (SANE_Int);
  s->opt[OPT_IMAGE_COUNT].constraint_type = SANE_CONSTRAINT_RANGE;
  s->val[OPT_IMAGE_COUNT].w = 0; /* neutral */
  s->opt[OPT_IMAGE_COUNT].cap |= SANE_CAP_EMULATED;
  s->opt[OPT_IMAGE_COUNT].constraint.range = &image_count_range;

  /* long paper mode */
  s->opt[OPT_LONG_PAPER_MODE].name  = "long-paper-mode";
  s->opt[OPT_LONG_PAPER_MODE].title = SANE_I18N ("Long Paper Mode");
  s->opt[OPT_LONG_PAPER_MODE].desc  = SANE_I18N ("Select this mode if you want to scan documents longer than what the ADF would normally support. Please note that it only supports automatic detection of the document height.");
  s->opt[OPT_LONG_PAPER_MODE].type  = SANE_TYPE_BOOL;
  s->opt[OPT_LONG_PAPER_MODE].unit  = SANE_UNIT_NONE;
  s->opt[OPT_LONG_PAPER_MODE].size  = sizeof (SANE_Bool);
  s->opt[OPT_LONG_PAPER_MODE].cap  |= SANE_CAP_EMULATED;
  /*if ( s->hw->sv->device_data.DuplexType.capability.supportLevel == kSDISupportLevelAvailable)
    {
      s->opt[OPT_LONG_PAPER_MODE].cap &= ~SANE_CAP_INACTIVE; // Active
    }
  else
    {
      s->opt[OPT_LONG_PAPER_MODE].cap  |= SANE_CAP_INACTIVE; // Inactive
    }*/
  s->opt[OPT_LONG_PAPER_MODE].constraint_type  = SANE_CONSTRAINT_NONE;
  s->val[OPT_LONG_PAPER_MODE].w = SANE_FALSE;

  /* transfer format */
  s->opt[OPT_TRANSFER_FORMAT].name = "transfer-format";
  s->opt[OPT_TRANSFER_FORMAT].title = "Transfer Format";
  s->opt[OPT_TRANSFER_FORMAT].desc = "Selecting a compressed format such as JPEG(=yes) normally results in faster device side processing.";

  s->opt[OPT_TRANSFER_FORMAT].type = SANE_TYPE_BOOL;
  s->opt[OPT_TRANSFER_FORMAT].unit  = SANE_UNIT_NONE;
  s->opt[OPT_TRANSFER_FORMAT].size = sizeof (SANE_Bool);
  s->opt[OPT_TRANSFER_FORMAT].cap  |= SANE_CAP_EMULATED;
  s->opt[OPT_TRANSFER_FORMAT].constraint_type  = SANE_CONSTRAINT_NONE;

  if ( s->hw->sv->device_data.TransferCompression.capability.supportLevel == kSDISupportLevelNone )
  {
      s->val[OPT_TRANSFER_FORMAT].w = SANE_FALSE;
      s->opt[OPT_TRANSFER_FORMAT].cap  |= SANE_CAP_INACTIVE;
  }else {
    s->val[OPT_TRANSFER_FORMAT].w = SANE_TRUE;
  }

  /* remove background */
  s->opt[OPT_REMOVE_BACKGROUND].name = "remove-background";
  s->opt[OPT_REMOVE_BACKGROUND].title = "Remove Background";
  s->opt[OPT_REMOVE_BACKGROUND].desc = "Remove Background";

  s->opt[OPT_REMOVE_BACKGROUND].type = SANE_TYPE_STRING;
  s->opt[OPT_REMOVE_BACKGROUND].size = max_string_size (s->hw->remove_background);
  s->opt[OPT_REMOVE_BACKGROUND].constraint_type = SANE_CONSTRAINT_STRING_LIST;
  s->opt[OPT_REMOVE_BACKGROUND].constraint.string_list = s->hw->remove_background;
  
  s->val[OPT_REMOVE_BACKGROUND].w = 0;

  if ( s->hw->sv->device_data.BackgroundRemoval.capability.supportLevel == kSDISupportLevelNone )
  {
      s->opt[OPT_REMOVE_BACKGROUND].cap  |= SANE_CAP_INACTIVE;
  }

  /* background color */
  s->opt[OPT_BACKGROUND_COLOR].name  = "background-color";
  s->opt[OPT_BACKGROUND_COLOR].title = SANE_I18N ("Background Color");
  s->opt[OPT_BACKGROUND_COLOR].desc  = SANE_I18N ("Background Color");
  s->opt[OPT_BACKGROUND_COLOR].type  = SANE_TYPE_STRING;
  s->opt[OPT_BACKGROUND_COLOR].unit  = SANE_UNIT_NONE;
  s->opt[OPT_BACKGROUND_COLOR].size  = max_string_size (s->hw->background_color);
  s->opt[OPT_BACKGROUND_COLOR].constraint_type = SANE_CONSTRAINT_STRING_LIST;
  s->opt[OPT_BACKGROUND_COLOR].constraint.string_list = s->hw->background_color;
  s->val[OPT_BACKGROUND_COLOR].w = 0;
  if ( s->hw->sv->device_data.BackgroundColor.capability.supportLevel == kSDISupportLevelNone )
  {
    s->opt[OPT_BACKGROUND_COLOR].cap  |= SANE_CAP_INACTIVE;
  }
  
  /* text enhance */
  s->opt[OPT_TEXTENHANCE].name = "text-enhance";
  s->opt[OPT_TEXTENHANCE].title = "Text Enhance";
  s->opt[OPT_TEXTENHANCE].desc = "Text Enhance";
  s->opt[OPT_TEXTENHANCE].unit  = SANE_UNIT_NONE;
  s->opt[OPT_TEXTENHANCE].type = SANE_TYPE_STRING;
  s->opt[OPT_TEXTENHANCE].size = max_string_size (s->hw->text_enhance);
  s->opt[OPT_TEXTENHANCE].constraint_type = SANE_CONSTRAINT_STRING_LIST;
  s->opt[OPT_TEXTENHANCE].constraint.string_list = s->hw->text_enhance;
  
  s->val[OPT_TEXTENHANCE].w = 0;

  if ( s->hw->sv->device_data.textEnhance.capability.supportLevel == kSDISupportLevelNone )
  {
      s->opt[OPT_TEXTENHANCE].cap  |= SANE_CAP_INACTIVE;
  }

  /*Remove PunchHole*/
  s->opt[OPT_REMOVE_PUNCHHOLE].name  = "remove-punch-hole";
  s->opt[OPT_REMOVE_PUNCHHOLE].title = SANE_I18N ("Remove PunchHole");
  s->opt[OPT_REMOVE_PUNCHHOLE].desc  = SANE_I18N ("Remove Punch Hole in image");
  s->opt[OPT_REMOVE_PUNCHHOLE].type  = SANE_TYPE_BOOL;
  s->opt[OPT_REMOVE_PUNCHHOLE].unit  = SANE_UNIT_NONE;
  s->opt[OPT_REMOVE_PUNCHHOLE].size  = sizeof (SANE_Bool);
  s->opt[OPT_REMOVE_PUNCHHOLE].cap  |= SANE_CAP_EMULATED;
  if ( s->hw->sv->device_data.RemovePunchHoles.capability.supportLevel == kSDISupportLevelNone )
    {
      s->opt[OPT_REMOVE_PUNCHHOLE].cap  |= SANE_CAP_INACTIVE;
    }
  s->opt[OPT_REMOVE_PUNCHHOLE].constraint_type  = SANE_CONSTRAINT_NONE;
  s->val[OPT_REMOVE_PUNCHHOLE].w = SANE_FALSE;


  /*Deficiency Correction(Edge correction)*/
  s->opt[OPT_DEFICIENCY_CORRECTION].name  = "edge-correction";
  s->opt[OPT_DEFICIENCY_CORRECTION].title = SANE_I18N ("edge correction");
  s->opt[OPT_DEFICIENCY_CORRECTION].desc  = SANE_I18N ("Remove the shadow created around the scanned image");
  s->opt[OPT_DEFICIENCY_CORRECTION].type  = SANE_TYPE_BOOL;
  s->opt[OPT_DEFICIENCY_CORRECTION].unit  = SANE_UNIT_NONE;
  s->opt[OPT_DEFICIENCY_CORRECTION].size  = sizeof (SANE_Bool);
  s->opt[OPT_DEFICIENCY_CORRECTION].cap  |= SANE_CAP_EMULATED;
  if ( s->hw->sv->device_data.DeficiencyCorrection.capability.supportLevel == kSDISupportLevelNone )
    {
      s->opt[OPT_DEFICIENCY_CORRECTION].cap  |= SANE_CAP_INACTIVE;
    }
  s->opt[OPT_DEFICIENCY_CORRECTION].constraint_type  = SANE_CONSTRAINT_NONE;
  s->val[OPT_DEFICIENCY_CORRECTION].w = SANE_FALSE;

  s->hw->sv->device_data.PagesTobeScanned.select = 0;

  s->scan_ready = false;

  return SANE_STATUS_GOOD;
}

void
sane_close (SANE_Handle handle)
{
  log_call("--------------sane_close--------------");
  Epson_Scanner *s, *prev;
  size_t i;

  s = (Epson_Scanner *) handle;

  if (s)
  {
    log_call("Scan terminated.");

    if(s->out_buffer){
      free(s->out_buffer);
      s->out_buffer = NULL;
    }

    if(s->hw){
      s->hw->sv->Disconnecting_Scanner();
      s->hw->sv->Dispose_Scanner();
    }

    if(s->hw){
      free(s->hw);
      s->hw = NULL;
    }

    /* remove handle from list of open handles */
    prev = 0;
    for (s = first_handle; s; s = s->next)
    {
      if (s == handle)
        break;
      prev = s;
    }

    if(s){
      if (prev)
        prev->next = s->next;
      else
        first_handle = s->next;

      /* image data acquisition related resources */
      //for (i = 0; i < LINES_SHUFFLE_MAX; ++i)
      //  delete (s->line_buffer[i]);
      
      //delete (s);
      free (s);
    }
  }else {
    return;
  }
}


const SANE_Option_Descriptor *
sane_get_option_descriptor (SANE_Handle handle, SANE_Int option)
{
  log_call("--------------sane_get_option_descriptor--------------");
  Epson_Scanner *s = (Epson_Scanner *) handle;

  if (option < 0 || option >= NUM_OPTIONS)
    {
      log_call ("(%d)", option);
      return NULL;
    }

  log_call ("(%s)", s->opt[option].name);
  log_call("--------------END--------------");
  return (s->opt + option);
}

static SANE_Status
getvalue (Epson_Scanner *s, SANE_Int option, void *value)
{
  SANE_Option_Descriptor *sopt = &(s->opt[option]);
  Option_Value *sval = &(s->val[option]);

  switch (option)
  {
  case OPT_NUM_OPTS:
  case OPT_DUPLEX:
  case OPT_TRANSFER_FORMAT:
  case OPT_LONG_PAPER_MODE:
  case OPT_RESOLUTION:
  case OPT_TL_X:
  case OPT_TL_Y:
  case OPT_BR_X:
  case OPT_BR_Y:
  case OPT_SKIP_BLANKPAGES:
  case OPT_BRIGHTNESS:
  case OPT_CONTRAST:
  case OPT_THRESHOLD:
  case OPT_GAMMA:
  case OPT_DESKEW:
  case OPT_REMOVE_PUNCHHOLE:
  case OPT_DEFICIENCY_CORRECTION:
  case OPT_IMAGE_COUNT:
    *((SANE_Word *) value) = sval->w;
    break;
  case OPT_X_RESOLUTION:
  case OPT_Y_RESOLUTION:
    sval = &(s->val[OPT_RESOLUTION]);
    *((SANE_Word *) value) = sval->w;
    break;
  case OPT_MODE:
  case OPT_ROTATE:
  case OPT_DOUBLE_FEED_DETECTION:
  case OPT_DROPOUT:
  case OPT_REMOVE_BACKGROUND:
  case OPT_BACKGROUND_COLOR:
  case OPT_TEXTENHANCE:
  case OPT_SCAN_AREA:
  case OPT_SOURCE:
    strcpy ((char *) value, sopt->constraint.string_list[sval->w]);
    break;
  }

  return SANE_STATUS_GOOD;
}

static int
search_string_list_index (const SANE_String_Const * list, SANE_String value)
{
  int index = 0;

  while (*list != NULL && strcmp (value, *list) != 0)
  {
    ++list;
    index++;
  }

  return index;
}

static const SANE_String_Const *
search_string_list (const SANE_String_Const * list, SANE_String value)
{
  log_call ("(%s)", value);

  while (*list != NULL && strcmp (value, *list) != 0)
  {
    ++list;
  }

  return ((*list == NULL) ? NULL : list);
}

static SANE_Status
setvalue (Epson_Scanner *s, SANE_Int option, void *value, SANE_Int * info)
{
  SANE_Option_Descriptor *sopt = &(s->opt[option]);
  Option_Value *sval = &(s->val[option]);

  SANE_Status status;
  const SANE_String_Const *optval;
  int optindex;
  SANE_Bool reload = SANE_FALSE;
  log_call ("(%s, value @%p)", sopt->name, value);

  status = SANE_STATUS_GOOD;

  optval = NULL;
  optindex = 0;

  if (sopt->constraint_type == SANE_CONSTRAINT_STRING_LIST)
  {
    log_call ("(%s)", (SANE_String)value);
    optval = search_string_list (sopt->constraint.string_list,
				 (char *) value);

    if (optval == NULL)
      return SANE_STATUS_INVAL;
    optindex = optval - sopt->constraint.string_list;
    log_call ("optindex : %d", optindex);
  }

  status = set_epson_value (s, option, value);
  if(status != SANE_STATUS_GOOD){
    log_call ("%s value invalid", sopt->name);
    return status;
  }

  switch (option)
  {
    case OPT_DROPOUT:
      sval->w = optindex;		// Simple lists
      break;

    case OPT_RESOLUTION:
    case OPT_X_RESOLUTION:
    case OPT_Y_RESOLUTION:
      sval->w = *((SANE_Int *) value);
      reload = SANE_TRUE;
      break;
    case OPT_TL_X:
    case OPT_TL_Y:
    case OPT_BR_X:
    case OPT_BR_Y:
    case OPT_GAMMA:
      log_call ("value : %f", SANE_UNFIX(*((SANE_Word *) value)));
      log_call("s->val[OPT_TL_X].w : %f", s->val[OPT_TL_X].w);
      log_call("s->val[OPT_TL_Y].w : %f", s->val[OPT_TL_Y].w);
      log_call("s->val[OPT_BR_X].w : %f", s->val[OPT_BR_X].w);
      log_call("s->val[OPT_BR_Y].w : %f", s->val[OPT_BR_Y].w);
      sval->w = *((SANE_Int *) value);
      if (NULL != info)
        *info |= SANE_INFO_RELOAD_PARAMS;
      break;
    case OPT_DUPLEX:
    case OPT_TRANSFER_FORMAT:
    case OPT_LONG_PAPER_MODE:
      sval->w = *((SANE_Int *) value);
      break;
    case OPT_DOUBLE_FEED_DETECTION:
    case OPT_REMOVE_BACKGROUND:
    case OPT_BACKGROUND_COLOR:
    case OPT_TEXTENHANCE:
    case OPT_ROTATE:
    case OPT_SOURCE:
    case OPT_SCAN_AREA:
      sval->w = optindex;
      reload = SANE_TRUE;
      break;

    case OPT_MODE:
      {
        if (sval->w == optindex)
          break;

        sval->w = optindex;
        reload = SANE_TRUE;
        break;
      }

    /*case OPT_BRIGHTNESS_METHOD:
      {
        const SANE_Range *r;
        SANE_Bool f
          = s->hw->gamma_user_defined[s->val[OPT_GAMMA_CORRECTION].w];

        if (sval->w == optindex) break;

        r = s->opt[OPT_BRIGHTNESS].constraint.range;

        if (0 == strcmp_c (brightness_method_list[0], // "hardware"
                          sopt->constraint.string_list[optindex]))
          {
            s->opt[OPT_BRIGHTNESS].cap &= ~SANE_CAP_EMULATED;
            s->opt[OPT_BRIGHTNESS].constraint.range = &s->hw->cmd->bright_range;
            setOptionState (s, !f, OPT_BRIGHTNESS, &reload);
          }
        else
          {
            s->opt[OPT_BRIGHTNESS].cap |= SANE_CAP_EMULATED;
            s->opt[OPT_BRIGHTNESS].constraint.range = &brightness_range;
            setOptionState (s, SANE_TRUE, OPT_BRIGHTNESS, &reload);
          }

        if (r != s->opt[OPT_BRIGHTNESS].constraint.range)
          {
            double v = s->val[OPT_BRIGHTNESS].w;

            if (0 < v)
              {
                require (0 < r->max);

                v /= r->max;
                r = s->opt[OPT_BRIGHTNESS].constraint.range;
                v *= r->max;
                v += 0.5;

                reload = SANE_TRUE;
              }
            else if (0 > v)
              {
                require (0 > r->min);

                v /= r->min;
                r = s->opt[OPT_BRIGHTNESS].constraint.range;
                v *= r->min;
                v -= 0.5;

                reload = SANE_TRUE;
              }
            else                  // 0 == v
              {}

            s->val[OPT_BRIGHTNESS].w = (SANE_Int) v;
          }

        sval->w = optindex;

        break;
      }
  */
    case OPT_SKIP_BLANKPAGES:
    case OPT_BRIGHTNESS:
    case OPT_CONTRAST:
    case OPT_THRESHOLD:
    case OPT_IMAGE_COUNT:
    //case OPT_TRANSFER_SIZE:
      sval->w = *((SANE_Int *) value);
      break;

    case OPT_DESKEW:
    case OPT_REMOVE_PUNCHHOLE:
    case OPT_DEFICIENCY_CORRECTION:
      break;
    default:
      return SANE_STATUS_INVAL;
    }

    if (reload && info != NULL)
    {
      *info |= SANE_INFO_RELOAD_OPTIONS | SANE_INFO_RELOAD_PARAMS;
    }
    
  if(sopt->name){
    SANE_Status status = set_epson_value (s, option, value);
    if(status != SANE_STATUS_GOOD){
      log_call ("%s value invalid", sopt->name);
      return status;
    }
  }

  return status;
}


SANE_Status
sane_control_option (SANE_Handle handle,
		     SANE_Int option,
		     SANE_Action action, void *value, SANE_Int * info)
{
  log_call("--------------sane_control_option--------------");
  if (option < 0 || option >= NUM_OPTIONS)
    return SANE_STATUS_INVAL;

  if (info != NULL)
    *info = 0;

  switch (action)
  {
  case SANE_ACTION_GET_VALUE:
    log_call("getvalue");
    return (getvalue ((Epson_Scanner *)handle, option, value));

  case SANE_ACTION_SET_VALUE:
    log_call("setvalue");
    return (setvalue ((Epson_Scanner *)handle, option, value, info));
  default:
    return SANE_STATUS_INVAL;
  }

  return SANE_STATUS_GOOD;
}

SANE_Status
sane_start (SANE_Handle handle)
{
  log_call("--------------sane_start--------------");
  Epson_Scanner *s = (Epson_Scanner *) handle;
  SANE_Status status = SANE_STATUS_GOOD;

  s->cancel_requested = false;
  s->scan_complete = false;
  s->scan_ready = true;

  //Sane supported image format RAW only.
  s->hw->sv->device_data.ImageFormatSettings.ImageFormat.select = kSDIImageFormatRaw;
  s->hw->sv->device_data.TransferCompression.select = true;

  //スキャン開始直前で解像度と定型サイズからスキャン領域を計算
  if(!handle_scan_area (s, (char *) s->hw->cur_doc_size)){
    select_current_doc_size(s, s->hw->sv->device_data);
    handle_scan_area (s, (char *) s->hw->cur_doc_size);
  }

  //simple-scanはサイズ自動時に最大サイズで設定してくる以外に判別ができないため
  if(sane_app_compatible("simple-scan")){
    double max_x = s->hw->sv->device_data.ScanArea.MaxScanAreaWidth.select / 100.0 * s->hw->sv->device_data.Resolution.select;
    double max_y = s->hw->sv->device_data.ScanArea.MaxScanAreaHeight.select / 100.0 * s->hw->sv->device_data.Resolution.select;
    if(max_x <= s->hw->sv->device_data.ScanArea.ScanAreaWidth.select &&
       max_y <= s->hw->sv->device_data.ScanArea.ScanAreaHeight.select){
      s->hw->sv->device_data.ScanArea.AutoSize.select = true;
    }else {
      s->hw->sv->device_data.ScanArea.AutoSize.select = false;
    }
  }

  if (0 == s->image_count)
  {
      log_call("----SDIScannerDriver_DoScanJobPtr_----");

      // functional unit update
    if (!(s->opt[OPT_SOURCE].cap & SANE_CAP_INACTIVE))
    {
        int size = s->opt[OPT_SOURCE].size;
        if (size > 1)
        {
          int current = s->val[OPT_SOURCE].w; 
          if (current < size)
          {       
            const char* value = s->opt[OPT_SOURCE].constraint.string_list[current];
            if (strcmp(value, AUTO_STR) == 0) {    
              log_call ("size = auto");
              if(s->hw->sv->GetPaperLoadedStatus())
              {
                log_call ("paper Loaded");
                s->hw->sv->device_data.FunctionalUnit.select = kSDIFunctionalUnitDocumentFeeder;
              } else {
                log_call ("No paper");
                s->hw->sv->device_data.FunctionalUnit.select = kSDIFunctionalUnitFlatbed;
              }
            }
          }
        }
     }
  
    s->hw->sv->Set_All_Value();
    log_call("duplex = %d", s->val[OPT_DUPLEX].w);
    s->hw->sv->SDIScannerDriver_DoScanJobPtr_(s->hw->sv->driver, kSDIOperationTypeNew);
    /* Back up original SANE options.
     * \todo string and word array have to copy individually.
     */
    memcpy (s->val_bak, s->val, (sizeof (s->val[0]) * NUM_OPTIONS));
  }else{
      if(!s->hw->sv->SDIScannerDriver_CheckNextTransferEventPtr_(s->hw->sv->driver, true))
      {
          status = SANE_STATUS_NO_DOCS;
      }
  }
  /*else if (!s->val[OPT_AUTO_SIZE].w)
  {
    /* Restore original SANE options.
     * \todo string and word array have to copy individually.
     */
  //  memcpy (s->val, s->val_bak, (sizeof (s->val[0]) * NUM_OPTIONS));
 // }

  /*if (!scan_area_is_valid (s))
    {
      err_fatal ("The image data resulting from the combination of the "
                 "specified scan area and resolution is too large.");
      return SANE_STATUS_INVAL;
    }*/

  return status;
}

static void
create_functionalunit_list (device *dev, int32_t item_list[], int32_t list_size)
{
  log_call("create_functionalunit_list");

  int i = 0;
  int current = 0;

  bool flatbedSupport = false;
  bool adfSupport = false;

// Confirm Flatbed and Document Feeder support
  i = 0;
  while(i < list_size) {
    if(item_list[i] == kSDIFunctionalUnitFlatbed) {
        flatbedSupport = true;
    }
    if(item_list[i] == kSDIFunctionalUnitDocumentFeeder) {
        adfSupport = true;
    }
    i++;
  }

  if (flatbedSupport && adfSupport)
  {
      dev->doc_sources[current++] = AUTO_STR;
  }

  i = 0;
  while(i < list_size){
    if(item_list[i] == kSDIFunctionalUnitFlatbed){
      dev->doc_sources[current++] = FB_STR;
    }else if(item_list[i] == kSDIFunctionalUnitDocumentFeeder){
      dev->doc_sources[current++] = ADF_STR;
      dev->doc_sources[current++] = ADF_FRONT_STR;
    }else if(item_list[i] == kSDIFunctionalUnitCarrierSheet){
      dev->doc_sources[current++] = CARRIERSHEET_STR;
    }
    i++;
  }
  dev->doc_sources[current] = NULL;    /* terminator */

  log_call("create_functionalunit_list end");
}

static void
create_double_feed_detection_list (device *dev, int32_t item_list[], int32_t list_size)
{
  int i = 0;
  int item_num = 0;
  dev->detect_double_feed[item_num++] = DETECT_DOUBLE_FEED_NONE_STR;
  while(i < list_size){
    if(item_list[i] == kSDIDoubleFeedDetectionLow){
      log_call("kSDIDoubleFeedDetectionLow");
      dev->detect_double_feed[item_num++] = DETECT_DOUBLE_FEED_THIN_STR;
    }else if(item_list[i] == kSDIDoubleFeedDetectionHigh){
      log_call("kSDIDoubleFeedDetectionHigh");
      dev->detect_double_feed[item_num++] = DETECT_DOUBLE_FEED_STANDARD_STR;
    }
    i++;
  }
  dev->detect_double_feed[item_num] = NULL;    /* terminator */
}

void
select_current_doc_size(Epson_Scanner * s, SCANPARA device_data)
{
  if(device_data.ScanArea.AutoSize.select == 1){
    log_call("DOC_AUTO_STR");
    s->hw->cur_doc_size = DOC_AUTO_STR;
  }else if(device_data.ScanArea.AutoSize.select == 2){
     log_call("DOC_AUTO_LONG_STR");
    s->hw->cur_doc_size = DOC_AUTO_LONG_STR;
  }else if(device_data.ScanArea.FixedSize == A3){
    log_call("DOC_A3_STR");
    s->hw->cur_doc_size = DOC_A3_STR;
  }else if(device_data.ScanArea.FixedSize == A4){
    s->hw->cur_doc_size = DOC_A4_STR;
  }else if(device_data.ScanArea.FixedSize == A4_Landscape){
    s->hw->cur_doc_size = DOC_A4_L_STR;
  }else if(device_data.ScanArea.FixedSize == A5){
    s->hw->cur_doc_size = DOC_A5_STR;
  }else if(device_data.ScanArea.FixedSize == A5_Landscape){
    s->hw->cur_doc_size = DOC_A5_L_STR;
  }else if(device_data.ScanArea.FixedSize == A6){
    s->hw->cur_doc_size = DOC_A6_STR;
  }else if(device_data.ScanArea.FixedSize == A6_Landscape){
    s->hw->cur_doc_size = DOC_A6_L_STR;
  }else if(device_data.ScanArea.FixedSize == A8){
    s->hw->cur_doc_size = DOC_A8_STR;
  }else if(device_data.ScanArea.FixedSize == A8_Landscape){
    s->hw->cur_doc_size = DOC_A8_L_STR;
  }else if(device_data.ScanArea.FixedSize == B4_JIS){
    s->hw->cur_doc_size = DOC_B4_STR;
  }else if(device_data.ScanArea.FixedSize == B4_JIS_Landscape){
    s->hw->cur_doc_size = DOC_B4_L_STR;
  }else if(device_data.ScanArea.FixedSize == B5_JIS){
    s->hw->cur_doc_size = DOC_B5_STR;
  }else if(device_data.ScanArea.FixedSize == B5_JIS_Landscape){
    s->hw->cur_doc_size = DOC_B5_L_STR;
  }else if(device_data.ScanArea.FixedSize == Letter){
    s->hw->cur_doc_size = DOC_LETTER_STR;
  }else if(device_data.ScanArea.FixedSize == Letter_Landscape){
    s->hw->cur_doc_size = DOC_LETTER_L_STR;
  }else if(device_data.ScanArea.FixedSize == Legal){
    s->hw->cur_doc_size = DOC_LEGAL_STR;
  }else if(device_data.ScanArea.FixedSize == Postcard){
    s->hw->cur_doc_size = DOC_POSTCARD_STR;
  }else if(device_data.ScanArea.FixedSize == Postcard_Landscape){
    s->hw->cur_doc_size = DOC_POSTCARD_L_STR;
  }else if(device_data.ScanArea.FixedSize == PlasticCard){
    s->hw->cur_doc_size = DOC_PLASTICCARD_STR;
  }else if(device_data.ScanArea.FixedSize == Tabloid){
    s->hw->cur_doc_size = DOC_TABLOID_STR;
  }else if(device_data.ScanArea.FixedSize == Maximum_Size){
    s->hw->cur_doc_size = DOC_MAXIMUM_STR;
  }else {
    s->hw->cur_doc_size = DOC_MANUAL_STR;
  }
}

static void
set_fixed_size(Epson_Scanner * s, SANE_String value)
{
  s->hw->sv->device_data.ScanArea.AutoSize.select = 0;
  if(strcmp(value, DOC_AUTO_STR) == 0){
    s->hw->sv->device_data.ScanArea.AutoSize.select = 1;
  }else if(strcmp(value, DOC_AUTO_LONG_STR) == 0){
    s->hw->sv->device_data.ScanArea.AutoSize.select = 2;
  }else if(strcmp(value, DOC_A3_STR) == 0){
    s->hw->sv->device_data.ScanArea.FixedSize = A3;
  }else if(strcmp(value, DOC_A4_STR) == 0){
    s->hw->sv->device_data.ScanArea.FixedSize = A4;
  }else if(strcmp(value, DOC_A4_L_STR) == 0){
    s->hw->sv->device_data.ScanArea.FixedSize = A4_Landscape;
  }else if(strcmp(value, DOC_A5_STR) == 0){
    s->hw->sv->device_data.ScanArea.FixedSize = A5;
  }else if(strcmp(value, DOC_A5_L_STR) == 0){
    s->hw->sv->device_data.ScanArea.FixedSize = A5_Landscape;
  }else if(strcmp(value, DOC_A6_STR) == 0){
    s->hw->sv->device_data.ScanArea.FixedSize = A6;
  }else if(strcmp(value, DOC_A6_L_STR) == 0){
    s->hw->sv->device_data.ScanArea.FixedSize = A6_Landscape;
  }else if(strcmp(value, DOC_A8_STR) == 0){
    s->hw->sv->device_data.ScanArea.FixedSize = A8;
  }else if(strcmp(value, DOC_A8_L_STR) == 0){
    s->hw->sv->device_data.ScanArea.FixedSize = A8_Landscape;
  }else if(strcmp(value, DOC_B4_STR) == 0){
    s->hw->sv->device_data.ScanArea.FixedSize = B4_JIS;
  }else if(strcmp(value, DOC_B4_L_STR) == 0){
    s->hw->sv->device_data.ScanArea.FixedSize = B4_JIS_Landscape;
  }else if(strcmp(value, DOC_B5_STR) == 0){
    s->hw->sv->device_data.ScanArea.FixedSize = B5_JIS;
  }else if(strcmp(value, DOC_B5_L_STR) == 0){
    s->hw->sv->device_data.ScanArea.FixedSize = B5_JIS_Landscape;
  }else if(strcmp(value, DOC_LETTER_STR) == 0){
    s->hw->sv->device_data.ScanArea.FixedSize = Letter;
  }else if(strcmp(value, DOC_LETTER_L_STR) == 0){
    s->hw->sv->device_data.ScanArea.FixedSize = Letter_Landscape;
  }else if(strcmp(value, DOC_LEGAL_STR) == 0){
    s->hw->sv->device_data.ScanArea.FixedSize = Legal;
  }else if(strcmp(value, DOC_POSTCARD_STR) == 0){
    s->hw->sv->device_data.ScanArea.FixedSize = Postcard;
  }else if(strcmp(value, DOC_POSTCARD_L_STR) == 0){
    s->hw->sv->device_data.ScanArea.FixedSize = Postcard_Landscape;
  }else if(strcmp(value, DOC_PLASTICCARD_STR) == 0){
    s->hw->sv->device_data.ScanArea.FixedSize = PlasticCard;
  }else if(strcmp(value, DOC_TABLOID_STR) == 0){
    s->hw->sv->device_data.ScanArea.FixedSize = Tabloid;
  }else if(strcmp(value, DOC_MAXIMUM_STR) == 0){
    s->hw->sv->device_data.ScanArea.FixedSize = Maximum_Size;
  }else if(strcmp(value, DOC_MANUAL_STR) == 0){
    s->hw->sv->device_data.ScanArea.FixedSize = Manual;
  }
}

static void
create_document_size_list (device *dev)
{
  int item_num = 0;

  double max_x_pixel = dev->sv->device_data.ScanArea.MaxScanAreaWidth.select;
  double max_y_pixel = dev->sv->device_data.ScanArea.MaxScanAreaHeight.select;
  double min_x_pixel = 0;//TBD
  double min_y_pixel = 0;//TBD

  double max_x_inchi = max_x_pixel / BASE_RESOLUTION;
  double max_y_inchi = max_y_pixel / BASE_RESOLUTION;
  double min_x_inchi = min_x_pixel / BASE_RESOLUTION;//TBD
  double min_y_inchi = min_y_pixel / BASE_RESOLUTION;//TBD

  for(int i = 0; i < dev->sv->device_data.ScanArea.AutoSize.capability.countOfList; i++){
    if(dev->sv->device_data.ScanArea.AutoSize.capability.list[i] == AutoSizeStandard){
      dev->document_size[item_num++] = DOC_AUTO_STR;
    }else if(dev->sv->device_data.ScanArea.AutoSize.capability.list[i] == AutoSizeLongPaper){
      dev->document_size[item_num++] = DOC_AUTO_LONG_STR;
    }
  }

  if(min_x_inchi <= 8.5 && 8.5 <= max_x_inchi){
    if(min_y_inchi <= 11 && 11 <= max_y_inchi){
      dev->document_size[item_num++] = DOC_LETTER_STR;
    }
  }

  if(min_x_inchi <= 11 && 11 <= max_x_inchi){
    if(min_y_inchi <= 8.5 && 8.5 <= max_y_inchi){
      dev->document_size[item_num++] = DOC_LETTER_L_STR;
    }
  }

  if(min_x_inchi <= 8.5 && 8.5 <= max_x_inchi){
    if(min_y_inchi <= 14 && 14 <= max_y_inchi){
      dev->document_size[item_num++] = DOC_LEGAL_STR;
    }
  }

  if(min_x_inchi <= 11.7 && 11.7 <= max_x_inchi){
    if(min_y_inchi <= 16.5 && 16.5 <= max_y_inchi){
      dev->document_size[item_num++] = DOC_A3_STR;
    }
  }

  if(min_x_inchi <= 8.3 && 8.3 <= max_x_inchi){
    if(min_y_inchi <= 11.7 && 11.7 <= max_y_inchi){
      dev->document_size[item_num++] = DOC_A4_STR;
    }
  }

  if(min_x_inchi <= 11.7 && 11.7 <= max_x_inchi){
    if(min_y_inchi <= 8.3 && 8.3 <= max_y_inchi){
      dev->document_size[item_num++] = DOC_A4_L_STR;
    }
  }

  if(min_x_inchi <= 5.8 && 5.8 <= max_x_inchi){
    if(min_y_inchi <= 8.3 && 8.3 <= max_y_inchi){
      dev->document_size[item_num++] = DOC_A5_STR;
    }
  }

  if(min_x_inchi <= 8.3 && 8.3 <= max_x_inchi){
    if(min_y_inchi <= 5.8 && 5.8 <= max_y_inchi){
      dev->document_size[item_num++] = DOC_A5_L_STR;
    }
  }

  if(min_x_inchi <= 4.1 && 4.1 <= max_x_inchi){
    if(min_y_inchi <= 5.8 && 5.8 <= max_y_inchi){
      dev->document_size[item_num++] = DOC_A6_STR;
    }
  }

  if(min_x_inchi <= 5.8 && 5.8 <= max_x_inchi){
    if(min_y_inchi <= 4.1 && 4.1 <= max_y_inchi){
      dev->document_size[item_num++] = DOC_A6_L_STR;
    }
  }

  if(min_x_inchi <= 2 && 2 <= max_x_inchi){
    if(min_y_inchi <= 2.9 && 2.9 <= max_y_inchi){
      dev->document_size[item_num++] = DOC_A8_STR;
    }
  }

  if(min_x_inchi <= 2.9 && 2.9 <= max_x_inchi){
    if(min_y_inchi <= 2 && 2 <= max_y_inchi){
      dev->document_size[item_num++] = DOC_A8_L_STR;
    }
  }

  if(min_x_inchi <= 10.1 && 10.1 <= max_x_inchi){
    if(min_y_inchi <= 14.3 && 14.3 <= max_y_inchi){
      dev->document_size[item_num++] = DOC_B4_STR;
    }
  }

  if(min_x_inchi <= 14.3 && 14.3 <= max_x_inchi){
    if(min_y_inchi <= 10.1 && 10.1 <= max_y_inchi){
      dev->document_size[item_num++] = DOC_B4_L_STR;
    }
  }

  if(min_x_inchi <= 7.2 && 7.2 <= max_x_inchi){
    if(min_y_inchi <= 10.1 && 10.1 <= max_y_inchi){
      dev->document_size[item_num++] = DOC_B5_STR;
    }
  }

  if(min_x_inchi <= 10.1 && 10.1 <= max_x_inchi){
    if(min_y_inchi <= 7.2 && 7.2 <= max_y_inchi){
      dev->document_size[item_num++] = DOC_B5_L_STR;
    }
  }

  if(min_x_inchi <= 11 && 11 <= max_x_inchi){
    if(min_y_inchi <= 17 && 17 <= max_y_inchi){
      dev->document_size[item_num++] = DOC_TABLOID_STR;
    }
  }

  if(min_x_inchi <= 3.9 && 3.9 <= max_x_inchi){
    if(min_y_inchi <= 5.8 && 5.8 <= max_y_inchi){
      dev->document_size[item_num++] = DOC_POSTCARD_STR;
    }
  }

  if(min_x_inchi <= 5.8 && 5.8 <= max_x_inchi){
    if(min_y_inchi <= 3.9 && 3.9 <= max_y_inchi){
      dev->document_size[item_num++] = DOC_POSTCARD_L_STR;
    }
  }

  if(min_x_inchi <= 2.1 && 2.1 <= max_x_inchi){
    if(min_y_inchi <= 3.4 && 3.4 <= max_y_inchi){
      //dev->document_size[item_num++] = DOC_PLASTICCARD_STR;
    }
  }

  if(min_x_inchi <= 3.4 && 3.4 <= max_x_inchi){
    if(min_y_inchi <= 2.1 && 2.1 <= max_y_inchi){
      dev->document_size[item_num++] = DOC_PLASTICCARD_STR;
    }
  }

  dev->document_size[item_num++] = DOC_MAXIMUM_STR;

  dev->document_size[item_num++] = DOC_MANUAL_STR;
  dev->document_size[item_num] = NULL;  

  //list_size = item_num;
}

SANE_Status
restrict_value_range(const int value, const int max, const int min)
{
  if( value > max ||
     value < min){
    return SANE_STATUS_INVAL;
  }
  return SANE_STATUS_GOOD;
}

static SANE_Status
set_epson_value (Epson_Scanner *s, SANE_Int option, void *value)
{
  SANE_Option_Descriptor *sopt = &(s->opt[option]);

  SANE_Status status = SANE_STATUS_GOOD;

  Option_Value *sval = &(s->val[option]);

  int pixel_value = 0;

  switch (option)
  {
    case OPT_DROPOUT:
      handle_dropout (s, (char *) value);
      break;
    case OPT_RESOLUTION:
    case OPT_X_RESOLUTION:
    case OPT_Y_RESOLUTION:
      if(restrict_value_range(*((SANE_Int *) value),
        s->hw->dpi_range.max,
        s->hw->dpi_range.min) != SANE_STATUS_GOOD){
        return SANE_STATUS_INVAL;
      }
      handle_resolution(s, *((SANE_Int *) value));
      break;
    case OPT_TL_X:
      pixel_value = (int)(calculate_pixel_size(BASE_RESOLUTION, SANE_UNFIX(*((SANE_Int *) value))) + 0.5555);
      if(restrict_value_range(pixel_value,
        s->hw->sv->device_data.ScanArea.MaxScanAreaWidth.select - 1,
        0) != SANE_STATUS_GOOD){
        log_call("size over.");
        handle_top_left_x (s, 0);
      }else {
        handle_top_left_x (s, (int)(calculate_pixel_size(s->hw->sv->device_data.Resolution.select, SANE_UNFIX(*((SANE_Int *) value))) + 0.5555));
      }

      /*if(restrict_value_range(*((SANE_Word *) value),
        s->hw->sv->device_data.ScanArea.MaxScanAreaWidth.select - 1,
        0) != SANE_STATUS_GOOD){
        return SANE_STATUS_INVAL;
      }
      handle_top_left_x (s, *((SANE_Word *) value));*/
      break;
    case OPT_TL_Y:
      pixel_value = (int)(calculate_pixel_size(BASE_RESOLUTION, SANE_UNFIX(*((SANE_Int *) value))) + 0.5555);
      if(restrict_value_range(pixel_value,
        s->hw->sv->device_data.ScanArea.MaxScanAreaHeight.select - 1,
        0) != SANE_STATUS_GOOD){
        log_call("size over.");
        handle_top_left_y (s, 0);
      }else {
        handle_top_left_y (s, (int)(calculate_pixel_size(s->hw->sv->device_data.Resolution.select, SANE_UNFIX(*((SANE_Int *) value))) + 0.5555));
      }

      /*if(restrict_value_range(*((SANE_Word *) value),
        s->hw->sv->device_data.ScanArea.MaxScanAreaHeight.select,
        0) != SANE_STATUS_GOOD){
        return SANE_STATUS_INVAL;
      }
      handle_top_left_y (s, *((SANE_Word *) value));*/
      break;
    case OPT_BR_X:
      //if(strcmp(s->hw->cur_doc_size, DOC_MANUAL_STR) == 0){
        pixel_value = (int)(calculate_pixel_size(BASE_RESOLUTION, SANE_UNFIX(*((SANE_Int *) value))) + 0.5555);
        if(restrict_value_range(pixel_value,//*((SANE_Word *) value),
          s->hw->sv->device_data.ScanArea.MaxScanAreaWidth.select,
          0) != SANE_STATUS_GOOD){
          log_call("size over.");
          handle_bottom_right_x (s, s->hw->sv->device_data.ScanArea.MaxScanAreaWidth.select / BASE_RESOLUTION * s->hw->sv->device_data.Resolution.select);
        }else {
          handle_bottom_right_x (s, (int)(calculate_pixel_size(s->hw->sv->device_data.Resolution.select, SANE_UNFIX(*((SANE_Int *) value))) + 0.5555));
        }
        //handle_bottom_right_x (s, pixel_value);//*((SANE_Word *) value));
      //}
      break;
    case OPT_BR_Y:
      //if(strcmp(s->hw->cur_doc_size, DOC_MANUAL_STR) == 0){
        pixel_value = (int)(calculate_pixel_size(BASE_RESOLUTION, SANE_UNFIX(*((SANE_Int *) value))) + 0.5555);
        if(restrict_value_range(pixel_value,
          s->hw->sv->device_data.ScanArea.MaxScanAreaHeight.select,
          0) != SANE_STATUS_GOOD){
          log_call("size over.");
          handle_bottom_right_y (s, s->hw->sv->device_data.ScanArea.MaxScanAreaHeight.select / BASE_RESOLUTION * s->hw->sv->device_data.Resolution.select);
        }else {
          handle_bottom_right_y (s, (int)(calculate_pixel_size(s->hw->sv->device_data.Resolution.select, SANE_UNFIX(*((SANE_Int *) value))) + 0.5555));
        }
      //}

      /*if(restrict_value_range(*((SANE_Word *) value),
        s->hw->sv->device_data.ScanArea.MaxScanAreaHeight.select,
        0) != SANE_STATUS_GOOD){
        *((SANE_Word *) value) = s->hw->sv->device_data.ScanArea.MaxScanAreaHeight.select;
      }
      handle_bottom_right_y (s, *((SANE_Word *) value));*/
      break;
    case OPT_DUPLEX:
      handle_duplex (s, *((SANE_Word *) value));
      break;
    case OPT_LONG_PAPER_MODE:
      handle_long_paper_mode (s, *((SANE_Word *) value));
      break;
    case OPT_DOUBLE_FEED_DETECTION:
      handle_double_feed_detection (s, (char *) value);
      break;
    case OPT_TRANSFER_FORMAT:
      handle_transfer_format (s, (char *) value);
      break;
    case OPT_REMOVE_BACKGROUND:
      handle_remove_background (s, (char *) value);
      break;
    case OPT_BACKGROUND_COLOR:
      handle_background_color (s, (char *) value);
      break;
    case OPT_TEXTENHANCE:
      handle_text_enhance  (s, (char *) value);
      break;
    case OPT_ROTATE:
      handle_rotate (s, (char *) value);
      break;
    case OPT_SOURCE:
      handle_source (s, (char *) value);
      break;
    case OPT_SCAN_AREA:
      set_fixed_size (s, (char *) value);
      s->hw->cur_doc_size = (SANE_String)value;
      break;
    case OPT_GAMMA:
      if(restrict_value_range(*((SANE_Word *) value),
        gamma_range.max,
        gamma_range.min) != SANE_STATUS_GOOD){
        return SANE_STATUS_INVAL;
      }
      log_call("gamma = %d", value);
      handle_gamma (s, (SANE_UNFIX(*((SANE_Int *) value))*10));
      break;
    case OPT_MODE:
      handle_mode (s, (char *) value);
      break;
    case OPT_SKIP_BLANKPAGES:
      if(restrict_value_range(*((SANE_Word *) value),
        skip_blankpages_range.max,
        skip_blankpages_range.min) != SANE_STATUS_GOOD){
        return SANE_STATUS_INVAL;
      }
      handle_skip_blankpages (s, *((SANE_Word *) value));
      break;
    case OPT_BRIGHTNESS:
      if(restrict_value_range(*((SANE_Word *) value),
        brightness_range.max,
        brightness_range.min) != SANE_STATUS_GOOD){
        return SANE_STATUS_INVAL;
      }
      log_call("brightne = %d", value);
      handle_brightness (s, *((SANE_Word *) value));
      break;
    case OPT_CONTRAST:
      if(restrict_value_range(*((SANE_Word *) value),
        contrast_range.max,
        contrast_range.min) != SANE_STATUS_GOOD){
        return SANE_STATUS_INVAL;
      }
      handle_contrast (s, *((SANE_Word *) value));
      break;
    case OPT_THRESHOLD:
      if(restrict_value_range(*((SANE_Word *) value),
        u8_range.max,
        u8_range.min) != SANE_STATUS_GOOD){
        return SANE_STATUS_INVAL;
      }
      handle_threshold (s, *((SANE_Word *) value));
      break;
    case OPT_IMAGE_COUNT:
      s->hw->sv->device_data.PagesTobeScanned.select =  *((SANE_Word *) value);
      break;
    case OPT_DESKEW:
      s->hw->sv->device_data.PaperDeskew.select = *((SANE_Word *) value);
      break;

    case OPT_REMOVE_PUNCHHOLE:
      s->hw->sv->device_data.RemovePunchHoles.select = *((SANE_Word *) value);
      break;

    case OPT_DEFICIENCY_CORRECTION:
      s->hw->sv->device_data.DeficiencyCorrection.select = *((SANE_Word *) value);
      break;

      break;
    default:
      return SANE_STATUS_INVAL;
    }

  return status;
}

static void
handle_dropout (Epson_Scanner *s, SANE_String value)
{
  log_call ("(%s)", value);

  if(strcmp(value, DROPOUT_NONE_STR) == 0){
    s->hw->sv->device_data.DropoutColor.select = kSDIDropoutColorNone;
  }else if(strcmp(value, DROPOUT_RED_STR) == 0){
    s->hw->sv->device_data.DropoutColor.select = kSDIDropoutColorRed;
  }else if(strcmp(value, DROPOUT_GREEN_STR) == 0) {
    s->hw->sv->device_data.DropoutColor.select = kSDIDropoutColorGreen;
  }else if(strcmp(value, DROPOUT_BLUE_STR) == 0) {
    s->hw->sv->device_data.DropoutColor.select = kSDIDropoutColorBlue;
  }
}

static void
handle_rotate (Epson_Scanner *s, SANE_String value)
{
  log_call ("(%s)", value);

  if(strcmp(value, ROTATE_0_STR) == 0){
    s->hw->sv->device_data.Orientation.select = kSDIOrientationNone;
  }else if(strcmp(value, ROTATE_90_STR) == 0){
    s->hw->sv->device_data.Orientation.select = kSDIOrientation90;
  }else if(strcmp(value, ROTATE_180_STR) == 0) {
    s->hw->sv->device_data.Orientation.select = kSDIOrientation180;
  }else if(strcmp(value, ROTATE_270_STR) == 0) {
    s->hw->sv->device_data.Orientation.select = kSDIOrientation270;
  }else if(strcmp(value, ROTATE_AUTO_STR) == 0) {
    s->hw->sv->device_data.Orientation.select = kSDIOrientationAuto;
  }
}

static void
handle_resolution (Epson_Scanner *s, SANE_Int value)
{
  log_call ("resolution : (%d)", value);
  s->hw->sv->device_data.Resolution.select = (SDIInt)value;
}

static void
handle_mode (Epson_Scanner *s, SANE_String value)
{
  log_call ("(%s)", value);

  if(strcmp(value, COLOR_STR) == 0){
    s->hw->sv->device_data.ColorType.select = kSDIColorTypeRGB24;
  }else if(strcmp(value, GRAYSCALE_STR) == 0){
    s->hw->sv->device_data.ColorType.select = kSDIColorTypeMono8;
  }else if(strcmp(value, MONOCHROME_STR) == 0) {
    s->hw->sv->device_data.ColorType.select = kSDIColorTypeMono1;
  }
}

static void
handle_duplex (Epson_Scanner *s, SANE_Bool value)
{
  if(value){
    log_call("-Double_Sided-");
    s->hw->sv->device_data.DuplexType.select = Double_Sided;
  }else {
    log_call("-Single_Sided-");
    s->hw->sv->device_data.DuplexType.select = Single_Sided;
  }
}

static void
handle_source (Epson_Scanner *s, SANE_String value)
{
  log_call ("(%s)", value);

 if(strcmp(value, FB_STR) == 0){
    s->hw->sv->device_data.FunctionalUnit.select = kSDIFunctionalUnitFlatbed;
  }else if(strcmp(value, ADF_STR) == 0) {
    s->hw->sv->device_data.FunctionalUnit.select = kSDIFunctionalUnitDocumentFeeder;
  }else if(strcmp(value, ADF_FRONT_STR) == 0) {
    s->hw->sv->device_data.FunctionalUnit.select = kSDIFunctionalUnitDocumentFeeder;
    s->hw->sv->device_data.DuplexType.select = Single_Sided;
  }else if(strcmp(value, CARRIERSHEET_STR) == 0) {
    s->hw->sv->device_data.FunctionalUnit.select = kSDIFunctionalUnitCarrierSheet;
  }
}

static void
handle_double_feed_detection (Epson_Scanner *s, SANE_String value)
{
  if(strcmp(value, DETECT_DOUBLE_FEED_NONE_STR) == 0){
    s->hw->sv->device_data.DoubleFeedDetectionValue.DoubleFeedDetection.select = false;
  }else if(strcmp(value, DETECT_DOUBLE_FEED_THIN_STR) == 0){
    s->hw->sv->device_data.DoubleFeedDetectionValue.DoubleFeedDetection.select = true;
    s->hw->sv->device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel.select = kSDIDoubleFeedDetectionLow;
  }else if(strcmp(value, DETECT_DOUBLE_FEED_STANDARD_STR) == 0){
    s->hw->sv->device_data.DoubleFeedDetectionValue.DoubleFeedDetection.select = true;
    s->hw->sv->device_data.DoubleFeedDetectionValue.DoubleFeedDetectionLevel.select = kSDIDoubleFeedDetectionHigh;
  }
}

static void
handle_transfer_format (Epson_Scanner *s, SANE_String value)
{
  log_call ("(%s)", value);

  if(strcmp(value, TRANSFER_RAW_STR) == 0){
    s->hw->sv->device_data.TransferCompression.select = false;
  }else if(strcmp(value, TRANSFER_JPEG_STR) == 0){
    s->hw->sv->device_data.TransferCompression.select = true;
  }
}

static void
handle_remove_background (Epson_Scanner *s, SANE_String value)
{
  if(strcmp(value, REMOVE_BACKGROUND_NONE_STR) == 0){
    s->hw->sv->device_data.BackgroundRemoval.select = kSDIBackgroundRemovalNone;
  }else if(strcmp(value, REMOVE_BACKGROUND_STANDARD_STR) == 0){
    s->hw->sv->device_data.BackgroundRemoval.select = kSDIBackgroundRemovalNormal;
  }else if(strcmp(value, REMOVE_BACKGROUND_HIGH_STR) == 0){
    s->hw->sv->device_data.BackgroundRemoval.select = kSDIBackgroundRemovalHigh;
  }
}

static void
handle_background_color (Epson_Scanner *s, SANE_String value)
{
  if(strcmp(value, BACKGROUND_COLOR_BLACK_STR) == 0){
    s->hw->sv->device_data.BackgroundColor.select = kSDIBackgroundColorBlack;
  }else if(strcmp(value, BACKGROUND_COLOR_WHITE_STR) == 0){
    s->hw->sv->device_data.BackgroundColor.select = kSDIBackgroundColorWhite;
  }
}

static void
handle_text_enhance (Epson_Scanner *s, SANE_String value)
{
  if(strcmp(value, TEXT_ENHANCE_NONE_STR) == 0){
    s->hw->sv->device_data.textEnhance.select = TextEnhanceNone;
  }else if(strcmp(value, TEXT_ENHANCE_NORMAL_STR) == 0){
    s->hw->sv->device_data.textEnhance.select = TextEnhanceNormal;
  }else if(strcmp(value, TEXT_ENHANCE_HIGH_STR) == 0){
    s->hw->sv->device_data.textEnhance.select = TextEnhanceHigh;
  }
}

double calculate_mm_size(int base_res, double value)
{
    //pixel -> mm
    return value * 25.4 / base_res;
}


double calculate_pixel_size(int base_res, double value)
{
    //mm -> pixel
    double mpp = 25.4 / base_res;
    return value / mpp;
}

double calculate_pixel_size_inchi(int base_res, double value)
{
    //inchi -> pixel
    return value * base_res;
}

static bool
handle_scan_area (Epson_Scanner *s, SANE_String value)
{
  int base_res = s->hw->sv->device_data.Resolution.select;
  double current_pixel_x = s->hw->sv->device_data.ScanArea.ScanAreaWidth.select  / BASE_RESOLUTION * base_res;
  double current_pixel_y = s->hw->sv->device_data.ScanArea.ScanAreaHeight.select  / BASE_RESOLUTION * base_res;

  s->hw->cur_doc_size = value;
  log_call("doc size : %s", s->hw->cur_doc_size);
  log_call("base res : %d", s->hw->sv->device_data.Resolution.select);
  if(strcmp(value, DOC_AUTO_STR) == 0){
    s->hw->sv->device_data.ScanArea.AutoSize.select = 1;
    //s->hw->sv->device_data.ScanArea.ScanAreaWidth.select = 0;
    //s->hw->sv->device_data.ScanArea.ScanAreaHeight.select = 0;
    return true;
  }else if(strcmp(value, DOC_AUTO_LONG_STR) == 0){
    s->hw->sv->device_data.ScanArea.AutoSize.select = 2;
    //s->hw->sv->device_data.ScanArea.ScanAreaWidth.select = 0;
    //s->hw->sv->device_data.ScanArea.ScanAreaHeight.select = 0;
    return true;
  }else if(strcmp(value, DOC_A3_STR) == 0){
    current_pixel_x = calculate_pixel_size(base_res, 297);
    current_pixel_y = calculate_pixel_size(base_res, 420);
  }else if(strcmp(value, DOC_A4_STR) == 0){
    current_pixel_x = calculate_pixel_size(base_res, 210);
    current_pixel_y = calculate_pixel_size(base_res, 297);
  }else if(strcmp(value, DOC_A4_L_STR) == 0){
    current_pixel_x = calculate_pixel_size(base_res, 297);
    current_pixel_y = calculate_pixel_size(base_res, 210);
  }else if(strcmp(value, DOC_A5_STR) == 0){
    current_pixel_x = calculate_pixel_size(base_res, 148);
    current_pixel_y = calculate_pixel_size(base_res, 210);
  }else if(strcmp(value, DOC_A5_L_STR) == 0){
    current_pixel_x = calculate_pixel_size(base_res, 210);
    current_pixel_y = calculate_pixel_size(base_res, 148);
  }else if(strcmp(value, DOC_A6_STR) == 0){
    current_pixel_x = calculate_pixel_size(base_res, 105);
    current_pixel_y = calculate_pixel_size(base_res, 148);
  }else if(strcmp(value, DOC_A6_L_STR) == 0){
    current_pixel_x = calculate_pixel_size(base_res, 148);
    current_pixel_y = calculate_pixel_size(base_res, 105);
  }else if(strcmp(value, DOC_A8_STR) == 0){
    current_pixel_x = calculate_pixel_size(base_res, 52);
    current_pixel_y = calculate_pixel_size(base_res, 74);
  }else if(strcmp(value, DOC_A8_L_STR) == 0){
    current_pixel_x = calculate_pixel_size(base_res, 74);
    current_pixel_y = calculate_pixel_size(base_res, 52);
  }else if(strcmp(value, DOC_B4_STR) == 0){
    current_pixel_x = calculate_pixel_size(base_res, 257);
    current_pixel_y = calculate_pixel_size(base_res, 364);
  }else if(strcmp(value, DOC_B4_L_STR) == 0){
    current_pixel_x = calculate_pixel_size(base_res, 364);
    current_pixel_y = calculate_pixel_size(base_res, 257);
  }else if(strcmp(value, DOC_B5_STR) == 0){
    current_pixel_x = calculate_pixel_size(base_res, 182);
    current_pixel_y = calculate_pixel_size(base_res, 257);
  }else if(strcmp(value, DOC_B5_L_STR) == 0){
    current_pixel_x = calculate_pixel_size(base_res, 257);
    current_pixel_y = calculate_pixel_size(base_res, 182);
  }else if(strcmp(value, DOC_8K_STR) == 0){
    current_pixel_x = calculate_pixel_size(base_res, 270);
    current_pixel_y = calculate_pixel_size(base_res, 390);
  }else if(strcmp(value, DOC_16K_STR) == 0){
    current_pixel_x = calculate_pixel_size(base_res, 195);
    current_pixel_y = calculate_pixel_size(base_res, 270);
  }else if(strcmp(value, DOC_16K_L_STR) == 0){
    current_pixel_x = calculate_pixel_size(base_res, 270);
    current_pixel_y = calculate_pixel_size(base_res, 195);
  }else if(strcmp(value, DOC_LETTER_STR) == 0){
    current_pixel_x = calculate_pixel_size(base_res, 216);
    current_pixel_y = calculate_pixel_size(base_res, 279);
  }else if(strcmp(value, DOC_LETTER_L_STR) == 0){
    current_pixel_x = calculate_pixel_size(base_res, 279);
    current_pixel_y = calculate_pixel_size(base_res, 216);
  }else if(strcmp(value, DOC_LEGAL_STR) == 0){
    current_pixel_x = calculate_pixel_size(base_res, 216);
    current_pixel_y = calculate_pixel_size(base_res, 356);
  }else if(strcmp(value, DOC_LEGAL_L_STR) == 0){
    current_pixel_x = calculate_pixel_size(base_res, 356);
    current_pixel_y = calculate_pixel_size(base_res, 216);
  }else if(strcmp(value, DOC_POSTCARD_STR) == 0){
    current_pixel_x = calculate_pixel_size(base_res, 100);
    current_pixel_y = calculate_pixel_size(base_res, 148);
  }else if(strcmp(value, DOC_POSTCARD_L_STR) == 0){
    current_pixel_x = calculate_pixel_size(base_res, 148);
    current_pixel_y = calculate_pixel_size(base_res, 100);
  /*}else if(strcmp(value, DOC_PLASTICCARD_STR) == 0){
    current_pixel_x = calculate_pixel_size(base_res, 54);
    current_pixel_y = calculate_pixel_size(base_res, 85.6);*/
  }else if(strcmp(value, DOC_PLASTICCARD_STR) == 0){
    current_pixel_x = calculate_pixel_size(base_res, 85.6);
    current_pixel_y = calculate_pixel_size(base_res, 54);
  }else if(strcmp(value, DOC_TABLOID_STR) == 0){
    current_pixel_x = calculate_pixel_size(base_res, 279);
    current_pixel_y = calculate_pixel_size(base_res, 432);
  }else if(strcmp(value, DOC_TABLOID_L_STR) == 0){
    current_pixel_x = calculate_pixel_size(base_res, 432);
    current_pixel_y = calculate_pixel_size(base_res, 279);
  }else if(strcmp(value, DOC_MAXIMUM_STR) == 0){
    current_pixel_x = (double)s->hw->sv->device_data.ScanArea.MaxScanAreaWidth.select / (double)BASE_RESOLUTION * (double)base_res;
    current_pixel_y = (double)s->hw->sv->device_data.ScanArea.MaxScanAreaHeight.select / (double)BASE_RESOLUTION * (double)base_res;
  }else if(strcmp(value, DOC_MANUAL_STR) == 0){
    s->hw->sv->device_data.ScanArea.AutoSize.select = 0;
    return true;
  }else {
    return false;
  }
  log_call ("x, y :(%f , %f)", current_pixel_x, current_pixel_y);
  s->hw->sv->device_data.ScanArea.AutoSize.select = 0;
  s->hw->sv->device_data.ScanArea.ScanAreaWidth.select = current_pixel_x;
  s->hw->sv->device_data.ScanArea.ScanAreaHeight.select = current_pixel_y;
  return true;
}

static void
handle_skip_blankpages (Epson_Scanner *s, SANE_Word value)
{
  log_call ("skip blankpages threshold : (%d)", value);
  s->hw->sv->device_data.BlankPageSkipLevelKey.select = (SDIInt)value;
  if (value != 0)
  {
    s->hw->sv->device_data.BlankPageSkipKey.select = true;
  }else{
     s->hw->sv->device_data.BlankPageSkipKey.select = false;
  }
}

static void
handle_brightness (Epson_Scanner *s, SANE_Word value)
{
  log_call ("brightness : (%d)", value);
  s->hw->sv->device_data.Brightness.select = (SDIInt)value;
}

static void
handle_contrast (Epson_Scanner *s, SANE_Word value)
{
  log_call ("contrast : (%d)", value);
  s->hw->sv->device_data.Contrast.select = (SDIInt)value;
}

static void
handle_threshold (Epson_Scanner *s, SANE_Word value)
{
  log_call ("threshold : (%d)", value);
  s->hw->sv->device_data.Threshold.select = (SDIInt)value;
}

static void
handle_gamma (Epson_Scanner *s, SANE_Int value)
{
  log_call ("gamma : (%d)", value);
  s->hw->sv->device_data.Gamma.select = value;
}

static void
handle_jpeg_quality (Epson_Scanner *s, SANE_Word value)
{
  log_call ("jpeg quality : (%d)", value);
  s->hw->sv->device_data.ImageFormatSettings.JpegQuality.select = (SDIInt)value;
}

static void
handle_long_paper_mode (Epson_Scanner *s, SANE_Bool value)
{
  log_call ("long_paper_mode : (%d)", value);
  if(value){
     s->hw->sv->device_data.ScanArea.AutoSize.select = 0;
     s->hw->sv->device_data.ScanArea.PaperEndDetection.select = 1;
     s->hw->sv->device_data.ScanArea.FixedSize = (DocumentSize)USER_DEFINE;
  }else {
     s->hw->sv->device_data.ScanArea.PaperEndDetection.select = 0;
  }
}

static void
handle_top_left_x (Epson_Scanner *s, SANE_Int value)
{
  log_call ("top left x : (%d)", value);
  SANE_Word width;
  s->hw->sv->device_data.ScanArea.ScanAreaOffsetX.select = (SDIInt)value;
}

static void
handle_top_left_y (Epson_Scanner *s, SANE_Int value)
{
  log_call ("top left y : (%d)", value);
  s->hw->sv->device_data.ScanArea.ScanAreaOffsetY.select = (SDIInt)value;
}

static void
handle_bottom_right_x (Epson_Scanner *s, SANE_Int value)
{
  log_call ("bottom right x : (%d)", value);
  //fprintf(stderr, "bottom right x : (%d)\n", value);
  s->hw->sv->device_data.ScanArea.ScanAreaWidth.select = (SDIInt)value;
}

static void
handle_bottom_right_y (Epson_Scanner *s, SANE_Int value)
{
  log_call ("bottom right y : (%d)", value);
  s->hw->sv->device_data.ScanArea.ScanAreaHeight.select = (SDIInt)value;
}

static bool
sane_app_compatible (SANE_String value)
{
  pid_t pid = getpid();
  char numStr[999];
  log_call("PID = %d", pid);
  snprintf(numStr, 999, "ps -p %d", pid);
  //system(numStr);

  FILE* gid = popen(numStr, "r");
  if (gid)
  {
    char buf[999];
    while(!feof(gid)){
      if(fgets(buf, 999, gid))
      {
        log_call("buf = %s", buf);
        if(strstr(buf, value)){
          pclose(gid);
      	  return true;
        }
      }
    }
    pclose(gid);
  }
  return false;
}