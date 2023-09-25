#pragma once

#include "../ESCommonTypedef.h"
#include "../../Include/IESBuffer.h"

namespace ES_CMN_FUNCS
{
	namespace BUFFER
	{
		// Heepメモリ確保
		UInt8* AllocHeapBuffer( UInt32 nSize);
		// Heapメモリ解放
		void FreeHeapBuffer( void* pBuff );

		// Globalメモリ確保
		UInt8* AllocGlobalBuffer( UInt32 nSize);
		// Globalメモリ解放
		void FreeGlobalBuffer( void* pBuff);

		// Virtualメモリ確保
		UInt8* AllocVirtuallBuffer( UInt32 nSize);
		// Virtualメモリ解放
		void FreeVirtualBuffer( void* pBuff);


		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//!
		//! IESBuffer
		//!
		//! @bref      バッファ格納用クラスの基底クラス
		//!
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		class CESBuffer : public IESBuffer{
		public:
			virtual ENUM_BUFFER_TYPE GetBufferType() = 0;

			// デフォルトコンストラクタ
			CESBuffer();

			virtual ~CESBuffer();

			// バッファが確保されているか
			bool IsEmpty();
			// バッファサイズを取得
			UInt32 GetLength() const;

			// バッファの先頭アドレス取得(変更不可)
			UInt8* GetConstBufferPtr() const;

			// バッファの先頭アドレス取得
			UInt8* GetBufferPtr();

			// バッファを確保する。
			// 以前に確保されたバッファは削除される
			bool AllocBuffer( UInt32 nBufferSize );

			// バッファを埋める
			bool FillBuffer(UInt8 nValue);

			// バッファサイズを変更する
			bool ReAllocBuffer(UInt32 nBufferSize);

			/// バッファを解放する
			void FreeBuffer();

			//所有権移動　MemCopyではなく、アドレスコピー　SrcからはDetachされる
			bool Attach( IESBuffer& cSrcBuffer );
			// バッファをクラスから切り離す
			UInt8* Detach();

			// バッファにコピーする
			bool CopyBuffer( const IESBuffer& cInBuffer );
			bool CopyBuffer( const UInt8* pBuffer, UInt32 nBufferSize, UInt32 nAllocSize = 0);
			bool CopyBuffer( const UInt32 nOffset,const UInt8* pBuffer, UInt32 nBufferSize);

			// 内部バッファの後ろに、バッファを追加する
			bool AppendBuffer( const UInt8* pBuffer, UInt32 nBufferSize );

			//Streamとして、Appendするための、バッファを事前確保する。
			//初期サイズをnBufferLenに指定、足りない場合は、nExpandLenずつ拡張される
			//Append完了後、必ずReleaseStreamBufferをCallすること
			bool GetStreamBuffer(UInt32 nBufferLen, UInt32 nExpandLen);

			//Streamに確保されているバッファ長を、実データ長に調整する。
			bool ReleaseStreamBuffer();

			//Stream mode で書き込み中の、有効データ長を取得する
			UInt32 GetValidDataLen();


		protected:
			virtual UInt8* AllocMemory( UInt32 ) = 0;
			virtual void FreeMemory( void* pBuff ) = 0;
			virtual UInt8* ReAllocMemory( void*, UInt32 ) = 0;

			void Attach( UInt8* pBuffer, UInt32 un32Size );

		private:
			UInt8* m_pBuffer;
			UInt32 m_nBufferSize;
			UInt32 m_nValidDataLen;
			UInt32 m_nStreamExpandLen;
		};

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//!
		//! CESHeapBuffer
		//!
		//! @bref      Heapメモリ用バッファクラス
		//!
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		class CESHeapBuffer : public CESBuffer
		{
		public:
			ENUM_BUFFER_TYPE GetBufferType() override{
				return BUFFER_TYPE_HEAP;
			}

			// デフォルトコンストラクタ
			CESHeapBuffer();
			// バッファサイズ指定のコンストラクタ
			CESHeapBuffer( UInt32 nBufferSize );
			// 文字列バッファ作成のコンストラクタ
			CESHeapBuffer( ESString strSrc );
			// コピーコンストラクタ
			CESHeapBuffer( const CESHeapBuffer& rhs );

			// デストラクタ
			virtual ~CESHeapBuffer();

			// コピーオペレーター
			CESHeapBuffer& operator=( CESHeapBuffer& rhs );
			// プラスオペレーター
			CESHeapBuffer& operator+=( CESHeapBuffer& rhs );

		protected:
			virtual UInt8* AllocMemory( UInt32 )	override;
			virtual void FreeMemory( void* pBuff )	override;
			virtual UInt8* ReAllocMemory( void*, UInt32 ) override;
		};


		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//!
		//! CESGlobalBuffer
		//!
		//! @bref      Globalメモリ用バッファクラス
		//!
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
		class CESGlobalBuffer : public CESBuffer
		{
		public:
			ENUM_BUFFER_TYPE GetBufferType(){
				return BUFFER_TYPE_GLOBAL;
			}

