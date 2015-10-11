#ifndef _brk_h_
#define _brk_h_


#ifndef _UNISTD_H
    #ifdef _UNISTD_H_
        #define _UNISTD_H _UNISTD_H_
    #endif
#endif
    

#define MMAP /* if defined use mmap() instead of sbrk/brk */

#ifdef MMAP
    void * end_heap(void);
#endif

#ifndef _UNISTD_H_ /* USE WITH CAUTION brk() and sbrk() have been removed from the POSIX standard, most systems implement them but the parameter types may vary */
    extern int brk(void *);
    extern void *sbrk( );
#endif

#endif /*_brk_h */
