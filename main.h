//
//  main.h
//  secondEdition
//
//  Created by Alex Filatov on 11/3/20.
//  Copyright © 2020 Alex Filatov. All rights reserved.
//

#ifndef main_h
#define main_h


#endif /* main_h */
typedef struct header Header;
typedef struct allocator Allocator;

struct header {
    char isUsed;
    size_t size;
    Header *next_block;
};

struct allocator {
    void *memoryBuffer;
    size_t size;
    Header *_head;
};

void init (Allocator *_allocator ,size_t size);
void *mem_alloc(Allocator *_allocator, size_t size);
void splitBlock(Allocator *_allocator, Header *header, size_t size);
void *mem_realloc(Allocator *_allocator, void *memPointer, size_t size);
void *first_fit(Header *head, size_t searchSize);
void mem_free(void *memPointer);
void mem_dump(Allocator *allocator);
size_t align(int n);
