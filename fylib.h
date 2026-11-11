#ifndef FYLIB_H
#define FYLIB_H

#include <ctype.h>
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

#define TYPESTR(x) _Generic((x), \
                        int: "int", \
                        long: "long", \
                        float: "float", \
                        double: "double", \
                        default: "something else")

// Macro that gives back a format specifier for a type
#define FMTSPEC(x) _Generic((x), \
                        int: "%d", \
                        long: "%ld", \
                        float: "%f", \
                        double: "%f", \
                        char *: "%s")
                        // TODO: add more types
                        
// Macro that prints a variable in the form "name = value"
#define PRINT_VAL(x) do { \
    char fmt[512]; \
    snprintf(fmt, sizeof fmt, #x " = %s\n", FMTSPEC(x)); \
    printf(fmt, (x)); \
} while(0)

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define ARRAY_LEN(arr) sizeof(arr) / sizeof(arr[0])

#define KiB(n) ((u64)(n) << 10)
#define MiB(n) ((u64)(n) << 20)
#define GiB(n) ((u64)(n) << 30)

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

#ifdef FYLIB_ARENA_IMPLEMENTATION

mem_arena* arena_create(u64 capacity){
    mem_arena* arena = (mem_arena*) malloc(capacity);

    arena->capacity = capacity;
    arena->pos = ARENA_BASE_POS;

    return arena;
}
void arena_destroy(mem_arena *arena){
    free(arena);
}
void* arena_push(mem_arena *arena, u64 size, b32 non_zero){
    u64 pos_aligned = ALIGN_UP_POW2(arena->pos, ARENA_ALIGN);
    u64 new_pos = pos_aligned + size;
    if(new_pos > arena->capacity) return NULL;

    arena->pos = new_pos;

    u8 *out = (u8*)arena + pos_aligned;
    if(!non_zero){
        memset(out, 0, size);
    }
    return out;
}
void arena_pop(mem_arena *arena, u64 size){
    size = MIN(size, arena->pos - ARENA_BASE_POS);
    arena->pos -= size;
}
void arena_pop_to(mem_arena *arena, u64 pos){
    u64 size = pos < arena->pos ? arena->pos - pos : 0;
    arena_pop(arena, size);
}
void arena_clear(mem_arena *arena){
    arena_pop_to(arena, ARENA_BASE_POS);
}
#endif

typedef struct{ 
    char *data;
    size_t count;
}string_view;

string_view str_from_cstr(char *cstr);
char* str_to_cstr(string_view self, mem_arena *arena);
void str_chop_left(string_view *s, size_t n);
void str_chop_right(string_view *s, size_t n);
void str_trim_left(string_view *s);
void str_trim_right(string_view *s);
void str_trim(string_view *s);
string_view str_chop_by_delim(string_view *s, char delim);
string_view str_chop_by_type(string_view *s, int (*istype)(int c));

#define STR_Fmt "%.*s"
#define STR_Arg(s) (s).count, (s).data

#ifdef FYLIB_STR_IMPLEMENTATION
string_view str_from_cstr(char *cstr){
    if(cstr == NULL){
        return (string_view){
            .data = " ",
            .count = 1,
        };
    }
    return (string_view){
        .data = cstr,
        .count = strlen(cstr),
    };
}
char* str_to_cstr(string_view self, mem_arena *arena){
    char *cstr = arena_push(arena, self.count + 1, true);
    memcpy(cstr, self.data, self.count);
    cstr[self.count] = '\0';
    return cstr;
}
void str_chop_left(string_view *s, size_t n){
    if (n > s->count) n = s->count;
    s->count -= n;
    s->data  += n;
}
void str_chop_right(string_view *sv, size_t n)
{
    if (n > sv->count) n = sv->count;
    sv->count -= n;
}

void str_trim_left(string_view *sv)
{
    while (sv->count > 0 && isspace(sv->data[0])) {
        str_chop_left(sv, 1);
    }
}

void str_trim_right(string_view *sv)
{
    while (sv->count > 0 && isspace(sv->data[sv->count-1])) {
        str_chop_right(sv, 1);
    }
}

void str_trim(string_view *sv)
{
    str_trim_left(sv);
    str_trim_right(sv);
}
string_view str_chop_by_delim(string_view *sv, char delim)
{
    size_t i = 0;
    while (i < sv->count && sv->data[i] != delim) {
        i += 1;
    }

    if (i < sv->count) {
        string_view result = {
            .data = sv->data,
            .count = i,
        };
        str_chop_left(sv, i + 1);
        return result;
    }

    string_view result = *sv;
    str_chop_left(sv, sv->count);
    return result;
}

string_view str_chop_by_type(string_view *sv, int (*istype)(int c))
{
    size_t i = 0;
    while (i < sv->count && !istype(sv->data[i])) {
        i += 1;
    }

    if (i < sv->count) {
        string_view result = {
            .data = sv->data,
            .count = i,
        };
        str_chop_left(sv, i + 1);
        return result;
    }

    string_view result = *sv;
    str_chop_left(sv, sv->count);
    return result;
}
#endif
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
