#pragma once

#ifndef WIN32
#include <iostream>
#include <fstream>
#endif

#include "../ESCommonTypedef.h"
#include "ESBuffer.h"

class CESFile
{
public:
typedef enum _ES_OPEN_MODES {
	ES_OPEN_MODE_READ,				//読み込みモード
	ES_OPEN_MODE_WRITE,				//書き込みモード
	ES_OPEN_MODE_APPEND,			//追記モード
	ES_OPEN_MODE_READ_PLUS,			//読み書きモード(既存ファイルがあれば開く)
	ES_OPEN_MODE_WRITE_PLUS			//読み書きモード(常に作成)
} ES_OPEN_MODE;

private:
#ifdef WIN32
	HANDLE m_hFileHandle;
#else
	std::fstream* m_hFileHandle;
	ES_OPEN_MODE m_reOpenMode;
#endif
	ESString m_strFilePath;

public:
	CESFile(void);
	virtual ~CESFile(void);

	static CESFile* CreateFileInstanceWithPath(ESString strPath,ES_OPEN_MODE un32Mode = ES_OPEN_MODE_READ);
	static CESFile* CreateTempFileInstanceWithPrefix(ESString strFolder, ESString strPrefix, ES_OPEN_MODE un32Mode = ES_OPEN_MODE_READ);

	ESString GetFileName();
	BOOL IsOpen();
	HANDLE GetFileHandle(){
		return m_hFileHandle;
	}

	BOOL Open(ESString strPath,ES_OPEN_MODE eOpenMode = ES_OPEN_MODE_READ);
	BOOL CreateTempFile(ESString strFolder, ESString strPrefix, ES_OPEN_MODE eOpenMode = ES_OPEN_MODE_WRITE_PLUS);

	BOOL WriteData(ES_CMN_FUNCS::BUFFER::IESBuffer& cBuffer);
	BOOL WriteData(UInt32 un32Length,LPVOID pData);
	void Flush(void);

	//ファイルポインタから、最後まで読み出す。（ファイルポインタは移動しない）
	void ReadAvailableData(ES_CMN_FUNCS::BUFFER::IESBuffer& cOutBuf);

	//ファイルポインタから、最後まで読み出す。（ファイルポインタは末尾へ移動する）
	void ReadDataToEndOfFile(ES_CMN_FUNCS::BUFFER::IESBuffer& cOutBuf);

	/*!@brief	指定されたバイト数を読み出す。
	@retval		成功：0以上のバイト数
	@retval		失敗：0
	*/
	int ReadDataOfLength(UInt32 un32Length,ES_CMN_FUNCS::BUFFER::IESBuffer& cOutBuf);
	int ReadDataOfLength(UInt32 un32Length,LPVOID pBuf);

	BOOL SeekToHead();
	BOOL SeekToFileOffset(UInt32 un32Offset);
	BOOL SeekToEndOfFile();

	UInt32 GetLength();

	BOOL CloseFile();
};
