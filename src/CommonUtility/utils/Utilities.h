#pragma once
#include <math.h>
#ifdef WIN32
#include <rpc.h>
#else
#if  !defined(__ANDROID__) && !TARGET_OS_IPHONE && !defined(TARGET_OS_EMBEDDED)
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/lexical_cast.hpp>

using namespace boost::uuids;

#endif

#endif
namespace ES_CMN_FUNCS
{

	inline float roundf(float x)
	{
		return x >= 0.0f ? floorf(x + 0.5f) : ceilf(x - 0.5f);
	}

#if  !defined(__ANDROID__) && !TARGET_OS_IPHONE && !defined(TARGET_OS_EMBEDDED)

	inline bool GenerateUUID(ESString& strResult)
	{
#ifdef WIN32
		UUID uuid;
		if(UuidCreate(&uuid) == RPC_S_OK)
		{
			ES_CHAR* pUuid = NULL;
			if(UuidToStringW(&uuid, reinterpret_cast<RPC_WSTR*>(&pUuid)) == RPC_S_OK)
			{
				if(pUuid)
				{
					strResult = ESString(pUuid);
					std::transform(strResult.cbegin(), strResult.cend(), strResult.begin(), toupper);
					RpcStringFree(reinterpret_cast<RPC_WSTR*>(&pUuid));
					return true;
				}
			}
		}
		return false;
#else
		// ランダムにユニークIDを生成
		const uuid id = random_generator()();

		// 文字列に変換
		strResult = boost::lexical_cast<std::string>(id);

		return true;
#endif
	}
#endif
};
