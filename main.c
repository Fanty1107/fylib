#define FYLIB_IMPLEMENTATION
#include "fylib.h"

int main(){
    mem_arena *arena = arena_create(MiB(1));
    char *content = ARN_PUSH_ARRAY(arena, char, 100);
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
    arena_destroy(arena);
    return 0;
}
