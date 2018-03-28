#ifndef __CLIENT_HPP__
#define __CLIENT_HPP_

#include <netinet/in.h>

class Client{
	bool running;
	int sock;
	struct sockaddr_in serv_addr;
public:
	Client();
	int set_serv_addr(const char * serv_ip, int port);
	int run();
	~Client();
};

#endif

