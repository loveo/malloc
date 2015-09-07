#include "node.h"
#include <stdio.h>
#define META_SIZE sizeof(struct node)

/*Creates a node at the given address with the given size*/
struct node * new_node(void* address, size_t size){
	struct node *p ;
	p = (struct node*) address;
	/*Point below the node*/
	p->chunk_pointer = p + 1;
	p->size = size;
	p->free = 0; 

	return (struct node*) address;
}

/*Adds a node to the end of a list, given its root*/
void add_end(struct node* root, struct node* end_node){
	struct node *current = root;
	while(current-> next != NULL)current = current->next;
	current->next = end_node;
	end_node->prev = current;
	end_node->next = NULL;
}

/*Adds a node before a given node*/
void addHere(struct node* here, struct node* add_node){
	if(here->prev != NULL)
	here->prev->next = add_node;
	add_node->prev = here->prev;
	here->prev = add_node;
	add_node->next = here;
}

/*Adds a node after a given node*/
void add_after(struct node* here, struct node* add_node){
	if(here->next == NULL){
		here->next = add_node;
		add_node->prev = here;
		add_node->next = NULL;
	}else{
		add_node->next = here->next;
		add_node->prev = here;
		here->next->prev = add_node;
		here->next = add_node;
	}
}

/*Adds a node first in a list, given its root*/
struct node* add_first(struct node* root, struct node* add_node){
	root->prev = add_node;
	add_node->next = root;
	add_node->prev = NULL;
	return add_node;
}

/*Removes the next node given a node*/
void remove_next(struct node* here){
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
struct node* remove_first(struct node* root){
	struct node* temp;
	if(root->next == NULL) return NULL;

	temp = root->next;
	temp->prev = NULL;
	root->next = NULL;
	return temp;
}