#include "malloc.h"
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

struct node* list = NULL;
void *breakAddress = 0;
int SMALLEST_QUICK = 8;

#define META_SIZE sizeof(struct node)
#define make4(x) (((((x)-1)/4)*4)+4)

#ifndef STRATEGY
int STRATEGY = 4;
#endif

#ifdef NRQUICKLISTS
int QUICK_STEPS = NRQUICKLISTS;

#define BIGGEST_QUICK (SMALLEST_QUICK << (QUICK_STEPS-1))
struct node* quickList[NRQUICKLISTS];
#endif

#ifndef NRQUICKLISTS
int BIGGEST_QUICK = 128;
int  QUICK_STEPS =  5;
struct node* quickList[5];
#endif

void quickCut(struct node*, size_t);
size_t pageRoundUp(size_t);
void *chooseAndCut(struct node*, size_t);
struct node* getQuickNode(size_t);
struct node* createMemory(size_t);
void concatNodes(struct node*);
void concatNext(struct node*);
void *firstMalloc(size_t);
void *worstMalloc(size_t);
void *bestMalloc(size_t);
void *quickMalloc(size_t);
void *quickRealloc(void*, size_t);
void addToQuickList(struct node*);
size_t roundUp(size_t);
void *getChunkPointer(struct node*);
void copy(struct node*, struct node*);
int isQuickChunk(size_t);
void quickConcat(struct node*);

/*Returns the break point of the heap*/
void * endHeap(void){
	if(breakAddress == 0) breakAddress = sbrk(0);
	return breakAddress;
}

/*Allocates a pointer to a chunk of the given size*/
void *malloc(size_t size){
	if(size == 0) return NULL; /*Quick fix a la Robert*/
	/*Allign size*/
	size = make4(size);
	/*Check what strategy should be used*/
	switch(STRATEGY){
		case 1: return firstMalloc(size);
		case 2: return bestMalloc(size);
		case 3: return worstMalloc(size);
		case 4: return quickMalloc(size);
	}
	return NULL;
}

/*Frees memory from a given pointer*/
void free(void *pointer){
	/*If it points nowhere, ignore it*/
	if(pointer == NULL) return;
	/*If malloc is using Quick Fit*/
	if(STRATEGY == 4){
		/*Get the node containing the chunk*/
		struct node* node = ((struct node*) pointer-1);
		/*If this pointer wasn't given by malloc, ignore it since we can't free it*/
		if(node->chunkPointer != pointer) return;

		/*If node is a 'Quick Node' treat it specially*/
		if(node->size <= BIGGEST_QUICK){
			if((node->size & (node->size -1)) == 0){
				addToQuickList(node);
				return;
			}
		}
		/*If not, set free flag as free*/
		node->free = 1;
		quickConcat(node);
	}else{/*If Quick Fit is not used*/
		/*Get the node containing the chunk*/
		struct node* node = ((struct node*)pointer-1);
		/*If this pointer wasn't given by malloc, ignore it*/
		if(node->chunkPointer != pointer) return;
		/*Set the node as free*/
		node->free = 1;
		/*Try to merge this node with neighbours*/
		concatNodes(node);
	}
}

/*Reallocates memory for a given pointer and size*/
void *realloc(void *pointer, size_t size){
	struct node* node;
	/*If pointer is NULL, allocate a new pointer for it*/
	if(pointer == NULL) return malloc(size);

	/*Allign the size*/
	size = make4(size);

	/*If Quick Fit is used, use another realloc method instead*/
	if(STRATEGY == 4) return quickRealloc(pointer, size);
	/*Get node containing this chunk*/
	node = ((struct node*)pointer-1);
	/*If this pointer points to a chunk given by malloc*/
	if(node->chunkPointer == pointer){
		/*If the old size is equal to the wanted, return the old chunk*/
		if(node->size == size) return node->chunkPointer;
		/*If the old node is too large*/
		if(node->size >= size){
			/*Check if there is a point in cutting the old node*/
			if(node->size - size - META_SIZE - 4 >= 0){
				/*Return the old node, cutted to the new size*/
				return chooseAndCut(node, size);
			}
		}else{/*If old node is too small*/
			/*If next node is free and big enough to hold our wanted size*/
			if(node->next != NULL && node->next->free && node->size + node->next->size + META_SIZE >= size){
				/*Merge this node with the next one*/
				concatNext(node);
				/*If the newly merged node is worth cutting*/
				if(node->size - size - META_SIZE - 4 >= 0){
					/*Cut the newly merged node to save memory*/
					chooseAndCut(node, size);
				}
			}else{/*If we can't merge with the next node*/
				/*Create a new node (malloc)*/	 
				struct node* newPointer = createMemory(size);
				/*If given address was NULL, we are out of memory*/ 
				if(newPointer == NULL) return NULL;
				/*Copy contents from old node to new node*/
				copy(node, newPointer);
				/*Return a pointer to the new nodes chunk*/
				return newPointer->chunkPointer;   
			}
		}
		return pointer;
	}
return NULL;
}

