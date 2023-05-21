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

// #define PORT 5550   /* Port that will be opened */
#define BACKLOG 20 /* Number of allowed connections */
#define BUFF_SIZE 1024

/* Receive and echo message to client */
void *echo(void *);
int check_port(char port[]);
int checkin(char username[], char password[]);
int checkout();

char *filename = "account.txt";
node *head = NULL;
char acc_name[50];

int main(int argc, char **argv)
{
	int listenfd, *connfd;
	struct sockaddr_in server;	/* server's address information */
	struct sockaddr_in *client; /* client's address information */
	int sin_size;
	pthread_t tid;

	head = init_account(filename);

	strcpy(acc_name, "none");
	// check for port and ip addr
	int port = 0;
	port = check_port(argv[1]);
	if (port <= 0)
	{
		perror("Error: Invalid port number!\n");
		exit(0);
	}

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{ /* calls socket() */
		perror("\nError: ");
		return 0;
	}
	// config and bind server addr
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = htonl(INADDR_ANY); /* INADDR_ANY puts your IP address automatically */

	if (bind(listenfd, (struct sockaddr *)&server, sizeof(server)) == -1)
	{
		perror("\nError: ");
		return 0;
	}

	if (listen(listenfd, BACKLOG) == -1)
	{
		perror("\nError: ");
		return 0;
	}

	sin_size = sizeof(struct sockaddr_in);
	client = malloc(sin_size);
	while (1)
	{
		connfd = malloc(sizeof(int));
		if ((*connfd = accept(listenfd, (struct sockaddr *)client, &sin_size)) == -1)
			perror("\nError: ");

		printf("You got a connection from %s\n", inet_ntoa((*client).sin_addr)); /* prints client's IP */

		/* For each client, spawns a thread, and the thread handles the new client */
		pthread_create(&tid, NULL, &echo, connfd);
	}

	close(listenfd);
	return 0;
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

void *echo(void *arg)
{
	int connfd;
	int bytes_sent, bytes_received;
	char buff[BUFF_SIZE + 1];
	char username[50], password[50];

	connfd = *((int *)arg);
	free(arg);
	pthread_detach(pthread_self());
	// polling for client messages
	while (1)
	{
		bytes_received = recv(connfd, buff, BUFF_SIZE, 0); // blocking
		buff[bytes_received] = '\0';

		if (bytes_received < 0)
			perror("\nError: ");
		else if (bytes_received == 0)
			printf("Connection closed.");

		if (strcmp(buff, "1") == 0)
		{
			// call checkout
			int is_checkout = checkout();
			printf("%d", is_checkout);
			strcpy(buff, "Logout successfully");
		}
		else
		{
			// separate the username and password
			char *token = strtok(buff, " ");
			strcpy(username, token);
			token = strtok(NULL, " ");
			strcpy(password, token);

			int result = checkin(username, password);
			if (result == 0)
			{
				strcpy(buff, "Login successfully");
				strcpy(acc_name, username);
			}
			else if (result == 1)
			{
				strcpy(buff, "Incorrect password");
			}
			else if (result == -1)
			{
				strcpy(buff, "Not found account");
			}
			else if (result == 2)
			{
				strcpy(buff, "Account is blocked");
			}
			else if (result == 3)
			{
				strcpy(buff, "Account is blocked");
				update_file(head);
			}
		}
		bytes_sent = send(connfd, buff, strlen(buff), 0); /* send to the client welcome message */
			if (bytes_sent < 0)
			{
				perror("\nError: ");

				close(connfd);
			}
	}
}

int checkin(char username[], char password[])
{
	// read account list from account.txt file
	printf("Account list read from file\n");
	display(head);

	account new_acc;
	strcpy(new_acc.username, username);
	strcpy(new_acc.password, password);

	node *cur = search(head, username);

	if (cur != NULL)
	{
		// check if that account already existed
		if ((cur->data).status == 1)
		{
			if (strcmp(new_acc.password, (cur->data).password) == 0)
			{
				if ((cur->data).is_signin == 1)
					printf("Account already signed in\n");
				else
				{
					(cur->data).is_signin = 1;				// is_signin = 1 mean user is logged in
					printf("Hello %s\n", new_acc.username); // login successfully
				}
				return 0; // successful
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

					return 3; // update file
				}
				else
				{
					return 1; // no need to update account file
				}
			}
		}
		else
		{
			printf("Account is blocked\n");
			return 2; // no need to update account file
		}
	}
	else
	{
		printf("Not found!");
		return -1;
	}
}

int checkout()
{
	node *cur = search(head, acc_name);

	if (cur != NULL)
	{
		if ((cur->data).is_signin == 1)
		{ // check if an account is logged in or not
			printf("Goodbye %s\n", (cur->data).username);
			(cur->data).is_signin = 0;
			strcpy(acc_name, "none");
			return 0;
		}
		else if ((cur->data).is_signin == 0)
		{
			printf("Account had not sign in\n");
			return 0;
		}
	}
	else
	{
		printf("Can not find account\n");
		return -1;
	}
}