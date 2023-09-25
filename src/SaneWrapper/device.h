#ifndef sane_device_hpp_
#define sane_device_hpp_

extern "C" {                    // needed until sane-backends-1.0.14
#include "epsonsane.h"
}

#include <string>
#include <vector>

#include "supervisor.h"

//document source
#define  AUTO_STR	SANE_I18N("Auto")
#define  FB_STR	SANE_I18N("Flatbed")
#define  ADF_STR	SANE_I18N("ADF")
#define  ADF_FRONT_STR SANE_I18N("ADF Front")
#define  CARRIERSHEET_STR	SANE_I18N("Carrier Sheet")
//scanning side
#define  SINGLE_SIDED_STR	SANE_I18N("Single-Sided")
#define  DOUBLE_SIDED_STR	SANE_I18N("Double-Sided")
//Document size
#define  DOC_AUTO_STR	SANE_I18N("Auto Detect")
#define  DOC_AUTO_LONG_STR	SANE_I18N("Auto Detect(long paper)")
#define  DOC_LETTER_STR	SANE_I18N("Letter")
#define  DOC_LETTER_L_STR	SANE_I18N("Letter (Landscape)")
#define  DOC_LEGAL_STR	SANE_I18N("Legal")
#define  DOC_LEGAL_L_STR	SANE_I18N("Legal (Landscape)")
#define  DOC_A3_STR	SANE_I18N("A3")
#define  DOC_A4_STR	SANE_I18N("A4")
#define  DOC_A4_L_STR	SANE_I18N("A4 (Landscape)")
#define  DOC_A5_STR	SANE_I18N("A5")
#define  DOC_A5_L_STR	SANE_I18N("A5 (Landscape)")
#define  DOC_A6_STR	SANE_I18N("A6")
#define  DOC_A6_L_STR	SANE_I18N("A6 (Landscape)")
#define  DOC_A8_STR	SANE_I18N("A8")
#define  DOC_A8_L_STR	SANE_I18N("A8 (Landscape)")
#define  DOC_B4_STR	SANE_I18N("B4 [JIS]")
#define  DOC_B4_L_STR	SANE_I18N("B4 (Landscape)[JIS]")
#define  DOC_B5_STR	SANE_I18N("B5 [JIS]")
#define  DOC_B5_L_STR	SANE_I18N("B5 (Landscape)[JIS]")
#define  DOC_8K_STR	SANE_I18N("8K")
#define  DOC_16K_STR	SANE_I18N("16K")
#define  DOC_16K_L_STR	SANE_I18N("16K (Landscape)")
#define  DOC_TABLOID_STR	SANE_I18N("Tabloid")
#define  DOC_TABLOID_L_STR	SANE_I18N("Tabloid (Landscape)")
#define  DOC_POSTCARD_STR	SANE_I18N("Postcard")
#define  DOC_POSTCARD_L_STR	SANE_I18N("Postcard (Landscape)")
#define  DOC_PLASTICCARD_STR	SANE_I18N("PlasticCard")
#define  DOC_PLASTICCARD_L_STR	SANE_I18N("PlasticCard (Landscape)")
#define  DOC_MAXIMUM_STR	SANE_I18N("Maximum")
#define  DOC_MANUAL_STR	SANE_I18N("Manual")
//Image type
#define  COLOR_STR	SANE_I18N("Color")
#define  GRAYSCALE_STR	SANE_I18N("Grayscale")
#define  MONOCHROME_STR	SANE_I18N("Monochrome")
//Rotate
#define  ROTATE_0_STR	SANE_I18N("0 degrees")
#define  ROTATE_90_STR	SANE_I18N("90 degrees")
#define  ROTATE_180_STR	SANE_I18N("180 degrees")
#define  ROTATE_270_STR	SANE_I18N("270 degrees")
#define  ROTATE_AUTO_STR	SANE_I18N("Auto")
//Image format
#define  FORMAT_JPEG_STR	SANE_I18N("JPEG")
#define  FORMAT_TIFF_STR	SANE_I18N("TIFF")
#define  FORMAT_MULTI_IFF_STR	SANE_I18N("Multi-TIFF")
#define  FORMAT_PDF_STR	SANE_I18N("PDF")
#define  FORMAT_PNG_STR	SANE_I18N("PNG")
#define  FORMAT_PNM_STR	SANE_I18N("PNM")
//Remove background
#define  REMOVE_BACKGROUND_NONE_STR	SANE_I18N("None")
#define  REMOVE_BACKGROUND_STANDARD_STR	SANE_I18N("Standard")
#define  REMOVE_BACKGROUND_HIGH_STR	SANE_I18N("High")
//Dropout
#define  DROPOUT_NONE_STR	SANE_I18N("None")
#define  DROPOUT_RED_STR	SANE_I18N("Red")
#define  DROPOUT_BLUE_STR	SANE_I18N("Blue")
#define  DROPOUT_GREEN_STR	SANE_I18N("Green")
//Transfer format
#define  TRANSFER_RAW_STR	SANE_I18N("RAW")
#define  TRANSFER_JPEG_STR	SANE_I18N("JPEG")
//Detect double feed
#define  DETECT_DOUBLE_FEED_NONE_STR	    SANE_I18N("None")
#define  DETECT_DOUBLE_FEED_THIN_STR    	SANE_I18N("Thin")
#define  DETECT_DOUBLE_FEED_STANDARD_STR	SANE_I18N("Standard")
#define  DETECT_DOUBLE_FEED_IMAGE_STR	    SANE_I18N("Image")
//Remove background
#define  REMOVE_BACKGROUND_NONE_STR	      SANE_I18N("None")
#define  REMOVE_BACKGROUND_STANDARD_STR   SANE_I18N("Standard")
#define  REMOVE_BACKGROUND_HIGH_STR	      SANE_I18N("High")

