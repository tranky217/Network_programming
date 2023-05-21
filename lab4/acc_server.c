/*UDP Echo Server*/
#include <stdio.h> /* These are the usual header files */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>
#include "acc.h"

// #define PORT 5550  /* Port that will be opened */
#define BUFF_SIZE 100
// #define BUFF_SIZE 50

int check_port(char port[])
{
	int serv_port = atoi(port);
	if (serv_port <= 0 || serv_port >= 70000)
	{
		serv_port = -1;
		return serv_port;
	}
}

int is_new_client(int ports[], int count, int new_port)
{
	for (int i = 0; i < count; i++)
	{
		if (ports[i] == new_port)
		{
			return 0;
		}
		else
		{
			continue;
		}
	}
	return 1;
}

int handle_login(int server_sock, struct sockaddr_in client, int sin_size, node *head, char username[])
{
	while (1)
	{
		sin_size = sizeof(struct sockaddr_in);
		char buff[BUFF_SIZE];
		int bytes_received = recvfrom(server_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *)&client, &sin_size);
		buff[bytes_received] = '\0';
		if (bytes_received < 0)
			perror("\nError: ");

		// take username and password to check
		account new_acc;
		char *token = strtok(buff, " ");
		strcpy(new_acc.username, token);
		printf("cur_acc: %s\n", username);
		if (strcmp(new_acc.username, username) == 0)
		{
			printf("Account already signed in\n");
			char res[] = "Account already signed in";
			int bytes_sent = sendto(server_sock, res, strlen(res), 0, (struct sockaddr *)&client, sin_size); /* send to the client welcome message */
			if (bytes_sent < 0)
				perror("\nError: ");
			return 1;
		}

		token = strtok(NULL, " ");
		strcpy(new_acc.password, token);
		node *cur = search(head, new_acc.username);

		if (cur != NULL)
		{ // check if that account already existed
			if ((cur->data).status == 1)
			{
				if (strcmp(new_acc.password, (cur->data).password) == 0)
				{
					if ((cur->data).is_signin == 1)
						printf("Account already signed in\n");
					(cur->data).is_signin = 1;				// is_signin = 1 mean user is logged in
					printf("Hello %s\n", new_acc.username); // login successfully
					strcpy(username, new_acc.username);

					char ok[] = "OK";
					int bytes_sent = sendto(server_sock, ok, strlen(ok), 0, (struct sockaddr *)&client, sin_size); /* send to the client welcome message */
					if (bytes_sent < 0)
						perror("\nError: ");
					return 1;
				}
				else
				{
					(cur->data).incorrect += 1; // increasing the number of times user type in incorrect password
					printf("Password is incorrect\n");

					if ((cur->data).incorrect >= 3)
					{ // each account has limited number of times for trying - here is 3
						(cur->data).status = 0;
						printf("review acc list:\n");
						display(head);

						printf("Account is blocked\n");
						char not_ok[] = "Account is blocked";
						int bytes_sent = sendto(server_sock, not_ok, strlen(not_ok), 0, (struct sockaddr *)&client, sin_size); /* send to the client welcome message */
						if (bytes_sent < 0)
							perror("\nError: ");
						return 2; // update file
					}
					else
					{
						char not_ok[] = "Not OK";
						int bytes_sent = sendto(server_sock, not_ok, strlen(not_ok), 0, (struct sockaddr *)&client, sin_size); /* send to the client welcome message */
						if (bytes_sent < 0)
							perror("\nError: ");
						break;
					}
				}
			}
			else
			{
				printf("Account is blocked\n");
				char not_ok[] = "Account is blocked";
				int bytes_sent = sendto(server_sock, not_ok, strlen(not_ok), 0, (struct sockaddr *)&client, sin_size); /* send to the client welcome message */
				if (bytes_sent < 0)
					perror("\nError: ");
				break;
			}
		}
		else
		{
			printf("Not found\n");
			char not_ok[] = "Not found";
			int bytes_sent = sendto(server_sock, not_ok, strlen(not_ok), 0, (struct sockaddr *)&client, sin_size); /* send to the client welcome message */
			if (bytes_sent < 0)
				perror("\nError: ");
			break;
		}
	}
	return 0;
}

