// ============================================
// The HashTable class file
//
// Copyright 2023 Georgia Tech. All rights reserved.
// The materials provided by the instructor in this course are for
// the use of the students currently enrolled in the course.
// Copyrighted course materials may not be further disseminated.
// This file must NOT be made publicly available anywhere.
//==================================================================

/****************************************************************************
 * Include the Public Interface
 *
 * By including the public interface at the top of the file, the compiler can
 * enforce that the function declarations in the the header are not in
 * conflict with the definitions in the file. This is not a guarantee of
 * correctness, but it is better than nothing!
 ***************************************************************************/
#include "hash_table.h"

/****************************************************************************
 * Include other private dependencies
 *
 * These other modules are used in the implementation of the hash table module,
 * but are not required by users of the hash table.
 ***************************************************************************/
#include <stdlib.h> // For malloc and free
#include <stdio.h>  // For printf

/****************************************************************************
 * Hidden Definitions
 *
 * These definitions are not available outside of this file. However, because
 * the are forward declared in hash_table.h, the type names are
 * available everywhere and user code can hold pointers to these structs.
 ***************************************************************************/
/**
 * This structure represents an a hash table.
 * Use "HashTable" instead when you are creating a new variable. [See top comments]
 */
struct _HashTable
{
    /** The array of pointers to the head of a singly linked list, whose nodes
        are HashTableEntry objects */
    HashTableEntry **buckets;

    /** The hash function pointer */
    HashFunction hash;

    /** The number of buckets in the hash table */
    unsigned int num_buckets;
};

/**
 * This structure represents a hash table entry.
 * Use "HashTableEntry" instead when you are creating a new variable. [See top comments]
 */
struct _HashTableEntry
{
    /** The key for the hash table entry */
    unsigned int key;

    /** The value associated with this hash table entry */
    void *value;

    /**
     * A pointer pointing to the next hash table entry
     * NULL means there is no next entry (i.e. this is the tail)
     */
    HashTableEntry *next;
};

/****************************************************************************
 * Private Functions
 *
 * These functions are not available outside of this file, since they are not
 * declared in hash_table.h.
 ***************************************************************************/
/**
 * createHashTableEntry
 *
 * Helper function that creates a hash table entry by allocating memory for it on
 * the heap. It initializes the entry with key and value, initialize pointer to
 * the next entry as NULL, and return the pointer to this hash table entry.
 *
 * @param key The key corresponds to the hash table entry
 * @param value The value stored in the hash table entry
 * @return The pointer to the hash table entry
 */
static HashTableEntry *createHashTableEntry(unsigned int key, void *value)
{
    // Allocate space needed for HashTableEntry, the key, and the value
    HashTableEntry* entry = (HashTableEntry*) malloc(sizeof(HashTableEntry));
    if (entry != NULL) {
        //Pointer to next entry as NULL
        entry->next = NULL;
        //Initializes key and value for entry
        entry->key = key;
        entry->value = value;
    }
    //return the entry whether or not it was NULL
    return entry;
}

/**
 * findItem
 *
 * Helper function that checks whether there exists the hash table entry that
 * contains a specific key.
 *
 * @param hashTable The pointer to the hash table.
 * @param key The key corresponds to the hash table entry
 * @return The pointer to the hash table entry, or NULL if key does not exist
 */
static HashTableEntry *findItem(HashTable *hashTable, unsigned int key)
{
    //use index of hash to go to correct bucket within hash
    HashTableEntry* temp = hashTable -> buckets[hashTable -> hash(key)];
    //check to make sure entry is not null
    while(temp != NULL) {  
        //if key within item matches, return item
        int tempKey = temp -> key;
        if (tempKey != key) {
            //go to next temporary key
            temp = temp -> next;
        } else {
            //return temp once matching key is found
            return temp;
        }
    }
    //if key not found, return NULL
    return NULL;
}

/****************************************************************************
 * Public Interface Functions
 *
 * These functions implement the public interface as specified in the header
 * file, and make use of the private functions and hidden definitions in the
 * above sections.
 ****************************************************************************/
// The createHashTable is provided for you as a starting point.
HashTable *createHashTable(HashFunction hashFunction, unsigned int numBuckets)
{
    // The hash table has to contain at least one bucket. Exit gracefully if
    // this condition is not met.
    if (numBuckets == 0)
    {
        printf("Hash table has to contain at least 1 bucket...\n");
        exit(1);
    }

    // Allocate memory for the new HashTable struct on heap.
    HashTable *newTable = (HashTable *)malloc(sizeof(HashTable));

    // Initialize the components of the new HashTable struct.
    newTable->hash = hashFunction;
    newTable->num_buckets = numBuckets;
    newTable->buckets = (HashTableEntry **)malloc(numBuckets * sizeof(HashTableEntry *));

    // As the new buckets are empty, init each bucket as NULL.
    unsigned int i;
    for (i = 0; i < numBuckets; ++i)
    {
        newTable->buckets[i] = NULL;
    }

    // Return the new HashTable struct.
    return newTable;
}

