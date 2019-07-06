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

list * insert(int key, char * value, list * bucket){
	list * parent = NULL;
	list * child = bucket;
	while(child){
		parent = child;
		child = child->next;
	}
	child = (list *)malloc(sizeof(list));
	child->key = key;
	child->value = strdup(value);
	if(parent)
		parent->next = child;
	return child;
}

void free_list(list * l){
	list * temp;
	while(l){
		temp = l->next;
		free(l->value);
		free(l);
		l = temp;
	}
}

list * search(list * l, int key){
	while(l){
		if(key == l->key)
			return l;
		l = l->next;
	}
	return NULL;
}

// returns new head
list * delete_if_exists(list * l, int key){
	if(!l){return;}
	list * ret  = l, parent = l;
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
			return;
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

int main(int argc, char** argv){
	printf("Test 1, inserting kv pairs\n");
	list * a = insert(4, "habersasherie", NULL);
	insert(6, "joyless", a);
	insert(34, "kumbaya", a);
	print_list(a);

	printf("\n Test 2, searching\n");
	list * ret;
	if((ret = search(a, 8)))
		printf("Search result %d : %s \n", ret->key, ret->value);
	else
		printf("Search result %d : value not found \n", 8);
	
	printf("Test 3, deleting key 4");
	free_list(a);

	return 0;
}

