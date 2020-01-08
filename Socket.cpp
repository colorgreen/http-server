#include "Socket.h"
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

int Socket::send(const char* data, int length)
{
	return ::send(fd, data, length, NULL);
}

int Socket::recv(char* data, int length)
{
	return ::recv(fd, data, length, NULL);
}
