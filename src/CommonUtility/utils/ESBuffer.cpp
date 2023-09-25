#include "stdafx.h"
#include "ESBuffer.h"
#include <assert.h>

#ifdef WIN32
#include <algorithm>
#endif
namespace ES_CMN_FUNCS
{
	namespace BUFFER
	{
		//--------------------------------------------
		// Heepメモリ確保
		//--------------------------------------------
		UInt8* AllocHeapBuffer( UInt32 nSize)
		{
#ifdef WIN32
			HANDLE hProcessHeap = ::GetProcessHeap();
			if ( hProcessHeap != NULL ){
				return (UInt8*)::HeapAlloc(hProcessHeap, HEAP_ZERO_MEMORY, nSize);
			}
#else
			UInt8* tmp_buf = (UInt8*)::malloc(nSize);
			if(tmp_buf){
				return (UInt8*)memset(tmp_buf, 0, nSize);
			}
#endif
			return NULL;
		}

		//--------------------------------------------
		// Heapメモリ解放
		//--------------------------------------------
		void FreeHeapBuffer( void* pBuff )
		{
			try {
				if ( pBuff != NULL ){
#ifdef WIN32
					HANDLE hProcessHeap = ::GetProcessHeap();
					if ( hProcessHeap != NULL ){
						::HeapFree(hProcessHeap, 0, (LPVOID)pBuff);
					}
#else
					free(pBuff);
					pBuff = NULL;
#endif
				}
			} catch(...){
			}
		}

		//--------------------------------------------
		// Heapメモリ再割り当て
		//--------------------------------------------
		UInt8* ReAllocHeapBuffer( void* pBuff, UInt32 nSize )
		{
#ifdef WIN32
			HANDLE hProcessHeap = ::GetProcessHeap();
			if ( hProcessHeap != NULL ){
				return (UInt8*)::HeapReAlloc(hProcessHeap, HEAP_ZERO_MEMORY, pBuff, nSize);
			}
			return NULL;
#else
			return (UInt8*)realloc(pBuff, nSize);
#endif
		}

#ifdef WIN32
		//--------------------------------------------
		// Globalメモリ確保
		//--------------------------------------------
		UInt8* AllocGlobalBuffer( UInt32 nSize)
		{
#ifdef WIN32
			return (UInt8*)::GlobalAlloc( GPTR, nSize );
#else
			return (UInt8*)AllocHeapBuffer( nSize );
#endif
		}

		//--------------------------------------------
		// Globalメモリ解放
		//--------------------------------------------
		void FreeGlobalBuffer( void* pBuff)
		{
			try {
				if ( pBuff != NULL ){
#ifdef WIN32
					::GlobalFree((HGLOBAL) pBuff);
#else
					free(pBuff);
					pBuff = NULL;
#endif
				}
			} catch(...){
			}
		}

		//--------------------------------------------
		// Globalメモリ再割り当て
		//--------------------------------------------
		UInt8* ReAllocGlobalBuffer( void* pBuff, UInt32 nSize )
		{
#ifdef WIN32
			return (UInt8*)::GlobalReAlloc( (HGLOBAL)pBuff, nSize, GPTR );
#else
			return (UInt8*)realloc( (void*)pBuff, nSize );
#endif
		}


		//--------------------------------------------
		// Virtualメモリ確保
		//--------------------------------------------
		UInt8* AllocVirtualBuffer( UInt32 nSize)
		{
#ifdef WIN32
			return (UInt8*)::VirtualAlloc(NULL,nSize,MEM_COMMIT,PAGE_READWRITE);
#else
			return (UInt8*)AllocHeapBuffer(nSize);
#endif
		}

		//--------------------------------------------
		// Virtualメモリ解放
		//--------------------------------------------
		void FreeVirtualBuffer( void* pBuff)
		{
			try {
				if ( pBuff != NULL ){
#ifdef WIN32
					::VirtualFree( pBuff,0,MEM_RELEASE);
#else
					free(pBuff);
					pBuff = NULL;
#endif
				}
			} catch(...){
			}
		}

