#include<stdio.h>
#include<stdlib.h>
#include<string.h>
// define the max length of username or password
#define LENGTH 30
// adding the is_signin, incorrect for 3rd and 4th features
typedef struct account{
    char username[LENGTH];
    char password[LENGTH];
    int status;
    int is_signin;
    int incorrect;
}account;
// linked list node
typedef struct node{
    account data;
    struct node* next;
}node;
// adding the new node to the end of the linked list
void append(node** head_ref,account new_data){
    node* new_node = (node*)malloc(sizeof(node));
    node* temp = *head_ref;
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
// showing content of the linked list
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
// write new account to file account.txt
void write_to_file(char* filename, account new_acc){
    FILE* fptr = fopen(filename, "a");
    if(fptr != NULL){
        fprintf(fptr, "%s %s %d\n", new_acc.username, new_acc.password, new_acc.status);
    }else{
        fclose(fptr);
        printf("Error!\n");
        exit(0);
        return ;
    }
}
// rewrite all the node of the linked list to file account.txt
// after updating the status field of a blocked account
void update_file(char* filename, node* head){
    FILE* fptr = fopen(filename, "w");
    account tmp;
    if(fptr != NULL){
        // loop through each node and write to file
        while(head != NULL){
            fprintf(fptr, "%s %s %d\n", (head->data).username, (head->data).password, (head->data).status);
            head = head->next;
        }
    }else{
        fclose(fptr);
        printf("Error!\n");
        exit(0);
        return ;
    }
}
// for creating the menu
void menu(){
    printf("USER MANAGEMENT PROGRAM\n");
    printf("-----------------------------------\n");
    printf("1. Register\n");
    printf("2. Sign in\n");
    printf("3. Search\n");
    printf("4. Sign out\n");
    printf("Your choice (1-4, other to quit):\n");
}

int main(){
    char* filename = "account.txt";
    // loading initial accounts from file to linked list
    node* head = init_account(filename);
    printf("After loading from account.txt\n");
    display(head);       
    //
    menu();
    // new account var for future use
    account new_acc;
    int option; // user's choice
    scanf("%d", &option);
    getchar();
    while(option == 1 || option == 2 || option == 3 || option == 4){
        switch(option){
            case 1:
            // register
            printf("Username: ");
            scanf("%s", new_acc.username);
            if(search(head, new_acc.username) != NULL){ // search for existed username
                printf("Account existed\n");
            }else{
                printf("Password: ");
                scanf("%s", new_acc.password);
                new_acc.incorrect = 0;
                new_acc.is_signin = 0;
                new_acc.status = 1;
                append(&head, new_acc); // add new account to linked list
                write_to_file(filename, new_acc); // write new account to file
                printf("Successful registration\n");
            }
            break;
            case 2:
            // sign in
            printf("Username: ");
            scanf("%s", new_acc.username);
            node* cur = search(head, new_acc.username);
            if(cur != NULL){ // check if that account already existed
                if((cur->data).status == 1){
                    printf("Password: ");
                    scanf("%s", new_acc.password);
                    if(strcmp(new_acc.password, (cur->data).password) == 0){ // compare the password
                        (cur->data).is_signin = 1; // is_signin = 1 mean user is logged in
                        printf("Hello %s\n", new_acc.username);}
                    else{
                        (cur->data).incorrect += 1; // increasing the number of times user type in incorrect password
                        printf("Password is incorrect\n"); 
                        if((cur->data).incorrect >= 3){ // each account has limited number of times for trying - here is 3
                            (cur->data).status = 0;
                            printf("Account is blocked\n");
                            //update to file
                            update_file(filename, head);
                        }
                    }
                }else{
                    printf("Account is blocked\n");
                }
            }else{
                printf("Can not find account\n");
            }
            break;
            case 3:
            // search
            printf("Username: ");
            scanf("%s", new_acc.username);
            node* cur1 = search(head, new_acc.username); // search for username in linked list
            if(cur1 != NULL){
                if((cur1->data).status == 1){
                    printf("Account is active\n");
                }else{
                    printf("Account is blocked\n");
                }
            }else{
                printf("Can not find account\n");
            }
            break;
            case 4:
            // sign out
            printf("Username: ");
            scanf("%s", new_acc.username);
            node* cur2 = search(head, new_acc.username);
            if(cur2 != NULL){
                if((cur2->data).is_signin == 1){ // check if an account is logged in or not
                    printf("Goodbye %s\n", (cur2->data).username);
                }else if((cur2->data).is_signin == 0){
                    printf("Account is not sign in\n");
                }
            }else{
                printf("Can not find account\n");
            }
            break;
            default:
            break;
        }
        menu();
        scanf("%d", &option);
        getchar();
    }   
    return 0;
}