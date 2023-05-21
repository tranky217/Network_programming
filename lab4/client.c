/*UDP Echo Client*/
#include <stdio.h>
#include <stdlib.h> /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "acc.h"

// #define SERV_PORT 5550
// #define SERV_IP "127.0.0.1"
#define BUFF_SIZE 100

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

int is_empty_str(char buffer[])
{
    if (strcmp(buffer, "") == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int login_request(int client_sock, struct sockaddr_in server_addr, int sin_size)
{
    char buff[] = "login";
    int sent = sendto(client_sock, buff, strlen(buff), 0, (struct sockaddr *)&server_addr, sin_size);
    if (sent < 0)
    {
        return 0; // false - cannot request to login
    }
    else
    {
        printf("Enter your account info: \n");
        while (1)
        {
            char username[50];
            char password[50];
            int bytes_sent, bytes_received;
            printf("Username: ");
            scanf("%s", username);
            printf("Password: ");
            scanf("%s", password);

            char buff[100];
            sprintf(buff, "%s %s", username, password);

            bytes_sent = sendto(client_sock, buff, strlen(buff), 0, (struct sockaddr *)&server_addr, sin_size);
            if (bytes_sent < 0)
            {
                perror("Error: ");
                close(client_sock);
                return 0;
            }
            // check message sent back from server if it signout or anything ?
            bytes_received = recvfrom(client_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *)&server_addr, &sin_size);
            buff[bytes_received] = '\0';
            if (bytes_received < 0)
            {
                perror("Error: ");
                close(client_sock);
                return 0;
            }
            else
            {
                printf("Reply from server: %s\n", buff);
                if (strcmp(buff, "OK") == 0)
                {
                    return 1; // success
                }
                else
                {
                    return 0;
                }
            }
        }
    }
    return 0;
}

int password_request(int client_sock, struct sockaddr_in server_addr, int sin_size)
{
    char buff[] = "password";
    int sent = sendto(client_sock, buff, strlen(buff), 0, (struct sockaddr *)&server_addr, sin_size);
    if (sent < 0)
    {
        return 0; // false - cannot request to login
    }
    else
    {
        char new_password[BUFF_SIZE];
        printf("Enter new password: ");
        scanf("%s", new_password);
        int bytes_sent = sendto(client_sock, new_password, strlen(new_password), 0, (struct sockaddr *)&server_addr, sin_size);
        if (bytes_sent < 0)
        {
            perror("Error: ");
            close(client_sock);
            return 0;
        }
        int bytes_received;
        while (1)
        {
            bytes_received = recvfrom(client_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *)&server_addr, &sin_size);
            if (bytes_received >= 0)
                break;
        }
        buff[bytes_received] = '\0';
        printf("Reply from server: %s\n", buff);
        return 1;
    }
}

int logout_request(int client_sock, struct sockaddr_in server_addr, int sin_size)
{
    char buff[] = "logout";
    int sent = sendto(client_sock, buff, strlen(buff), 0, (struct sockaddr *)&server_addr, sin_size);
    if (sent < 0)
    {
        return 0; // false - cannot request to login
    }
    int bytes_received;
    while (1)
    {
        bytes_received = recvfrom(client_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *)&server_addr, &sin_size);
        if (bytes_received >= 0)
            break;
    }
    buff[bytes_received] = '\0';
    if (bytes_received < 0)
    {
        perror("Error: ");
        close(client_sock);
        return 0;
    }
    printf("Reply from server: %s\n", buff);
    return 1;
}

int main(int argc, char **argv)
{
    int client_sock;
    char buff[BUFF_SIZE];
    char init[] = "init";
    struct sockaddr_in server_addr, client_addr;
    int bytes_sent, bytes_received, sin_size;
    char serv_ip[16];
    int serv_port = 0;
    int client_port = 0;

    int is_login = 0;

    printf("Insert your string to get the message from the other one\n");

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
    // check for valid port number
    serv_port = check_port(argv[2]);
    if (serv_port == -1)
    {
        perror("Error: Invalid port number!\n");
        exit(0);
    }

    // Step 1: Construct a UDP socket
    if ((client_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    { /* calls socket() */
        perror("\nError: ");
        exit(0);
    }

    // Step 2: Define the address of the server
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(serv_port);
    server_addr.sin_addr.s_addr = inet_addr(serv_ip);

    printf("\nInsert option: ");
    printf("\n 1 for login");
    printf("\n 2 for update password");
    printf("\n 3 to logout");
    printf("\n otherwise for messaging\n");
    int opt = 0;
    sin_size = sizeof(struct sockaddr);
    int sent = sendto(client_sock, init, strlen(init), 0, (struct sockaddr *)&server_addr, sin_size);
    if (sent < 0)
    {
        perror("Error:");
    }
    printf("\n");
    while (1)
    {
        // Step 3: Communicate with server
        printf("Enter option: ");
        scanf("%d", &opt);
        getchar(); // read newline from after scanf
        if (opt == 1)
        {
            int res = login_request(client_sock, server_addr, sin_size);
            if (res == 1)
            {
                is_login = 1;
            }
        }
        else if (opt == 2)
        {
            if (is_login == 1)
            {
                int res = password_request(client_sock, server_addr, sin_size);
            }
            else
                printf("Client has not signed in yet!\n");
        }
        else if (opt == 3)
        {
            if (is_login == 1)
            {
                int res = logout_request(client_sock, server_addr, sin_size);
            }
            else
                printf("Client has not signed in yet!\n");
        }
        else
        {
            if (is_login == 0)
            {
                printf("Client has not signed in yet!\n");
                continue;
            }
            printf("To communicate with another client, both clients need to send a message\n");
            printf("\nInsert string to send: ");
            char buff[BUFF_SIZE];
            memset(buff, '\0', (strlen(buff) + 1));
            fgets(buff, BUFF_SIZE, stdin);
            buff[strlen(buff) - 1] = '\0';

            if (is_empty_str(buff))
            {
                break;
            }

            bytes_sent = sendto(client_sock, buff, strlen(buff), 0, (struct sockaddr *)&server_addr, sin_size);
            if (bytes_sent < 0)
            {
                perror("Error: ");
                close(client_sock);
                return 0;
            }
            // communicate with server only
            bytes_received = recvfrom(client_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *)&server_addr, &sin_size);
            if (bytes_received < 0)
            {
                perror("Error: ");
                close(client_sock);
                return 0;
            }
            buff[bytes_received] = '\0';
            printf("Reply from server:\n%s", buff);
        }
    }
    // }
    close(client_sock);
    return 0;
}
