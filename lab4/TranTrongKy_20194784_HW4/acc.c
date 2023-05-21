#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"acc.h"

void append(node** head_ref,account new_data){
    node* new_node = (node*)malloc(sizeof(node));
    node* temp = *head_ref;
    new_data.incorrect = 0;
    new_data.is_signin = 0;
    new_node->data = new_data;
    new_node->next = NULL;
    if(*head_ref == NULL){
        *head_ref = new_node;
        return;
    }
    while(temp->next != NULL){
        temp = temp->next;
    }
    temp->next = new_node;
}
void display(node* head){
    while(head != NULL){
        printf("%s : %s : %d\n",(head->data).username,(head->data).password,(head->data).status);
        head = head->next;
    }
}
// search and return the node with username
node* search(node* head,char key_username[]){
    node* cur;
    for(cur = head; cur!=NULL; cur = cur->next){
        if(strcmp((cur->data).username,key_username) == 0) return cur;
    }
    return NULL;
}
// loading all current accounts exited in account.txt file
node* init_account(char *filename){
    FILE* fptr = fopen(filename, "r");
    if(fptr != NULL){
        node* head = NULL;
        account new_acc;
        // scanf through each line of the file and append each account into the linked list
        while(fscanf(fptr, "%s %s %d\n", new_acc.username, new_acc.password, &(new_acc.status)) != EOF){
            append(&head, new_acc);
        }
        return head;
    }else{
        fclose(fptr);
        return NULL;
    }
}
// rewrite all the node of the linked list to file account.txt
// after updating the status field of a blocked account
void update_file(node* head){
    FILE* fptr = fopen("account.txt", "w");
    if (fptr != NULL)
    {
        // loop through each node and write to file
        // printf("inside update file\n");
        while(head != NULL){
            // printf("%s %s %d\n", (head->data).username, (head->data).password, (head->data).status);
            fprintf(fptr, "%s %s %d\n", (head->data).username, (head->data).password, (head->data).status);
            head = head->next;
        }
        fclose(fptr);
    }
    else
    {
        fclose(fptr);
        printf("Error!\n");
        exit(0);
        return ;
    }
}