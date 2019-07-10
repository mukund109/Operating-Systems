#ifndef HASHTABLE_H
#define HASHTABLE_H

typedef struct HashTable HashTable;

HashTable * create_hashtable(unsigned int capacity);

void free_table(HashTable * table);

void print_table(HashTable * table);

void insert(HashTable * table, int key, char * value);

// returns a malloced copy of value
char * get(HashTable * table, int key);

// deletes quietly if it key exists
void delete_entry(HashTable* table, int key);

int run_tests();
#endif
