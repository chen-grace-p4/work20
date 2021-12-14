#include "pipe_networking.h"


/*=========================
  server_handshake
  args: int * to_client
  Performs the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.
  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
  int from_client = 0;
	//server creates WKP and waits for connection
  mkfifo(WKP, 0644);
	printf("created well known pipe\n");

  from_client = open(WKP, O_RDONLY);
	printf("opened well known pipe for reading\n");
	//server recieves message from client
  char message[100];
  read(from_client, message, sizeof(message));
  message[strlen(message)] = 0;
	printf("read well known pipe\n");
	//server removes wkp
  remove(WKP);
	printf("removed well known pipe\n");

  *to_client = open(message, O_WRONLY);
	printf("opened secret pipe %s\n", message);
	//server sents message to client
  write(*to_client, ACK, sizeof(ACK));
	printf("sent ACK\n");
	//server recieves final verification message
  read(from_client, message, sizeof(message));
  printf("got final message\n");

  return from_client;
}


/*=========================
  client_handshake
  args: int * to_server
  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.
  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
  int from_server = 0;

	//client creates secret pipe
	char pid[100];
  sprintf(pid, "%d", getpid());
  from_server = mkfifo(pid, 0644);
	printf("created secret pipe: %s\n", pid);
  
  *to_server = open(WKP, O_WRONLY);
	printf("opened well known pipe\n");
	//client sends message to server
  write(*to_server, pid, strlen(pid));
	printf("sent secret pipe message\n");

  from_server = open(pid, O_RDONLY);
	printf("opened secret pipe\n");
	//client recieves response
  char message[100];
  read(from_server, message, sizeof(message));
	printf("recieved ACK\n");
	//client removes secret pipe
  remove(pid);
	printf("removed secret pipe\n");
	//client sends final verification message
  write(*to_server, ACK, sizeof(ACK));
	printf("sent final ACK\n");

  return from_server;
}
