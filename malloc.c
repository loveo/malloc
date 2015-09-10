#include "malloc.h"
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

struct node* list = NULL;   /*The 'free list' holding both free and used nodes*/
void *break_address = 0;    /*Address to end of heap where next memory chunk will be allocated*/
int SMALLEST_QUICK = 8;     /*The smallest size of a quick block*/

/*Define strategies for more readability*/
#define STRATEGY_FIRST 1
#define STRATEGY_BEST  2
#define STRATEGY_WORST 3
#define STRATEGY_QUICK 4

#define META_SIZE sizeof(struct node)   /*Define META_SIZE to equal the size of node struct*/
#define make4(x) (((((x)-1)/4)*4)+4)    /*Define make4 to round up an integer to closest number divisible by 4*/

/*Sets Strategy to default strategy if not defined*/
#ifndef STRATEGY
    int STRATEGY = STRATEGY_QUICK;
#endif

/*Defines qiuck list varaibles if user has defined NRQUICKLISTS*/
#ifdef NRQUICKLISTS
    int QUICK_STEPS = NRQUICKLISTS;

    #define BIGGEST_QUICK (SMALLEST_QUICK << (QUICK_STEPS-1))
    struct node* quick_list[NRQUICKLISTS];
#endif

/*Defines default quick list variables otherwise*/
#ifndef NRQUICKLISTS
    int BIGGEST_QUICK = 128;
    int QUICK_STEPS =  5;
    struct node* quick_list[5];
#endif

void quick_cut(struct node*, size_t);
size_t page_round_up(size_t);
void *choose_and_cut(struct node*, size_t);
struct node* get_quick_node(size_t);
struct node* create_memory(size_t);
void concat_nodes(struct node*);
void concat_next(struct node*);
void *first_malloc(size_t);
void *worst_malloc(size_t);
void *best_malloc(size_t);
void *quick_malloc(size_t);
void *quick_realloc(void*, size_t);
void add_to_quick_list(struct node*);
size_t round_up(size_t);
void *get_chunk_pointer(struct node*);
void copy(struct node*, struct node*);
int is_quick_chunk(size_t);
void quick_concat(struct node*);

/*Returns the break point of the heap*/
void * end_heap(void){
    if(break_address == 0) break_address = sbrk(0);
    return break_address;
}

/*Allocates a pointer to a chunk of the given size*/
void *malloc(size_t size){
    if(size == 0) return NULL; /*Quick fix a la Robert*/
    /*Allign size*/
    size = make4(size);
    /*Check what strategy should be used*/
    switch(STRATEGY){
        case STRATEGY_FIRST: return first_malloc(size);
        case STRATEGY_BEST:  return best_malloc(size);
        case STRATEGY_WORST: return worst_malloc(size);
        case STRATEGY_QUICK: return quick_malloc(size);
    }
    return NULL;
}

