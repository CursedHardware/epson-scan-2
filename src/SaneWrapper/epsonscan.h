#ifndef epsonscan_h
#define epsonscan_h

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "device.h"
#include "defines.h"

#include "message.h"
#include "defaultsettings.h"
#include "../Include/Config.h"

typedef struct
{
  SANE_Byte *lut;
  int depth;

} LUT;


/* convenience union to access option values given to the backend
 */
typedef union
{
  SANE_Bool b;
  SANE_Word w;
  SANE_Word *wa;		/* word array */
  SANE_String s;
}
Option_Value;

/* string constants for GUI elements that are not defined SANE-wide */

#define SANE_NAME_GAMMA_CORRECTION "gamma-correction"
#define SANE_TITLE_GAMMA_CORRECTION SANE_I18N("Gamma Correction")
#define SANE_DESC_GAMMA_CORRECTION SANE_I18N("Selects the gamma correction value from a list of pre-defined devices or the user defined table, which can be downloaded to the scanner")

#define SANE_EPSON_FOCUS_NAME "focus-position"
#define SANE_EPSON_FOCUS_TITLE SANE_I18N("Focus Position")
#define SANE_EPSON_FOCUS_DESC SANE_I18N("Sets the focus position to either the glass or 2.5mm above the glass")
#define SANE_EPSON_WAIT_FOR_BUTTON_NAME "wait-for-button"
#define SANE_EPSON_WAIT_FOR_BUTTON_TITLE SANE_I18N("Wait for Button")
#define SANE_EPSON_WAIT_FOR_BUTTON_DESC SANE_I18N("After sending the scan command, wait until the button on the scanner is pressed to actually start the scan process.");
#define SANE_EPSON_MONITOR_BUTTON_NAME "monitor-button"
#define SANE_EPSON_MONITOR_BUTTON_TITLE SANE_I18N("Monitor Button")
#define SANE_EPSON_MONITOR_BUTTON_DESC SANE_I18N("Indicates whether a button on the scanner has been pressed.");
#define SANE_EPSON_DETECT_DOC_SIZE_NAME "detect-doc-size"
#define SANE_EPSON_DETECT_DOC_SIZE_TITLE SANE_I18N("Auto-detect document size")
#define SANE_EPSON_DETECT_DOC_SIZE_DESC SANE_I18N("Activates document size auto-detection.  The scan area will be set to match the detected document size.")
#define SANE_EPSON_SCAN_AREA_IS_VALID_NAME "scan-area-is-valid"
#define SANE_EPSON_SCAN_AREA_IS_VALID_TITLE SANE_I18N("Scan Area Is Valid")
#define SANE_EPSON_SCAN_AREA_IS_VALID_DESC SANE_I18N("Indicates whether the current scan area settings are valid.")

#define SANE_EPSON_ADF_AUTO_SCAN_NAME  "adf-auto-scan"
#define SANE_EPSON_ADF_AUTO_SCAN_TITLE SANE_I18N("ADF Auto Scan")
#define SANE_EPSON_ADF_AUTO_SCAN_DESC  SANE_I18N("Skips per sheet device setup for faster throughput.")

#define SANE_EPSON_ADF_DFD_SENSITIVITY_NAME  "double-feed-detection-sensitivity"
#define SANE_EPSON_ADF_DFD_SENSITIVITY_TITLE SANE_I18N("Double Feed Detection Sensitivity")
#define SANE_EPSON_ADF_DFD_SENSITIVITY_DESC  SANE_I18N("Sets the sensitivity with which multi-sheet page feeds are detected and reported as errors.")

#define SANE_EPSON_ADF_DUPLEX_DIRECTION_MATCHES_NAME "adf-duplex-direction-matches"
#define SANE_EPSON_ADF_DUPLEX_DIRECTION_MATCHES_TITLE SANE_I18N("ADF Duplex Direction Matches")
#define SANE_EPSON_ADF_DUPLEX_DIRECTION_MATCHES_DESC SANE_I18N("Indicates whether the device's ADF duplex mode, if available, scans in the same direction for the front and back.")

#define SANE_EPSON_POLLING_TIME_NAME "polling-time"
#define SANE_EPSON_POLLING_TIME_TITLE SANE_I18N("Polling Time")
#define SANE_EPSON_POLLING_TIME_DESC SANE_I18N("Time between queries when waiting for device state changes.")

