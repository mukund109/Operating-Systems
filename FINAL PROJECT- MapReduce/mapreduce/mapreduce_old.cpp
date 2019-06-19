//#include "mapreduce.h"
//#include <mutex>
//#include <list>
//#include <string>
//
//// no hash tables because 
//// 1. lists easier to implement
//// 2. rehashing can get complicated with locks
//// 3. the STL implementation isn't thread safe
//
//struct Node{
//	std::mutex lock;
//	std::string key;
//	std::list<std::string> values; 
//	Node * next;
//};
//
//Node * head = NULL;
//std::mutex global_write_lock;
//
//// find the key in the list, inserts if not found
//Node * lookup_insert(std::string key){
//	global_write_lock.lock();
//	Node * current = head;
//	Node * tail = current;
//	
//	while (current != NULL){
//		if(current->key == key){
//			global_write_lock.unlock();
//			return current;
//		}
//		tail = current;
//		current = head->next;
//	}
//	
//	tail->next = new Node;
//	tail = tail->next;
//	tail->key = key;
//	global_write_lock.unlock();
//	return tail;
//}
//
//void insert_value(Node * node, std::string value){
//	node->lock.lock();
//	node->values.push_back(value);
//	node->lock.unlock();
//}
//