/*Frees memory from a given pointer*/
void free(void *pointer){
    /*If it points nowhere, ignore it*/
    if(pointer == NULL) return;
    /*If malloc is using Quick Fit*/
    if(STRATEGY == STRATEGY_QUICK){
        /*Get the node containing the chunk*/
        struct node* node = ((struct node*) pointer-1);
        /*If this pointer wasn't given by malloc, ignore it since we can't free it*/
        if(node->chunk_pointer != pointer) return;

        /*If node is a 'Quick Node' treat it specially*/
        if(node->size <= BIGGEST_QUICK){
            if((node->size & (node->size -1)) == 0){
                add_to_quick_list(node);
                return;
            }
        }
        /*If not, set free flag as free*/
        node->free = 1;
        quick_concat(node);
    }else{/*If Quick Fit is not used*/
        /*Get the node containing the chunk*/
        struct node* node = ((struct node*)pointer-1);
        /*If this pointer wasn't given by malloc, ignore it*/
        if(node->chunk_pointer != pointer) return;
        /*Set the node as free*/
        node->free = 1;
        /*Try to merge this node with neighbours*/
        concat_nodes(node);
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
    if(STRATEGY == STRATEGY_QUICK) return quick_realloc(pointer, size);
    /*Get node containing this chunk*/
    node = ((struct node*)pointer-1);
    /*If this pointer points to a chunk given by malloc*/
    if(node->chunk_pointer == pointer){
        /*If the old size is equal to the wanted, return the old chunk*/
        if(node->size == size) return node->chunk_pointer;
        /*If the old node is too large*/
        if(node->size >= size){
            /*Check if there is a point in cutting the old node*/
            if(node->size - size - META_SIZE - 4 >= 0){
                /*Return the old node, cutted to the new size*/
                return choose_and_cut(node, size);
            }
        }else{/*If old node is too small*/
            /*If next node is free and big enough to hold our wanted size*/
            if(node->next != NULL && node->next->free && node->size + node->next->size + META_SIZE >= size){
                /*Merge this node with the next one*/
                concat_next(node);
                /*If the newly merged node is worth cutting*/
                if(node->size - size - META_SIZE - 4 >= 0){
                    /*Cut the newly merged node to save memory*/
                    choose_and_cut(node, size);
                }
            }else{/*If we can't merge with the next node*/
                /*Create a new node (malloc)*/   
                struct node* new_pointer = create_memory(size);
                /*If given address was NULL, we are out of memory*/ 
                if(new_pointer == NULL) return NULL;
                /*Copy contents from old node to new node*/
                copy(node, new_pointer);
                /*Free the old node since it's no longer used*/
                free(node);
                /*Return a pointer to the new nodes chunk*/
                return new_pointer->chunk_pointer;   
            }
        }
        return pointer;
    }
    return NULL;
}

/*Chooses a node to use and tries to cut it to save some memory*/
void *choose_and_cut(struct node* node, size_t size){
    struct node* cut_node; 
    node->free = 0;
    /*If node is worth cutting*/
    if(node-> size >= size + META_SIZE + 4){
        /*Create a new node*/
        cut_node = new_node(node->chunk_pointer + size, node->size - size - META_SIZE);
        /*Add the new node after the given node*/
        add_after(node, cut_node);
        /*Mark the new node as free*/
        cut_node->free = 1; 

        /*Set the given nodes size to the wanted size*/
        node->size = size;

        /*If we are using Quick Fit*/
        if(STRATEGY == STRATEGY_QUICK){
            /*If the left over size is small enough to fit Quick List*/
            if(cut_node->size <= BIGGEST_QUICK){
                /*If size is 8, 16, 32, 64 etc*/
                if(cut_node->size & ((cut_node->size -1) == 0)){
                    /*Remove the node from the list and add it to the Quick List instead*/
                    node->next = cut_node->next;
                    cut_node->next = NULL;
                    cut_node->prev = NULL;
                    add_to_quick_list(cut_node);
                }
            }
        } 
        return node->chunk_pointer;
    }
    return node->chunk_pointer;
}

/*Cuts a page into Quick Chunk of a given size*/
void quick_cut(struct node* node, size_t size){
    /*As long as the node can be cut*/
    while(node->size > size+META_SIZE){
        /*Create a left over node*/
        struct node* quick_node = new_node(node->chunk_pointer + size, node->size -size-META_SIZE);
        node->size = size;
        node->next = NULL;
        node->prev = NULL;
        /*Add this node to the Quick List*/
        add_to_quick_list(node);
        /*Set this node as the left over node*/
        node = quick_node;
    }/*Repeat until a Quick Node can't be cut*/

    /*Add the left over node to the ordinary list*/
    if(list == NULL)
        list = node;
    else
        add_end(list, node);
}

/*Creates a memory node of the given size*/
struct node* create_memory(size_t size){
    void* p;
    /*The size (multiple of page size) of the node*/
    size_t node_size = page_round_up(size)*sysconf(_SC_PAGESIZE);
    break_address = end_heap();
    /*Ask for memory from mmap*/
    p =  mmap(break_address, node_size, PROT_WRITE|PROT_READ,MAP_FIXED|MAP_SHARED|MAP_ANONYMOUS, -1,0 );
    /*If an ok pointer was returned*/
    if(p >= 0){
        struct node* node;  
        /*Increase the breakAdress for next mmap*/
        break_address += node_size;
        /*Create a node but remove header size from its size*/
        node = new_node(p, node_size-META_SIZE);

        /*If we are using Quick Fit and node is small enough to fit Quick List*/
        if(STRATEGY == STRATEGY_QUICK && size <= BIGGEST_QUICK){
            /*Cut the node into many Quick Nodes*/
            quick_cut(node, size);
            /*Return a free Quick Node*/
            return get_quick_node(size);  
        }else{/*If not using Quick Fit*/
            /*Add the node to the free list*/
            if(list == NULL){
                list = node;
            }else{
                add_end(list, node);
            }
            /*Try to cut the node if possible to save memory*/
            choose_and_cut(node, size);
        }
        return node; 
    }else{/*No memory left*/
        return NULL;
    }
}

/*Try to merge nodes with neighbours, only used in Quick Fit*/
void quick_concat(struct node* node){
    /*If next node is free*/
    if(node->next != NULL && node->next->free){
        struct node* next = node->next;
        /*If next node is not a Quick Node*/
        if(!is_quick_chunk(next->size)){
            /*If next node lies next to this node in the memory*/
            if(node->chunk_pointer + node->size == next){
                /*Merge nodes and remove next node*/
                node->size += META_SIZE + next->size;
                remove_next(node);
            }
        }
    }

    /*If prev node is free*/
    if(node->prev != NULL && node->prev->free){
        struct node* prev = node->prev;
        /*If prev node is not a Quick Node*/
        if(!is_quick_chunk(prev->size)){
            /*If prev node lies next to this node in the memory*/
            if(prev->chunk_pointer + prev->size == node){
                /*Merge nodes and remove this node*/
                prev->size += node->size + META_SIZE;
                remove_next(prev);
            }
        }
    }
}

/*Try to merge node with neighbouring nodes*/
void concat_nodes(struct node* node){
    /*If next node is free*/
    if(node->next != NULL && node->next->free){
        struct node* next = node->next;
        /*Merge nodes and remove next*/
        node->size += next->size + META_SIZE;
        remove_next(node);
    }

    /*If prev node is free*/
    if(node->prev != NULL && node->prev->free){
        struct node* prev = node->prev;
        /*Merge nodes and remode this node*/
        prev->size += node->size + META_SIZE;
        remove_next(prev);
    }
}

/*Try to merge this node with next node*/
void concat_next(struct node* node){
    /*If next node is free*/
    if(node->next != NULL && node->next->free){
        struct node* next = node->next;
        /*Merge nodes and remove next node*/
        node->size += next->size + META_SIZE;
        remove_next(node);
    }
}

/*Copy data from a node to another*/
void copy(struct node* from, struct node* to){
    int *from_data, *to_data;
    size_t i;
    from_data = from->chunk_pointer;
    to_data = to->chunk_pointer;
    /*Copy data until the smallest size have been filled*/
    for(i = 0; i*4 < from->size && i * 4 < to->size; ++i)
        to_data[i] = from_data[i];
}

/*Does malloc according to First Fit algorithm*/
void *first_malloc(size_t size){
    struct node* current = list;
    /*Searches through the node list*/
    while(current != NULL){
        /*If a node is free and big enough*/
        if(current->free && current->size >= size){
            /*Cut it if possible and return its address*/
            return choose_and_cut(current, size); 
        }
        current = current->next;
    }
    /*If no chunk was found, create a new node and return it*/
    return get_chunk_pointer(create_memory(size));
}

/*Does malloc according to Worst Fit algorithm*/
void *worst_malloc(size_t size){
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
        return get_chunk_pointer(create_memory(size));
    }else{
        /*Return the biggest node found and cut it if possible*/
        return choose_and_cut(biggest,size);
    }
}

