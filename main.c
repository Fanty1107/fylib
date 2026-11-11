#define FYLIB_ARENA_IMPLEMENTATION
#define FYLIB_STR_IMPLEMENTATION
#include "fylib.h"

int main(){
    mem_arena *arena = arena_create(MiB(2));
    char *content = arena_push(arena, 1024, false);
    READ_FILE("test.txt", &content);
    if(content == NULL){
        fprintf(stderr, "Unable to read file");
        return -1;
    }
    printf("%s", content);


    int *number = NULL;
    arr_push(number, 89);
    arr_push(number, 420);
    arr_push(number, 1337);
    arr_push(number, 80085);
    for (size_t i = 0; i < arr_len(number); ++i) {
        printf("%d\n", number[i]);
    }
    arr_free(number);
    FILE *f = fopen(__FILE__, "rb");
    size_t cap = 1024*1024;
    char *buffer = arena_push(arena, cap, false);
    size_t size = fread(buffer, 1, cap, f);

    string_view s ={
        .data = buffer,
        .count = size,
    };
    size_t word_count = 0;
    while (s.count > 0) {
        str_trim_left(&s);
        if (s.count == 0) break;
        string_view word = str_chop_by_type(&s, isspace);
        printf(STR_Fmt"\n", (int)STR_Arg(word));
        word_count += 1;
    }
    printf("word_count = %zu\n", word_count);
    arena_destroy(arena);
    return 0;
}
