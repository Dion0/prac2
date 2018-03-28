#ifndef __SERVER_HPP__
#define __SERVER_HPP__

#include <netinet/in.h>
#include <sys/select.h>

class Server{
	int listen_sock;
	bool listening;
	struct sockaddr_in address;

	fd_set read_fds;
	int max_d; 
	int *client_socks;
	//TODO: actually use it (v) in .cpp
	struct sockaddr_in *client_addr; 
	int client_cnt, client_capacity;
	const int client_base, client_add;
public:
	enum class SERV_ERRORS{
		SOCKET_ERR,
		BIND_ERR,
		LISTEN_ERR,
		ACCEPT_ERR
	};
	SERV_ERRORS ERR_RES;
	
	Server();	
	int run();
	int add_client(int fd);
	void gen_fd_set();
	void process_fd_set();
	void handle_input();
	~Server();
};

#endif

