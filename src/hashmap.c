#include "hashmap.h"
#include <stdlib.h>
#include <string.h>

#define SUCCESS 1
#define FAIL 0

#ifdef DEBUG
    #define DEBUG_OUTPUT(string) (printf("[debug] %s\n",string))
#else
    #define DEBUG_OUTPUT(string)
#endif

/* PRIVATE */

static unsigned int sizes[] = {5, 13, 23, 47, 97, 193, 383, 769, 1531, 3067, 6143, 12289, 24571, 49157, 98299};

// ref: http://www.mactech.com/articles/mactech/Vol.16/16.10/AssociativeArrays/index.html
unsigned int hash(char *c, unsigned int step, unsigned int size) {
    unsigned int hashvalue = 0;
    if (!*c) return 0; // sanity
    do {
        hashvalue += *c++;
        hashvalue *= step;
    } while (*c);
    return (hashvalue % size);
}

unsigned int next_size(unsigned int size) {
    unsigned int count = sizeof(sizes) / sizeof(unsigned int);
    for (unsigned int i = 0; i < count; i++) {
        if (sizes[i] > size) {
            return sizes[i];
        }
    }
    fprintf(stderr, "Warning: Could not increase size more with prime numbers, doubling instead\n");
    return size * 2;
}

item *set_item(item *list, char *key, void *value, int *count) {
    if (key == NULL) {
        fprintf(stderr, "Warning: trying to read NULL key, returning NULL\n");
        return NULL;
    }
    if (list == NULL) { // Create new
        item *temp = malloc(sizeof(item));
        if (temp == NULL) {
            fprintf(stderr, "Error: could not allocate memory for item\n");
            exit(EXIT_FAILURE);
        }
        temp->key = malloc(sizeof(char)*(strlen(key)+1));
        if (temp->key == NULL) {
            fprintf(stderr, "Error: could not allocate memory for item key\n");
            exit(EXIT_FAILURE);
        }
        strcpy(temp->key, key);
        temp->value = value;
        temp->next = NULL;
        (*count)++;
        DEBUG_OUTPUT("item added");
        return temp;
    }
    
    if (strcmp(key, list->key) == 0) {
        if (list->value != value) {
            free(list->value);
            list->value = value;
            DEBUG_OUTPUT("item updated");
        }
        return list;
    }
    list->next = set_item(list->next, key, value, count);
    return list;
}

item *get_item(item *list, char *key) {
    if (list == NULL) { // No match
        return NULL;
    }
    if (strcmp(key, list->key) == 0) {
        return list;
    }
    return get_item(list->next, key);
}

item *remove_item(item *list, char *key, int *count) {
    if (list == NULL) { // No match
        return NULL;
    }
    if (strcmp(key, list->key) == 0) {
        item *next = list->next;
        free(list->key);
        free(list->value);
        free(list);
        (*count)--;
        DEBUG_OUTPUT("item removed");
        return next;
    }
    list->next = remove_item(list->next, key, count);
    return list;
}

item *remove_items(item *list, void (*free_value)(void *), int *count) {
    if (list == NULL) {
        return NULL;
    }
    if (list->next != NULL) {
        list->next = remove_items(list->next, free_value, count);
    }
    free(list->key);
    if (free_value != NULL) {
        free_value(list->value);
    }
    else {
        free(list->value);
    }
    free(list);
    (*count)--;
    DEBUG_OUTPUT("item removed");
    return NULL;
}

int key_exists(item *list, char *key) {
    if (list == NULL) { // No match
        return 0;
    }
    if (strcmp(key, list->key) == 0) {
        return 1;
    }
    return key_exists(list->next, key);
}

item *update_item_list(item *list, item *match) {
    match->next = NULL;
    if (list == NULL) {
        return match;
    }
    if (strcmp(match->key, list->key) == 0) {
        return list;
    }
    if (list->next == NULL) { // Put last
        list->next = match;
        return list;
    }
    list->next = update_item_list(list->next, match);
    return list;
}

void print_map(item *list, FILE *output, void (*to_string)(FILE *, void *)) {
    
}

/* PUBLIC */

