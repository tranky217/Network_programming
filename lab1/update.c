#include<stdio.h>
#include<stdlib.h>
#include<string.h>
void update_file(char* filename, char target[]){
    FILE* fptr = fopen(filename, "w+");
    char username[30];
    char password[30];
    int status = 0;
    // account tmp;
    // int cur_pos[2];
    // int index = 0;
    int cur_pos = 0;
    if(fptr != NULL){
        while(fscanf(fptr, "%s %s %d\n", username, password, &status) != EOF){
            cur_pos = ftell(fptr);
            // if(index == 0) index = 1;
            // if(index == 1) index = 0;
            printf("%d\n", cur_pos);
            if(strcmp(username, target) == 0){
                // (cur_pos[0] > cur_pos[1]) ? pos = cur_pos[1] : pos = cur_pos[0];
                fseek(fptr, -2, SEEK_CUR);
                fprintf(fptr, "%d", status);
                printf("?");
            }
        }
    }else{
        fclose(fptr);
        printf("Error!\n");
        exit(0);
        return ;
    }
}
int main(){
    update_file("account.txt", "soitc");
    return 0;
}