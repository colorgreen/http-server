#pragma once
#include <string>
#include <map>

class HttpResponse
{
	friend class HttpServer;
	
	int statusCode;
	std::string version;
	std::map<std::string, std::string> headers;
	std::map<int, std::string> codeDescrtiptions = {
            {100,"Continue"},
            {101,"Switching Protocols"},
            {110,"Connection, Timed, Out"},
            {111,"Connection, refused"},
            {200,"OK"},
            {201,"Created"},
            {202,"Accepted"},
            {203,"Non-Authoritative, Information"},
            {204,"No, content"},
            {205,"Reset, Content"},
            {206,"Partial, Content"},
            {300,"Multiple, Choices"},
            {301,"Moved, Permanently"},
            {302,"Found"},
            {303,"See, Other"},
            {304,"Not, Modified"},
            {305,"Use, Proxy"},
            {306,"Switch, Proxy"},
            {307,"Temporary, Redirect"},
            {310,"Too, many, redirects"},
            {400,"Bad, Request"},
            {401,"Unauthorized"},
            {402,"Payment, Required"},
            {403,"Forbidden"},
            {404,"Not, Found"},
            {405,"Method, Not, Allowed"},
            {406,"Not, Acceptable"},
            {407,"Proxy, Authentication, Required"},
            {408,"Request, Timeout"},
            {409,"Conflict"},
            {410,"Gone"},
            {411,"Length, required"},
            {412,"Precondition, Failed"},
            {413,"Request, Entity, Too, Large"},
            {414,"Request-URI, Too, Long"},
            {415,"Unsupported, Media, Type"},
            {416,"Requested, Range, Not, Satisfiable"},
            {417,"Expectation, Failed"},
            {418,"I’m, a, teapot"},
            {422,"Unprocessable, entity"},
            {451,"Unavailable, For, Legal, Reasons"},
            {500,"Internal, Server, Error"},
            {501,"Not, Implemented"},
            {502,"Bad, Gateway"},
            {503,"Service, Unavailable"},
            {504,"Gateway, Timeout"},
            {505,"HTTP, Version, Not, Supported"},
            {506,"Variant, Also, Negotiates"},
            {507,"Insufficient, Storage, (WebDAV)"},
            {508,"Loop, Detected, (WebDAV)"},
            {509,"Bandwidth, Limit, Exceeded"},
            {510,"Not, Extended"},
            {511,"Network, Authentication, Required"}};

public:
	void setStatusCode(int code);
	void addHeader(std::string key, int value);
	void addHeader(std::string key, std::string value);

	std::string getHead() const;
};

