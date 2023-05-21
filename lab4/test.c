#include <stdio.h>
#include <string.h>

#define BUFF_SIZE 100
int main()
{
    int count = 0;
    int opt = 0;
    while (1)
    {
        printf("scanf: ");
        scanf("%d", &opt);
        printf("\nopt = %d", opt);
        getchar();
        printf("\nInsert string to send: ");
        char buff[BUFF_SIZE];
        memset(buff, '\0', (strlen(buff) + 1));
        fgets(buff, BUFF_SIZE, stdin);
        printf("length: %ld", strlen(buff));
        buff[strlen(buff) - 1] = '\0';
        printf("string after insert: %s\n", buff);
        printf("length: %ld", strlen(buff));
        count++;
        if (count == 5)
        {
            break;
        }
    }

    return 0;
}