hashmap *hm_create(unsigned int size, float load_factor, int hash_step) {
    unsigned int new_size = next_size(size);
    hashmap *map = malloc(sizeof(hashmap));
    if (map == NULL) {
        fprintf(stderr, "Error: could not allocate memory for hashmap\n");
        exit(EXIT_FAILURE);
    }
    map->items = malloc(sizeof(void *) * new_size);
    if (map->items == NULL) {
        fprintf(stderr, "Error: could not allocate memory for items\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < new_size; i++) {
        map->items[i] = NULL;
    }
    map->count = 0;
    map->size = new_size;
    map->load_factor = load_factor;
    map->hash_step = hash_step;
    DEBUG_OUTPUT("hashmap created");
    return map;
}

void hm_rehash(hashmap *map, unsigned int new_size) {
    DEBUG_OUTPUT("rehashing hashmap");
    if (new_size < map->size) {
        fprintf(stderr, "Error: cannot rehash with a smaller size\n");
        exit(EXIT_FAILURE);
    }
    map->items = realloc(map->items, sizeof(item) * new_size);
    if (map->items == NULL) {
        fprintf(stderr, "Error: could not reallocate memory for items\n");
        exit(EXIT_FAILURE);
    }
    // Reset new memory area
    for (unsigned int i = map->size; i < new_size; i++) {
        map->items[i] = NULL;
    }
    // Rehash old items
    item *temp, *next;
    unsigned int index;
    for (unsigned int i = 0; i < map->size; i++) {
        temp = map->items[i];
        map->items[i] = NULL;
        while (temp != NULL) {
            index = hash(temp->key, map->hash_step, new_size);
            next = temp->next;
            map->items[index] = update_item_list(map->items[index], temp);
            temp = next;
        }
    }
    map->size = new_size;
}

int hm_delete(hashmap *map, void (*free_value)(void *)) {
    hm_clear(map, free_value);
    free(map->items);
    free(map);
    DEBUG_OUTPUT("hashmap deleted");
    return SUCCESS;
}

int hm_clear(hashmap *map, void (*free_value)(void *)) {
    int counter = 0;
    for (int i = 0; i < map->size; i++) {
        map->items[i] = remove_items(map->items[i], free_value, &counter);
    }
    map->count += counter;
    DEBUG_OUTPUT("hashmap cleared");
    return SUCCESS;
}

void *hm_get(hashmap *map, char *key) {
    if (key == NULL) {
        fprintf(stderr, "Warning: trying to read NULL key, returning NULL\n");
        return NULL;
    }
    unsigned int index = hash(key, map->hash_step, map->size);
    item *temp = get_item(map->items[index], key);
    return (temp == NULL) ? NULL : temp->value;
}

int hm_set(hashmap *map, char *key, void *value) {
    if (key == NULL) {
        fprintf(stderr, "Warning: trying to read NULL key, returning NULL\n");
        return FAIL;
    }
    unsigned int index = hash(key, map->hash_step, map->size);
    int counter = 0;
    map->items[index] = set_item(map->items[index], key, value, &counter);
    map->count += counter;
    if ((float)map->count/(float)map->size > map->load_factor) {
        hm_rehash(map, next_size(map->size));
    }
    return SUCCESS;
}

int hm_unset(hashmap *map, char *key) {
    unsigned int index = hash(key, map->hash_step, map->size);
    int counter = 0;
    map->items[index] = remove_item(map->items[index], key, &counter);
    map->count += counter;
    return SUCCESS;
}

int hm_isset(hashmap *map, char *key) {
    unsigned int index = hash(key, map->hash_step, map->size);
    return key_exists(map->items[index], key);
}

void hm_print(hashmap *map, FILE *output, void (*to_string)(FILE *, char *, void *)) {
    if (to_string == NULL) {
        return;
    }
    for (int i = 0; i < map->size; i++) {
        item *temp = map->items[i];
        while (temp) {
            to_string(output, temp->key, temp->value);
            temp = temp->next;
        }
    }
}

void hm_dump(hashmap *map, FILE *output, void (*to_string)(FILE *, char *, void *)) {
    fprintf(output, "\n### hashmap dump ###############################################################\n\n");
    fprintf(output, " Version: %s\n", HM_VERSION);
    fprintf(output, " Size: %d\n", map->size);
    fprintf(output, " Items: %d\n", map->count);
    fprintf(output, " Load: %f\n", (float)map->count / (float)map->size);
    fprintf(output, " Load factor: %f\n", map->load_factor);
    fprintf(output, "\n################################################################################\n\n");
    if (map->count == 0) {
        fprintf(output, " (no items)\n\n");
    }
    for (int i = 0; i < map->size; i++) {
        fprintf(output, " index %d\n", i);
        item *temp = map->items[i];
        while (temp) {
            fprintf(output, "  - key: \"%s\"\tvalue: %lX\t", temp->key, (long)temp->value);
            if (to_string != NULL) {
                to_string(output, temp->key, temp->value);
            }
            fprintf(output, "\n");
            temp = temp->next;
        }
    }
    fprintf(output, "\n################################################################################\n\n");
}