void destroyHashTable(HashTable *hashTable)
{
    // TODO: Implement
    // 1. Loop through each bucket of the hash table to remove all items.
    //      1a. set temp to be the first entry of the ith bucket
    //      1b. delete all entries
    //Make sure hashTable is not NULL, which if case will run
    if (hashTable != NULL) {
        //iterate through each bucket
        for (int j = 0; j < hashTable -> num_buckets; j++) {
            //check that current bucket is not NULL
            while (hashTable -> buckets[j] != NULL) {
                //assign entry to the entry at bucket[j]
                HashTableEntry* entry = (HashTableEntry*)hashTable -> buckets[j];
                //assign nEntry to entrys next
                HashTableEntry* nEntry = entry -> next;
                //assign vEntry to entry value
                void* vEntry = entry -> value;
                // 2. Free buckets entry and value
                free(entry);
                free(vEntry);
                //iteratation for next bucket[j]
                hashTable -> buckets[j] = nEntry;
            }
            // go to next bucket to free its entries
        }
    }
    // 3. Free hash table
    //free bucket pointers once values and nexts within are freed        COULD BE PROBLEM
    //HashTableEntry* bucketPt = (HashTableEntry*)hashTable -> buckets;
    free(hashTable -> buckets);
    //free hashTable
    free(hashTable);
}

void *insertItem(HashTable *hashTable, unsigned int key, void *value)
{
    // TODO: Implement

    //1. First, we want to check if the key is present in the hash table.
    int index = hashTable -> hash(key);
    //check for preexsisting item under specified key
    HashTableEntry* entry = findItem(hashTable, key);
    //2. If the key is present in the hash table, store new value and return old value
    if(entry == NULL) {
        //3. If entry is NULL, create entry for new value and return NULL
        //create new entry
        entry = createHashTableEntry(key, value);
        //new entry has same next
        entry -> next = hashTable -> buckets[index];
        //save new entry under correct bucket in hashTable
        hashTable -> buckets[index] = entry;
        //return NULL
        return NULL; 
    }
    //if entry Not NULL, return prev and store new value
    //save previous value under old
    void* previous = entry -> value;
    //store the new value
    entry -> value = value;
    //return the prev value
    return previous;
}

void *getItem(HashTable *hashTable, unsigned int key)
{
    // TODO: Implement
    //1. First, we want to check if the key is present in the hash table.
    HashTableEntry* entry = findItem(hashTable, key);
    //2. If the key exist, return the value of entry
    //3. If not. just return NULL
    if (entry != NULL) {
        return entry -> value;
    }
    return NULL;
}

void *removeItem(HashTable *hashTable, unsigned int key)
{
    // TODO: Implement
    //1. Get the bucket number and the head entry
    //initialize entry, entrys value and entrys next
    HashTableEntry* entry;
    HashTableEntry* nEntry;
    void* vEntry;
    //check if the key is present in the hash table
    int index = hashTable -> hash(key);
    //check if bucket at correct index is in hashTable, else return NULL
    if (hashTable -> buckets[index] != NULL) {
        //2. If the head holds the key, change the head to the next value, and return the old value
        //assign entry to correct bucket
        entry = (HashTableEntry*)hashTable -> buckets[index];
        if (hashTable -> buckets[index] -> key == key) {
            //assign vEntry to value
            vEntry = entry -> value;
            //change head to next value
            hashTable -> buckets[index] = hashTable -> buckets[index] -> next;
            // entry found, free next Entry
            free(entry);
            return vEntry;
        }
        //3. If not the head, search for the key to be removed 
        //initialize current and go through if not first value in bucket
        HashTableEntry* current = entry -> next;
        HashTableEntry* previous = hashTable -> buckets[index];
        //while next is not NULL
        while (current != NULL) {
            //if match, remove and join right and left parts
            if (current -> key == key) {
                //value of next entry
                vEntry = (void*)current -> value;
                //nEntry now points to correct next
                nEntry = current -> next;
                // reassigns left next with right entry
                previous -> next = (HashTableEntry*)current -> next;
                //correct entry found, free next entry
                free(nEntry);
                return (vEntry);
            }
            //reassign previous to one on currently
            previous = current;
            //current now becomes next to check
            current = current -> next;
        }
        
    
    } 
    //4. If the key is not present in the list, return NULL
    //returns NULL if key not present in any part of the list
    return NULL;
    
}

void deleteItem(HashTable *hashTable, unsigned int key)
{
    // TODO: Implement
    //1. Remove the entry and free the returned data
    //use removeItem to remove Entry from Hash
    //HashTableEntry* entryDelete =  (HashTableEntry*)removeItem(hashTable, key);   //PROBLEM 
    //free returned data
    free(removeItem(hashTable, key));
}