int handle_logout(int server_sock, struct sockaddr_in client, int sin_size, node *head, char username[])
{
	node *cur2 = search(head, username);
	if (cur2 != NULL)
	{
		if ((cur2->data).is_signin == 1)
		{ // check if an account is logged in or not
			printf("Goodbye %s\n", (cur2->data).username);
			(cur2->data).is_signin = 0;
		}
		else if ((cur2->data).is_signin == 0)
		{
			printf("Account had not sign in\n");
			return 0;
		}
	}
	else
	{
		printf("Can not find account\n");
	}
	strcpy(username, " ");
	char ok[] = "OK";
	int bytes_sent = sendto(server_sock, ok, strlen(ok), 0, (struct sockaddr *)&client, sin_size); /* send to the client welcome message */
	if (bytes_sent < 0)
		perror("\nError: ");
	return 1;
}

int handle_password(int server_sock, struct sockaddr_in client, int sin_size, node *head, char username[])
{
	char buff[BUFF_SIZE];
	char num[BUFF_SIZE / 2];
	char alphabet[BUFF_SIZE / 2];
	sin_size = sizeof(struct sockaddr_in);

	// recieved new password from client
	int bytes_received;
	while (1)
	{
		bytes_received = recvfrom(server_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *)&client, &sin_size);
		if(bytes_received >= 0)
			break;
	}
	buff[bytes_received] = '\0';

	char original[strlen(buff)];
	strcpy(original, buff);

	if (bytes_received < 0)
		perror("\nError: ");
	int i_num = 0;
	int i_char = 0;
	int is_fault = 0;

	for (int i = 0; i < bytes_received; i++)
	{
		if (isdigit(buff[i]))
		{
			num[i_num++] = buff[i];
		}
		else if (isalpha(buff[i]))
		{
			alphabet[i_char++] = buff[i];
		}
		else
		{
			is_fault = 1;
			break;
		}
	}
	if (is_fault == 1)
	{
		char buff[BUFF_SIZE] = "invalid password";
		int bytes_sent = sendto(server_sock, buff, strlen(buff), 0, (struct sockaddr *)&client, sin_size);
		if (bytes_sent < 0)
			perror("\nError: ");
		printf("Error: invalid password\n");
		return 0;
	}
	else
	{
		num[i_num] = '\0';
		alphabet[i_char] = '\0';
	}
	if (i_num > 0 && i_char > 0)
	{
		sprintf(buff, "%s %s", num, alphabet);
	}
	else
	{
		if (i_num == 0)
			sprintf(buff, "%s", alphabet);
		if (alphabet == 0)
			sprintf(buff, "%s", num);
	}
	printf("encrypted password: %s\n", buff);

	int bytes_sent = sendto(server_sock, buff, strlen(buff), 0, (struct sockaddr *)&client, sin_size);
	if (bytes_sent < 0)
		perror("\nError: ");
	// update password to the list
	node *cur1 = search(head, username); // search for username in linked list
	if (cur1 != NULL)
	{
		strcpy((cur1->data).password, original);
		return 1;
	}
	else
	{
		printf("Can not find account\n");
	}
	return 0;
}