			// デフォルトコンストラクタ
			CESGlobalBuffer();
			// バッファサイズ指定のコンストラクタ
			CESGlobalBuffer( UInt32 nBufferSize );
			// 文字列バッファ作成のコンストラクタ
			CESGlobalBuffer( ESString strSrc );
			// コピーコンストラクタ
			CESGlobalBuffer( const CESGlobalBuffer& rhs );

			// デストラクタ
			virtual ~CESGlobalBuffer();

			// コピーオペレーター
			CESGlobalBuffer& operator=( CESGlobalBuffer& rhs );
			//プラスオペレーター
			CESGlobalBuffer& operator+=( CESGlobalBuffer& rhs );

		protected:
			virtual UInt8* AllocMemory( UInt32 )	override;
			virtual void FreeMemory( void* pBuff )	override;
			virtual UInt8* ReAllocMemory( void*, UInt32 ) override;
		};
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//!
		//! CESVirualBuffer
		//!
		//! @bref      Virtualメモリ用バッファクラス
		//!				巨大メモリブロック確保時はこちらを利用すること
		//!				4KB未満のブロック確保は低速、かつ非効率なので極力避けること。
		//!
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		class CESVirtualBuffer : public CESBuffer
		{
		public:
			ENUM_BUFFER_TYPE GetBufferType(){
				return BUFFER_TYPE_VIRTUAL;
			}

			/// デフォルトコンストラクタ
			CESVirtualBuffer();
			// バッファサイズ指定のコンストラクタ
			CESVirtualBuffer( UInt32 nBufferSize );
			// 文字列バッファ作成のコンストラクタ
			CESVirtualBuffer( ESString strSrc );
			// コピーコンストラクタ
			CESVirtualBuffer( const CESVirtualBuffer& rhs );

			// デストラクタ
			virtual ~CESVirtualBuffer();

			// コピーオペレーター
			CESVirtualBuffer& operator=( CESVirtualBuffer& rhs );
			//プラスオペレーター
			CESVirtualBuffer& operator+=( CESVirtualBuffer& rhs );

		protected:
			virtual UInt8* AllocMemory( UInt32 )	override;
			virtual void FreeMemory( void* pBuff )	override;
			virtual UInt8* ReAllocMemory( void*, UInt32 ) override;
		};
#endif
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//!
		//! CESIBufferStream
		//!
		//! @bref      IESBufferデーター読み込み用ストリーム
		//!
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		class CESIBufferStream {
		public:
			// コンストラクタ
			CESIBufferStream( IESBuffer* pBuffer );

			// セットされているバッファの長さを返す
			UInt32 GetLength();

			// バッファデーターを指定サイズ分読み込む
			UInt32 Read( UInt8* pOutBuffer, UInt32 nSize );

		protected:
			IESBuffer*	m_pBuffer;
			UInt32		m_nSeekPos;
		};




		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//!
		//! CESOBufferStream
		//!
		//! @bref      IESBufferデーター書き込み用ストリーム
		//!
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		class CESOBufferStream {
		public:
			// コンストラクタ
			CESOBufferStream( IESBuffer* pBuffer );

			// セットされているバッファの長さを返す
			UInt32 GetLength();

			//書き込み済みのデータ長を取得する。
			UInt32 GetWrittenLength();

			// バッファデーターを指定サイズ分読み込む
			UInt32 Write( UInt8* pOutBuffer, UInt32 nSize );

		protected:
			IESBuffer*	m_pBuffer;
			UInt32		m_nSeekPos;
		};

	};
};
