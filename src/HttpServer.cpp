#include "HttpServer.h"
#include <regex>
#include <string>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>

#include "Socket.h"
#include "HttpException.h"

using namespace std;

HttpServer::HttpServer(Socket& s) : socket(&s), publicdir("public/")
{
	time_t t;
    time( & t );

	response.version = "1.1";
	response.setStatusCode(500);
	response.addHeader("Date", asctime( localtime( &t ) ) );
	response.addHeader("Server", "Prosty server HTTP, Projekt na sieci");

	string data;

	const int BUFFSIZE = 2048;
	char buff[BUFFSIZE];

	int iResult;
	//do
	//{
		iResult = socket->recv(buff, BUFFSIZE);
		if (iResult > 0)
		{
			printf("Bytes received: %d\n", iResult);
			data += buff;
		}
		//else if (iResult == 0)
		//	printf("Connection closing...\n");
		//else
		//{
		//	if (WSAGetLastError() != WSAEWOULDBLOCK)
		//	{
		//		throw HttpException(500, "Could not receive data");
		//	}
		//}
	//} while (iResult > 0);

	parseData(data);
}

HttpServer::~HttpServer(){
	socket->close();
}

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
	regex r("^[A-Z]+ /(.*) ");
	smatch m;
	if (!regex_search(data, m, r))
		throw HttpException(500, "Could not parse url");

	url = m[1];
}


void HttpServer::parseVersion(const std::string& data)
{
	regex r("^[A-Z]+ .+ HTTP/(\\d+\\.\\d+)");
	smatch m;
	if (!regex_search(data, m, r))
		throw HttpException(500, "Could not parse version");

	version = m[1];
	printf("HTTP_VERSION: %s\n", version.c_str());

	response.version = version;
	if( version != "1.1" )
		throw HttpException(505, "Protocol version is not supported");
}

void HttpServer::parseData(std::string & data)
{
	try
	{
		printf("%s\n", data.c_str());

		parseMethod(data);
		parseUrl(data);
		parseVersion(data);

		printf("Handling method %s\n", method.c_str() );

		if (method == "GET")
			handleGET(data);
	}
	catch( HttpException & exception )
	{
		printf("EXCEPTION\n");

		response.setStatusCode(exception.getStatusCode());
		response.addHeader("Content-Type","text/html");
		sendResponseHead();
		socket->send(exception.getMessage().c_str(), exception.getMessage().size());
	}	
}


HttpResponse HttpServer::getResponse() const
{
	return response;
}

void HttpServer::sendResponseHead() const
{
	string s = response.getHead();
	printf("Sending response: \n%s\n", s.c_str() );
	socket->send(s.c_str(), s.size());
}

void HttpServer::handleGET(const std::string& data)
{	
	if( url == "" ){
		url = "index.html";
	}

	string path = publicdir+url;

	std::ifstream infile(path);
	printf("Url: %s\n", url.c_str());

	if( !infile )
		throw HttpException(404, string(path + " not found").c_str() );
	
	response.setStatusCode(200);
	sendResponseHead();

	std::string line;
	while (std::getline(infile, line))
		socket->send(line.c_str(), line.size());
}

std::string HttpServer::currentDate() {
  	auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
    return ss.str();
}