/*Does malloc according to Best Fit algorithm*/
void *best_malloc(size_t size){
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
        return get_chunk_pointer(create_memory(size));
    }else{
        /*Return the smallest node and cut it if possible*/
        return choose_and_cut(closest, size);
    }
}

/*Does malloc according to Quick Fit algorithm*/
void *quick_malloc(size_t size){
    /*If the size is small enough to fit in a Quick List*/
    if(size <= BIGGEST_QUICK){
        struct node* free;
        /*Fetches a free node from the Quick List OR creates a new one*/
        free = get_quick_node(size);
        /*If the pointer is NULL, no memory is left*/
        if(free == NULL) return NULL;
        /*Mark this node as not free*/
        free->free = 0;
        /*Return the node*/
        return free->chunk_pointer;
    }else{/*For larger sizes, use first_malloc*/
        return first_malloc(size);
    }
}

/*Realloc used only when Quick Fit is used*/
void *quick_realloc(void *pointer, size_t size){
    struct node* node;
    struct node* new_node;
    void* p;
    /*Fetch the node holding this chunk*/
    node = (struct node*) pointer-1;
    /*If the pointer wasn't allocated by malloc, ignore call*/
    if(node->chunk_pointer != pointer) return NULL;
    /*If wanted size was old size, return old node*/
    if(node->size == size) return node->chunk_pointer;
    /*If new size still fits in the same Quick Node, return old node*/
    if(node->size >= size && size > node->size/2) return node->chunk_pointer;

    /*Create a new node*/
    p = malloc(size);
    /*If pointer is NULL, no memory is left*/
    if(p == NULL) return NULL;
    /*Get the node*/
    new_node = ((struct node*) p-1 ); 
    /*Copy data from old node to new node*/
    copy(node, new_node);
    /*Free the old node*/
    free(node->chunk_pointer);
    /*Return the new node*/
    return new_node->chunk_pointer;
}

