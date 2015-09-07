#include <stdlib.h>

/*Holds meta data for the memory chunks*/
struct node{
 /*Points to the next memory node*/
 struct node* prev;
 /*Points to the prev memory node*/
 struct node* next;
 /*The size of the memory this node points to*/
 size_t size;
 /*0 is not free, otherwise free*/
 int free;
 /*Points to the memory that the user can use*/
 void* chunkPointer;

};

struct node* newNode(void*, size_t);
void addEnd(struct node*, struct node*);
void addHere(struct node*, struct node*);
void addAfter(struct node*, struct node*);
struct node* addFirst(struct node*, struct node*);
void removeNext(struct node*);
struct node* removeFirst(struct node*);
