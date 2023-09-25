#ifndef CAPITEM_H
#define CAPITEM_H

#include <list>

#define OPTIONS 100
#define USER_DEFINE 101
#define COMBOSEPARATOR 102
#define SETTING 103

typedef enum {LIST_type, RANG_type, BOOL_type}ItemType;

typedef enum {Flatbed = 0, ADF = 1, Carrier_Sheet = 2, Auto = 3, Unit_Settings = 4} FunctionalUnit;
typedef enum {Auto_Detect, Auto_Detect_long, A3,
              A4, A4_Landscape,
              A5, A5_Landscape,
              A6, A6_Landscape,
              A8, A8_Landscape,
              B4_JIS, B4_JIS_Landscape,
              B5_JIS, B5_JIS_Landscape,
              s8K, s16K, s16K_Landscape,
              Letter, Letter_Landscape,
              Legal, Legal_Landscape,
              Postcard, Postcard_Landscape,
              PlasticCard, PlasticCard_Landscape,
              Tabloid, Tabloid_Landscape,
              Maximum_Size, Manual, Marquee,
              A0, A1, A2,
              B1_JIS, B2_JIS, B3_JIS
             } DocumentSize;
typedef enum
{
    AutoSizeNone = 0,
    AutoSizeStandard = 1,
    AutoSizeLongPaper = 2
}AutoSizeType;
typedef enum {Single_Sided = 0, Double_Sided = 1} ScanningSide;
typedef enum {Reflective = 0, Color_Positive_Film = 1, Mono_Negative_Film  = 2, Color_Negative_Film =3 } DocumentType;
typedef enum {ColorTypeAuto = -1,
              ColorTypeRGB24 = 0,
              ColorTypeMono8 = 1,
              ColorTypeMono1 = 2,
              ColorTypeRGB48 = 3,
              ColorTypeMono16 = 4,
              ColorTypeSettings = 5
             } ColorType;

typedef enum {AutoColorPixelTypeMono = 0,
              AutoColorPixelTypeGray = 1,
              AutoColorPixelTypeAuto = 2
             } AutoColorPixelType;

typedef enum {res_75 = 0,
              res_100 = 1,
              res_150 = 2,
              res_200 = 3,
              res_240 = 4,
              res_300 = 5,
              res_400 = 6,
              res_600 = 7,
              res_1200 = 8,
              other = 9
             } ResolutionList;
typedef enum {OrientationNone = 0,
              Orientation90 = 1,
              Orientation180 = 2,
              Orientation270 = 3,
              OrientationAuto = 4
             } Orientation;

typedef enum
{
    DropoutColorNone  = 0,
    DropoutColorRed   = 1,
    DropoutColorGreen = 2,
    DropoutColorBlue  = 3
} DropoutColor;

typedef enum {ImageFormatMultiTIFF,
              ImageFormatJPEG,
              ImageFormatTIFF,
              ImageFormatPDF,
              ImageFormatPNG,
              ImageFormatPNM,
              ImageFormatColorJpegMonoRaw,
              ImageFormatOFD,
              Options
             } ImageFormat;

typedef enum{JpegEncodeStandard,
             JpegEncodeProgressive
            } JpegEncode;

typedef enum{TiffCompressionNone,
             TiffCompressionCCITFAX4
            } TiffCompression;

typedef enum {Document, Picture, Desktop} Folder;

typedef enum {Off, On, Settings} OnOffComboBox;

typedef enum {AFM_Off, AFM_On, AFMC_On} AutomaticFeedingMode;

typedef enum {DoubleFeedDetectionLow = 0,
              DoubleFeedDetectionHigh = 1
             } DoubleFeedDetection;

typedef enum
{
   BackgroundRemovalNone = 0,
   BackgroundRemovalNormal = 1,
   BackgroundRemovalHigh = 2
}BackgroundRemoval;

typedef enum
{
   TextEnhanceNone = 0,
   TextEnhanceNormal = 1,
   TextEnhanceHigh = 2
}TextEnhance;

typedef enum {unit_mm, unit_inchi}Units;

typedef enum {t_Off = 0, t_3 = 3, t_5 = 5, t_10 = 10, t_15 = 15,
              t_30 = 30, t_60 = 60, t_120 = 120, t_240 = 240, t_480 = 480, t_720 = 720}Time;

typedef enum
{
    ImageOptionNone = 0,
    ImageOptionDropout = 1,
    ImageOptionBackgroundRemoval = 2,
    ImageOptionTextEnhance = 3
}ImageOption;

typedef enum
{
    BackgroundColorWhite = 0,
    BackgroundColorBlack = 1,
    BackgroundColorGray = 2,
    BackgroundColorNone = 3
}BackgroundColor;

