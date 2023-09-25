//
//  FKSource.cpp
//  FileFormatKit
//
//  Created by Nakazawa Yuichi on 13/12/14.
//  Copyright (c) 2014 EPSON All rights reserved.
//
#include "stdafx.h"

#include "FKSource.h"

CFKSource::CFKSource()
{
	m_eType = kFKSourceTypePath;
	m_bFull = false;

}

CFKSource::~CFKSource()
{

}

IFKSource* CFKSource::CreateAndInitInstanceWithType(FKSourceType eSourceType, ES_CMN_FUNCS::BUFFER::IESBuffer& cSource, BOOL bIsFull)
{

	CFKSource* pcThis = (CFKSource*) new (std::nothrow) CFKSource();

	if(pcThis){

		pcThis->m_cSource.Attach(cSource);

		pcThis->m_bFull = bIsFull;
		pcThis->m_eType = eSourceType;
	}
	return pcThis;
}

ES_CMN_FUNCS::BUFFER::IESBuffer& CFKSource::GetSource()
{
	return m_cSource;
}
const ES_CMN_FUNCS::BUFFER::IESBuffer& CFKSource::GetConstSource() const
{
	return m_cSource;
}