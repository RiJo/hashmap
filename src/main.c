#include "hashmap.h"
#include <stdio.h>
#include <stdlib.h>

void to_string_dump(FILE *output, char *key, void *value) {
    fprintf(output, "int: %d", *((int *)value));
}

void to_string_print(FILE *output, char *key, void *value) {
    fprintf(output, "%s: %d\n", key, *((int *)value));
}

void free_int(void *value) {
    free((int *)value);
}

int main(void) {
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    int *c = malloc(sizeof(int));
    int *d = malloc(sizeof(int));
    int *e = malloc(sizeof(int));

    *a = 1;
    *b = 2;
    *c = 3;
    *d = 4;
    *e = 5;

    hashmap *x = hm_create(2, 0.1f, 13);

    hm_dump(x, stdout, to_string_dump);

    hm_set(x, "a", a);
    hm_set(x, "b", b);

    hm_dump(x, stdout, to_string_dump);

    hm_set(x, "c", c);
    hm_set(x, "k", d);
    hm_set(x, "k", d);
    hm_set(x, "k", e);
    
    printf("Items:\n");
    hm_print(x, stdout, to_string_print);
    printf("\n");
    
    printf("is key \"k\" set? : %d\n", hm_isset(x, "k"));

    hm_dump(x, stdout, to_string_dump);

    printf("item 1: %d (address: %lX)\n", *(int *)hm_get(x, "a"), (long)hm_get(x, "a"));
    printf("item 2: %d (address: %lX)\n", *(int *)hm_get(x, "b"), (long)hm_get(x, "b"));
    printf("item 3: %d (address: %lX)\n", *(int *)hm_get(x, "c"), (long)hm_get(x, "c"));
    printf("item 4: %d (address: %lX)\n", *(int *)hm_get(x, "k"), (long)hm_get(x, "k"));
    printf("item 4: %d (address: %lX)\n", 0, (long)hm_get(x, NULL));

    hm_clear(x, free_int);

    printf("is key \"k\" set? : %d\n", hm_isset(x, "k"));

    hm_dump(x, stderr, to_string_dump);

    hm_delete(x, free_int);

    return EXIT_SUCCESS;
}