#define SANE_EPSON_NEEDS_POLLING_NAME "needs-polling"
#define SANE_EPSON_NEEDS_POLLING_TITLE SANE_I18N("Needs Polling")
#define SANE_EPSON_NEEDS_POLLING_DESC SANE_I18N("Indicates whether the scanner needs to poll.")

#define SANE_EPSON_CALIBRATE_NAME "calibrate"
#define SANE_EPSON_CALIBRATE_TITLE SANE_I18N("Calibrate")
#define SANE_EPSON_CALIBRATE_DESC SANE_I18N("Performs color matching to make sure that the document's color tones are scanned correctly.")

#define SANE_EPSON_CLEAN_NAME "clean"
#define SANE_EPSON_CLEAN_TITLE SANE_I18N("Clean")
#define SANE_EPSON_CLEAN_DESC SANE_I18N("Cleans the scanners reading section.")

#define LINES_SHUFFLE_MAX	(17)	/* 2 x 8 lines plus 1 */

#define SANE_EPSON_MAX_RETRIES	(120)	/* how often do we retry during warmup ? */

#define MAX_READ_ATTEMPTS       10      /* maximum number of attempts at
                                           reading scan data */
#define DEFAULT_POLLING_TIME	(1000 * 1000)	/* usec */

enum {
  OPT_NUM_OPTS = 0,
  /* General group */
  OPT_DEVICE_GENERAL_GROUP,
  OPT_MODE,
  OPT_SOURCE,
  OPT_DUPLEX,
  OPT_RESOLUTION,
  OPT_X_RESOLUTION,
  OPT_Y_RESOLUTION,
  OPT_SCAN_AREA,
  /* Geometry group */
  OPT_GEOMETRY_GROUP,
  OPT_TL_X,
  OPT_TL_Y,
  OPT_BR_X,
  OPT_BR_Y,
  /* Enhancement group */
  OPT_ENHANCEMENT_GROUP,
  OPT_DESKEW,
  OPT_DROPOUT,
  OPT_TEXTENHANCE,
  OPT_ROTATE,
  OPT_SKIP_BLANKPAGES,
  OPT_BRIGHTNESS,
  OPT_CONTRAST,
  OPT_THRESHOLD,
  /* Other group */
  OPT_OTHER_GROUP,
  OPT_DOUBLE_FEED_DETECTION,
  OPT_GAMMA,
  OPT_IMAGE_COUNT,
  OPT_LONG_PAPER_MODE,
  OPT_TRANSFER_FORMAT,
  OPT_REMOVE_BACKGROUND,
  OPT_BACKGROUND_COLOR,
  OPT_REMOVE_PUNCHHOLE,
  OPT_DEFICIENCY_CORRECTION,
  //  OPT_TRANSFER_SIZE,
  /*  */
  NUM_OPTIONS
};

/*! Software representation of the \e logical device.
 */
struct Epson_Scanner
{
  struct Epson_Scanner *next;

  bool cancel_requested;
  bool scan_complete;

  bool scan_ready;

  const void *dip;
  device *hw;
  SANE_Option_Descriptor opt[NUM_OPTIONS];
  Option_Value val[NUM_OPTIONS];
  Option_Value val_bak[NUM_OPTIONS];

  SANE_Byte *line_buffer[LINES_SHUFFLE_MAX];
  size_t     cap_line_buffer;

  SANE_Int color_shuffle_line;	/* current line number for color shuffling */
  SANE_Int line_distance;	/* current line distance */
  SANE_Int current_output_line;	/* line counter when color shuffling */

  SANE_Bool invert_image;
  SANE_Word gamma_table[3][256];
  double    cct[9];
  LUT      *lut;
  double    brightness;
  double    contrast;

  /*! Number of frames acquired so far.
   *  This includes partial frames if scan_finish() is called before a
   *  frame is completed.
   */
  unsigned int image_count;

  /*! Number of raw scan lines data gotten from scanner
   *  This corresponds to the \c ESC_d parameter.
   */
  unsigned int line_count;

  SDIImage* outImageData;
  SANE_Byte *out_buffer;

  SANE_Int total_datasize;
  SANE_Int cur_pos;
  int pixels_per_line2;
  int skip;
  int bytes_per_line2;
  
};

typedef struct Epson_Scanner Epson_Scanner;

SANE_Status estimate_parameters (Epson_Scanner *, SANE_Parameters *);
SANE_Status fetch_image_data (Epson_Scanner *, SANE_Byte *, SANE_Int,
                              SANE_Int *);

void select_current_doc_size (Epson_Scanner * s, SCANPARA device_data);

#endif /* not epsonscan_h */