/*Chooses a node to use and tries to cut it to save some memory*/
void *chooseAndCut(struct node* node, size_t size){
	struct node* cutNode; 
	node->free = 0;
	/*If node is worth cutting*/
	if(node-> size >= size + META_SIZE + 4){
		/*Create a new node*/
		cutNode = newNode(node->chunkPointer + size, node->size - size - META_SIZE);
		/*Add the new node after the given node*/
		addAfter(node, cutNode);
		/*Mark the new node as free*/
		cutNode->free = 1; 

		/*Set the given nodes size to the wanted size*/
		node->size = size;

		/*If we are using Quick Fit*/
		if(STRATEGY == 4){
			/*If the left over size is small enough to fit Quick List*/
			if(cutNode->size <= BIGGEST_QUICK){
				/*If size is 8, 16, 32, 64 etc*/
				if(cutNode->size & ((cutNode->size -1) == 0)){
				    /*Remove the node from the list and add it to the Quick List instead*/
					node->next = cutNode->next;
					cutNode->next = NULL;
					cutNode->prev = NULL;
					addToQuickList(cutNode);
				}
			}
		} 
		return node->chunkPointer;
	}
	return node->chunkPointer;
}

/*Cuts a page into Quick Chunk of a given size*/
void quickCut(struct node* node, size_t size){
	/*As long as the node can be cut*/
	while(node->size > size+META_SIZE){
		/*Create a left over node*/
		struct node* quickNode = newNode(node->chunkPointer + size, node->size -size-META_SIZE);
		node->size = size;
		node->next = NULL;
		node->prev = NULL;
		/*Add this node to the Quick List*/
		addToQuickList(node);
		/*Set this node as the left over node*/
		node = quickNode;
	}/*Repeat until a Quick Node can't be cut*/

	/*Add the left over node to the ordinary list*/
	if(list == NULL)
		list = node;
	else
		addEnd(list, node);
}

/*Creates a memory node of the given size*/
struct node* createMemory(size_t size){
	void* p;
	/*The size (multiple of page size) of the node*/
	size_t nodeSize = pageRoundUp(size)*sysconf(_SC_PAGESIZE);
	breakAddress = endHeap();
	/*Ask for memory from mmap*/
	p =  mmap(breakAddress, nodeSize, PROT_WRITE|PROT_READ,MAP_FIXED|MAP_SHARED|MAP_ANONYMOUS, -1,0 );
	/*If an ok pointer was returned*/
	if(p >= 0){
		struct node* node;  
		/*Increase the breakAdress for next mmap*/
		breakAddress += nodeSize;
		/*Create a node but remove header size from its size*/
		node = newNode(p, nodeSize-META_SIZE);

		/*If we are using Quick Fit and node is small enough to fit Quick List*/
		if(STRATEGY == 4 && size <= BIGGEST_QUICK){
			/*Cut the node into many Quick Nodes*/
			quickCut(node, size);
			/*Return a free Quick Node*/
			return getQuickNode(size);  
		}else{/*If not using Quick Fit*/
			/*Add the node to the free list*/
			if(list == NULL){
				list = node;
			}else{
				addEnd(list, node);
			}
			/*Try to cut the node if possible to save memory*/
			chooseAndCut(node, size);
		}
		return node; 
	}else{/*No memory left*/
		return NULL;
	}
}

