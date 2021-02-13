
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <winsock.h>
#include <time.h>
#include <ctype.h>

#pragma comment(lib, "Ws2_32.lib")


#define PORT 3490                     // the port users will be connecting to
#define BUFFER_LENGTH 256
#define BACKLOG 10                	 // how many pending connections queue will hold

bool game_running(int player_socket , int secret_num) {
	char msg_less[] = "Number is smaller.";
	char msg_more[] = "Number is bigger.";         
	char msg_equal[] = "You guessed the number. Congratz.";
	char msg_wrong[] = "Wrong input.Try again.";


	char recv_buffer[BUFFER_LENGTH];
	char send_buffer[BUFFER_LENGTH];

	memset(&recv_buffer, 0, BUFFER_LENGTH);
	memset(&send_buffer, 0, BUFFER_LENGTH);

	int guessed_num;
	int send_len, recv_len;

	recv_len = recv(player_socket, recv_buffer, sizeof(recv_buffer), 0);       //server receives user's number
	printf(" Received number: %s\n", recv_buffer);
	

      if (recv_len == 0 || recv_len == -1)                           //ir empty buffer or error in recv - close client's socket
		return false;

		memset(&send_buffer, 0, BUFFER_LENGTH);
		

		guessed_num = -1;
		switch (recv_len - 1) {              //converting chars tp integers
		case 0:
			return false;
			break;
		case 1:
			if (isdigit(recv_buffer[0]))
				guessed_num = recv_buffer[0] - '0';
			break;
		case 2:
			if (isdigit(recv_buffer[0]) && isdigit(recv_buffer[1]))
				guessed_num = (recv_buffer[0] - '0') * 10 + (recv_buffer[1] - '0');
			break;
		case 3:
			if (isdigit(recv_buffer[0]) && isdigit(recv_buffer[1]) && isdigit(recv_buffer[2]))
				guessed_num = (recv_buffer[0] - '0') * 100 + (recv_buffer[1] - '0') * 10 + (recv_buffer[2] - '0');
			break;
		default:
			break;

		}

		if (guessed_num >= 0 && guessed_num <= 100)    //checks guesses and interacts with user(sends back data)
		{
			if (guessed_num > secret_num)
				send(player_socket, msg_less, sizeof(msg_less), 0);

			if (guessed_num < secret_num)
				send(player_socket, msg_more, sizeof(msg_more), 0);

			if (guessed_num == secret_num)
			{
				send(player_socket, msg_equal, sizeof(msg_equal), 0);
				return false;
			}

		}
		else
			send(player_socket, msg_wrong, sizeof(msg_wrong), 0);
		return true;

}
int main(int argc, char *argv[])
{
	WSADATA wsaData;
	WSAStartup(0x0202, &wsaData);

	SOCKET listening_socket, client_socket;           // listen on lsitening_socket, new connections will be accepted on client_socket socket
	struct sockaddr_in serv_addr, client_addr;         //server address, client address
	fd_set fdset;

	char send_buffer[BUFFER_LENGTH];
	char recv_buffer[BUFFER_LENGTH];

	int send_len, recv_len;                            //how many bytes were sent after calling send() 
	int max_sd;

	char message[] = "Connection established. \n";

	char msg[] = "Enter a number from 0 to 100. ";

	int connected_sockets[BACKLOG];                      //number of sockets connected
	int random_numbers[BACKLOG];                         //each socket has their own random number

	srand(time(NULL));

	for (int i = 0; i < BACKLOG; ++i) {
		connected_sockets[i] = -1;           //not checked all nums are 0
		random_numbers[i] = 0;
	}


	listening_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);          //creating listening socket
	if (listening_socket == -1) {
		perror("ERROR opening socket");
		exit(1);
	}
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;                                     //setting IPv4 or IPv6
	serv_addr.sin_port = htons(PORT);                                  //setting and converting port number to a sequence of bytes
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);                    //host ip

	if (bind(listening_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {   //binding server address, socket and port
		perror("ERROR in binding");
		exit(1);
	}


	if (listen(listening_socket, BACKLOG) == -1) {                              //listening for new connections on listening socket
		perror("ERROR in listen()");
		exit(1);
	}

	printf("server: waiting for connections...\n");

	while (1) {  // main accept() loop

		FD_ZERO(&fdset);                           //set of socket descriptors set to zero

		FD_SET(listening_socket, &fdset);              //adding listening socket to set, and choosing it as max value
		max_sd = listening_socket;   

		for (int i = 0; i < BACKLOG; ++i) {
			if (connected_sockets[i] != -1) {
				FD_SET(connected_sockets[i], &fdset);                  //adding other client sockets to this set

				if (connected_sockets[i] > max_sd) {
					max_sd = connected_sockets[i];                   //looking for the biggest one
				}
			}
		}

		if (select(max_sd + 1, &fdset, NULL, NULL, NULL) == -1) {        //selecting from multiple clients, helps monitor few at the same time
			perror("ERROR in select()");
			exit(1);
		}



		if (FD_ISSET(listening_socket, &fdset) != 0) {                 
			int client_addr_len = sizeof(client_addr);
			memset(&client_addr, 0, client_addr_len);
			if ((client_socket = accept(listening_socket, (struct sockaddr *)&client_addr, &client_addr_len)) != -1) {    //accepting new client through new socket
				for (int i = 0; i < BACKLOG; ++i) {
					if (connected_sockets[i] == -1) {
						connected_sockets[i] = client_socket;     //adding socket descriptor to array
						random_numbers[i] = rand() % 100 + 1;     //getting a random number for a client
						break;
					}
				}
				printf("Connected:  %s\n", inet_ntoa(client_addr.sin_addr));
				if( recv(client_socket, recv_buffer, sizeof(recv_buffer), 0) == -1)              
				
				if (send(client_socket, message, strlen(message), 0) != strlen(message))   
				{
					perror("ERROR in send()\n");
					exit(1);
				}
				
				if( send(client_socket, msg, sizeof(msg), 0) != strlen(msg))   //asking to guess a number
				     
			}
		}


		for (int i = 0; i < BACKLOG; i++) {
			if (connected_sockets[i] != -1) {
				if (FD_ISSET(connected_sockets[i], &fdset)) {        //if socket descriptor is in the set, continue the game
					
					if (!game_running(connected_sockets[i], random_numbers[i]))    //if no received data, failed to receive data or won the game - close socket
					{
						connected_sockets[i] = -1;
						random_numbers[i] = 0;
						FD_CLR(connected_sockets[i], &fdset);
						closesocket(connected_sockets[i]);
					}
					
				}
			}
		}
	}

     return 0;
}