		//--------------------------------------------
		// Virtualメモリ再割り当て
		//--------------------------------------------
		UInt8* ReAllocVirtualBuffer( void* pBuff,UInt32 nOldSize, UInt32 nNewSize )
		{

			void* pNewBuf = AllocVirtualBuffer(nNewSize);

			if(pNewBuf){
				memcpy_s(pNewBuf,nNewSize,pBuff,nOldSize);
				FreeVirtualBuffer(pBuff);

			}
			return (UInt8*)pNewBuf;;
		}
#endif

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//!
		//! IESBuffer
		//!
		//! @bref      バッファ格納用クラスの基底クラス
		//!
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//=========================================================================
		//!
		//! デフォルトコンストラクタ
		//!
		//=========================================================================
		CESBuffer::CESBuffer()
			: m_pBuffer( NULL ),
			  m_nBufferSize( 0 ),
			  m_nValidDataLen(0),
			  m_nStreamExpandLen(0)
		{
		}
		//=========================================================================
		//!
		//! デフォルトデストラクタ
		//!
		//=========================================================================
		CESBuffer::~CESBuffer(){


		}

		//=========================================================================
		//!
		//! バッファが確保されているか
		//!
		//=========================================================================
		bool CESBuffer::IsEmpty()
		{
			return (m_pBuffer == NULL ) ? true : false;
		}

		//=========================================================================
		//!
		//! バッファサイズを取得
		//!
		//=========================================================================
		UInt32 CESBuffer::GetLength() const
		{
			return m_nBufferSize;
		}

		//=========================================================================
		//!
		//! バッファの先頭アドレスを取得
		//!
		//=========================================================================
		UInt8* CESBuffer::GetConstBufferPtr() const
		{
			return m_pBuffer;
		}

		//=========================================================================
		//!
		//! バッファの先頭アドレスを取得
		//!
		//=========================================================================
		UInt8* CESBuffer::GetBufferPtr()
		{
			return m_pBuffer;
		}

		//=========================================================================
		//!
		//! バッファを確保する。
		//!
		//! @note 以前に確保されたバッファは削除される
		//!
		//=========================================================================
		bool CESBuffer::AllocBuffer( UInt32 nBufferSize )
		{
			FreeBuffer();
			m_pBuffer = AllocMemory( nBufferSize );
			if ( m_pBuffer != NULL ){
				m_nBufferSize = nBufferSize;
				m_nValidDataLen = nBufferSize;
				return true;
			} else {
				assert(false);
				return false;
			}
		}


		//=========================================================================
		//!
		//! バッファを埋める
		//!
		//=========================================================================
		bool CESBuffer::FillBuffer(UInt8 nValue)
		{
			for (UInt32 i = 0; i < m_nBufferSize; i++)
			{
				m_pBuffer[i] = nValue;
			}

			return true;
		}


		//=========================================================================
		//!
		//! バッファサイズを変更する。
		//!
		//=========================================================================
		bool CESBuffer::ReAllocBuffer( UInt32 nBufferSize )
		{
			if (nBufferSize > m_nBufferSize)
			{
				UInt8* pBuffer = ReAllocMemory(m_pBuffer, nBufferSize );
				if (pBuffer != NULL) {
					m_pBuffer     = pBuffer;
					m_nBufferSize = nBufferSize;
					m_nValidDataLen= nBufferSize;
					return true;
				} else {
					return false;
				}
			}else{
				m_nBufferSize = nBufferSize;
				m_nValidDataLen= nBufferSize;
				return true;
			}
		}


		//=========================================================================
		//!
		//! バッファを解放する。
		//!
		//=========================================================================
		void CESBuffer::FreeBuffer()
		{
			if ( m_pBuffer ){
				FreeMemory( (void*)m_pBuffer );
				m_pBuffer = NULL;
				m_nBufferSize = 0;
				m_nValidDataLen= 0;
			}
		}

		//=========================================================================
		//!
		//! 所有権移動
		//!
		//=========================================================================
		bool CESBuffer::Attach( IESBuffer& cSrcBuffer )
		{
			bool bRet = true;
			if ( GetBufferType() == cSrcBuffer.GetBufferType() ){
				UInt32 un32Len  = cSrcBuffer.GetLength();
				UInt8* pBufTemp = cSrcBuffer.Detach();

				CESBuffer::Attach( pBufTemp, un32Len );
				bRet = true;
			} else {
				bRet = CESBuffer::CopyBuffer( cSrcBuffer );
				if ( bRet ){
					cSrcBuffer.FreeBuffer();
				}
			}
			return bRet;
		}

