#ifndef __mapreduce_h__
#define __mapreduce_h__

#include <string>
#include <list>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <pthread.h>
#include <iostream>

typedef std::unordered_map<std::string, std::list<std::string>> kv_list;

class Getter{
public:
	Getter();
	std::string get(std::string, int);
private:
	std::unordered_map<unsigned long int, kv_list>::iterator thread_it;
	std::list<std::string>::const_iterator value_it;
	bool value_it_init;
};

// Different function pointer types used by MR

typedef void (*Mapper)(std::string file_name);

typedef void (*Reducer)(std::string key, Getter * get_func, int partition_number);

typedef unsigned long (*Partitioner)(std::string key, int num_partitions);

// External functions: these are what you must define
void MR_Emit(const std::string& key, const std::string& value);

unsigned long MR_DefaultHashPartition(std::string key, int num_partitions);

void MR_Run(std::vector<std::string>filenames, 
			Mapper map, int num_mappers, 
			Reducer reduce, int num_reducers, 
			Partitioner partition = MR_DefaultHashPartition);

#endif // __mapreduce_h__
