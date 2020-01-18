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

HttpServer::HttpServer(Socket &s) : socket(&s), publicdir("public/") {
    time_t t;
    time(&t);
    string
    date(asctime(localtime(&t)));
    date.pop_back();

    response.version = "1.1";
    response.setStatusCode(500);
    response.addHeader("Date", date);
    response.addHeader("Server", "Prosty server HTTP, Projekt na sieci");

    string data;

    const int BUFFSIZE = 16384;
    char buff[BUFFSIZE];
    int iResult;
    int sum = 0;
    string check;

    do {
        iResult = socket->recv(buff, 1);
        rHeaders += buff;

        try {
            check = rHeaders.substr(rHeaders.length() - 4, 4);
            if (check == "\r\n\r\n") break;
        }
        catch (const std::out_of_range &e) {}
    } while (iResult > 0);

    bodySize = 0;
    smatch m;

    if (regex_search(rHeaders, m, regex("Content-Length:.*\\r\\n"))) {
        string contentLength = m.str(0).c_str();
        regex_search(contentLength, m, regex("[0-9]+"));
        bodySize = stoi(m.str(0).c_str());

        rBody = new unsigned char[bodySize];

        while (sum != bodySize) {
            iResult = socket->recv(buff, BUFFSIZE);
            memcpy(rBody + sum, buff, iResult);
            sum += iResult;
        }
    }

    parseData(rHeaders);
}

HttpServer::~HttpServer() {
    socket->close();
}

void HttpServer::parseMethod(const std::string &data) {
    regex r("^(GET|HEAD|PUT|DELETE)");
    smatch m;
    if (!regex_search(data, m, r))
        throw HttpException(500, "Could not parse method");

    method = m[0];
}

void HttpServer::parseUrl(const std::string &data) {
    regex r("^[A-Z]+ /(.*) ");
    smatch m;
    if (!regex_search(data, m, r))
        throw HttpException(500, "Could not parse url");

    url = m[1];
}


void HttpServer::parseVersion(const std::string &data) {
    regex r("^[A-Z]+ .+ HTTP/(\\d+\\.\\d+)");
    smatch m;
    if (!regex_search(data, m, r))
        throw HttpException(500, "Could not parse version");

    version = m[1];
    printf("HTTP_VERSION: %s\n", version.c_str());

    response.version = version;
    if (version != "1.1")
        throw HttpException(505, "Protocol version is not supported");
}

void HttpServer::parseHeaders(const std::string &data) {
    regex r("^.*\\n(.*)(\\r\\n|\\n)");
    smatch m;
    regex_search(data, m, r);

    string s = m[1];
    printf("HEADERS\n%s\n==========", s.c_str());
}

void HttpServer::parseData(const std::string &data) {
    try {
        printf("%s\n", data.c_str());

        parseMethod(data);
        parseUrl(data);
        parseVersion(data);
        parseHeaders(data);

        printf("Handling method %s\n", method.c_str());

        if (method == "GET")
            handleGETHEAD(data);
        if (method == "HEAD")
            handleGETHEAD(data, false);
        if (method == "PUT")
            handlePUT(data);
        else
            throw HttpException(405, "Method not allowed");
    }
    catch (HttpException &exception) {
        printf("EXCEPTION\n");

        response.setStatusCode(exception.getStatusCode());
        response.addHeader("Content-Type", "text/html");
        sendResponseHead();
        socket->send(exception.getMessage().c_str(), exception.getMessage().size());
    }
}


HttpResponse HttpServer::getResponse() const {
    return response;
}

void HttpServer::sendResponseHead() const {
    string s = response.getHead();
    printf("Sending response: \n%s\n", s.c_str());
    socket->send(s.c_str(), s.size());
}

void HttpServer::handleGETHEAD(const std::string &data, bool body) {
    if (url == "") {
        url = "index.html";
    }

    string path = publicdir + url;

    std::ifstream infile(path, ios::in | ios::binary);
    printf("Url: %s\n", url.c_str());

    if (!infile)
        throw HttpException(404, string(path + " not found").c_str());

    infile.seekg(0, ios::end);
    response.addHeader("Content-Length", infile.tellg());
    infile.seekg(0, ios::beg);

    handleContentType(getExtension(path));

    /////////////////
    response.setStatusCode(200);
    sendResponseHead();

    if (!body) return;

    const int S = 2048;
    char buff[S] = {0};

    while (!infile.eof()) {
        infile.read(buff, S);
        socket->send(buff, infile.gcount());
    }
}

void HttpServer::handlePUT(const std::string &data) {
    if (url == "") {
        url = "index.html";
    }

    string path = publicdir + url;

    std::ofstream writefile(path, ios::out | ios::binary);
    printf("Url: %s\n", url.c_str());

    writefile.write((char *) rBody, bodySize);
    writefile.close();
}


std::string HttpServer::getExtension(std::string filename) {
    regex r("\\.(.*)$");
    smatch m;
    if (!regex_search(filename, m, r))
        throw HttpException(500, "Could not find file extension");

    return m[1];
}

void HttpServer::handleContentType(const std::string &extenstion) {
    if (extenstion == "txt" || extenstion == "TXT")
        response.addHeader("Content-Type", "text/plain");
    if (extenstion == "json" || extenstion == "JSON")
        response.addHeader("Content-Type", "application/json");
    if (extenstion == "html" || extenstion == "HTML")
        response.addHeader("Content-Type", "text/html");
    if (extenstion == "php" || extenstion == "PHP")
        response.addHeader("Content-Type", "text/html");
    if (extenstion == "jpg" || extenstion == "JPG")
        response.addHeader("Content-Type", "image/jpeg");
    if (extenstion == "png" || extenstion == "PNG")
        response.addHeader("Content-Type", "image/png");
    if (extenstion == "ttf" || extenstion == "TTF")
        response.addHeader("Content-Type", "font/ttf");
    if (extenstion == "mp4" || extenstion == "MP4")
        response.addHeader("Content-Type", "video/mp4");
}

std::string HttpServer::currentDate() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
    return ss.str();
}
