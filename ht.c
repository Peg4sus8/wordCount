// Simple hash table implemented in C.

#include "ht.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Hash table entry (slot may be filled or empty).
typedef struct {
    const char* key;  // key is NULL if this slot is empty
    void* value;
} ht_entry;

// Hash table structure: create with ht_create, free with ht_destroy.
struct ht {
    ht_entry* entries;  // hash slots
    size_t capacity;    // size of _entries array
    size_t length;      // number of items in hash table
};

#define INITIAL_CAPACITY 16  // must not be zero

ht* ht_create(void) {
    // Allocate space for hash table struct.
    ht* table = malloc(sizeof(ht));
    if (table == NULL) {
        return NULL;
    }
    table->length = 0;
    table->capacity = INITIAL_CAPACITY;

    // Allocate (zero'd) space for entry buckets.
    table->entries = calloc(table->capacity, sizeof(ht_entry));
    if (table->entries == NULL) {
        free(table); // error, free table before we return!
        return NULL;
    }
    return table;
}

void ht_destroy(ht* table) {
    // First free allocated keys.
    for (size_t i = 0; i < table->capacity; i++) {
        free((void*)table->entries[i].key);
    }

    // Then free entries array and table itself.
    free(table->entries);
    free(table);
}

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

// Return 64-bit FNV-1a hash for key (NUL-terminated). See description:
// https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
static uint64_t hash_key(const char* key) {
    uint64_t hash = FNV_OFFSET;
    for (const char* p = key; *p; p++) {
        hash ^= (uint64_t)(unsigned char)(*p);
        hash *= FNV_PRIME;
    }
    return hash;
}

void* ht_get(ht* table, const char* key) {
    // AND hash with capacity-1 to ensure it's within entries array.
    uint64_t hash = hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(table->capacity - 1));

    // Loop till we find an empty entry.
    while (table->entries[index].key != NULL) {
        if (strcmp(key, table->entries[index].key) == 0) {
            // Found key, return value.
            return table->entries[index].value;
        }
        // Key wasn't in this slot, move to next (linear probing).
        index++;
        if (index >= table->capacity) {
            // At end of entries array, wrap around.
            index = 0;
        }
    }
    return NULL;
}

// Internal function to set an entry (without expanding table).
static const char* ht_set_entry(ht_entry* entries, size_t capacity,
        const char* key, void* value, size_t* plength) {
    // AND hash with capacity-1 to ensure it's within entries array.
    uint64_t hash = hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(capacity - 1));

    // Loop till we find an empty entry.
    while (entries[index].key != NULL) {
        if (strcmp(key, entries[index].key) == 0) {
            // Found key (it already exists), update value.
            entries[index].value = value;
            return entries[index].key;
        }
        // Key wasn't in this slot, move to next (linear probing).
        index++;
        if (index >= capacity) {
            // At end of entries array, wrap around.
            index = 0;
        }
    }

    // Didn't find key, allocate+copy if needed, then insert it.
    if (plength != NULL) {
        key = strdup(key);
        if (key == NULL) {
            return NULL;
        }
        (*plength)++;
    }
    entries[index].key = (char*)key;
    entries[index].value = value;
    return key;
}

// Expand hash table to twice its current size. Return true on success,
// false if out of memory.
static bool ht_expand(ht* table) {
    // Allocate new entries array.
    size_t new_capacity = table->capacity * 2;
    if (new_capacity < table->capacity) {
        return false;  // overflow (capacity would be too big)
    }
    ht_entry* new_entries = calloc(new_capacity, sizeof(ht_entry));
    if (new_entries == NULL) {
        return false;
    }

    // Iterate entries, move all non-empty ones to new table's entries.
    for (size_t i = 0; i < table->capacity; i++) {
        ht_entry entry = table->entries[i];
        if (entry.key != NULL) {
            ht_set_entry(new_entries, new_capacity, entry.key,
                         entry.value, NULL);
        }
    }

    // Free old entries array and update this table's details.
    free(table->entries);
    table->entries = new_entries;
    table->capacity = new_capacity;
    return true;
}

