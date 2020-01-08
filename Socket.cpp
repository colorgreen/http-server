#include "Socket.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
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
