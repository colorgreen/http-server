#include "HttpResponse.h"
#include <string>
#include <sstream>

void HttpResponse::setStatusCode(int code)
{
	this->statusCode = code;
}

void HttpResponse::addHeader(std::string key, int value)
{
	std::stringstream ss;
	ss << value;
	headers[key] = ss.str();
}

void HttpResponse::addHeader(std::string key, std::string value)
{
	headers[key] = value;
}

std::string HttpResponse::getHead() const
{
	std::stringstream ss;

	ss << "HTTP/" << version << " " << statusCode << " "<<codeDescrtiptions[statusCode]<<"\n";

	for (auto h : headers)
		ss << h.first << ": " << h.second << "\n";
	ss << "\r\n";

	return ss.str();
}
