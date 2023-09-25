
#pragma once


namespace ES_CMN_FUNCS
{
 namespace BUFFER
 {
   enum ENUM_BUFFER_TYPE{
     BUFFER_TYPE_GLOBAL=0,
     BUFFER_TYPE_HEAP,
     BUFFER_TYPE_VIRTUAL
   };

   class IESBuffer{
     public:
       virtual ~IESBuffer() {}

       virtual ENUM_BUFFER_TYPE GetBufferType()                                                 = 0;

       // バッファが確保されているか
       virtual bool IsEmpty()                                                                   = 0;
       // バッファサイズを取得
       virtual UInt32 GetLength() const                                                         = 0;

       // バッファの先頭アドレス取得(変更不可)
       virtual UInt8* GetConstBufferPtr() const                                                 = 0;

       // バッファの先頭アドレス取得
       virtual UInt8* GetBufferPtr()                                                            = 0;

       // バッファを確保する。
       // 以前に確保されたバッファは削除される
       virtual bool AllocBuffer( UInt32 nBufferSize )                                           = 0;

       // バッファを埋める
       virtual bool FillBuffer(UInt8 nValue)                                                    = 0;

       // バッファサイズを変更する
       virtual bool ReAllocBuffer(UInt32 nBufferSize)                                           = 0;

       // バッファを解放する
       virtual void FreeBuffer()                                                                = 0;

       //所有権移動　MemCopyではなく、アドレスコピー　SrcからはDetachされる
       virtual bool Attach( IESBuffer& cSrcBuffer )                                             = 0;
       // バッファをクラスから切り離す
       virtual UInt8* Detach()                                                                  = 0;

       // バッファにコピーする
       virtual bool CopyBuffer( const IESBuffer& cInBuffer )                                    = 0;
       virtual bool CopyBuffer( const UInt8* pBuffer, UInt32 nBufferSize, UInt32 nAllocSize = 0)= 0;
       virtual bool CopyBuffer(const UInt32 un32Offset,const UInt8* ,UInt32 nCopySize)			 =0;

       // 内部バッファの後ろに、バッファを追加する。Streamモードでは、有効データの後ろに追加される。
       virtual bool AppendBuffer( const UInt8* pBuffer, UInt32 nBufferSize )                    = 0;

       //Streamとして、Appendするための、バッファを事前確保する。
       //初期サイズをnBufferLenに指定、足りない場合は、nExpandLenずつ拡張される
       //Append完了後、必ずReleaseStreamBufferをCallすること
       virtual bool GetStreamBuffer(UInt32 nBufferLen, UInt32 nExpandLen)						 =	0;

       //Streamに確保されているバッファ長を、実データ長に調整する。
       virtual bool ReleaseStreamBuffer()														 =	0;

       //Stream mode で書き込み中の、有効データ長を取得する
       virtual UInt32 GetValidDataLen()														 = 0;
    };
 }
}
