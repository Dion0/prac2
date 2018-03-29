#include "Logger.hpp"

int main(){
	Logger *i = Logger::instance();
	i->log("xdd\n");
	i->set_file("test2.txt");
	i->log("loool 4HEad");
}


