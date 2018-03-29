#include "Logger.hpp"

#include <cstdio>
#include <ctime>

Logger *Logger::p_instance = nullptr;
const char *Logger::default_file = "logs/log.txt";

Logger::Logger(){
	f = nullptr;
	set_file(default_file);
	log("Logger constructed");
}

Logger *Logger::instance(){
	if (p_instance == nullptr){
		p_instance = new Logger;
	}
	return p_instance;
}

void Logger::set_file(const char *filename, const char* mode){
	if (f != nullptr){
		fclose(f);
	}
	f = fopen(filename, mode);
	if (f == nullptr){
		perror("error in set_file:\n");
	}
}

int Logger::log(const char* str, bool print_time){
	if (f == nullptr){
		fprintf(stderr, "you have to set_file before logging\n");
		return -1;
	}
	if (!print_time){
		fprintf(f, "\t%s\n", str);
		return 0;
	}
	std::time_t cur_time = std::time(nullptr);
	std::tm *now = std::localtime(&cur_time);
	fprintf(f, "[%d-%d-%d %d:%d:%d]: %s\n", now->tm_year+1900,
			now->tm_mon+1, now->tm_mday,
			now->tm_hour, now->tm_min, now->tm_sec, str);
	return 0;
}

void Logger::operator delete(void *ptr, std::size_t sz){
	printf("logger delete call\n");
}

Logger::~Logger(){
	if (p_instance != nullptr)
		::operator delete(p_instance);
	printf("~Logger\n");
}

