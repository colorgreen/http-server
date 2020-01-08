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

//struktura zawierająca dane, które zostaną przekazane do wątku
struct thread_data_t
{
	int socket;
};

//funkcja opisującą zachowanie wątku - musi przyjmować argument typu (void *) i zwracać (void *)
void *ThreadBehavior(void *t_data)
{
    pthread_detach(pthread_self());
    struct thread_data_t *data = (struct thread_data_t*)t_data;
    //dostęp do pól struktury: (*th_data).pole
    //TODO (przy zadaniu 1) klawiatura -> wysyłanie albo odbieranie -> wyświetlanie

	Socket s(data->socket);
	HttpServer server(s);

	printf("Wysylam odpowiedz\n");
	
//char response[] = "HTTP/1.1 200 OK\n"
//"Content-Type: text/html; charset=utf-8\n"				
//"\r\ncoklwiek"	;
//	write(data->socket, response, strlen(response));
//	close(data->socket);
printf("Zamykam polaczenie\n");

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
       printf("Błąd przy próbie utworzenia wątku, kod błędu: %d\n", create_result);
       exit(-1);
    }

    //TODO (przy zadaniu 1) odbieranie -> wyświetlanie albo klawiatura -> wysyłanie
}

int main(int argc, char* argv[])
{
   int server_socket_descriptor;
   int clientSocket;
   int bind_result;
   int listen_result;
   char reuse_addr_val = 1;
   struct sockaddr_in server_address;

   //inicjalizacja gniazda serwera
   
   memset(&server_address, 0, sizeof(struct sockaddr));
   server_address.sin_family = AF_INET;
   server_address.sin_addr.s_addr = htonl(INADDR_ANY);
   server_address.sin_port = htons(SERVER_PORT);

   server_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
   if (server_socket_descriptor < 0)
   {
       fprintf(stderr, "%s: Błąd przy próbie utworzenia gniazda..\n", argv[0]);
       exit(1);
   }
   setsockopt(server_socket_descriptor, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse_addr_val, sizeof(reuse_addr_val));

   bind_result = bind(server_socket_descriptor, (struct sockaddr*)&server_address, sizeof(struct sockaddr));
   if (bind_result < 0)
   {
       fprintf(stderr, "%s: Błąd przy próbie dowiązania adresu IP i numeru portu do gniazda.\n", argv[0]);
       exit(1);
   }

   listen_result = listen(server_socket_descriptor, QUEUE_SIZE);
   if (listen_result < 0) {
       fprintf(stderr, "%s: Błąd przy próbie ustawienia wielkości kolejki.\n", argv[0]);
       exit(1);
   }

	printf("Rozpoczecie serwera\n");

   while(1)
   {
	struct sockaddr_in sa;
	socklen_t sa_size;

       clientSocket = accept(server_socket_descriptor,  (struct sockaddr *) &sa, &sa_size);
       if (clientSocket < 0)
       {
           fprintf(stderr, "%s: Błąd przy próbie utworzenia gniazda dla połączenia.\n", argv[0]);
           exit(1);
       }
	printf("New connection from %s:%d\n", inet_ntoa(sa.sin_addr), sa.sin_port);
       handleConnection(clientSocket);
   }

   close(server_socket_descriptor);
   return(0);
}
