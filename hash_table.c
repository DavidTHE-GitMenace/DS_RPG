// hash_table.c — Basic String Hash Table in C

#include "hash_table.h"
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

// Hash function (DJB2)
uint64_t djb_hash(const char *str) // This function essenitally creates an index for the string
{
    uint64_t hash = 5381; // Any prime number
    while (*str)
    {
        hash = ((hash << 5) + hash) + (unsigned char)(*str); // hash * 33 + current_character
        // hash <<5 = hash *32
        // (hash *32) +hash = hash(32+1) because of factoring
        // Thus we have hash *33
        str++;
    }
    return hash;
}

// Initialize the hash table
void init_table(HashTable *table) // Initializes each bucket entry to NULL
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        table->buckets[i] = NULL;
    }
}

// Insert or update a key-value pair
void put(HashTable *table, const char *key, const char *value)
{
    uint64_t hash = djb_hash(key);            // Converts the string key into a number
    int index = hash % TABLE_SIZE;            // Makes valid index of the string key for the table
    HashEntry *entry = table->buckets[index]; // entry of foucs

    // Check if key exists — update if so
    while (entry != NULL)
    {
        if (strcmp(entry->key, key) == 0) // Found the key
        {
            entry->value = value; // Update existing value
            return;
        }
        entry = entry->next;
    }

    // Key not found — insert new entry at head
    entry = malloc(sizeof(HashEntry));
    entry->key = key;
    entry->value = value;
    // Fail safe for collions in the hash leads to the same : Chaining method
    // ttable[index] → [new_entry] → [old_entry] → NULL
    entry->next = table->buckets[index]; // old entry
    table->buckets[index] = entry;       // new entry
}

// Retrieve a value by key
const char *get(HashTable *table, const char *key)
{
    uint64_t hash = djb_hash(key);            // Converts the string key into a number
    int index = hash % TABLE_SIZE;            // Makes valid index of the string key for the table
    HashEntry *entry = table->buckets[index]; // entry of foucs

    // Check if key exists
    while (entry != NULL)
    {
        if (strcmp(entry->key, key) == 0) // Found the key
        {
            return entry->value;
        }
        entry = entry->next;
    }
    return NULL; // not found
}