/*Add a free Quick Node to the Quick List, only used in Quick Fit*/
void add_to_quick_list(struct node* node){
    int i,  pow = SMALLEST_QUICK;
    /*Figure out what index this size belongs to*/
    for(i = 0; i < QUICK_STEPS; ++i){
        if(node->size == pow){/*Correct index found*/
            /*Mark node as free*/
            node->free = 1;
            /*If Quick List is empty, use this as the list*/
            if(quick_list[i] == NULL){
                node->next = NULL;
                node->prev = NULL;
                quick_list[i] = node;
            }else{/*Else add this node first in the list*/
                node->next = NULL;
                node->prev = NULL;
                quick_list[i] = add_first(quick_list[i],node);
            }
            return;
        }else{
            pow *= 2;
        }
    }
}

/*Get a Quick Node from the Qiuck List OR create a new one if needed*/
struct node *get_quick_node(size_t size){
    int i, temp_size = SMALLEST_QUICK;
    /*Figures out what index to use*/
    for(i = 0; i < QUICK_STEPS; ++i){
         if(size <= temp_size){
            /*If the Quick List is empty, create a new node*/
            if(quick_list[i] == NULL){
                return create_memory(round_up(size));
            }else{
                /*Return first node and remove it from the Quick List*/
                struct node* free = quick_list[i];
                quick_list[i] = remove_first(free);
                free->free = 0;
                return free;
            }
        }
        temp_size *= 2;
    }
    return NULL;
}

/*Rounds a size up to closest Quick Size (16,32, 64.. etc)*/
size_t round_up(size_t size){
    int i, temp = SMALLEST_QUICK;
    for(i = 0; i < QUICK_STEPS; ++i){
        if(size <= temp) return temp;
        temp *= 2;
    }
    return size;
}

/*Returns the correct amount of pages to use to a given size*/
size_t page_round_up(size_t size){
    long page_size = sysconf(_SC_PAGESIZE);
    return ((META_SIZE+size-1)/page_size)+1;
}

/*Gets the chunk pointer from a node*/
void *get_chunk_pointer(struct node * node_pointer){
    if(node_pointer == NULL) return NULL;
    return node_pointer->chunk_pointer;
}

/*Checks if a size is a Quick Size*/
int is_quick_chunk(size_t size){
    if(size <= BIGGEST_QUICK)
        if(size & ((size -1) == 0))
            return 1;
    return 0;
}