#ifndef saneopts_h
#define saneopts_h

#define SANE_NAME_SCAN_MODE		"mode"
#define SANE_NAME_SCAN_SOURCE		"source"

#define SANE_NAME_SCAN_TL_X		"tl-x"
#define SANE_NAME_SCAN_TL_Y		"tl-y"
#define SANE_NAME_SCAN_BR_X		"br-x"
#define SANE_NAME_SCAN_BR_Y		"br-y"
#define SANE_NAME_SCAN_RESOLUTION	"resolution"
#define SANE_NAME_SCAN_X_RESOLUTION	"x-resolution"
#define SANE_NAME_SCAN_Y_RESOLUTION	"y-resolution"
#define SANE_NAME_BRIGHTNESS		"brightness"
#define SANE_NAME_CONTRAST		"contrast"
#define SANE_NAME_THRESHOLD		"threshold"

#define SANE_TITLE_NUM_OPTIONS		SANE_I18N("Number of options")

#define SANE_TITLE_SCAN_MODE		SANE_I18N("Scan mode")
#define SANE_TITLE_SCAN_SOURCE		SANE_I18N("Scan source")
#define SANE_TITLE_SCAN_TL_X		SANE_I18N("Top-left x")
#define SANE_TITLE_SCAN_TL_Y		SANE_I18N("Top-left y")
#define SANE_TITLE_SCAN_BR_X		SANE_I18N("Bottom-right x")
#define SANE_TITLE_SCAN_BR_Y		SANE_I18N("Bottom-right y")
#define SANE_TITLE_SCAN_RESOLUTION	SANE_I18N("Scan resolution")
#define SANE_TITLE_SCAN_X_RESOLUTION	SANE_I18N("X-resolution")
#define SANE_TITLE_SCAN_Y_RESOLUTION	SANE_I18N("Y-resolution")
#define SANE_TITLE_BRIGHTNESS		SANE_I18N("Brightness")
#define SANE_TITLE_CONTRAST		SANE_I18N("Contrast")
#define SANE_TITLE_THRESHOLD		SANE_I18N("Threshold")

#define SANE_DESC_NUM_OPTIONS \
SANE_I18N("Read-only option that specifies how many options a specific " \
"devices supports.")

#define SANE_DESC_SCAN_MODE \
SANE_I18N("Selects the scan mode (e.g., lineart, monochrome, or color).")

#define SANE_DESC_SCAN_SOURCE \
SANE_I18N("Selects the scan source (such as a document-feeder).")

#define SANE_DESC_SCAN_TL_X \
SANE_I18N("Top-left x position of scan area.")

#define SANE_DESC_SCAN_TL_Y \
SANE_I18N("Top-left y position of scan area.")

#define SANE_DESC_SCAN_BR_X \
SANE_I18N("Bottom-right x position of scan area.")

#define SANE_DESC_SCAN_BR_Y \
SANE_I18N("Bottom-right y position of scan area.")

#define SANE_DESC_SCAN_RESOLUTION \
SANE_I18N("Sets the resolution of the scanned image.")

#define SANE_DESC_SCAN_X_RESOLUTION \
SANE_I18N("Sets the horizontal resolution of the scanned image.")

#define SANE_DESC_SCAN_Y_RESOLUTION \
SANE_I18N("Sets the vertical resolution of the scanned image.")

#define SANE_DESC_BRIGHTNESS \
SANE_I18N("Controls the brightness of the acquired image.")

#define SANE_DESC_CONTRAST \
SANE_I18N("Controls the contrast of the acquired image.")

#define SANE_DESC_THRESHOLD \
SANE_I18N("Select minimum-brightness to get a white point")

#endif /* saneopts_h */
