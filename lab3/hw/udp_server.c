/*UDP Echo Server*/
#include <stdio.h>          /* These are the usual header files */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>

// #define PORT 5550  /* Port that will be opened */ 
#define BUFF_SIZE 1024
#define BUFF_HALF 512

int check_port(char port[]){
	int serv_port = atoi(port);
	if(serv_port <= 0 || serv_port >= 70000){
		serv_port = -1;
		return serv_port;
	}
}

int is_new_client(int ports[], int count, int new_port){
	for (int i = 0; i < count; i++)
	{
		if(ports[i] == new_port){
			return 0;
		}else{
			continue;
		}
	}
	return 1;
}

int main(int argc, char** argv)
{
	int server_sock; /* file descriptors */
	char buff[BUFF_SIZE];
	int bytes_sent, bytes_received;
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	int sin_size;
	int port_num = 0;

	char num[BUFF_HALF];
	char alphabet[BUFF_HALF];
	//Step 1: Construct a UDP socket
	if ((server_sock=socket(AF_INET, SOCK_DGRAM, 0)) == -1 ){  /* calls socket() */
		perror("\nError: ");
		exit(0);
	}
	port_num = check_port(argv[1]);
	if(port_num <= 0){
		perror("Error: Invalid port number!\n");
		exit(0);
	}
	
	//Step 2: Bind address to socket
	server.sin_family = AF_INET;         
	server.sin_port = htons(port_num);   /* Remember htons() from "Conversions" section? =) */
	server.sin_addr.s_addr = INADDR_ANY;  /* INADDR_ANY puts your IP address automatically */   
	bzero(&(server.sin_zero),8); /* zero the rest of the structure */

	if(bind(server_sock,(struct sockaddr*)&server,sizeof(struct sockaddr))==-1){ /* calls bind() */
		perror("\nError: ");
		exit(0);
	}     

	int count = 0;
	int ports[2];
	//Step 3: Communicate with clients
	while(1){
		sin_size=sizeof(struct sockaddr_in);
		// use recvfrom because server does not know which client is sending the message
		// recvfrom allows server to know the address of sender
		bytes_received = recvfrom(server_sock, buff, BUFF_SIZE-1, 0, (struct sockaddr *) &client, &sin_size);
		buff[bytes_received] = '\0';
		
		if (bytes_received < 0)
			perror("\nError: ");
		else{
			if(strcmp(buff, "init") == 0){
				int is_new = is_new_client(ports, count, ntohs(client.sin_port));
				if(is_new == 1){
					ports[count++] = ntohs(client.sin_port);
				}
				if(count == 3){
					exit(0);
				}
			}else{
				int i_num = 0;
				int i_char = 0;
				int is_fault = 0;
				for (int i = 0; i < bytes_received - 1; i ++)
				{
					if(isdigit(buff[i])){
						num[i_num++] = buff[i];
					}else if(isalpha(buff[i])){
						alphabet[i_char++] = buff[i];
					}else{
						is_fault = 1;
						break;
					}
				}
				if(is_fault == 1){
					perror("Error: invalid input\n");
					exit(0);
				}else{
					num[i_num] = '\0';
					alphabet[i_char] = '\0';
				}
				printf("[%s:%d]: %s", inet_ntoa(client.sin_addr), ntohs(client.sin_port), buff);

				strcpy(buff, num);
				strcat(buff, "\n");
				strcat(buff, alphabet);

				int sender = ntohs(client.sin_port);
				int receiver = ports[0] == sender ? ports[1] : ports[0];
				client.sin_port = htons(receiver);
				bytes_sent = sendto(server_sock, buff, BUFF_HALF, 0, (struct sockaddr *) &client, sin_size ); /* send to the client welcome message */
				if (bytes_sent < 0)
					perror("\nError: ");
			}
		}
	}
	
	close(server_sock);
	return 0;
}
