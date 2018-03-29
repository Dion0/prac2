#ifndef __SERVER_HPP__
#define __SERVER_HPP__

#include <netinet/in.h>
#include <sys/select.h>
#include <vector>

#include "Logger.hpp"

class Server{
	const int client_base, client_buffer_base;
	int listen_sock;
	bool listening;
	struct sockaddr_in address;
	Logger *logger;
	
	fd_set read_fds;
	int max_d;
	std::size_t client_cnt;
	std::vector<int> client_socks;
	std::vector< std::vector<char> > client_buffer;
	//TODO: actually use it (v) in .cpp
	std::vector<struct sockaddr_in> client_addr; 
	std::vector<socklen_t> client_len;
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
	int add_client(int fd, struct sockaddr_in s, socklen_t l);
	char *client_str(int ind) const;
	char *client_str(struct sockaddr_in s, int sd) const;
	void handle_client(std::size_t ind);
	void handle_input();
	void gen_fd_set();
	void log(const char* str, int client_num = -1);
	void process_fd_set();
	void print_clients() const;
	int remove_client(std::size_t ind);
	
	void print_client(int ind) const;
	void print_client(struct sockaddr_in s, int sd) const;
	~Server();
};

#endif

