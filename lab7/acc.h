// define the max length of username or password
#define LENGTH 50
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

void append(node **head_ref, account new_data);
void display(node *head);
node *search(node *head, char key_username[]);
node *init_account(char *filename);
void update_file(node *head);