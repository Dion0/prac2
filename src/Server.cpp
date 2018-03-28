#include "Server.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <new>

const int PORT = 8888;
const int c_base = 10;
const int c_add = 5;
const int stdin_buffer_size = 256;

Server::Server() : client_base(c_base), client_add(c_add){
	listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	listening = false;
	if (listen_sock == -1){
		ERR_RES = SERV_ERRORS::SOCKET_ERR;
		perror("couldn't create listen socket\n");
		return;
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
	int bind_res = bind(listen_sock, (struct sockaddr*) &address,
						sizeof(address));
	if (0 != bind_res){
		ERR_RES = SERV_ERRORS::BIND_ERR;
		perror("couldn't bind to socket\n");
		return;
	}
	client_cnt = 0;
	client_capacity = c_base;
	client_socks = new(std::nothrow) int[client_capacity];
	if (client_socks == nullptr){
		fprintf(stderr, "failed to allocate memory for "
				"client descriptors\n");
		client_capacity = 0;
	}
}

int Server::add_client(int fd){
	if (client_cnt >= client_capacity - 1){
		client_capacity += client_add;
		int *tmp = new(std::nothrow) int[client_capacity];
		if (tmp == nullptr){
			fprintf(stderr, "failed to allocate temporary "
					"array while adding client\n");
			return -1;
		}
		for (int i = 0; i < client_cnt; i++)
			tmp[i] = client_socks[i];
		delete [] client_socks;
		client_socks = tmp;
	}
	client_socks[client_cnt++] = fd;
	return 0;
}

void Server::gen_fd_set(){
	max_d = listen_sock > STDOUT_FILENO ?
		listen_sock : STDOUT_FILENO; //lmoa autism
	FD_ZERO(&read_fds);
	FD_SET(STDOUT_FILENO, &read_fds);
	FD_SET(listen_sock, &read_fds);
	for (int i = 0; i < client_cnt; i++){
		FD_SET(client_socks[i], &read_fds);
		if (client_socks[i] > max_d)
			max_d = client_socks[i];
	}
}

void Server::process_fd_set(){
	int t_sock;
	struct sockaddr_in t_addr;
	socklen_t t_addr_len = sizeof(t_addr);

	if (FD_ISSET(listen_sock, &read_fds)){
		t_sock = accept(listen_sock, (struct sockaddr*) &t_addr,
						&t_addr_len);
		if (-1 == t_sock){
			ERR_RES = SERV_ERRORS::ACCEPT_ERR;
			perror("failed to accept\n");
		}else{
			add_client(t_sock);
		}
	}
	if (FD_ISSET(STDOUT_FILENO, &read_fds)){
		handle_input();
	}
}

void Server::handle_input(){
	static char stdin_buffer[stdin_buffer_size];
	static ssize_t read_cnt;
	read_cnt = read(STDOUT_FILENO, stdin_buffer,
					stdin_buffer_size-1);
	stdin_buffer[read_cnt] = 0;
	if (read_cnt > 3){ 
		if (0 == strncmp("log", stdin_buffer, 3))
			printf("trying to log:%s", stdin_buffer+3);
		if (0 == strncmp("end", stdin_buffer, 3))
			listening = false;
	}
}

int Server::run(){
	if (-1 == listen(listen_sock, 5)){
		ERR_RES = SERV_ERRORS::LISTEN_ERR;
		perror("failed to listen\n");
		return -1;
	}

	listening = true;
					
	while (listening){
		gen_fd_set();
		int sel_res = select(max_d + 1, &read_fds, NULL, NULL,
							 NULL);
		if (sel_res < 1){
			fprintf(stderr, "select error in run()\n");
			listening = false;
			continue;
		}
		process_fd_set();
	}
	printf("endo endo\n");
	return 0;
}

Server::~Server(){
	for (int i = 0; i < client_cnt; i++){
		shutdown(client_socks[i], 2);
		close(client_socks[i]);
	}
	delete [] client_socks;
}