/*Try to merge nodes with neighbours, only used in Quick Fit*/
void quickConcat(struct node* node){
	/*If next node is free*/
	if(node->next != NULL && node->next->free){
		struct node* next = node->next;
		/*If next node is not a Quick Node*/
		if(!isQuickChunk(next->size)){
			/*If next node lies next to this node in the memory*/
			if(node->chunkPointer + node->size == next){
				/*Merge nodes and remove next node*/
				node->size += META_SIZE + next->size;
				removeNext(node);
			}
		}
	}

	/*If prev node is free*/
	if(node->prev != NULL && node->prev->free){
		struct node* prev = node->prev;
		/*If prev node is not a Quick Node*/
		if(!isQuickChunk(prev->size)){
			/*If prev node lies next to this node in the memory*/
			if(prev->chunkPointer + prev->size == node){
				/*Merge nodes and remove this node*/
				prev->size += node->size + META_SIZE;
				removeNext(prev);
			}
		}
	}
}

/*Try to merge node with neighbouring nodes*/
void concatNodes(struct node* node){
	/*If next node is free*/
	if(node->next != NULL && node->next->free){
		struct node* next = node->next;
		/*Merge nodes and remove next*/
		node->size += next->size + META_SIZE;
		removeNext(node);
	}

	/*If prev node is free*/
	if(node->prev != NULL && node->prev->free){
		struct node* prev = node->prev;
		/*Merge nodes and remode this node*/
		prev->size += node->size + META_SIZE;
		removeNext(prev);
	}
}

/*Try to merge this node with next node*/
void concatNext(struct node* node){
	/*If next node is free*/
	if(node->next != NULL && node->next->free){
		struct node* next = node->next;
		/*Merge nodes and remove next node*/
		node->size += next->size + META_SIZE;
		removeNext(node);
	}
}

/*Copy data from a node to another*/
void copy(struct node* from, struct node* to){
	int *fromData, *toData;
	size_t i;
	fromData = from->chunkPointer;
	toData = to->chunkPointer;
	/*Copy data until the smallest size have been filled*/
	for(i = 0; i*4 < from->size && i * 4 < to->size; ++i)
		toData[i] = fromData[i];
}

/*Does malloc according to First Fit algorithm*/
void *firstMalloc(size_t size){
	struct node* current = list;
	/*Searches through the node list*/
	while(current != NULL){
		/*If a node is free and big enough*/
		if(current->free && current->size >= size){
			/*Cut it if possible and return its address*/
			return chooseAndCut(current, size); 
		}
		current = current->next;
	}
	/*If no chunk was found, create a new node and return it*/
	return getChunkPointer(createMemory(size));
}

/*Does malloc according to Worst Fit algorithm*/
void *worstMalloc(size_t size){
	struct node* current = list;
	struct node* biggest = NULL;
	/*Loop through the entire list*/
	while(current != NULL){
		/*If the found node is free and big enough*/
		if(current-> free && current->size >= size){
			/*If the found node is bigger then the old biggest*/
			if(biggest == NULL || biggest->size < current->size){
				/*Mark it as biggest*/
				biggest = current; 
			}
		}
		current = current->next;
	}
	/*If we didn't find a big enough node*/
	if(biggest == NULL){
		/*Create a new node and return it*/
		return getChunkPointer(createMemory(size));
	}else{
		/*Return the biggest node found and cut it if possible*/
		return chooseAndCut(biggest,size);
	}
}

/*Does malloc according to Best Fit algorithm*/
void *bestMalloc(size_t size){
	struct node* current = list;
	struct node* closest = NULL;
	/*Loop through all nodes*/
	while(current != NULL){
		/*If found node is free and big enough*/
		if(current->free && current->size >= size){
			/*If this node is smaller than the last smallest*/
			if(closest == NULL || closest-> size > current->size){
				/*Mark this node as best fit*/
				closest = current;
			}
		}
		current = current->next;
	}
	/*If no node was found*/
	if(closest == NULL){
		/*Create a new node and return it*/
		return getChunkPointer(createMemory(size));
	}else{
		/*Return the smallest node and cut it if possible*/
		return chooseAndCut(closest, size);
	}
}

