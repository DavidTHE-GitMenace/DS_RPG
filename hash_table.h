// hash_table.h

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define TABLE_SIZE 53

    typedef struct HashEntry
    {
        const char *key;
        const char *value;
        struct HashEntry *next;
    } HashEntry;

    typedef struct
    {
        HashEntry *buckets[TABLE_SIZE];
    } HashTable;

    uint64_t djb_hash(const char *str);
    void init_table(HashTable *table);
    void put(HashTable *table, const char *key, const char *value);
    const char *get(HashTable *table, const char *key);

#ifdef __cplusplus
}
#endif
