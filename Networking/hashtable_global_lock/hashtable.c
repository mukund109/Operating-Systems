#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdlib.h> // abs
#include <assert.h>
#include <string.h> //strdup
#include "rwlock.h"
#include "hashtable.h"

typedef struct list{
	int key;
	char * value;
	struct list * next;
} list;


// returns pointer to node, or NULL if key isn't found
list * search(list * l, int key){
	while(l){
		if(key == l->key)
			return l;
		l = l->next;
	}
	return NULL;
}

// if bucket is NULL, then a new one is allocated
// returns the newly allocated node
list * list_insert(int key, char * value, list * bucket){

	// if key already exists, overwrite value
	list * node;
	if((node = search(bucket, key))){
		free(node->value);
		node->value = strdup(value);
		return node;
	}

	// if key doesn't exist, create new node
	list * parent = NULL;
	list * child = bucket;
	while(child!=NULL){
		parent = child;
		child = child->next;
	}
	child = (list *) malloc(sizeof(list));
	child->key = key;
	child->value = strdup(value);
	child->next = NULL;	
	if(parent)
		parent->next = child;
	return child;
}

// safely deletes list 
void free_list(list * l){
	list * temp;
	while(l){
		temp = l->next;
		free(l->value);
		free(l);
		l = temp;
	}
}


// returns new head
list * delete_if_exists(list * l, int key){
	if(!l){return NULL;}
	list * ret  = l;
	list * parent = l;
	if(l->key == key){
		ret = l->next;
		free(l->value);
		free(l);
		return ret;
	}
	
	while(l){
		if(l->key == key){
			parent->next = l->next;
			free(l->value);
			free(l);
			return ret;
		}
		parent = l;
		l = l->next;
	}
	return ret;
}

void print_list(list* l){
	while(l){
		printf("%d : %s \n", l->key, l->value);
		l = l->next;
	}
}

/* Hash Table */

typedef struct HashTable{
	list* * buckets;	
	rwlock_t * global_lock;
	unsigned int capacity;
} HashTable;


unsigned int hash_fn(int key){
	return abs(key);
}

HashTable * create_hashtable(unsigned int capacity){
	HashTable * ret = malloc(sizeof(HashTable));
	ret->buckets = malloc(capacity*sizeof(list*));
	ret->capacity = capacity;
	ret->global_lock = malloc(sizeof(rwlock_t));
	rwlock_init(ret->global_lock);
	for(int i = 0; i<capacity; i++){
		ret->buckets[i] = NULL;
	}
	return ret;
}

void free_table(HashTable * table){
	for (int i = 0; i<table->capacity; i++){
		free_list(table->buckets[i]);	
	}
	free(table->buckets);
	rwlock_destroy(table->global_lock);
	free(table);
}

void print_table(HashTable * table){
	rwlock_acquire_readlock(table->global_lock);
	for(int i = 0; i<table->capacity; i++){
		print_list(table->buckets[i]);
	}
	rwlock_release_readlock(table->global_lock);
}
void insert(HashTable * table, int key, char * value){
	unsigned int index = hash_fn(key) % table->capacity;
	rwlock_acquire_writelock(table->global_lock);
	
	if (table->buckets[index] == NULL)
		table->buckets[index] = list_insert(key, value, NULL);
	else
		list_insert(key, value, table->buckets[index]);
	
	rwlock_release_writelock(table->global_lock);
}

char * get(HashTable * table, int key){
	unsigned int index = hash_fn(key) % table->capacity;
	rwlock_acquire_readlock(table->global_lock);
	list * node = search(table->buckets[index], key);
	if(!node) {rwlock_release_readlock(table->global_lock); return NULL;}
	char * rv = strdup(node->value);
	rwlock_release_readlock(table->global_lock);
	return rv;
}

// doesn't raise an error if key doesn't exist
void delete_entry(HashTable* table, int key){
	unsigned int index = hash_fn(key) % table->capacity;
	rwlock_acquire_writelock(table->global_lock);
	table->buckets[index] = delete_if_exists(table->buckets[index], key);
	rwlock_release_writelock(table->global_lock);
}

int run_tests(){
	printf("Test 1, inserting kv pairs\n");
	list * a = list_insert(4, "habersasherie", NULL);
	list_insert(6, "joyless", a);
	list_insert(34, "kumbaya", a);
	printf("head: %p \n", a);
	print_list(a);

	printf("\nTest 2, searching\n");
	list * ret;
	if((ret = search(a, 8)))
		printf("Search result %d : %s \n", ret->key, ret->value);
	else
		printf("Search result %d : value not found \n", 8);
	
	printf("\nTest 3, overwriting key 4\n");
	list_insert(4, "badmash", a);
	printf("head: %p \n", a);
	print_list(a);

	printf("\nTest 4, deleting head - 4\n");
	a = delete_if_exists(a, 4);
	printf("head: %p \n", a);
	print_list(a);
	
	printf("\nInserting new elements\n");
	list_insert(93, "bruh", a);
	//print_list(a);
	list_insert(32, "cable_guy", a);
	printf("head: %p \n", a);
	print_list(a);
	
	printf("\nTest 5, deleting non element 100\n");
	a = delete_if_exists(a, 100);
	printf("head: %p \n", a);
	print_list(a);

	printf("\nTest 6, deleting a few more\n");
	a = delete_if_exists(a, 6);
	a = delete_if_exists(a, 93);
	printf("head: %p \n", a);
	print_list(a);
	
	free_list(a);
	
	printf("\nTest 7, creating hashtable\n");
	HashTable * table = create_hashtable(10);
	printf("created table of capacity %d \n", table->capacity);
	print_table(table);

	printf("\nTest 8, inserting into table\n");
	insert(table, -4, "Buoyancy");
	insert(table, 4, "Concurrency");
	insert(table, 81, "bittoo");
	insert(table, 2394, "kawasaki");
	print_table(table);

	printf("\nTest 9, retrieving from table\n");
	char * val1 = get(table, 5);
	char * val2 = get(table, 2394);
	char * val3 = get(table, 4);
	printf("5:%s, 2394:%s, 4:%s \n", val1, val2, val3);
	print_table(table);

	printf("\nTest 10, deleting from table\n");
	delete_entry(table, 5);
	delete_entry(table, -4);
	delete_entry(table, 81);
	print_table(table);

	free(val2); free(val3);
	
	free_table(table);
	return 0;
}