/*Does malloc according to Quick Fit algorithm*/
void *quickMalloc(size_t size){
	/*If the size is small enough to fit in a Quick List*/
	if(size <= BIGGEST_QUICK){
		struct node* free;
		/*Fetches a free node from the Quick List OR creates a new one*/
		free = getQuickNode(size);
		/*If the pointer is NULL, no memory is left*/
		if(free == NULL) return NULL;
		/*Mark this node as not free*/
		free->free = 0;
		/*Return the node*/
		return free->chunkPointer;
	}else{/*For larger sizes, use firstMalloc*/
		return firstMalloc(size);
	}
}

/*Realloc used only when Quick Fit is used*/
void *quickRealloc(void *pointer, size_t size){
	struct node* node;
	struct node* newNode;
	void* p;
	/*If no pointer was given, retun NULL*/
	if(pointer == NULL) return NULL;
	/*Fetch the node holding this chunk*/
	node = (struct node*) pointer-1;
	/*If the pointer wasn't allocated by malloc, ignore call*/
	if(node->chunkPointer != pointer) return NULL;
	/*If wanted size was old size, return old node*/
	if(node->size == size) return node->chunkPointer;
	/*If new size still fits in the same Quick Node, return old node*/
	if(node->size >= size && size > node->size/2) return node->chunkPointer;

	/*Create a new node*/
	p = malloc(size);
	/*If pointer is NULL, no memory is left*/
	if(p == NULL) return NULL;
	/*Get the node*/
	newNode = ((struct node*) p-1 ); 
	/*Copy data from old node to new node*/
	copy(node, newNode);
	/*Free the old node*/
	free(node->chunkPointer);
	/*Return the new node*/
	return newNode->chunkPointer;
}

/*Add a free Quick Node to the Quick List, only used in Quick Fit*/
void addToQuickList(struct node* node){
	int i,  pow = SMALLEST_QUICK;
	/*Figure out what index this size belongs to*/
	for(i = 0; i < QUICK_STEPS; ++i){
		if(node->size == pow){/*Correct index found*/
			/*Mark node as free*/
			node->free = 1;
			/*If Quick List is empty, use this as the list*/
			if(quickList[i] == NULL){
				node->next = NULL;
				node->prev = NULL;
				quickList[i] = node;
			}else{/*Else add this node first in the list*/
				node->next = NULL;
				node->prev = NULL;
				quickList[i] = addFirst(quickList[i],node);
			}
			return;
		}else{
			pow *= 2;
		}
	}
}

/*Get a Quick Node from the Qiuck List OR create a new one if needed*/
struct node *getQuickNode(size_t size){
	int i, tempSize = SMALLEST_QUICK;
	/*Figures out what index to use*/
	for(i = 0; i < QUICK_STEPS; ++i){
		 if(size <= tempSize){
			/*If the Quick List is empty, create a new node*/
			if(quickList[i] == NULL){
				return createMemory(roundUp(size));
			}else{
				/*Return first node and remove it from the Quick List*/
				struct node* free = quickList[i];
				quickList[i] = removeFirst(free);
				free->free = 0;
				return free;
			}
		}
		tempSize *= 2;
	}
	return NULL;
}

/*Rounds a size up to closest Quick Size (16,32, 64.. etc)*/
size_t roundUp(size_t size){
	int i, temp = SMALLEST_QUICK;
	for(i = 0; i < QUICK_STEPS; ++i){
		if(size <= temp) return temp;
		temp *= 2;
	}
	return size;
}

/*Returns the correct amount of pages to use to a given size*/
size_t pageRoundUp(size_t size){
	long pageSize = sysconf(_SC_PAGESIZE);
	return ((META_SIZE+size-1)/pageSize)+1;
}

/*Gets the chunk pointer from a node*/
void *getChunkPointer(struct node * nodePointer){
	if(nodePointer == NULL) return NULL;
	return nodePointer->chunkPointer;
}

/*Checks if a size is a Quick Size*/
int isQuickChunk(size_t size){
	if(size <= BIGGEST_QUICK)
		if(size & ((size -1) == 0))
			return 1;
	return 0;
}