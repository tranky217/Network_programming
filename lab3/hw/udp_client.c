/*UDP Echo Client*/
#include <stdio.h>   
#include <stdlib.h>       /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

// #define SERV_PORT 5550
// #define SERV_IP "127.0.0.1"
#define BUFF_SIZE 1024

int check_ip(char ip_addr[]){
	struct sockaddr_in sa;
	int result = inet_pton(AF_INET, ip_addr, &(sa.sin_addr));
	return result; // 0 is invalid
}

int check_port(char port[]){
	int serv_port = atoi(port);
	if(serv_port <= 0 || serv_port >= 70000){
		serv_port = -1;
		return serv_port;
	}
}

int is_empty_str(char buffer[]){
	if(strcmp(buffer, "") == 0){
		return 1;
	}else{
		return 0;
	}
}
int main(int argc, char** argv){
	int client_sock;
	char buff[BUFF_SIZE];
	char init[] = "init";
	struct sockaddr_in server_addr, client_addr;
	int bytes_sent,bytes_received, sin_size;
	char serv_ip[16];
	int serv_port = 0;
	int client_port = 0;

	printf("Insert your string to get the message from the other one\n");

	if(argc <= 1){
		perror("Error: Not enough input!\n");
		exit(0);
	}
	// check for valid ip address
	if(check_ip(argv[1]) == 0){
		perror("Error: Invalid ip address!\n");
		exit(0);
	}else{
		strcpy(serv_ip, argv[1]);
	}
	// check for valid port number
	serv_port = check_port(argv[2]);
	if(serv_port == -1){
		perror("Error: Invalid port number!\n");
		exit(0);
	}

	//Step 1: Construct a UDP socket
	if ((client_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ){  /* calls socket() */
		perror("\nError: ");
		exit(0);
	}

	//Step 2: Define the address of the server
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(serv_port);
	server_addr.sin_addr.s_addr = inet_addr(serv_ip);

	sin_size = sizeof(struct sockaddr);
	sendto(client_sock, init, strlen(init), 0, (struct sockaddr *) &server_addr, sin_size);
	printf("Initializing...\n");
	while(1){
		//Step 3: Communicate with server
		printf("\nInsert string to send: ");
		memset(buff,'\0',(strlen(buff)+1));
		fgets(buff, BUFF_SIZE, stdin);
		
		if(is_empty_str(buff)){
			break;
		}
		
		bytes_sent = sendto(client_sock, buff, strlen(buff), 0, (struct sockaddr *) &server_addr, sin_size);
		if(bytes_sent < 0){
			perror("Error: ");
			close(client_sock);
			return 0;
		}

		bytes_received = recvfrom(client_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *) &server_addr, &sin_size);
		if(bytes_received < 0){
			perror("Error: ");
			close(client_sock);
			return 0;
		}
		buff[bytes_received] = '\0';
		printf("Reply from server:\n%s", buff);
	}
	close(client_sock);
	return 0;
}
