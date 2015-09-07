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
 void* chunk_pointer;

};

struct node* new_node(void*, size_t);
void add_end(struct node*, struct node*);
void add_here(struct node*, struct node*);
void add_after(struct node*, struct node*);
struct node* add_first(struct node*, struct node*);
void remove_next(struct node*);
struct node* remove_first(struct node*);
