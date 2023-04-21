#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>

int separate_message(char str[], char num[], char alphabet[], int size){
	int is_fault = 0;
	int i = 0;
	int i_num = 0;
	int i_alpha = 0;
    for (int i = 0; i < bytes_received - 1; i++) {
                if (isdigit(buff[i])) {
                    nums[lenNums++] = buff[i];
                } else if (isalpha(buff[i])) {
                    characters[lenChar++] = buff[i];
                } else {
					error = 1;
					break;
				}
            }
            nums[lenNums] = '\0';
            characters[lenChar] = '\0';
			if (error) {
				strcpy(buff, "Error");
			} else {
	            sprintf(buff, "%s\n%s", nums, characters);
			}
    return is_fault;
}


int main() {
    char buff[30] = "heeloowworld121323";
    char num[30];
    char alphabet[30];
    printf("%ld\n", strlen(buff));
    separate_message(buff, num, alphabet, strlen(buff));
}
