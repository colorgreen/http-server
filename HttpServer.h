#pragma once
#include <string>
#include <map>
#include "HttpResponse.h"

class Socket;

class HttpServer
{

public:

	HttpServer(Socket& socket);
	
	// enum Method { Get, Post, Put, Delete, Patch, Head };

	HttpResponse getResponse() const;
	
private:
	Socket * socket;
	HttpResponse response;
	
	std::string version;
	std::string url;
	std::string	method;

	std::map<std::string, std::string> headers;

	std::string body;

	void parseMethod(const std::string& data);
	void parseUrl(const std::string& data);
	void parseVersion(const std::string& data);
	void parseData(std::string& data);
	
	void setStatusCode(int statusCode);
	void sendResponseHead() const;
	void handleGET(const std::string& cs);
};