		//=========================================================================
		//!
		//! バッファをクラスから切り離す
		//!
		//=========================================================================
		UInt8* CESBuffer::Detach()
		{
			UInt8* pTmp = m_pBuffer;
			m_pBuffer = NULL;
			m_nBufferSize = 0;
			m_nValidDataLen=0;
			return pTmp;
		}

		//=========================================================================
		//!
		//! バッファにコピーする
		//!
		//=========================================================================
		bool CESBuffer::CopyBuffer( const IESBuffer& cInBuffer )
		{
			return CopyBuffer( cInBuffer.GetConstBufferPtr(), cInBuffer.GetLength(), cInBuffer.GetLength() );
		}
		bool CESBuffer::CopyBuffer( const UInt8* pBuffer, UInt32 nBufferSize, UInt32 nAllocSize /*=0*/ )
		{
			FreeBuffer();

			if ( nBufferSize == 0 ){
				return true;
			}
			try {
				if ( nAllocSize < nBufferSize ){
					nAllocSize = nBufferSize;
				}
				AllocBuffer( nAllocSize );
				if ( IsEmpty() ){
					return false;
				}
				errno_t err = ::memcpy_s( m_pBuffer, m_nBufferSize, pBuffer, nBufferSize );

				return ( err == 0 );
			} catch(...){
				return false;
			}
		}

		bool CESBuffer::CopyBuffer( const UInt32 nOffset,const UInt8* pBuffer, UInt32 nBufferSize){

			if ( nBufferSize == 0 ){
				return true;
			}
			try {
				if ( nOffset + nBufferSize > GetLength() ){
					return false;
				}
				errno_t err = ::memcpy_s( m_pBuffer + nOffset, m_nBufferSize- nOffset, pBuffer, nBufferSize );
				return ( err == 0 );
			} catch(...){
				return false;
			}
		}

		//=========================================================================
		//!
		//! 既存バッファーにアタッチする。型安全のため、サブクラスのメソッドを経由させる
		//!
		//=========================================================================
		void CESBuffer::Attach( UInt8* pBuffer, UInt32 un32Size )
		{
			FreeBuffer();

			m_pBuffer		= pBuffer;
			m_nBufferSize	= un32Size;
			m_nValidDataLen = un32Size;
		}

		//=========================================================================
		//!
		//! 内部バッファの後ろに、バッファを追加する
		//!
		//=========================================================================
		bool CESBuffer::AppendBuffer( const UInt8* pBuffer, UInt32 nBufferSize )
		{

			if ( nBufferSize == 0 ){
				return true;
			}

			UInt8* pCpyPos = NULL;
			if ( IsEmpty() ){
				//空っぽの場合

				if ( !AllocBuffer( nBufferSize ) ){
					return false;
				}
				pCpyPos = GetBufferPtr();
				m_nValidDataLen = 0;
			} else {
				//すでに値を保持している場合

				UInt32 nNewBufferSize = GetLength();

				if(m_nStreamExpandLen != 0){
					//Stream mode では拡張
					if( GetValidDataLen() + nBufferSize > GetLength()){
						nNewBufferSize = GetValidDataLen() + std::max<UInt32>(m_nStreamExpandLen,nBufferSize);
					}
					pCpyPos = GetBufferPtr() + GetValidDataLen();//= oldBufferLen
					//足りている場合は拡張しない
				}else{
					nNewBufferSize = m_nBufferSize + nBufferSize;
					pCpyPos = GetBufferPtr();
				}

				if(nNewBufferSize > GetLength()){
					//サイズ伸張ではReallocを行う
					UInt8* pRealloc = ReAllocMemory( GetBufferPtr(), nNewBufferSize );
					if ( pRealloc == NULL ){
						assert(false);
						return false;
					} else {
						m_pBuffer = pRealloc;
						m_nBufferSize = nNewBufferSize;
						pCpyPos = m_pBuffer + GetValidDataLen();//= oldBufferLen
					}

				}
			}
			if ( pCpyPos != NULL ){
				memcpy_s( pCpyPos, m_nBufferSize - GetValidDataLen(), pBuffer, nBufferSize );
				m_nValidDataLen += nBufferSize;
			}
			return true;
		}

