//
//  FKWriterKeys.h
//  FileFormatKit
//
//  Created by Nakazawa Tuichi on 2014/12/18.
//  Copyright (c) 2014年 epson. All rights reserved.
//
#pragma once

/**
  FKWriterのパラメータ定義の集合
*/

typedef ESDictionary FKWriterInitDict;

typedef ESDictionary FKWriterInitializeDestDict;
typedef ESDictionary FKWriterPageOpenDict;
typedef ESDictionary FKWriterPageCloseDict;
typedef ESDictionary FKWriterFinalizeDestDict;
/**
 FKWriterのインスタンス化オプション FKWriterInitDict
*/

/**
  PDFLibのパス
  .../../xxx.bundle
*/
STATIC_CONST_KEY kFKWriterInitOptionPDFLibPathKey                = ES_STRING("pdfLibPath");         //NSString

/**
  PDF暗号化モジュールのパス
 .../../xxx.bundle

*/
STATIC_CONST_KEY kFKWriterInitOptionPDFEncryptionModulePathKey   = ES_STRING("pdfEncryptionPath");  //NSString

/**
  OCRコンポーネントのパス
  .../../xxx.bundle
*/
STATIC_CONST_KEY kFKWriterInitOptionOCRComponentModulePathKey    = ES_STRING("pdfOCRComponentPath"); //NSString

//#pragma mark - Jpeg

/**
  JpegにおけるFKWriterのページオープンオプション FKWriterPageOpenDict
*/

/**
  Jpeg圧縮率
 - Value Type:      uint32_t
 - Allowed Value:   0 - 100
*/
STATIC_CONST_KEY kFKWriterPageOpenOptionJpegQualityKey           = ES_STRING("quality");         //NSNumber

/**
Jpegのエンコーディング形式
- Value Type:      BOOL
- Allowed Value:   YES, NO

 YES: プログレッシブ形式
 NO : 通常形式
*/
STATIC_CONST_KEY kFKWriterPageOpenOptionJpegProgressiveModeKey   = ES_STRING("progressiveMode"); //NSNumber

/**
 ICCプロファイルのパス
 .../../xxx.icc
*/
STATIC_CONST_KEY kFKWriterPageOpenOptionJpegICCProfilePathKey    = ES_STRING("iccProfilePath");  //NSString

//#pragma mark - PDF

/*
 PDFにおけるFKWriterのページオープンオプション FKWriterPageOpenDict
*/

/**
  PDFの上、左のマージン量 (inch)
 - Value Type:      float
 - Allowed Value:   0.0-
*/
STATIC_CONST_KEY kFKWriterPageOpenOptionPDFTopMarginKey          = ES_STRING("topMarginInch");   // NSNumber
STATIC_CONST_KEY kFKWriterPageOpenOptionPDFLeftMarginKey         = ES_STRING("leftMarginInch");  // NSNumber

/**
 PDFの出力サイズ(pixel)
 - Value Type:      uint32_t
 - Allowed Value:   0-
*/
STATIC_CONST_KEY kFKWriterPageOpenOptionPDFPaperWidthKey         = ES_STRING("paperWidthInch");  // NSNumber
STATIC_CONST_KEY kFKWriterPageOpenOptionPDFPaperHeightKey        = ES_STRING("paperHeightInch"); // NSNumber

/**
 PDFの回転角度
 - Value Type:      FKWriterPageOpenOptionPDFRotation
 - Allowed Value:   kFKWriterPageOpenOptionPDFRotation0
*/
STATIC_CONST_KEY kFKWriterPageOpenOptionPDFRotationKey           = ES_STRING("rotation"); // NSNumber
typedef  enum :uint32_t
{
    kFKWriterPageOpenOptionPDFRotation0    = 0,
    kFKWriterPageOpenOptionPDFRotation90   = 90,
    kFKWriterPageOpenOptionPDFRotation180  = 180,
    kFKWriterPageOpenOptionPDFRotation270  = 270,
}FKWriterPageOpenOptionPDFRotation;


/**
 PDFにおけるFKWriterの書き込み対象初期化オプション FKWriterInitializeDestDict
*/

/**
 PDFAを使用するか？
 - Value Type:      BOOL
 - Allowed Value:   YES, NO
*/
STATIC_CONST_KEY kFKWriterInitializeDestOptionPDFA               = ES_STRING("isPDFA");// BOOL (NSNumber)

/**
  ユーザーパスワード
 - Value Type:      NSString
*/
STATIC_CONST_KEY kFKWriterInitializeDestOptionUserPasseword      = ES_STRING("userPassword"); //NSString

/**
 オーナーパスワード
 - Value Type:      NSString
*/
STATIC_CONST_KEY kFKWriterInitializeDestOptionOwnerPasseword     = ES_STRING("ownerPassword");//NSString

