#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#define HEADER_SIZE sizeof(Header)

size_t align(int n) {
    int align = 8;
    if (n % 4 == 0 && n != 0) return n;
    if (n < 4) return 4;
    while (n > align) align = align + 4;
    return align;
}

void init (Allocator *_allocator ,size_t size) {
    size_t bufSize = sizeof(char) * size;
    void *memoryBuffer = malloc(bufSize);
    Header *p = (Header*) memoryBuffer;

    p->size = size - HEADER_SIZE;
    p->isUsed = 0;
    p->next_block = NULL;

    _allocator->memoryBuffer = memoryBuffer;
    _allocator->size = bufSize;
    _allocator->_head = p;
}

void *mem_alloc(Allocator *_allocator, size_t size){
    size = align(size);
    Header *header = (Header*)first_fit(_allocator->_head, size);
    if (header == NULL || size > _allocator->size){
        return NULL;
    }
    
    if (header->size >= size + HEADER_SIZE) {
        splitBlock(_allocator, header, size);
    }
    header->size = size;

    header->isUsed = 1;
    return header + 1;
}

void *mem_realloc(Allocator *_allocator, void *memPointer, size_t size){
    size = align(size);
    char *offset = (char *) memPointer;
    
    Header *oldSpaceHeader = (Header*)(offset - HEADER_SIZE);
    Header *newSpaceHeader = (Header*)first_fit(_allocator->_head, size);
    if (newSpaceHeader == NULL) {
        newSpaceHeader = mem_alloc(_allocator, size);
    }
    if (newSpaceHeader == NULL) {
        return NULL;
    }
    oldSpaceHeader->isUsed = 0;
    
    if (newSpaceHeader->size >= size + HEADER_SIZE) {
        splitBlock(_allocator, newSpaceHeader, size);
    }
    printf("%lu\n", newSpaceHeader->size);
    newSpaceHeader->isUsed = 1;
    void *res = memcpy((char*)(newSpaceHeader + 1), memPointer, oldSpaceHeader->size);
    mem_free(memPointer);
    return res;
}

void mem_free(void *memPointer){
    char *offset = (char *) memPointer;
    Header *header = (Header*)(offset - HEADER_SIZE);
    header->isUsed = 0;
    if (header->next_block != NULL && header->next_block->isUsed == 0) {
        header->size += header->next_block->size;
        if (header->next_block->next_block != NULL){
            header->next_block = header->next_block->next_block;
        }
        header->next_block = NULL;
    }
}

void splitBlock(Allocator *_allocator, Header *header, size_t size){
    char *offset = (char *) header + HEADER_SIZE;
    size_t newSize = header->size - size;
    Header *newHeader = (Header*) (offset + size);

    newHeader->size = newSize - HEADER_SIZE;
    newHeader->isUsed = 0;
    newHeader->next_block = NULL;

    header->next_block = newHeader;
    header->size = size;
    header->isUsed = 1;
}

void *first_fit(struct header *head, size_t searchSize){
    while (head != NULL) {
        if (searchSize <= head->size && head->isUsed == 0 ) {
            return head;
        }
        head = head->next_block;
    }
    return NULL;
}
void mem_dump(Allocator *allocator){
    Header *header = allocator->_head;
    while (header != NULL){
        printf("Used: %d  Size: %d Addr: %p\n", header->isUsed, header->size, header);
        header = header->next_block;
    }
}
int main() {
    struct allocator _allocator;
    init(&_allocator, 1000);
    int *res;
    res = (int*)mem_alloc(&_allocator, 4);
    *res = 999;
    res = (int*)mem_realloc(&_allocator, res, 8);
    //printf("%d\n", *res);
    mem_dump(&_allocator);
    return 0;
}
