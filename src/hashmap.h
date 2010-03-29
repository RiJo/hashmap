#include <stdio.h>

#ifndef HASHMAP_H
#define HASHMAP_H

#define HM_VERSION "1.0.0-b6"

struct node {
    char *key;
    void *value;
    struct node *next;
};
typedef struct node item;

typedef struct {
    item **items;
    unsigned int count; // Number of items
    unsigned int size; // Size of hashmap
    float load_factor;
    int hash_step;
} hashmap;

hashmap *hm_create(unsigned int, float, int);
void hm_rehash(hashmap *, unsigned int);
int hm_delete(hashmap *, void (*)(void *));
int hm_clear(hashmap *, void (*)(void *));
void *hm_get(hashmap *, char *); /* O(1) */
int hm_set(hashmap *, char *, void *); /* O(1) */
int hm_unset(hashmap *, char *); /* O(1) */
int hm_isset(hashmap *, char *); /* O(1) */
void hm_print(hashmap *, FILE *, void (*)(FILE *, char *, void *));
void hm_dump(hashmap *, FILE *, void (*)(FILE *, char *, void *));

#endif