//Background Color
#define  BACKGROUND_COLOR_BLACK_STR	      SANE_I18N("Black")
#define  BACKGROUND_COLOR_WHITE_STR       SANE_I18N("White")

//Text Enhance
#define  TEXT_ENHANCE_NONE_STR	      SANE_I18N("None")
#define  TEXT_ENHANCE_NORMAL_STR      SANE_I18N("Normal")
#define  TEXT_ENHANCE_HIGH_STR	      SANE_I18N("High")

typedef struct
{
  SANE_Int   last;
  SANE_Int   size;
  SANE_Word *list;
  SANE_Bool  deep;
} resolution_info;

class device : public SANE_Device
{
  std::string name_;
  std::string vendor_;
  std::string model_;
  std::string type_;

  //! Implements the common part of object creation and copying
  void init ();

public:
  //! Creates a copy of a device object
  device (const device& dev);
  //! Assigns one device to another
  device& operator= (const device& dev);

  //! Releases all resources associated with a \c SANE_Device list
  static void release ();

  //! Holds on to an array of \c SANE_Device pointers
  /*! The backend "remembers" the last \c device::list it has returned
   *  through sane_get_devices() via this static member variable.  The
   *  backend owns the resources associated with that list and handles
   *  the release() of these resources when appropriate.
   */
  static const SANE_Device **list;

  //! Holds on to the resources associated with a \c SANE_Device list
  /*! This static member variable holds on to the objects pointed to
   *  by the elements of list.
   */
  static std::vector< device > *pool;

  static SANE_Status create_epson_device(void);

  resolution_info resolution;   /* full-blown list */
  resolution_info res;          /* advertised list */

  SANE_Range dpi_range;

  SANE_String_Const scan_mode[4];
  SANE_String_Const doc_sources[5];
  SANE_String_Const scanning_side[3];
  SANE_String_Const document_size[25];
  SANE_String_Const image_type[3];
  SANE_String_Const rotate[6];
  SANE_String_Const image_format[6];
  SANE_String_Const dropout_color[5];
  SANE_String_Const transfer_format[3];
  SANE_String_Const detect_double_feed[5];
  SANE_String_Const remove_background[4];
  SANE_String_Const background_color[3];
  SANE_String_Const text_enhance[4];

  SANE_Range x_range;
  SANE_Range y_range;
  
  Supervisor* sv;
  SDIDeviceInfo devInfo;

  SANE_String_Const cur_doc_size;

  long ChannelPerPixel;
  /*
  typedef struct {
	SDIInt version;			     	  	   // out  version of this struct definition
	char modelID[MAX_MODEL_ID];   	   // out ES00xx..
	SDIInt  productID;   				   // out usb pid
	char ipAddress[MAX_IP_ADDR];  	   // out 192..
	char displayName[MAX_DISPLAYNAME]; // displayname
  }SDIDeviceInfo*/

};

#endif  /* sane_device_hpp_ */
