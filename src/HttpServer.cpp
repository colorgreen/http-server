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
	string date(asctime( localtime( &t ) ));
	date.pop_back();

	response.version = "1.1";
	response.setStatusCode(500);
	response.addHeader("Date", date );
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
	regex r("^(GET|HEAD|PUT|DELETE)");
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

	std::ifstream infile(path, ios::in | ios::binary);
	printf("Url: %s\n", url.c_str());

	if( !infile )
		throw HttpException(404, string(path + " not found").c_str() );
	
	infile.seekg(0, ios::end);
	response.addHeader("Content-Length", infile.tellg());
	infile.seekg(0, ios::beg);

	handleContentType(getExtension(path));

	
	response.setStatusCode(200);
	sendResponseHead();

	const int S = 2048;
	char buff[S] = {0};
  
  while(!infile.eof()){
    infile.read(buff, S);
    socket->send(buff, infile.gcount());
  }
}

std::string HttpServer::getExtension(std::string filename)
{
  regex r("\\.(.*)$");
  smatch m;
  if (!regex_search(filename, m, r))
    throw HttpException(500,"Could not find file extension");
  
  return m[1];
}

void HttpServer::handleContentType(const std::string& extenstion)
{
  if( extenstion == "txt" || extenstion == "TXT" )
    response.addHeader("Content-Type", "text/plain");
  if( extenstion == "json" || extenstion == "JSON" )
    response.addHeader("Content-Type", "application/json");
  if( extenstion == "html" || extenstion == "HTML" )
    response.addHeader("Content-Type", "text/html");
  if( extenstion == "php" || extenstion == "PHP" )
    response.addHeader("Content-Type", "text/html");
  if( extenstion == "jpg" || extenstion == "JPG" )
    response.addHeader("Content-Type", "image/jpeg");
  if( extenstion == "png" || extenstion == "PNG" )
    response.addHeader("Content-Type", "image/png");
  if( extenstion == "ttf" || extenstion == "TTF" )
    response.addHeader("Content-Type", "font/ttf");
  if( extenstion == "mp4" || extenstion == "MP4" )
    response.addHeader("Content-Type", "video/mp4");
}

std::string HttpServer::currentDate() {
  	auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
    return ss.str();
}
