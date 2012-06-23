#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <test.h>
#include <string.h>


char *getString(unsigned int i) {
    char *str = (char *)malloc(sizeof(char) * 50);
    sprintf(str, "schluessel%u", i);
    return str;
}


void *getVal(unsigned int i) {
        int *val = (int *)malloc(sizeof(int));
        *(val) = i;
        return (void *)val;
}


void fillHash(struct HashTable *hash, int no) {
    int i = 0;
    void *val;
    char *str;
    for(i = 0; i < no; i++) {
        val = getVal(i);
        str = getString(i);
        hashTableSet(hash, str, val);
        free(str);
    };
}


int testHashTable(int n, int m) {  
    int i;
    int *val;
    int result = 0;
    char *str;
    char **keys;
    struct HashTable *hash = hashTableCreate(n, 0);
    fillHash(hash, m);
    for(i = 0; i < m; i++) {
        str = getString(i);
        val = hashTableGet(hash, str);
        free(str);
        result = result || ((val && *val == i) ? 0 : 1);
    };

    for(i = m - 1; i > 0; i -= 2) {
        str = getString(i);
        val = hashTableDelete(hash, str);
        result = result || ((hashTableGet(hash, str)) ? 1 : 0);
        if(val) free(val);
        free(str);
    };

    keys = hashTableKeys(hash);
    i = 0;
    while(keys[i++] != 0);
    free(keys);

    result = result || ( i - 1 == (m - 1) / 2);
    
    for(i = m - 2; i > 0; i -= 2) {
        str = getString(i);
        val = hashTableGet(hash, str);
        result = result || ((val && *val == i) ? 0 : -1);
        free(str);
    };

    /* Check whether requests on keys that are not present are answered with
     * NULL as expected */
    for(i = m + 2; i < m + 10; i += 1) {
        str = getString(i);
        val = hashTableGet(hash, str);
        result = result || !(val == NULL);
        val = hashTableSet(hash, str, getVal(i));
        result = result || !(val == NULL);
    };

    keys = hashTableKeys(hash);
    i = 0;
    while(keys[i] != 0) {
        val = hashTableGet(hash, keys[i++]); 
        result = result || (val == 0);
        if(val != 0) free(val);
    };

    hashTableDispose(hash);

    free(keys);
    return result;
}



int main(int argc, char **argv) {
    int res;

    printf("Testing hash.c\n");

    res = test(testHashTable(1,100), "HashTable with 1 bucket/ 100 entries"); 
    res = res | test(testHashTable(20, 10), "HashTable with 20 buckets/ 10 entries"); 
    res = res | test(testHashTable(100, 150), "HashTable with 100 buckets/ 150 entries"); 
    res = res | test(testHashTable(100, 1000), "HashTable with 100 buckets/ 1000 entries"); 
    res = res | test(testHashTable(1000, 10000), "HashTable with 1000 buckets/ 10000 entries"); 

    return res;
}

