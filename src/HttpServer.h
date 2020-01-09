#pragma once
#include <string>
#include <map>
#include "HttpResponse.h"

class Socket;

class HttpServer
{

public:

	HttpServer(Socket& socket);
	~HttpServer();
	
	HttpResponse getResponse() const;
	
private:
	Socket * socket;
	HttpResponse response;
	
	std::string publicdir;
	std::string version;
	std::string url;
	std::string	method;

	std::map<std::string, std::string> headers;

	std::string body;

	void parseMethod(const std::string& data);
	void parseUrl(const std::string& data);
	void parseVersion(const std::string& data);
	void parseData(const std::string& data);
	void parseHeaders(const std::string& data);
	
	std::string getExtension(std::string filename);
	
	void sendResponseHead() const;
	void handleContentType(const std::string& extension);
	void handleGETHEAD(const std::string& cs, bool body = true);

	std::string currentDate();
};

