#pragma once
#if 0
	namespace
	{
		#include <sstream>
		class stopwatch
		{
		public:
			explicit stopwatch(const std::string& message):message_(message)
			{
				std::stringstream ss;
				ss << "@,>>,[" << message_.c_str() << "],";
				OutputDebugStringA(ss.str().c_str());
				QueryPerformanceFrequency(&freq_);
				QueryPerformanceCounter(&begin_);
			}
			explicit stopwatch(const std::string& message, const std::string& key):message_(message)
			{
				std::stringstream ss;
				ss << "@,>>,[" << message_.c_str() << ":" << key.c_str() <<"],";
				OutputDebugStringA(ss.str().c_str());
				QueryPerformanceFrequency(&freq_);
				QueryPerformanceCounter(&begin_);
			}
			~stopwatch()
			{
				QueryPerformanceCounter(&end_);
				std::stringstream ss;
				ss << "@,<<,[" << message_.c_str() << "],";
				ss << int(double(end_.QuadPart - begin_.QuadPart) * 1000.0 / double(freq_.QuadPart));
				OutputDebugStringA(ss.str().c_str());
			}
		private:
			LARGE_INTEGER freq_;
			LARGE_INTEGER begin_;
			LARGE_INTEGER end_;
			std::string message_;
		};	
		#define INTERVALTIME_LOG ::stopwatch stopwatch_(__FUNCTION__);
	};

#else
		#define INTERVALTIME_LOG ;
#endif