		//Streamとして、Appendするための、バッファを事前確保する。
		//初期サイズをnBufferLenに指定、足りない場合は、nExpandLenずつ拡張される
		//Append完了後、必ずReleaseStreamBufferをCallすること
		bool CESBuffer::GetStreamBuffer(UInt32 nBufferLen, UInt32 nExpandLen){

			bool bRet= AllocBuffer(nBufferLen);
				if(bRet){
					m_nValidDataLen    = 0;
					m_nStreamExpandLen = nExpandLen;
				}else{
					m_nStreamExpandLen = 0;
				}
			return bRet;

		}

			//Streamに確保されているバッファ長を、実データ長に調整する。
		bool CESBuffer::ReleaseStreamBuffer(){

			if(m_nValidDataLen > m_nBufferSize){
				assert(false);
			}

			if(m_nValidDataLen ==0){
				FreeBuffer();
			}

			m_nBufferSize = m_nValidDataLen;
			m_nStreamExpandLen =0;

			return TRUE;
		}

		//Stream mode で書き込み中の、有効データ長を取得する
		UInt32 CESBuffer::GetValidDataLen(){
			return m_nValidDataLen;
		}
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//!
		//! CESHeapBuffer
		//!
		//! @bref      Heapメモリ用バッファクラス
		//!
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//=========================================================================
		//!
		//! デフォルトコンストラクタ
		//!
		//=========================================================================
		CESHeapBuffer::CESHeapBuffer()
		{
		}

		//=========================================================================
		//!
		//! バッファサイズ指定のコンストラクタ
		//!
		//=========================================================================
		CESHeapBuffer::CESHeapBuffer( UInt32 nBufferSize )
		{
			AllocBuffer( nBufferSize );
		}

		//=========================================================================
		//!
		//! 文字列バッファ作成のコンストラクタ
		//!
		//=========================================================================
		CESHeapBuffer::CESHeapBuffer( ESString strSrc )
		{
			if ( !strSrc.empty() ){
				UInt32 nStrLength = (UInt32)(strSrc.length() * sizeof(ES_CHAR));
				UInt32 nBufferSize = nStrLength + sizeof(ES_CHAR);

				CopyBuffer( (UInt8*)strSrc.c_str(), nStrLength, nBufferSize );
			}
		}

		//=========================================================================
		//!
		//! コピーコンストラクタ
		//!
		//=========================================================================
		CESHeapBuffer::CESHeapBuffer( const CESHeapBuffer& rhs )
		{
			CopyBuffer( rhs );
		}

		//=========================================================================
		//!
		//! デストラクタ
		//!
		//=========================================================================
		CESHeapBuffer::~CESHeapBuffer()
		{
			FreeBuffer();
		}

		//=========================================================================
		//!
		//! コピーオペレーター
		//!
		//=========================================================================
		CESHeapBuffer& CESHeapBuffer::operator=( CESHeapBuffer& rhs )
		{
			CopyBuffer( rhs );
			return *this;
		}

		//=========================================================================
		//!
		//! プラスオペレーター
		//!
		//=========================================================================
		CESHeapBuffer& CESHeapBuffer::operator+=( CESHeapBuffer& rhs )
		{
			AppendBuffer( rhs.GetBufferPtr(), rhs.GetLength() );
			return *this;
		}

		//=========================================================================
		//!
		//! Heepメモリ確保
		//!
		//=========================================================================
		UInt8* CESHeapBuffer::AllocMemory( UInt32 nSize)
		{
			return AllocHeapBuffer( nSize );
		}

		//=========================================================================
		//!
		//! Heapメモリ解放
		//!
		//=========================================================================
		void CESHeapBuffer::FreeMemory( void* pBuff )
		{
			FreeHeapBuffer( pBuff );
		}

