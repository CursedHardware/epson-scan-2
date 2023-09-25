#include <stdio.h>
#include <iostream>
#include <boost/any.hpp>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/regex.hpp>
#include <utils/ESBuffer.h>

#include "Emulator.h"


class MyDelegateReciver : public IESScannerDelegate
{
  /** 
	 連続する一連のスキャンの開始を通知する。
	 連続するスキャンは、ADFでの間断ないスキャンを指す。
	*/
	virtual void CALLBACK ScannerWillBeginContinuousScanning(IESScanner* pScanner)
  {

  }

	/**
	 連続する一連のスキャンの終了を通知する。
	 連続するスキャンは、ADFでの間断ないスキャンを指す。
	*/
	virtual void CALLBACK ScannerDidEndContinuousScanning(IESScanner* pScanenr)
  {

  }

	/**
	 1ページ分のスキャン開始を通知する。
	 */
	virtual void CALLBACK ScannerWillScanToScannedImage( IESScanner* pScanner, IESScannedImage* pESImage )
  {

  }

	/**
	 1ページ分のスキャン終了を通知する。
	 */
	virtual void CALLBACK ScannerDidScanToScannedImage( IESScanner* pScanner, IESScannedImage* pESImage )
  {

  }

	/**
	 スキャンのキャンセルが行われることを通知する。
	 */
	virtual void CALLBACK ScannerWillCancelScanning( IESScanner* pScanner )
  {

  }

	/**
	 スキャンのキャンセルを通知する。
	 */
	virtual void CALLBACK ScannerDidCancelScanning( IESScanner* pScanner)
  {

  }

	/**
	 スキャンの完了を通知する。
	 AFM有効時はAFMの終了時のみ発行され、それ以外では発行されない。
	 */
	virtual void CALLBACK ScannerDidCompleteScanningWithError( IESScanner* pScanner, ESErrorCode err)
  {

  }

	/**
	 エラーによるスキャンの中断を通知する。
	 */
	virtual void CALLBACK ScannerDidInterruptScanningWithError(IESScanner* pcScanner ,ESErrorCode err)
  {

  }

	/**
	  通信エラーを通知する。
	 */
	virtual void CALLBACK ScannerDidEncounterDeviceCommunicationError( IESScanner* pScanner, ESErrorCode err )
  {

  }

	/**
	 ウォームアップ開始を通知する。
	 */
	virtual void CALLBACK ScannerWillWarmUp( IESScanner* pScanner )
  {

  }

	/**
	 ウォームアップ終了を通知する。
	 */
	virtual void CALLBACK ScannerDidWarmUp( IESScanner* pScanner )
  {

  }

	virtual void CALLBACK NetworkScannerDidRequestStartScanning( IESScanner* pScanner )
  {

  }

	virtual void CALLBACK NetworkScannerDidRequestStopScanning( IESScanner* pScanner )
  {

  }

	virtual void CALLBACK ScannerDidDisconnect( IESScanner* pScanner )
  {

  }

	virtual void CALLBACK NetworkScannerDidReceiveServerError( IESScanner* pScanner )
  {

  }

	virtual BOOL CALLBACK NetworkScannerShouldPreventTimeout( IESScanner* pScanner )
  {

  }

	virtual void CALLBACK NetworkScannerDidTimeout(  IESScanner* pScanner )
  {

  }

	/**
	 ネットワークスキャナーが別のクライアントによって使用されていることを通知する。
	 */
	virtual void CALLBACK ScannerIsReservedByHost( IESScanner* pScanner, const ES_CHAR* pszAddress )
  {

  }

	/**
	 スキャナーのプッシュスキャンボタンが押されたことを通知する。
	 */
	virtual void CALLBACK ScannerDidPressButton( UInt8 un8ButtonNumber )
  {

  }
	/**
	 スキャナーのストップボタンが押されたことを通知する。
	 */
	virtual void CALLBACK ScannerDidRequestStop(IESScanner *pScanner)
  {

  }
};


int main(int argc, char const *argv[]) {
  /* code */

  ESString str;
  ESDictionary dict;
  ESNumber PreviousPageNum = 0;
  dict["DisplayName"] = (ESString)"DS-7000";
  dict["modelID"] = (ESNumber)0x13C;
  dict["type"] = (ESNumber)1;
  dict["Label"] = (ESString)"USB Device Label";

  ESDictionary dict2;
  dict2["ConnectionSetting"] = dict;

  ES_CMN_FUNCS::JSON::DictionaryToJSON(dict2, str);
  printf("%s", str.c_str());
  
  Emulator *test_emu = new Emulator();

  test_emu->SetConnection(str.c_str());

  test_emu->SetValuesWithJSON(str.c_str());

  MyDelegateReciver *rec = new MyDelegateReciver();
  test_emu->SetDelegate(rec);

  test_emu->Open();

  test_emu->SetLoadingPaperNum(1);

  //test_emu->ScanInBackground();
  test_emu->Scan();

  /*int end = 1;
  while (end)
  {
    sleep(1);
  }*/
  //sleep(10);

  if(test_emu->IsOpened()){
    test_emu->Close();
    test_emu->DestroyInstance();
    test_emu = NULL;
    delete rec;
  }

  return 0;
}
