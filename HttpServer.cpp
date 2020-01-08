#include "HttpServer.h"
#include <regex>
#include <string>
#include "HttpException.h"
#include "Socket.h"
#include <fstream>
#include <servprov.h>

using namespace std;

void HttpServer::parseMethod(const std::string& data)
{
	regex r("^(GET|HEAD|POST|PUT|DELETE|PATCH)");
	smatch m;
	if (!regex_search(data, m, r))
		throw HttpException(500,"Could not parse method");

	method = m[0];
}

void HttpServer::parseUrl(const std::string& data)
{
	regex r("^[A-Z]+ (.+) ");
	smatch m;
	if (!regex_search(data, m, r))
		throw HttpException(500, "Could not parse url");

	url = m[1];
}


void HttpServer::parseVersion(const std::string& data)
{
	regex r("^[A-Z]+ .+ HTTP\/(\\d+\.\\d+)");
	smatch m;
	if (!regex_search(data, m, r))
		throw HttpException(500, "Could not parse version");

	version = m[1];
	response.version = version;
}


void HttpServer::parseData(std::string & data)
{
	printf("%s\n", data.c_str());

	parseMethod(data);
	parseUrl(data);
	parseVersion(data);


	if (method == "GET")
		handleGET(data);

	setStatusCode(201);
}

HttpServer::HttpServer(Socket& s)
{
	this->socket = &s;

	string data;

	const int BUFFSIZE = 2048;
	char buff[BUFFSIZE];

	int iResult;
	do
	{
		iResult = socket->recv(buff, BUFFSIZE);
		if (iResult > 0)
		{
			printf("Bytes received: %d\n", iResult);
			data += buff;
		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				throw HttpException(500, "Could not receive data");
			}
		}
	} while (iResult > 0);

	parseData(data);
}

HttpResponse HttpServer::getResponse() const
{
	return response;
}


void HttpServer::setStatusCode(int statusCode)
{
	response.statusCode = statusCode;
}

void HttpServer::sendResponseHead() const
{
	string s = response.getHead();
	socket->send(s.c_str(), s.size());
}

void HttpServer::handleGET(const std::string& data)
{
	sendResponseHead();
	
	std::ifstream infile(url);

	std::string line;
	while (std::getline(infile, line))
		socket->send(line.c_str(), line.size());
}
