#include "mapreduce.h"

std::unordered_map<unsigned long int, kv_list> data;
std::hash<std::string> hash_fn;

struct arg_t{
	Mapper map;
	std::string filename;
};

void * mapper_thread(void * args){
	arg_t* params = (arg_t *) args;

	auto filename = params->filename;
	auto map_func = params->map;
	map_func(filename);
	
	delete params;
	return NULL;
}


Getter::Getter(): thread_it(data.begin()), value_it_init(false){}
	
std::string Getter::get(std::string key, int partition_number){
		
	while(thread_it!=data.end()){
		if(thread_it->second.count(key)==0){
			thread_it++;
			continue;
		}
		const auto& value_list = thread_it->second[key];
		
		if(!value_it_init){
			value_it = value_list.begin();
			value_it_init = true;
		}
		
		while(value_it!=value_list.end()){
			auto rv = *value_it;
			value_it++;
			return rv;
		}
		
		value_it_init = false;
		thread_it++;
	}
	return "";
}

struct argr_t{
	Reducer reduce;
	Partitioner partition;
	unsigned int pn;
	int num_partitions;
};

void * reducer_thread(void * args){
	
	//unpack arguments
	auto argr = (argr_t *) args;
	Reducer reduce = argr->reduce;
	Partitioner pf = argr->partition;
	unsigned int pn = argr->pn;
	
	int num_partitions = argr->num_partitions;

	//get keys associated with this partition
	std::unordered_set<std::string> keys;
	for (auto& t: data){
			for(auto& k: t.second){
				if(pf(k.first, num_partitions) == pn){
					keys.insert(k.first);
				}
			}
	}
	
	//invoke the 'reduce' function for every key
	for (auto& key: keys){
		Getter get_fn;
		reduce(key, &get_fn, pn);
	}
	
	delete argr;
	return NULL;
}

void MR_Emit(const std::string& key, const std::string& value){
	
	unsigned long int id = pthread_self();
	data[id][key].push_back(value);
}

unsigned long MR_DefaultHashPartition(std::string key, int num_partitions) {

    return hash_fn(key) % num_partitions;
}

void MR_Run(std::vector<std::string>filenames, Mapper map, 
			int num_mappers, Reducer reduce, int num_reducers, 
			Partitioner partition){

	data.reserve(num_mappers);
	
	//Mapper threads 
	std::vector<pthread_t> threads(num_mappers, pthread_t());
	unsigned int i = 0;
	for (auto& thread : threads){
		arg_t * args = new arg_t({map, filenames[i]});
		pthread_create(&thread, NULL, mapper_thread, (void *) args);
		i++;
	}
	
	for (auto& thread : threads){
		pthread_join(thread, NULL);
	}
	

	std::vector<pthread_t> rthreads(num_reducers, pthread_t());
	i = 0;
	for (auto& thread : rthreads){
		argr_t * args = new argr_t({reduce, partition, i, num_reducers});
		pthread_create(&thread, NULL, reducer_thread, (void *) args);
		i++;
	}
	for (auto& thread : rthreads){
		pthread_join(thread, NULL);
	}
	
}

