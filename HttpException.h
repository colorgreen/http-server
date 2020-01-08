#pragma once
#include <string>
#include <sstream>
#include "HttpResponse.h"

class HttpException
{
	int code;
	std::string message;

public:
	HttpException(int code) : code(code)
	{
	}

	HttpException(int code, std::string message) : code(code), message(message)
	{
	}

	std::string toResponse()
	{
		std::stringstream ss;
		
		ss << "HTTP/1.1 " << code << " OK\n"
			"Content-Type: text/html; charset=utf-8\n"
			"\r\n" << message;

		return ss.str();
	}

	int getStatusCode() { return code; }
	std::string getMessage() { return message; }

};