/**
 編集を許可する
 - Value Type:      BOOL
 - Allowed Value:   YES, NO
*/
STATIC_CONST_KEY kFKWriterInitializeDestOptionAllowsEditing      = ES_STRING("allowsEditing");//BOOL (NSNumber)

/**
  印刷を許可する
 - Value Type:      BOOL
 - Allowed Value:   YES, NO
*/
STATIC_CONST_KEY kFKWriterInitializeDestOptionAllowsPrinting     = ES_STRING("allowsPrinting");//BOOL (NSNumber)

/**
  PDFの圧縮方法を指定する
 - Value Type:      FKWriterInitializeDestOptionCompression
 - Allowed Value:   FKWriterInitializeDestOptionCompression
*/
STATIC_CONST_KEY kFKWriterInitializeDestOptionCompression        = ES_STRING("compression");//BOOL (NSNumber)
typedef enum:uint8_t
{
   kFKWriterInitializeDestOptionCompressionStd, // モノクロG４　カラーJPEG
   kFKWriterInitializeDestOptionCompressionHigh,// 高圧縮PDFモジュールを使用する
}FKWriterInitializeDestOptionCompression;

/**
 検索可能PDFを作成する
 - Value Type:      BOOL
 - Allowed Value:   YES, NO
*/
STATIC_CONST_KEY kFKWriterInitializeDestOptionSearchablePDFEnabled = ES_STRING("searchablePDFEnabled");

/**
 検索可能PDFの対象言語
 - Value Type    : ESOcrLang
 - Allowed Value : ESOcrLang
*/
STATIC_CONST_KEY kFKWriterInitializeDestOptionSearchablePDFLang    = ES_STRING("searchablePDFLang");

/**
 PDFA ICCプロファイルパス
 @caution PDFAでない場合には設定値が無視される。
 - Valut Type: NSString
 */
STATIC_CONST_KEY kFKWriterInitializeDestOptionPDFICCProfilePathKey        = ES_STRING("iccProfilePath"); // NSNumber

/**
 PDFA ICCプロファイル チャンネル数
 @caution PDFAでない場合には設定値が無視される。
 - Valut Type: NSString
 */
STATIC_CONST_KEY kFKWriterInitializeDestOptionPDFICCProfileChannelsKey   = ES_STRING("iccProfileChannels"); // NSNumber

//#pragma mark - TIFF

/**
 TIFFにおけるFKWriterの書き込み対象初期化オプション FKWriterInitializeDestDict
 - Value Type:      FKWriterInitializeDestOptionTiffDataType
*/
STATIC_CONST_KEY kFKWriterInitializeDestOptionTiffDataTypeKey        = ES_STRING("dataType");//(NSNumber)
typedef  enum : uint8_t
{
    kFKWriterInitializeDestOptionTiffDataTypeRaw,     /* RAWデータを入力する */
    kFKWriterInitializeDestOptionTiffDataTypeEncoded, /* JPEGデータ等を入力する */
}FKWriterInitializeDestOptionTiffDataType;

/**
 TIFFにおけるFKWriterのページオープンオプション FKWriterPageOpenDict
*/

/**
  TIFF画像を接続するかどうか？
 - Value Type:      BOOL
 - Allowed Value:   YES, NO
*/
STATIC_CONST_KEY kFKWriterPageOpenOptionTiffAppendModeKey        = ES_STRING("appendMode");// BOOL (NSNumber)

/**
  ICCプロファイルのパス
 - Value Type:      NSString
*/
STATIC_CONST_KEY kFKWriterPageOpenOptionTiffICCProfilePathKey    = ES_STRING("iccProfilePath");//NSString

/**
 TIFF圧縮方法の指定
 - Value Type:      FKWriterPageOpenOptionTiffCompression
*/
STATIC_CONST_KEY kFKWriterPageOpenOptionTiffCompressionKey       = ES_STRING("compression");//NSNumber
typedef  enum : uint8_t
{
    kFKWriterPageOpenOptionTiffCompressionNone,
    kFKWriterPageOpenOptionTiffCompressionCCITG4,
    kFKWriterPageOpenOptionTiffCompressionLzw,
    kFKWriterPageOpenOptionTiffCompressionJpeg
}FKWriterPageOpenOptionTiffCompression;



/**
  BmpにおけるFKWriterのページオープンオプション FKWriterPageOpenDict
*/

/**
  BMPがトップダウンかどうか（デフォルトはボトムアップ）
 - Value Type:      bool
 - Allowed Value:   true/false
*/
static  LPCTSTR kFKWriterPageOpenIsBmpTopDown			           = ES_STRING("isBmpTopDown");         //bool
