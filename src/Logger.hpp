#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include <cstdio>

class Logger{
	static Logger *p_instance;
	static const char* default_file;
   
	FILE *f;
	
	Logger();
	static void operator delete(void *l, std::size_t sz);
	~Logger();
public:
	Logger(Logger const&) =delete;
	Logger& operator=(Logger const&) =delete;

	static Logger *instance();
	void set_file(const char* fname, const char* mode = "a");
	int log(const char* str, bool print_time = true);
};


#endif

