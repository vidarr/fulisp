/*
 * (C) 2010 Michael J. Beer
 * All rights reserved.
 *
 * Redistribution  and use in source and binary forms, with or with‐
 * out modification, are permitted provided that the following  con‐
 * ditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above  copy‐
 * right  notice,  this  list  of  conditions and the following dis‐
 * claimer in the documentation and/or other materials provided with
 * the distribution.
 *
 * 3.  Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote  products  derived
 * from this software without specific prior written permission.
 *
 * THIS  SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBU‐
 * TORS "AS IS" AND ANY EXPRESS OR  IMPLIED  WARRANTIES,  INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE  ARE  DISCLAIMED.  IN  NO  EVENT
 * SHALL  THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DI‐
 * RECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS IN‐
 * TERRUPTION)  HOWEVER  CAUSED  AND  ON  ANY  THEORY  OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING  NEGLI‐
 * GENCE  OR  OTHERWISE)  ARISING  IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef __HASH_C__
#define __HASH_C__

#define HASHTABLE_STD_SIZE 15

#define HASH_TABLE_SAFE_KEY

struct HashEntry {
    char *key;
    void *value;
    struct HashEntry *next;
};

struct HashTable {
    struct HashEntry **hash;
    unsigned int size;
    unsigned int (*hashFunc)(struct HashTable *, char *);
};

/**
 * Creates new hash table
 * @param n number of buckets of the hash table
 * @param hashFunc the hashing function to use
 * @return hash table
 */
struct HashTable *hashTableCreate(unsigned int n,
                                  unsigned int (*hashFunc)(struct HashTable *,
                                                           char *));

/**
 *  Deletes an hash table
 *  @param delete if != 0 the key will be freed
 */
void hashTableDispose(struct HashTable *hash);

/**
 * Associates a key eith a value
 * If existent, the old value will be deleted
 * If not existent, an entry will be created
 * @param hashTable the hash table to be used
 * @param key
 * @param val Pointer to the value (void)
 *
 * @return pointer to the old value or NULL if previously not set
 */
void *hashTableSet(struct HashTable *hashTable, char *key, void *val);

/**
 * Deletes an entry of an hash table
 * @param hashTable The hash to be used
 * @param hashTable the hash table to be used
 * @param key the key of the entry to be deleted
 * @param delete if != 0, the key will be freed
 *
 * @return pointer to the value that has been deleted, or 0 if entry not found
 */
void *hashTableDelete(struct HashTable *hashTable, char *key);

/**
 * Returns the value associated to a key
 * @param hashTable the hash to be used
 * @param key the key to be searched
 *
 * @return pointer to the value ass. to the key, or 0 if not found
 */
void *hashTableGet(struct HashTable *hashTable, char *key);

/**
 * Standard hash function
 * @param key the key to be mapped onto a number
 *
 * @return an hash function suited for n buckets
 */
unsigned int stdHashFunction(struct HashTable *hash, char *key);

/**
 *  Returns an array containing pointers to all keys of a paticular hash table
 *  The array is terminated by a null pointer
 *  The keys themselves will not be copied!
 *  @param hash the hash table to be evaluated
 *  @return an array containing pointers to every key kept within the hash
 */
char **hashTableKeys(struct HashTable *hash);

#endif
