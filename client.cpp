#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <winsock.h>

#pragma comment(lib, "Ws2_32.lib")


#define PORT 3490  // the port users will be connecting to
#define BUFFER_LENGTH 256
#define HOST "127.0.0.1"


void errorHandling(char* err_name, int closingSocket) {
	perror(err_name);
	closesocket(closingSocket);
	printf("Disconnected.\n");
	WSACleanup();
	exit(1);
}


int main(int argc, char *argv[]) {
	WSADATA wsaData;
	WSAStartup(0x0202, &wsaData);

	SOCKET server_connect;
	struct sockaddr_in serv_addr;

	char send_buffer[BUFFER_LENGTH];
	char recv_buffer[BUFFER_LENGTH];

	int send_len, recv_len;

	server_connect = socket(AF_INET, SOCK_STREAM, 0);     //gets socket descriptor

	if (server_connect == INVALID_SOCKET) {
		errorHandling("Failed to get socket descriptor", server_connect);
	}

	memset(&serv_addr, 0, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;                //ipv4 ar ipv6
	serv_addr.sin_port = htons(PORT);                  //port or http
	serv_addr.sin_addr.s_addr = inet_addr(HOST);           //host ip

	if (connect(server_connect, (const sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {  //connecting to server
		perror("Failed to connect.");
		
	}

	printf("Please press any key and enter.\n");
    fgets(send_buffer, sizeof(send_buffer), stdin);
	memset(&recv_buffer, 0, BUFFER_LENGTH);
	send(server_connect, send_buffer, strlen(send_buffer), 0);
	printf("\n");

	if (recv(server_connect, recv_buffer, BUFFER_LENGTH, 0) == -1) { //informs that connected
		perror("ERROR in recv()");
		exit(1);
	}
	
	memset(&recv_buffer, 0, BUFFER_LENGTH);
	memset(&send_buffer, 0, BUFFER_LENGTH);

	if (recv(server_connect, recv_buffer, BUFFER_LENGTH, 0) == -1) {
		perror("ERROR in recv()");
		exit(1);
	}
	printf("%s\n", recv_buffer);
	

	while (1) {
		memset(&recv_buffer, 0, BUFFER_LENGTH);

       fgets(send_buffer, sizeof(send_buffer), stdin);
	
		if (send(server_connect, send_buffer, strlen(send_buffer), 0) == -1) {
			perror("ERROR in send()");
			exit(1);
		}

		if (recv(server_connect, recv_buffer, BUFFER_LENGTH, 0) == -1) {
			perror("ERROR in recv()");
			exit(1);
		}
		printf("%s\n", recv_buffer);
		
		

		if (recv_buffer[0] == 'Y')  //this is so wrong >.<
			break;

		memset(&recv_buffer, 0, BUFFER_LENGTH);
		memset(&send_buffer, 0, BUFFER_LENGTH);

	}

	closesocket(server_connect);
	printf("Disconnected.\n");
	WSACleanup();
	return 0;
}