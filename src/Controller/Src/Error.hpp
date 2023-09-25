#pragma once
#include <boost/exception/all.hpp>
#include <boost/throw_exception.hpp>
#include <stdexcept>

namespace epsonscan {
		
		// this class can not be constructed
		class Error
		{
		};

		class RuntimeError : public Error, public boost::exception, public std::runtime_error
		{
			public:
				RuntimeError(const char* message) : std::runtime_error(message)
				{
				}

				RuntimeError(const char* message, SDIError error) : std::runtime_error(message), error_(error)
				{
				}

				SDIError GetCode()
				{
					return error_;
				}

			private:
				SDIError error_ = kSDIErrorUnknownError;
		}; 

		#define ES_REQUIRE(expression, string) {  if (!(expression)) { BOOST_THROW_EXCEPTION(RuntimeError(string)); }  }

}// nemespace epsonscan



