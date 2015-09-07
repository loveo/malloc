#include "node.h"
#include <stdio.h>
#define META_SIZE sizeof(struct node)

/*Creates a node at the given address with the given size*/
struct node * newNode(void* address, size_t size){
 
 struct node *p ;
 p = (struct node*) address;
 /*Point below the node*/
 p->chunkPointer = p + 1;
 p->size = size;
 p->free = 0; 

 return (struct node*) address;

}

/*Adds a node to the end of a list, given its root*/
void addEnd(struct node* root, struct node* endNode){
 struct node *current = root;
 while(current-> next != NULL)current = current->next;
 current->next = endNode;
 endNode->prev = current;
 endNode->next = NULL;

}

/*Adds a node before a given node*/
void addHere(struct node* here, struct node* addNode){
 if(here->prev != NULL)
 	here->prev->next = addNode;
 addNode->prev = here->prev;
 here->prev = addNode;
 addNode->next = here;

}

/*Adds a node after a given node*/
void addAfter(struct node* here, struct node* addNode){
 if(here->next == NULL){
 	here->next = addNode;
	addNode->prev = here;
	addNode->next = NULL;
 }else{
	addNode->next = here->next;
	addNode->prev = here;
	here->next->prev = addNode;
	here->next = addNode;
 }
}

/*Adds a node first in a list, given its root*/
struct node* addFirst(struct node* root, struct node* addNode){
 root->prev = addNode;
 addNode->next = root;
 addNode->prev = NULL;
 return addNode;
}

/*Removes the next node given a node*/
void removeNext(struct node* here){
 struct node* next = here->next;
 if(here->next == NULL) return;

 if(here->next->next == NULL){
  here->next->prev = NULL;
  here->next = NULL;
 }else{
  next->next->prev = here;
  next->prev = NULL;
  here->next = next->next;
  next->next = NULL;
 } 
}

/*Removes the first node, given the root*/
struct node* removeFirst(struct node* root){
 struct node* temp;
 if(root->next == NULL) return NULL;

 temp = root->next;
 temp->prev = NULL;
 root->next = NULL;
 return temp;

}


