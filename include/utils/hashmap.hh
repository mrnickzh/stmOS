#ifndef HASHMAP_H
#define HASHMAP_H

#define UNUSED (void*)0
#define DELETED (void*)1

typedef struct hashmap_element {
    char *key;
    int value;
} hashmap_element;

typedef struct hashmap {
    hashmap_element *elements;
    int size;
    int fill;
} hashmap;

void hashmap_init(hashmap *map);
void hashmap_put(hashmap *map, char *key, int value);
void hashmap_del(hashmap *map, char *key);
int hashmap_get(hashmap *map, char *key);

#endif // HASHMAP_H