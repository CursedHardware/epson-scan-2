#include "Key.hpp"
#include "Error.hpp"
#include "Utility.hpp"
#include <memory.h>
#include <boost/format.hpp>
#include <algorithm>

namespace epsonscan {

		template<> void Key<SDIInt>::SetValue(SDIValueType valType, void* value, SDIInt size)
		{
			if (value) {
				SDI_TRACE_LOG("SetValueInt %s %d", GetName().c_str(), *reinterpret_cast<SDIInt*>(value));
				SetValue(*reinterpret_cast<SDIInt*>(value));
			}	
		}
	
		template<> void Key<std::string>::SetValue(SDIValueType valType, void* value, SDIInt size)
		{
			if (value) {
					std::string stringVal = std::string(reinterpret_cast<SDIChar*>(value));
					SetValue(stringVal);
				}		
		}
		template<> void Key<SDIInt>::GetValue(SDIValueType valType, void* value, SDIInt size)
		{	
			if (value) {
						SDIInt intVal = 0;
						GetValue(intVal);
						memcpy(value, &intVal, sizeof(SDIInt));

						SDI_TRACE_LOG("GetValueInt %s %d", GetName().c_str(), intVal);

				}
		}

		template<> void Key<std::string>::GetValue(SDIValueType valType, void* value, SDIInt size)
		{	
			if (value) {
						std::string string;
						GetValue(string);
						memcpy(value, string.c_str(), std::min<int32_t>(size, string.size()));
				}
		}

}


