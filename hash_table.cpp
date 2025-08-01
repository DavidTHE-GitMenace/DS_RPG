// hash_table.c — Basic String Hash Table in C

#include "hash_table.h"
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

// Hash function (DJB2)
uint64_t djb_hash(const char *str) // Creates a numeric index from a string
{
    uint64_t hash = 5381; // Starting value (commonly used prime)
    while (*str)
    {
        hash = ((hash << 5) + hash) + (unsigned char)(*str); // hash * 33 + c
        str++;
    }
    return hash;
}

// Initialize the hash table
void init_table(HashTable *table) // Set all bucket entries to NULL
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        table->buckets[i] = NULL;
    }
}

// Insert or update a key-value pair
void put(HashTable *table, const char *key, void *value)
{
    uint64_t hash = djb_hash(key);            // Hash the string key
    int index = hash % TABLE_SIZE;            // Compute valid table index
    HashEntry *entry = table->buckets[index]; // Start at bucket head

    // Check if key already exists — update its value if so
    while (entry != NULL)
    {
        if (strcmp(entry->key, key) == 0) // Found key match
        {
            entry->value = value; // Overwrite existing value
            return;
        }
        entry = entry->next;
    }

    // Key not found — create a new entry and insert at bucket head
    entry = (HashEntry *)malloc(sizeof(HashEntry));
    if (!entry)
        return; // malloc failed

    entry->key = strdup(key); // Copy the key string
    if (!entry->key)
    { // strdup failed
        free(entry);
        return;
    }

    entry->value = value;

    // Handle collisions with chaining
    entry->next = table->buckets[index]; // New → Old → ...
    table->buckets[index] = entry;       // Bucket → New
}

// Retrieve a value by key
void *get(HashTable *table, const char *key)
{
    uint64_t hash = djb_hash(key);            // Hash the key
    int index = hash % TABLE_SIZE;            // Get index
    HashEntry *entry = table->buckets[index]; // Start at bucket head

    // Search linked list for the key
    while (entry != NULL)
    {
        if (strcmp(entry->key, key) == 0)
        {
            return entry->value;
        }
        entry = entry->next;
    }
    return NULL; // Key not found
}

// Free all memory in the hash table
void free_table(HashTable *table)
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        HashEntry *entry = table->buckets[i];
        while (entry != NULL)
        {
            HashEntry *next = entry->next;
            free((char *)entry->key); // Free duplicated key
            // If you own value memory, free(entry->value); here
            free(entry); // Free entry itself
            entry = next;
        }
        table->buckets[i] = NULL;
    }
}