typedef enum
{
    Timeout_none = 0,
    Timeout_10 = 10,
    Timeout_30 = 30,
    Timeout_60 = 60,
    Timeout_180 = 180,
    Timeout_300 = 300
}AutomaticModeTimeout;

typedef enum
{
    GuidePositionLeft   = 0,
    GuidePositionCenter = 1,
    GuidePositionCenterManual = 2,
    GuidePositionRight  = 3
}GuidePosition;

typedef enum
{
    ADFCardNotSupport = 0,
    ADFCardSupport,
    ADFCardPortrait,
    ADFCardLandscape
}ADFCard;

typedef enum
{
    SensorGlassDirtSensitivityOff		= 0,
    SensorGlassDirtSensitivityLow		= 1,
    SensorGlassDirtSensitivityNormal	= 2,
}SensorGlassDirtSensitivity;

typedef enum
{
    ADFPaperProtection_Off		= 0,
    ADFPaperProtection_Low		= 1,
    ADFPaperProtection_Normal	= 2,
    ADFPaperProtection_High		= 3,
}ADFPaperProtection;

typedef enum
{
    BehaviorWhenDoubleFeed_Immediately = 0,
    BehaviorWhenDoubleFeed_StopAfterEjecting
}BehaviorWhenDoubleFeed;

typedef enum
{
    DirectPowerOn_Off = 0,
    DirectPowerOn_On
}DirectPowerOn;

typedef enum
{
    NonConnectPowerOff_Off = 0,
    NonConnectPowerOff_On
}NonConnectPowerOff;

typedef enum
{
    KindSFDocument = 11,
    KindSFPhoto = 12,
    KindMFBusiness = 21,
    KindMFConsumer = 22,
}ScannerKind;

typedef enum
{
    ADFDuplexType1Pass    = 1,
    ADFDuplexType2Pass    = 2,
}AdfDuplexType;

typedef enum
{
   AdminUnsupported = 0,
   AdminLockOn = 1,
   AdminLockOff = 2
}AdministratorStatus;

typedef enum
{
    TextEnhanceLevelNone	    = 0,
    TextEnhanceLevelStandard    = 1,
    TextEnhanceLevelHigh 	    = 2
}TextEnhanceLevel;


typedef enum
{
    RemovePunchHoleNone 	 =  0,
    RemovePunchHoleAll       =  1,   ///< 上下左右
    RemovePunchHoleTopBottom =  2,   ///< 上下
    RemovePunchHoleLeftRight =  3,   ///< 左右
}RemovePunchhole;



typedef struct {
    ItemType item_type;
    int item_num;
    std::list<float> item_list;
    float select;
    bool enabled;
    bool option;
    double max_size_x;
    double max_size_y;
    double min_size_x;
    double min_size_y;
    int sub_value;

} SCANITEM;

typedef struct {
    Units units;
    bool last_used_settings;
    bool used_default_settings;
    bool compression;
    bool open_img_folder;
    bool quality_preview;
    int preview_resolution;
    int preview_immediately_resolution;// Resolution immediately after preview

    int sleep_timer;
    Time ac_power;
    Time usb_power;

    int roller_kit_counter;
    int roller_counter;
    int retard_roller_counter;
    int paper_protection_counter;

    int cleaning_warning_notify_counter;
    int roller_kit_notify_counter;
    int cleaning_warning_counter;
    bool cleaning_warning_notify;

    int WindowPositionX;
    int WindowPositionY;
    int WindowWidth;
    int WindowHeight;

    SensorGlassDirtSensitivity sensor_glass_dirtsensitivity;
    ADFPaperProtection adf_paper_protection;
    BehaviorWhenDoubleFeed behavior_when_doublefeed;
    DirectPowerOn direct_power_on;
    NonConnectPowerOff non_connect_poweroff;

} CONFIGURATION;


/*typedef struct {

    SCANITEM Document_Source;
    SCANITEM Scanning_Side;
    SCANITEM Document_Size;
    SCANITEM Image_Type;
    SCANITEM Resolution;
    SCANITEM Rotate;
    SCANITEM Deskew;
    SCANITEM Add_Pages;
    SCANITEM Skip_Blankpage;
    SCANITEM Detect_Double_Feed;

    SCANITEM Remove_Background;
    SCANITEM Dropout;
    SCANITEM Brightness;
    SCANITEM Contrast;
    SCANITEM Gamma;
    SCANITEM Threshold;

    SCANITEM Image_Format;
    SCANITEM File_Name;
    SCANITEM Folder;

} SCANPARA;*/

class capitem
{
public:
    capitem();
};

#endif // CAPITEM_H
