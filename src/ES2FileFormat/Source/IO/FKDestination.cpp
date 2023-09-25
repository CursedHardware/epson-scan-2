//
//  FKDestination.cpp
//  FileFormatKit
//
//  Created by Nakazawa Yuichi on 13/12/14.
//  Copyright (c) 2014 EPSON All rights reserved.
//
#include "stdafx.h"
#include "ESJsonUtils.h"
#include "FKDestination.h"


CFKDestination::CFKDestination()
{
	m_pcDelegate = NULL;
	m_eFKDestinationType = kFKDestinationTypePath;


}

CFKDestination::~CFKDestination()
{

}

IFKDestination* CFKDestination::CreateAndInitInstanceWithType( FKDestinationType eType, ES_CMN_FUNCS::BUFFER::IESBuffer& cSource )
{

	CFKDestination* pcThis = (CFKDestination*) new (std::nothrow) CFKDestination();

	if(pcThis){

		pcThis->m_eFKDestinationType = eType;
		{

			pcThis->m_cSource.Attach(cSource);

		}
	}
	return pcThis;
}


void CFKDestination::SetUserDataAsJson(ES_CHAR_CPTR pszUserDataAsJson) {

	ESImageInfo info;

	if(pszUserDataAsJson){
		ES_IMAGE_INFO::MakeImageInfoFromJson(info,pszUserDataAsJson);

	}

	SetUserData(info);

}
void CFKDestination::GetUserDataAsJson(IESResultString& strUserDataAsJson) {

	const ESImageInfo& imageinfo = GetUserData();

	ESString strImageJson;
	ES_IMAGE_INFO::GetImageInfoAsJson(strImageJson,imageinfo);

	strUserDataAsJson.Set(strImageJson.c_str());

}

void CFKDestination::NotifyDidBeginReceivingForImageInfo( ES_CHAR_CPTR pszImageInfoAsJson )
{
	if(m_pcDelegate){
		m_pcDelegate->DestinationDidBeginReceiving( this, pszImageInfoAsJson );
	}
}

void CFKDestination::NotifyDidEndReceivingForImageInfo( ES_CHAR_CPTR pszImageInfoAsJson )
{
	if(m_pcDelegate){
		m_pcDelegate->DestinationDidEndReceiving( this, pszImageInfoAsJson );
	}
}

void CFKDestination::NotifyReceiveImageData( ES_CMN_FUNCS::BUFFER::IESBuffer& cData, ES_CHAR_CPTR pszImageInfoAsJson )
{
	if(m_pcDelegate){
		m_pcDelegate->DestinationDidReceiveImageData( this, cData, pszImageInfoAsJson );
	}
}

ES_CMN_FUNCS::BUFFER::IESBuffer& CFKDestination::GetSource()
{
	return m_cSource;
}
