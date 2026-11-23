#include "hashmap.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

uint32_t hash(char *data, uint32_t len) {
    uint32_t h = 5381;
    for (int i = 0; i < len; i++) {
        h = h * 33 + data[i];
    }
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    return h;
}

void hashmap_init(hashmap *map) {
    map->size = 4;
    map->fill = 0;
    map->elements = (hashmap_element*)calloc(map->size, sizeof(hashmap_element));
}

static void internal_put(hashmap *map, char *key, int value) {
    uint32_t index = hash(key, strlen(key)) % map->size;
    hashmap_element tmp = {key, value};
    
    int first_deleted = -1;
    int i = 0;
    while (i < map->size) {
        int idx = (index + i) % map->size;
        hashmap_element elem = map->elements[idx];
        
        if (elem.key == UNUSED) {
            map->elements[(first_deleted != -1) ? first_deleted : idx] = tmp;
            map->fill++;
            return;
        }
        else if (elem.key == DELETED) {
            if (first_deleted == -1) { first_deleted = (int)idx; }
        }
        else if (!(strcmp(key, elem.key))) {
            map->elements[idx] = tmp;
            return;
        }
        
        i++;
    }
}

void hashmap_expand(hashmap *map) {
    int oldsize = map->size; 
    map->size = map->size * 2;
    map->fill = 0;
    
    hashmap_element *old_elements = map->elements;
    map->elements = (hashmap_element*)calloc(map->size, sizeof(hashmap_element));
    
    for (int i = 0; i < oldsize; i++) {
        hashmap_element elem = old_elements[i];
        printf("%s %d\n", elem.key, elem.value);
        if ((elem.key != UNUSED) && (elem.key != DELETED)) {
            internal_put(map, elem.key, elem.value);
        }
    }
    
    free(old_elements);
}

void hashmap_put(hashmap *map, char *key, int value) {
    if (map->fill > (map->size * 0.5f)) {
        hashmap_expand(map);
    }
    internal_put(map, key, value);
}

int hashmap_get(hashmap *map, char *key) {
    uint32_t index = hash(key, strlen(key)) % map->size;
    
    int i = 0;
    while (i < map->size) {
        int idx = (index + i) % map->size;
        hashmap_element elem = map->elements[idx];
        if (elem.key == UNUSED) {
            return -1;
        }
        else if ((elem.key != DELETED) && !(strcmp(key, elem.key))) {
            return elem.value;
        }
        i++;
    }
    return -1;
}

void hashmap_del(hashmap *map, char *key) {
    uint32_t index = hash(key, strlen(key)) % map->size;
    
    int i = 0;
    while (i < map->size) {
        int idx = (index + i) % map->size;
        hashmap_element elem = map->elements[idx];
        if (elem.key == UNUSED) {
            return;
        }
        else if ((elem.key != DELETED) && !(strcmp(key, elem.key))) {
            map->elements[idx].key = DELETED;
            map->fill--;
            return;
        }
        i++;
    }
    return;
}

int main()
{
    srand(time(NULL));
    
    hashmap map;
    hashmap_init(&map);

    char **keys = calloc(20, sizeof(char*));
    
    for (int i = 0; i < 20; i++) {
        keys[i] = calloc(5, sizeof(char));
        for (int j = 0; j < 4; j++) {
            keys[i][j] = 'a' + (rand() % 26);
        }
        keys[i][4] = '\0';
        printf("%d: %s\n", i, keys[i]);
    }
    
    for (int i = 0; i < 20; i++) {
        int r = (int)(rand() % 100);
        hashmap_put(&map, keys[i], r);
        
        printf("%s - %d, size: %d\n", keys[i], r, map.size);
    }
    
    for (;;) {
        char buf[5];
        fgets(buf, 5, stdin);
        
        printf("%d\n", hashmap_get(&map, buf));
    }

    return 0;
}
