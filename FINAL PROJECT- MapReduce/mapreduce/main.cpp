#include <iostream>
#include "mapreduce.h"
#include <pthread.h>
#include <unistd.h>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>

using c = std::chrono::system_clock;
using ms = std::chrono::milliseconds;

std::string str_transform(std::string s) {
	s.erase(std::remove_if(s.begin(), s.end(), 
			[](char c){ return c==','||c=='.'|| c=='!' || c==':' || c=='\''; }),
			s.end());
							
    std::transform(s.begin(), s.end(), s.begin(), 
                   [](unsigned char c)
				   { return std::tolower(c); });
    return s;
}

// Defining map function
void map(std::string file){
	std::ifstream f(file);
	std::string line;
	std::string word;
	if(f.is_open()){
		while(getline(f, line)){
		
			std::istringstream iss(line);
			while (iss >> word){
				MR_Emit(str_transform(word), "1");
			}
		}
		f.close();
	}
}


//Output files
std::ofstream out1, out2;


//Defining reduce function
void reduce(std::string key, Getter * get_func, int partition_number){
	std::string val = get_func->get(key, partition_number);
	int count = 0;
	while(val!=""){
		count += std::stoi(val);
		val = get_func->get(key, partition_number);
	}
	
	if (partition_number==0 and out1.is_open()){
		out1 << key << ", " << count << "\n";
	}
	else if(partition_number==1 and out2.is_open()){
		out2 << key << ", " << count << "\n";
	}
}

//single-threaded wordcount for comparison
void wordcount(std::vector<std::string> filenames){

	std::ofstream out("single-threaded.out");
	std::unordered_map<std::string, int> data;
	
	for(auto& file : filenames){
		
		std::ifstream f(file);
		std::string line;
		std::string word;
		if(f.is_open()){
			while(getline(f, line)){
				std::istringstream iss(line);
				while (iss >> word){
					word = str_transform(word);
					if(data.count(word)==0){
						data[word] = 1;
					}
					else
						data[word] += 1;
				}
			}
			f.close();
		}
	}
	
	for (auto it = data.begin(); it != data.end(); it++){
		out << it->first << ", " << it->second << "\n";
	}
	out.close();
}

int main(int argc, char **argv)
{
	std::vector<std::string> files = {"1.txt", "2.txt", "3.txt", 
	"4.txt"};//, "5.txt", "6.txt", "7.txt", "8.txt","9.txt", "10.txt", "11.txt"};
	auto before = c::now();
	out1 = std::ofstream("1.out");
	out2 = std::ofstream("2.out");
	MR_Run(files, map, 4, reduce, 4);
	out1.close();
	out2.close();
	auto duration = std::chrono::duration_cast<ms>(c::now() - before);
	std::cout << "Multi-threaded mapreduce: " << duration.count()/1000.0 << "ms" << std::endl;
	
	before = c::now();
	wordcount(files);
	duration = std::chrono::duration_cast<ms>(c::now() - before);
	std::cout << "Single-threaded: " << duration.count()/1000.0 << "ms" << std::endl;
	
	return 0;
}
