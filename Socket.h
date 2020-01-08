#pragma once
class Socket
{
	int fd;

public:
	Socket(int fd) : fd(fd) {};
	
	int send(const char* data, int length);

	int recv(char* data, int length);

	void close();
};
