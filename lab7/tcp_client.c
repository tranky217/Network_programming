#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

// #define SERVER_ADDR "127.0.0.1"
// #define SERVER_PORT 5550
#define BUFF_SIZE 1024

int check_ip(char ip_addr[]);
int check_port(char port[]);

char acc_name[50];

int main(int argc, char **argv)
{
	int client_sock;
	char buff[BUFF_SIZE + 1];
	struct sockaddr_in server_addr; /* server's address information */
	int user_len, pw_len, bytes_sent, bytes_received, msg_len;
	char username[50];
	char password[50];
	int is_login = 0;
	int is_logout = 0;

	strcpy(acc_name, "none");
	// custom server ip and port number
	char serv_ip[16];
	int serv_port = 0;
	// check for server ip and port number
	if (argc <= 1)
	{
		perror("Error: Not enough input!\n");
		exit(0);
	}
	// check for valid ip address
	if (check_ip(argv[1]) == 0)
	{
		perror("Error: Invalid ip address!\n");
		exit(0);
	}
	else
	{
		strcpy(serv_ip, argv[1]);
	}
	// check for valid port
	serv_port = check_port(argv[2]);
	if (serv_port == -1)
	{
		perror("Error: Invalid port number!\n");
		exit(0);
	}

	// Step 1: Construct socket
	client_sock = socket(AF_INET, SOCK_STREAM, 0);

	// Step 2: Specify server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(serv_port);
	server_addr.sin_addr.s_addr = inet_addr(serv_ip);

	// Step 3: Request to connect server
	if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
	{
		printf("\nError!Can not connect to sever! Client exit imediately! ");
		return 0;
	}

	// Step 4: Communicate with server
	while (1)
	{
		if (is_login == 0)
		{
			// send message
			printf("\nInsert username and password to login: \n");
			printf("Username: ");
			memset(username, '\0', (strlen(username) + 1));
			fgets(username, 50, stdin);
			user_len = strlen(username);
			username[user_len - 1] = '\0';

			if(is_logout == 1){
				if(strcmp(username, acc_name) != 0){
					printf("Can not login to another account\n");
					continue;
				}
			}
			
			printf("Password: ");
			memset(password, '\0', (strlen(password) + 1));
			fgets(password, 50, stdin);
			pw_len = strlen(password);
			password[pw_len - 1] = '\0';

			sprintf(buff, "%s %s", username, password);
			msg_len = user_len + pw_len;
		}
		else{
			printf("\nPress anything to logout: \n");
			memset(buff, '\0', (strlen(buff) + 1));
			fgets(buff, BUFF_SIZE, stdin);

			msg_len = strlen(buff);
			buff[msg_len - 1] = '\0';

			strcpy(buff, "1");
		}

		bytes_sent = send(client_sock, buff, msg_len, 0);
		if (bytes_sent < 0)
			perror("\nError: ");

		// receive echo reply
		bytes_received = recv(client_sock, buff, BUFF_SIZE, 0);
		if (bytes_received < 0)
			perror("\nError: ");
		else if (bytes_received == 0)
			printf("Connection closed.\n");

		buff[bytes_received] = '\0';
		printf("Reply from server: %s", buff);
		if(strcmp(buff, "Login successfully") == 0){
			is_login = 1;
			strcpy(acc_name, username);
		}
		else if (strcmp(buff, "Logout successfully") == 0)
		{
			is_login = 0;
			is_logout = 1;
		}
	}

	// Step 4: Close socket
	close(client_sock);
	return 0;
}

int check_ip(char ip_addr[])
{
	struct sockaddr_in sa;
	int result = inet_pton(AF_INET, ip_addr, &(sa.sin_addr));
	return result; // 0 is invalid
}

int check_port(char port[])
{
	int serv_port = atoi(port);
	if (serv_port <= 0 || serv_port >= 70000)
	{
		serv_port = -1;
		return serv_port;
	}
}