int main(int argc, char **argv)
{
	// read accounts from files
	char *filename = "account.txt";
	node *head = init_account(filename);
	printf("Account list read from file\n");
	display(head);

	int server_sock; /* file descriptors */
	char buff[BUFF_SIZE];
	int bytes_sent, bytes_received;
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	int sin_size;
	int port_num = 0;

	// Step 1: Construct a UDP socket
	if ((server_sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{ /* calls socket() */
		perror("\nError: ");
		exit(0);
	}
	port_num = check_port(argv[1]);
	if (port_num <= 0)
	{
		perror("Error: Invalid port number!\n");
		exit(0);
	}

	// Step 2: Bind address to socket
	server.sin_family = AF_INET;
	server.sin_port = htons(port_num);	 /* Remember htons() from "Conversions" section? =) */
	server.sin_addr.s_addr = INADDR_ANY; /* INADDR_ANY puts your IP address automatically */
	bzero(&(server.sin_zero), 8);		 /* zero the rest of the structure */

	if (bind(server_sock, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
	{ /* calls bind() */
		perror("\nError: ");
		exit(0);
	}

	account cur_acc[2];
	int count = 0;
	int ports[2] = {0, 0};
	char username[50];
	// Step 3: Communicate with clients
	while (1)
	{
		sin_size = sizeof(struct sockaddr_in);
		// use recvfrom because server does not know which client is sending the message
		// recvfrom allows server to know the address of sender
		bytes_received = recvfrom(server_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *)&client, &sin_size);
		buff[bytes_received] = '\0';

		if (bytes_received < 0)
			perror("\nError: ");
		else
		{
			if (strcmp(buff, "init") == 0)
			{
				int is_new = is_new_client(ports, count, ntohs(client.sin_port));
				if (is_new == 1)
				{
					ports[count++] = ntohs(client.sin_port);
					strcpy(cur_acc[count - 1].username, " ");
					cur_acc[count - 1].is_signin = 0;
					strcpy(cur_acc[count - 1].password, " ");
				}
				if (count == 3)
				{
					exit(0);
				}
			}
			else if (strcmp(buff, "login") == 0)
			{
				int res = handle_login(server_sock, client, sin_size, head, username);
				if (res == 1)
				{
					int sender = ntohs(client.sin_port);
					int corresponding_acc = ports[0] == sender ? 0 : 1;
					
					strcpy(cur_acc[corresponding_acc].username, username);
					cur_acc[corresponding_acc].is_signin = 1;
				}else if(res == 2){
					// update to file
					update_file(head);
				}
			}
			else if (strcmp(buff, "logout") == 0)
			{
				int sender = ntohs(client.sin_port);
				int corresponding_acc = ports[0] == sender ? 0 : 1;

				strcpy(username, cur_acc[corresponding_acc].username);
				int res = handle_logout(server_sock, client, sin_size, head, username);
			}
			else if (strcmp(buff, "password") == 0)
			{
				int sender = ntohs(client.sin_port);
				int corresponding_acc = ports[0] == sender ? 0 : 1;
				strcpy(username, cur_acc[corresponding_acc].username);

				int res = handle_password(server_sock, client, sin_size, head, username);
				if(res == 1){
					update_file(head);
				}
			}
			else
			{
				printf("[%s:%d]: %s", inet_ntoa(client.sin_addr), ntohs(client.sin_port), buff);
				if (count == 2)
				{
					int sender = ntohs(client.sin_port);
					int receiver = ports[0] == sender ? ports[1] : ports[0];
					int corresponding_acc = ports[0] == sender ? 0 : 1;
					client.sin_port = htons(receiver);

					char str[strlen(buff)];
					strcpy(str, buff);

					sprintf(buff,"Message from:\n%s: %s\n", cur_acc[corresponding_acc].username, str);
					bytes_sent = sendto(server_sock, buff, BUFF_SIZE, 0, (struct sockaddr *)&client, sin_size); /* send to the client welcome message */
					if (bytes_sent < 0)
						perror("\nError: ");
				}
				else
				{
					int bytes_sent = sendto(server_sock, buff, BUFF_SIZE, 0, (struct sockaddr *)&client, sin_size); /* send to the client welcome message */
					if (bytes_sent < 0)
						perror("\nError: ");
				}
			}
		}
	}

	close(server_sock);
	return 0;
}