const char* ht_set(ht* table, const char* key, void* value) {
    assert(value != NULL);
    if (value == NULL) {
        return NULL;
    }

    // If length will exceed half of current capacity, expand it.
    if (table->length >= table->capacity / 2) {
        if (!ht_expand(table)) {
            return NULL;
        }
    }

    // Set entry and update length.
    return ht_set_entry(table->entries, table->capacity, key, value,
                        &table->length);
}

size_t ht_length(ht* table) {
    return table->length;
}

hti ht_iterator(ht* table) {
    hti it;
    it._table = table;
    it._index = 0;
    return it;
}

bool ht_next(hti* it) {
    // Loop till we've hit end of entries array.
    ht* table = it->_table;
    while (it->_index < table->capacity) {
        size_t i = it->_index;
        it->_index++;
        if (table->entries[i].key != NULL) {
            // Found next non-empty item, update iterator key and value.
            ht_entry entry = table->entries[i];
            it->key = entry.key;
            it->value = entry.value;
            return true;
        }
    }
    return false;
}

merged_ht* merged_ht_create(int numEntries){
     // Allocate space for hash table struct.
    merged_ht* table = malloc(sizeof(merged_ht) * numEntries);
    if (table == NULL) {
        return NULL;
    }

    return table;
}

void merged_ht_destroy(merged_ht* table){
     free(table);
}

void merge(merged_ht* mergedTable, int l, int m, int r){
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    // Vengono creati degli array temporanei
    int L1[n1], R1[n2];
    char L2[n1][WORD], R2[n2][WORD];
 
    // Vengono riempiti
    for (i = 0; i < n1; i++){
        strcpy(L2[i], mergedTable[l + i].word);
        L1[i] = mergedTable[l + i].freq;
    }
    for (j = 0; j < n2; j++){
        strcpy(R2[j], mergedTable[m + 1 + j].word);
        R1[j] = mergedTable[m + 1 + j].freq;
    }

    // Vengono uniti gli array temporanei negli array di partenza nella struttura dati
    i = 0; // Indice iniziale del primo sottoarray
    j = 0; // Indice iniziale del secondo sottoarray
    k = l; // Indice iniziale del sottoarray unito

    while (i < n1 && j < n2) {
        if (L1[i] > R1[j]) {
            strcpy(mergedTable[k].word, L2[i]);
            mergedTable[k].freq = L1[i];

            i++;
        }
        else if(L1[i] < R1[j]){
            strcpy(mergedTable[k].word, R2[j]);
            mergedTable[k].freq = R1[j];
            
            j++;
        } else {
            if(strcmp(L2[i], R2[j]) < 0){
                strcpy(mergedTable[k].word, L2[i]);
                mergedTable[k].freq = L1[i];

                i++;
            } else {
                strcpy(mergedTable[k].word, R2[j]);
                mergedTable[k].freq = R1[j];
                
                j++;
            }
        }
        k++;
    }
 
    // Vengono copiati gli elementi rimanenti del primo sottoarray, se ce ne sono
    while (i < n1) {
        strcpy(mergedTable[k].word, L2[i]);
        mergedTable[k].freq = L1[i];
        i++;
        k++;
    }
 
    // Vengono copiati gli elementi rimanenti del secondo sottoarray, se ce ne sono
    while (j < n2) {
        strcpy(mergedTable[k].word, R2[j]);
        mergedTable[k].freq = R1[j];
        j++;
        k++;
    }
}

void mergeSort(merged_ht* mergedTable, int l, int r){
    if (l < r) {
        // Uguale a (l+r)/2, ma evita l'overflow per l e h grandi
        int m = l + (r - l) / 2;
 
        // Ordiniamo ricorsivamente la prima e la seconda metà degli array
        mergeSort(mergedTable, l, m);
        mergeSort(mergedTable, m + 1, r);
 
        merge(mergedTable, l, m, r);
    }
}

void setword(merged_ht *table, const char *string){
    strcpy(table->word, string);
}