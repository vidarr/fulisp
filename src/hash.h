/**
 * (C) 2010 Michael J. Beer
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
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
    unsigned int(* hashFunc)(struct HashTable *, char *);
};



/**
 * Creates new hash table 
 * @param n number of buckets of the hash table
 * @param hashFunc the hashing function to use
 * @return hash table
 */
struct HashTable * hashTableCreate(unsigned int n, unsigned int(* hashFunc)(struct
            HashTable *, char *));  



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
void *hashTableSet(struct HashTable * hashTable, char *key, void *val);


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
void *hashTableGet(struct HashTable * hashTable, char *key);


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
