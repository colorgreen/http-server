#pragma once
#include <string>
#include <map>

class HttpResponse
{
	friend class HttpServer;
	
	int statusCode;
	std::string version;
	std::map<std::string, std::string> headers;

public:
	void setStatusCode(int code);
	void addHeader(std::string key, int value);
	void addHeader(std::string key, std::string value);

	std::string getHead() const;
};

