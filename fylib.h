#ifndef FYLIB_H
#define FYLIB_H

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef i8 b8;
typedef i32 b32;

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define ARRAY_LEN(arr) sizeof(arr) / sizeof(arr[0])

#define KiB(n) ((u64)(n) << 10)
#define MiB(n) ((u64)(n) << 20)
#define GiB(n) ((u64)(n) << 30)

//TODO: string
typedef struct{ 
    char *data;
    size_t length;
}string;

#define ALIGN_UP_POW2(n,p) (((u64)(n) + ((u64)(p) - 1)) & (~((u64)(p) - 1)))


//arena alloc
typedef struct{
    u64 capacity;
    u64 pos;
}mem_arena;

#define ARENA_BASE_POS (sizeof(mem_arena))
#define ARENA_ALIGN (sizeof(void*))

#define ARN_PUSH_STRUCT(arena, type) (type*)arena_push((arena), sizeof(type), false)
#define ARN_PUSH_STRUCT_NZ(arena, type) (type*)arena_push((arena), sizeof(type), true)
#define ARN_PUSH_ARRAY(arena, type, count) (type*)arena_push((arena), sizeof(type) * (count), false)
#define ARN_PUSH_ARRAY_NZ(arena, type, count) (type*)arena_push((arena), sizeof(type) * (count), true)

mem_arena* arena_create(u64 capacity);
void arena_destroy(mem_arena *arena);
void arena_clear(mem_arena *arena);

void* arena_push(mem_arena *arena, u64 size, b32 non_zero);

void arena_pop(mem_arena *arena, u64 size);
void arena_pop_to(mem_arena *arena, u64 pos);

u64 arena_get_pos(mem_arena *arena);

//data structs:

typedef struct{
    size_t count;
    size_t capacity;
}Header;

#define ARR_INIT_CAPACITY 30

#define arr_push(arr, x)                                                              \
    do {                                                                              \
        if (arr == NULL) {                                                            \
            Header *header = malloc(sizeof(*arr)*ARR_INIT_CAPACITY + sizeof(Header)); \
            header->count = 0;                                                        \
            header->capacity = ARR_INIT_CAPACITY;                                     \
            arr = (void*)(header + 1);                                                \
        }                                                                             \
        Header *header = (Header*)(arr) - 1;                                          \
        if (header->count >= header->capacity) {                                     \
            header->capacity *= 2; \
            header = realloc(header, sizeof(*arr)*header->capacity + sizeof(Header)); \
            arr = (void*)(header + 1);                                                \
        } \
        (arr)[header->count++] = (x);                                                 \
    } while(0)

#define arr_len(arr) ((Header*)(arr) - 1)->count

#define arr_free(arr) free((Header*)(arr) - 1)

//FILE operations:
#define READ_FILE(path, buffer_out) do { \
    FILE *file = fopen((path), "r"); \
    if (file == NULL) { \
        *(buffer_out) = NULL; \
        break;\
    } \
    if (fseek(file, 0, SEEK_END) != 0) { \
        fclose(file); \
        *(buffer_out) = NULL; \
        break;\
    } \
    long len = ftell(file); \
    if (len < 0) { \
        fclose(file); \
        *(buffer_out) = NULL; \
        break;\
    } \
    if (fseek(file, 0, SEEK_SET) != 0) { \
        fclose(file); \
        *(buffer_out) = NULL; \
        break;\
    } \
    char *buffer = malloc((size_t)len + 1); \
    if (buffer == NULL) { \
        fclose(file); \
        *(buffer_out) = NULL; \
        break;\
    } \
    size_t bytes_read = fread(buffer, 1, (size_t)len, file); \
    fclose(file); \
    if (bytes_read != (size_t)len) { \
        free(buffer); \
        *(buffer_out) = NULL; \
        break;\
    } \
    buffer[bytes_read] = '\0'; \
    *(buffer_out) = buffer; \
} while(0)
#endif