		//=========================================================================
		//!
		//! Heapメモリ再割り当て
		//!
		//=========================================================================
		UInt8* CESHeapBuffer::ReAllocMemory( void* pBuffer, UInt32 nSize )
		{
			return ReAllocHeapBuffer( pBuffer, nSize );
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//!
		//! CESGlobalBuffer
		//!
		//! @bref      Globalメモリ用バッファクラス
		//!
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//=========================================================================
		//!
		//! デフォルトコンストラクタ
		//!
		//=========================================================================
#ifdef WIN32
		CESGlobalBuffer::CESGlobalBuffer()
		{
		}

		//=========================================================================
		//!
		//! バッファサイズ指定のコンストラクタ
		//!
		//=========================================================================
		CESGlobalBuffer::CESGlobalBuffer( UInt32 nBufferSize )
		{
			AllocBuffer( nBufferSize );
		}

		//=========================================================================
		//!
		//! 文字列バッファ作成のコンストラクタ
		//!
		//=========================================================================
		CESGlobalBuffer::CESGlobalBuffer( ESString strSrc )
		{
			if ( !strSrc.empty() ){
				UInt32 nStrLength = strSrc.length() * sizeof(ES_CHAR);
				UInt32 nBufferSize = nStrLength + sizeof(ES_CHAR);

				CopyBuffer( (UInt8*)strSrc.c_str(), nStrLength, nBufferSize );
			}
		}

		//=========================================================================
		//!
		//! コピーコンストラクタ
		//!
		//=========================================================================
		CESGlobalBuffer::CESGlobalBuffer( const CESGlobalBuffer& rhs )
		{
			CopyBuffer( rhs );
		}

		//=========================================================================
		//!
		//! デストラクタ
		//!
		//=========================================================================
		CESGlobalBuffer::~CESGlobalBuffer()
		{
			FreeBuffer();
		}

		//=========================================================================
		//!
		//! コピーオペレーター
		//!
		//=========================================================================
		CESGlobalBuffer& CESGlobalBuffer::operator=( CESGlobalBuffer& rhs )
		{
			CopyBuffer( rhs );
			return *this;
		}

		//=========================================================================
		//!
		//! プラスオペレーター
		//!
		//=========================================================================
		CESGlobalBuffer& CESGlobalBuffer::operator+=( CESGlobalBuffer& rhs )
		{
			AppendBuffer( rhs.GetBufferPtr(), rhs.GetLength() );
			return *this;
		}

		//=========================================================================
		//!
		//! Globalメモリ確保
		//!
		//=========================================================================
		UInt8* CESGlobalBuffer::AllocMemory( UInt32 nSize)
		{
			return AllocGlobalBuffer( nSize );
		}

		//=========================================================================
		//!
		//! Globalメモリ解放
		//!
		//=========================================================================
		void CESGlobalBuffer::FreeMemory( void* pBuff)
		{
			FreeGlobalBuffer( pBuff );
		}

		//=========================================================================
		//!
		//! Heapメモリ再割り当て
		//!
		//=========================================================================
		UInt8* CESGlobalBuffer::ReAllocMemory( void* pBuffer, UInt32 nSize )
		{
			return ReAllocGlobalBuffer( pBuffer, nSize );
		}


		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//!
		//! CESVirtualBuffer
		//!
		//! @bref      Virtualメモリ用バッファクラス
		//!
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//=========================================================================
		//!
		//! デフォルトコンストラクタ
		//!
		//=========================================================================
		CESVirtualBuffer::CESVirtualBuffer()
		{
		}

		//=========================================================================
		//!
		//! バッファサイズ指定のコンストラクタ
		//!
		//=========================================================================
		CESVirtualBuffer::CESVirtualBuffer( UInt32 nBufferSize )
		{
			AllocBuffer( nBufferSize );
		}

		//=========================================================================
		//!
		//! 文字列バッファ作成のコンストラクタ
		//!
		//=========================================================================
		CESVirtualBuffer::CESVirtualBuffer( ESString strSrc )
		{
			if ( !strSrc.empty() ){
				UInt32 nStrLength = strSrc.length() * sizeof(ES_CHAR);
				UInt32 nBufferSize = nStrLength + sizeof(ES_CHAR);

				CopyBuffer( (UInt8*)strSrc.c_str(), nStrLength, nBufferSize );
			}
		}

		//=========================================================================
		//!
		//! コピーコンストラクタ
		//!
		//=========================================================================
		CESVirtualBuffer::CESVirtualBuffer( const CESVirtualBuffer& rhs )
		{
			CopyBuffer( rhs );
		}

		//=========================================================================
		//!
		//! デストラクタ
		//!
		//=========================================================================
		CESVirtualBuffer::~CESVirtualBuffer()
		{
			FreeBuffer();
		}

		//=========================================================================
		//!
		//! コピーオペレーター
		//!
		//=========================================================================
		CESVirtualBuffer& CESVirtualBuffer::operator=( CESVirtualBuffer& rhs )
		{
			CopyBuffer( rhs );
			return *this;
		}

		//=========================================================================
		//!
		//! プラスオペレーター
		//!
		//=========================================================================
		CESVirtualBuffer& CESVirtualBuffer::operator+=( CESVirtualBuffer& rhs )
		{
			AppendBuffer( rhs.GetBufferPtr(), rhs.GetLength() );
			return *this;
		}

		//=========================================================================
		//!
		//! Globalメモリ確保
		//!
		//=========================================================================
		UInt8* CESVirtualBuffer::AllocMemory( UInt32 nSize)
		{
			return AllocVirtualBuffer( nSize );
		}

		//=========================================================================
		//!
		//! Globalメモリ解放
		//!
		//=========================================================================
		void CESVirtualBuffer::FreeMemory( void* pBuff)
		{
			FreeVirtualBuffer( pBuff );
		}

		//=========================================================================
		//!
		//! Heapメモリ再割り当て
		//!
		//=========================================================================
		UInt8* CESVirtualBuffer::ReAllocMemory( void* pBuffer, UInt32 nSize )
		{
			return ReAllocVirtualBuffer( pBuffer,GetLength(), nSize );
		}
#endif
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//!
		//! CESIBufferStream
		//!
		//! @bref      IESBufferデーター読み込み用ストリーム
		//!
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//=========================================================================
		//!
		//! コンストラクタ
		//!
		//=========================================================================
		CESIBufferStream::CESIBufferStream( IESBuffer* pBuffer )
		{
			m_pBuffer = pBuffer;
			m_nSeekPos = 0;
		}

		//=========================================================================
		//!
		//! セットされているバッファの長さを返す
		//!
		//=========================================================================
		UInt32 CESIBufferStream::GetLength(){
			return ( m_pBuffer == NULL ) ? 0 : m_pBuffer->GetLength();
		}

		//=========================================================================
		//!
		//! バッファデーターを指定サイズ分読み込む
		//!
		//=========================================================================
		UInt32 CESIBufferStream::Read( UInt8* pOutBuffer, UInt32 nSize )
		{
			UInt32 nRead = 0;

			UInt32 nDataSize = GetLength();
			if ( nDataSize <= m_nSeekPos ){
				return 0;
			}
			nDataSize -= m_nSeekPos;
			if ( nSize <= nDataSize ){
				nRead = nSize;
			} else {
				nRead = nSize - nDataSize;
			}
			errno_t err = memcpy_s( pOutBuffer, nSize, m_pBuffer->GetBufferPtr() + m_nSeekPos, nRead );
			m_nSeekPos += nRead;
			if ( err != 0 ){
				return 0;
			}
			return nRead;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//!
		//! CESOBufferStream
		//!
		//! @bref      IESBufferデーター書き込み用ストリーム
		//!
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//=========================================================================
		//!
		//! コンストラクタ
		//!
		//=========================================================================
		CESOBufferStream::CESOBufferStream( IESBuffer* pBuffer )
		{
			m_pBuffer = pBuffer;
			m_nSeekPos = 0;
		}

		//=========================================================================
		//!
		//! セットされているバッファの長さを返す
		//!
		//=========================================================================
		UInt32 CESOBufferStream::GetLength(){
			return ( m_pBuffer == NULL ) ? 0 : m_pBuffer->GetLength();
		}

		UInt32 CESOBufferStream::GetWrittenLength(){
			return m_nSeekPos;
		}

		//=========================================================================
		//!
		//! バッファデーターを指定サイズ分書き込む
		//!
		//=========================================================================
		UInt32 CESOBufferStream::Write( UInt8* pOutBuffer, UInt32 nSize )
		{
			UInt32 nWrite = 0;

			UInt32 nDataSize = GetLength();
			if ( nDataSize <= m_nSeekPos ){
				return 0;
			}
			nDataSize -= m_nSeekPos;
			if ( nSize <= nDataSize ){
				nWrite = nSize;
			} else {
				nWrite = nSize - nDataSize;
			}
			errno_t err = memcpy_s( m_pBuffer->GetBufferPtr() + m_nSeekPos, nDataSize, pOutBuffer, nWrite );

			m_nSeekPos += nWrite;
			if ( err != 0 ){
				return 0;
			}
			return nWrite;
		}


	}; // namespace BUFFER
}; // namespace ES_CMN_FUNCS
