// Simple hash table implemented in C.

#ifndef _HT_H
#define _HT_H

#include <stdbool.h>
#include <stddef.h>


#define WORD 20

// Hash table structure: create with ht_create, free with ht_destroy.
typedef struct ht ht;

typedef struct {
    char word[WORD];
    int freq;
} merged_ht;


// Create hash table and return pointer to it, or NULL if out of memory.
extern ht* ht_create(void);

// Free memory allocated for hash table, including allocated keys.
extern void ht_destroy(ht* table);

// Get item with given key (NUL-terminated) from hash table. Return
// value (which was set with ht_set), or NULL if key not found.
extern void* ht_get(ht* table, const char* key);

// Set item with given key (NUL-terminated) to value (which must not
// be NULL). If not already present in table, key is copied to newly
// allocated memory (keys are freed automatically when ht_destroy is
// called). Return address of copied key, or NULL if out of memory.
extern const char* ht_set(ht* table, const char* key, void* value);

// Return number of items in hash table.
extern size_t ht_length(ht* table);

// Hash table iterator: create with ht_iterator, iterate with ht_next.
typedef struct {
    const char* key;  // current key
    void* value;      // current value

    // Don't use these fields directly.
    ht* _table;       // reference to hash table being iterated
    size_t _index;    // current index into ht._entries
} hti;

// Return new hash table iterator (for use with ht_next).
extern hti ht_iterator(ht* table);

// Move iterator to next item in hash table, update iterator's key
// and value to current item, and return true. If there are no more
// items, return false. Don't call ht_set during iteration.
extern bool ht_next(hti* it);

//FUNZIONI DI MERGE
extern merged_ht* merged_ht_create(int numEntries);
extern void merged_ht_destroy(merged_ht* table);
extern void merge(merged_ht* mergedtable, int l, int m, int r);
extern void mergeSort(merged_ht* mergedTable, int l, int r);
extern void setword(merged_ht *table, const char *string);

#endif // _HT_H
