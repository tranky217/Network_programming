#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#define MAX_LINE 100
// return 0 if the argument is domain name and 1 otherwise
int check_input(char ip_addr[]){
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ip_addr, &(sa.sin_addr));
    return result != 0;
}

int main(int argc, char **argv){
    // variables for domain name -> IP address
    struct addrinfo *p, *listp, hints;
    char buf[MAX_LINE];
    int rc, flags;    
    // for checking command line arguments
    if(argc != 2){
        fprintf(stderr, "Usage: %s <hostname>\n", argv[0]);
        exit(0);
    }
    // define hints
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; // return ip4 only
    hints.ai_socktype = SOCK_STREAM;
    // get the ip address of the domain name and store the result in listp
    if((rc = getaddrinfo(argv[1], NULL, &hints, &listp)) != 0){
        printf("Not found information\n");
        // fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(rc));
        exit(1);
    }
    if(check_input(argv[1]) == 0){
        flags = NI_NUMERICHOST; // numeric version of ip
        int official = 1;
        // listp is a linked list of results
        for(p = listp; p; p = p->ai_next){
            getnameinfo(p->ai_addr, p->ai_addrlen, buf, MAX_LINE, NULL, 0, flags);
            if(official == 1){
                printf("Official IP:\n");
                official += 1;
            }else if(official == 2){
                printf("Alias IP:\n");
                official += 1;
            } 
            printf("%s\n", buf);
        }
        freeaddrinfo(listp);
    }
    else{
        flags = 0;
        int official = 1;
        for(p = listp; p; p = p->ai_next){
            getnameinfo(p->ai_addr, p->ai_addrlen, buf, MAX_LINE, NULL, 0, flags);
            if(official == 1){
                printf("Official Domain:\n");
                official += 1;
            }else if(official == 2){
                printf("Alias Domain:\n");
                official += 1;
            } 
            printf("%s\n", buf);
        }
        freeaddrinfo(listp);
    }
    return 0;
}
