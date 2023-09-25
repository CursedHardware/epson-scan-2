#pragma once

#include "TransferEvent.hpp"
#include <boost/optional.hpp>
#include <queue>
#include <../CommonUtility/utils/CritSec.h>

namespace epsonscan
{
//----------------------------------------=----------------------------------------┐
/*!@brief	Image Manager
*/
class TransferMgr 
{
public :
	TransferMgr() ;
	virtual ~TransferMgr() ;


	/**
	画像データインスタンスが取り出し可能であるかどうかを返す。
	closeされておらず、queueが空である場合は、処理がブロックされる。
	*/
	bool	CanDequeue() ;

	/**
	画像データインスタンスが取り出し可能であるかどうかを返す。
	*/
	bool	CanDequeueWithoutWait() ;

	/**
	使用を開始する。
	*/
	void	Open() ;

	/**
	使用を終了する。
	*/
	void	Close() ;

	/**
	使用を中止する。
	queue内のすべてのSDIImageDataに対し、closeが送られる。キャンセル時に使用する。
	*/
	void	Abort() ;

	/**
	画像データインスタンスをqueueの末尾に入れる。
	ImageDataの参照カウンタは操作しない
	*/
	void	EnqueEvent(TransferEvent event) ;

	/**
	画像データインスタンスをqueueの先頭から取り出す。
	closeされておらず、queueが空である場合は、処理がブロックされる。
	ImageDataの参照カウンタは操作しない
	*/
	boost::optional<TransferEvent>	DequeueEvent() ;

	size_t		GetQueueCount() {
		return m_Queue.size();
	}

	void	Reset();


	void	SetDrained(bool bDrained) {
		m_bDrained = bDrained;
	}
	bool	GetDrained() const {
		return m_bDrained;
	}

private :

	void	EnterCriticalSection_local() 
	{
		m_stCritSec.Lock();
	}

	void	LeaveCriticalSection_local() 
	{
		m_stCritSec.Unlock();
	}


	bool			isOpened() ;
	boost::optional<TransferEvent>	lastEventWithDequeue(bool dequeue) ;
public :
	void	wait();
private :
	CritSec	m_stCritSec ;

	bool	m_bOpend ;

	std::deque<TransferEvent>	m_Queue ;

	bool	m_bDrained;
	int		m_nCurrentPageNumber;
} ;

} // namespace epsonscan
