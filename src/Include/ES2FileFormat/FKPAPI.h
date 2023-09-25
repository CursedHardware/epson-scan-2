// 以下の ifdef ブロックは DLL からのエクスポートを容易にするマクロを作成するための
// 一般的な方法です。この DLL 内のすべてのファイルは、コマンド ラインで定義された FILEFORMATKIT_EXPORTS
// シンボルを使用してコンパイルされます。このシンボルは、この DLL を使用するプロジェクトでは定義できません。
// ソースファイルがこのファイルを含んでいる他のプロジェクトは、
// FILEFORMATKIT_EXPORTS 関数を DLL からインポートされたと見なすのに対し、この DLL は、このマクロで定義された
// シンボルをエクスポートされたと見なします。
#ifdef WIN32
#ifdef FILEFORMATKIT_EXPORTS
#define FILEFORMATKIT_API __declspec(dllexport)
#else
#define FILEFORMATKIT_API __declspec(dllimport)
#endif //FILEFORMATKIT_EXPORTS
#else
#define FILEFORMATKIT_API
#endif //WIN32
#include  "../Include/ES2FileFormat/FileFormatKit.h"
#include  "../CommonUtility/utils/ESBuffer.h"

typedef IFKSource* (*PFN_INIT_FKSOURCE_INSTANCE  )(FKSourceType eSourceType, ES_CMN_FUNCS::BUFFER::IESBuffer& pcSource, BOOL bIsFull);
typedef IFKDestination* (*PFN_INIT_FKDESTINATION_INSTANCE)(FKDestinationType eType, ES_CMN_FUNCS::BUFFER::IESBuffer& pcSource);
typedef IFKReader* (*PFN_INIT_FKREADER_INSTANCE  )(FKReaderType eReaderType);
typedef IFKWriter* (*PFN_INIT_FKWRITER_INSTANCE  )(FKWriterType eWriterType,  ES_CHAR_CPTR pszOptionAsJson);
typedef BOOL (*PFN_DECODE_JPEG_WITH_RESIZE  )(const ES_CMN_FUNCS::BUFFER::IESBuffer& cSrcJpegData, const ES_CMN_FUNCS::BUFFER::IESBuffer& cOutResizedRawData, const ST_ES_SIZE_UN32& stRequestSize, const IESResultString& strOutImageInfoAsJson);

#ifdef WIN32
#define	FILE_FORMAT_KIT_MODULE_NAME	ES_STRING("ES2FileFormat.dll")
#else
#define	FILE_FORMAT_KIT_MODULE_NAME	ES_STRING("libes2fileformat.so")
#endif

extern "C" {


//	FILEFORMATKIT_EXPORTS

	FILEFORMATKIT_API IFKSource* CreateAndInitFKSourceInstance(FKSourceType eSourceType, ES_CMN_FUNCS::BUFFER::IESBuffer& pcSource, BOOL bIsFull);
	FILEFORMATKIT_API IFKDestination* CreateAndInitFKDestinationInstance(FKDestinationType eType, ES_CMN_FUNCS::BUFFER::IESBuffer& pcSource);
	FILEFORMATKIT_API IFKReader* CreateAndInitFKReaderInstance(FKReaderType eReaderType);
	FILEFORMATKIT_API IFKWriter* CreateAndInitFKWriterInstance(FKWriterType eWriterType,  ES_CHAR_CPTR pszOptionAsJson);
	FILEFORMATKIT_API BOOL DecodeJpegWithResize(const ES_CMN_FUNCS::BUFFER::IESBuffer& cSrcJpegData,ES_CMN_FUNCS::BUFFER::IESBuffer& cOutResizedRawData,ST_ES_SIZE_UN32& stRequestSize,IESResultString& strOutImageInfoAsJson);
}
