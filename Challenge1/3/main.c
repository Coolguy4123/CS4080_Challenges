#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char* string;
    struct Node* next;
    struct Node* prev;
}Node;

struct Node* createNode(const char* x){
    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
    if (new_node == NULL) {
        return NULL;
    }

    new_node->string = malloc(strlen(x) + 1);

    strcpy(new_node->string, x); //Copy and assign string
    new_node->prev = NULL;
    new_node->next = NULL;

    return new_node;
}


void insert(Node** head, int pos, const char* string){
    Node *new_node = createNode(string);
    Node *curr = *head;
    int cnt = 0;

    if (head == NULL || new_node == NULL) {
        return;
    }

    if (*head == NULL) {
        *head = new_node;
        return;
    }

    while (curr != NULL && cnt < pos){
        cnt++;
        curr = curr->next;
    }

    if (curr == NULL) {
        Node *tail = *head;
        while (tail->next != NULL) {
            tail = tail->next;
        }
        new_node->prev = tail;
        tail->next = new_node;
        return;
    }

    new_node->next = curr;
    new_node->prev = curr->prev;

    if (curr->prev != NULL) {
        curr->prev->next = new_node;
    } else {
        *head = new_node;
    }

    curr->prev = new_node;
}

Node* find(struct Node* head, const char* string){
    Node* curr = head;
    while (curr != NULL){
        if(strcmp(string, curr->string) == 0){ //Matching string
            return curr;
        }
        curr = curr->next; //Move pointer
    }

    return NULL;
}

void del(Node** head, Node* delNode){
    if (head == NULL || *head == NULL || delNode == NULL){ //Edge case
        return;
    }

    if (delNode->prev != NULL) { //Unlinking "prev"
        delNode->prev->next = delNode->next;
    } else { //First node to be deleted
        *head = delNode->next;
    }

    if (delNode->next != NULL){ //Unlinking "next"
        delNode->next->prev = delNode->prev;
    }

    // Free up memory
    free(delNode->string);
    free(delNode);
}

void printList(Node* head){
    while (head != NULL){
        printf("%s\n", head->string);
        head = head->next;
    }
}

int main(void) {
    Node* dll = NULL;

    insert(&dll, 0, "123");
    insert(&dll, 1, "456");
    insert(&dll, 1, "789");

    printf("Original list:\n");
    printList(dll);

    // Delete the found node
    Node* found = find(dll, "456");
    if (found != NULL) {
        printf("Found: %s\n", found->string);
        del(&dll, found);
    }

    printf("After deletion:\n");
    printList(dll);

    return 0;
}
