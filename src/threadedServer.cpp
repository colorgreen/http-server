#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include "Socket.h"
#include "HttpServer.h"
#include "HttpException.h"

#define SERVER_PORT 8080
#define QUEUE_SIZE 5
#define BUFFSIZE 2048

struct thread_data_t
{
	int socket;
};

void *ThreadBehavior(void *t_data)
{
    pthread_detach(pthread_self());
    struct thread_data_t *data = (struct thread_data_t*)t_data;
	Socket s(data->socket);
	HttpServer server(s);
    server.stop();
    printf("Connection closed\n");

	delete data;
    pthread_exit(NULL);
}


void handleConnection(int clientSocket) {

    int create_result = 0;
    pthread_t thread1;

	thread_data_t * data = new thread_data_t();
	data->socket = clientSocket;
    create_result = pthread_create(&thread1, NULL, ThreadBehavior, (void *)data);
    if (create_result){
       printf("Error on creating new thread: %d\n", create_result);
       exit(-1);
    }
}

int main(int argc, char* argv[])
{
   int server_socket_descriptor;
   int bind_result;
   int listen_result;
   char reuse_addr_val = 1;
   struct sockaddr_in server_address;

   memset(&server_address, 0, sizeof(struct sockaddr));
   server_address.sin_family = AF_INET;
   server_address.sin_addr.s_addr = htonl(INADDR_ANY);
   server_address.sin_port = htons(SERVER_PORT);

   server_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
   if (server_socket_descriptor < 0)
   {
       fprintf(stderr, "%s: Error on main socket.\n", argv[0]);
       exit(1);
   }
   setsockopt(server_socket_descriptor, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse_addr_val, sizeof(reuse_addr_val));

   bind_result = bind(server_socket_descriptor, (struct sockaddr*)&server_address, sizeof(struct sockaddr));
   if (bind_result < 0)
   {
       fprintf(stderr, "%s: Error on binding IP or PORT.\n", argv[0]);
       exit(1);
   }

   listen_result = listen(server_socket_descriptor, QUEUE_SIZE);
   if (listen_result < 0) {
       fprintf(stderr, "%s: Error on setting queue size.\n", argv[0]);
       exit(1);
   }

	printf("Starting server\n");

   while(1)
   {
	struct sockaddr_in sa;
	socklen_t sa_size;

       int clientSocket = accept(server_socket_descriptor,  (struct sockaddr *) &sa, &sa_size);
       if (clientSocket < 0)
       {
           fprintf(stderr, "%s: Error on acceping new connection.\n", argv[0]);
           exit(1);
       }
	    printf("New connection from %s:%d\n", inet_ntoa(sa.sin_addr), sa.sin_port);
       handleConnection(clientSocket);
   }

   close(server_socket_descriptor);
   return(0);
}
