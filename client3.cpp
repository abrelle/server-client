#ifdef _WIN32
#include <winsock.h>
#define socklen_t int
#pragma comment(lib, "Ws2_32.lib")
#else
#define INVALID_SOCKET -1
#define SOCKET int
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>


#define PORT 3490  // the port users will be connecting to
#define BUFFER_LENGTH 256
#define HOST "127.0.0.1"


void errorHandling(const char* err_name, int closingSocket) {
	
	perror(err_name);
	#ifdef _WIN32
		WSACleanup();
		closesocket(closingSocket);
	#else
		close(closingSocket);
	#endif	
	printf("Disconnected.\n");
	exit(1);
}


int main(int argc, char *argv[]) {
	
	#ifdef _WIN32
		WSADATA wsaData;
		WSAStartup(0x0202, &wsaData);
	#endif	

	SOCKET server_connect;
	struct sockaddr_in serv_addr;

	char send_buffer[BUFFER_LENGTH];
	char recv_buffer[BUFFER_LENGTH];

	int send_len, recv_len;

	server_connect = socket(AF_INET, SOCK_STREAM, 0);     //gets socket descriptor

	if (server_connect == INVALID_SOCKET)
		errorHandling("Failed to get socket descriptor.", server_connect);

	memset(&serv_addr, 0, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;                //ipv4 ar ipv6
	serv_addr.sin_port = htons(PORT);                  //port or http
	serv_addr.sin_addr.s_addr = inet_addr(HOST);           //host ip

	if (connect(server_connect, (const sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)       //connecting to server
		errorHandling("Failed to connect to server.", server_connect);

	printf("Please press any key and enter.\n\n");
	fgets(send_buffer, sizeof(send_buffer), stdin);
	memset(&recv_buffer, 0, BUFFER_LENGTH);


	if (send(server_connect, send_buffer, strlen(send_buffer), 0) != strlen(send_buffer))
		errorHandling("Failed to sent a message.", server_connect);

	if (recv(server_connect, recv_buffer, BUFFER_LENGTH, 0) == -1)  //informs that connected
		errorHandling("Error in rev()", server_connect);

	memset(&recv_buffer, 0, BUFFER_LENGTH);
	memset(&send_buffer, 0, BUFFER_LENGTH);

	if (recv(server_connect, recv_buffer, BUFFER_LENGTH, 0) == -1) 
		errorHandling("Error in rev()", server_connect);


	printf("%s\n", recv_buffer);


	while (1) {
		memset(&recv_buffer, 0, BUFFER_LENGTH);

		fgets(send_buffer, sizeof(send_buffer), stdin);

		if (send(server_connect, send_buffer, strlen(send_buffer), 0) != strlen(send_buffer)) 
			errorHandling("Failed to send a message.", server_connect);
		

		if (recv(server_connect, recv_buffer, BUFFER_LENGTH, 0) == -1) 
			errorHandling("Error in rev()", server_connect);
		
		printf("%s\n", recv_buffer);



		if (recv_buffer[0] == 'Y')  //this is so wrong >.<
			break;

		memset(&recv_buffer, 0, BUFFER_LENGTH);
		memset(&send_buffer, 0, BUFFER_LENGTH);

	}
   
	#ifdef _WIN32
		WSACleanup();
		closesocket(server_connect);
	#else
		close(server_connect);
	#endif
	
	printf("Disconnected.\n");
	return 0;
}