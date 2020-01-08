CC=g++
CFLAGS=-lpthread
DEPS = Socket.h HttpException.h  HttpResponse.h  HttpServer.h
OBJ = Socket.o HttpException.o HttpResponse.o HttpServer.o threadedServer.cpp

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
