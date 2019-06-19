#include <iostream>
#include "mapreduce.h"
#include <pthread.h>
#include <unistd.h>
#include <string>

void map_dummy(std::string file){
	if (file=="1.txt")
		MR_Emit("hi", "low");
	else if (file=="2.txt")
		MR_Emit("black", "white");
	else if (file=="3.txt")
		MR_Emit("hot", "cold");
	else if (file=="4.txt"){
		MR_Emit("big", "small");
		MR_Emit("big", "tiny");
		MR_Emit("gigantic", "miniscule");
	}
}

void reducer_dummy(std::string key, Getter * get_func, int partition_number){
	std::string val = get_func->get(key, partition_number);
	std::string rv = "";
	while(val!=""){
		rv += val;
	}
	std::cout << "Key: " << key << ", reduced value: " << rv << std::endl;
}


int main(int argc, char **argv)
{
	MR_Run({"1.txt", "2.txt", "3.txt", "4.txt"}, map_dummy, 4, reducer_dummy, 5);
	return 0;
}
