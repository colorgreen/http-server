#include "HttpResponse.h"
#include <string>
#include <sstream>

void HttpResponse::addHeader(std::string key, std::string value)
{
	headers[key] = value;
}

std::string HttpResponse::getHead() const
{
	std::stringstream ss;

	ss << "HTTP/" << version << " " << statusCode << " OK\n";

	for (auto h : headers)
		ss << h.first << ": " << h.second << "\n";
	ss << "\r\n";

	return ss.str();
}
