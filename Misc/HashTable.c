#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h> // abs
#include <assert.h>
#include <string.h> //strdup

typedef struct list{
	int key;
	char * value;
	struct list * next;
} list;

typedef struct HashTable{
	list* *buckets;
	
	unsigned int size;
} HashTable;


unsigned int hash_fn(int key, unsigned int size){
	return abs(key) % size;
}

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
list * insert(int key, char * value, list * bucket){

	// if key already exists, overwrite value
	list * node;
	if((node = search(bucket, key))){
		free(node->value);
		node->value = strdup(value);
		return node;
	}

	printf("bucket : %p \n ", bucket);
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

H



int main(int argc, char** argv){
	printf("Test 1, inserting kv pairs\n");
	list * a = insert(4, "habersasherie", NULL);
	insert(6, "joyless", a);
	insert(34, "kumbaya", a);
	printf("head: %p \n", a);
	print_list(a);

	printf("\nTest 2, searching\n");
	list * ret;
	if((ret = search(a, 8)))
		printf("Search result %d : %s \n", ret->key, ret->value);
	else
		printf("Search result %d : value not found \n", 8);
	
	printf("\nTest 3, overwriting key 4\n");
	insert(4, "badmash", a);
	printf("head: %p \n", a);
	print_list(a);

	printf("\nTest 4, deleting head - 4\n");
	a = delete_if_exists(a, 4);
	printf("head: %p \n", a);
	print_list(a);
	
	printf("\nInserting new elements\n");
	insert(93, "bruh", a);
	//print_list(a);
	insert(32, "cable_guy", a);
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

	return 0;
}

