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

	int getStatusCode() { return code; }
	std::string getMessage() { return message; }

};
