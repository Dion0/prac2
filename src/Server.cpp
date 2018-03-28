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
#include <vector>

const int PORT = 8888;
const int c_base = 10;
const int stdin_buffer_size = 256;
const int c_b_size = 2048;

Server::Server() : client_base(c_base),
				   client_buffer_base(c_b_size){
	listening = false;
	client_cnt = 0;

	listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == -1){
		ERR_RES = SERV_ERRORS::SOCKET_ERR;
		perror("couldn't create listen socket\n");
		return;
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	int tmp = 1;
	setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR,
			   &tmp, sizeof(tmp));
	int bind_res = bind(listen_sock, (struct sockaddr*) &address,
						sizeof(address));
	if (0 != bind_res){
		ERR_RES = SERV_ERRORS::BIND_ERR;
		perror("couldn't bind to socket\n");
		return;
	}
}

int Server::add_client(int fd, struct sockaddr_in adr,
					   socklen_t len){
	printf("client connected:\n");
	print_client(fd, adr);
	client_cnt++;
	client_socks.push_back(fd);
	client_buffer.push_back(std::vector<char>
							(client_buffer_base));
	client_addr.push_back(adr);
	client_len.push_back(len);
	return 0;
}

int Server::remove_client(std::size_t ind){
	if (ind >= client_cnt)
		return -1;
	shutdown(client_socks[ind], SHUT_RDWR);
	close(client_socks[ind]);
	client_socks.erase(client_socks.begin() + ind);
	return 0;
}

void Server::gen_fd_set(){
	max_d = listen_sock > STDOUT_FILENO ?
		listen_sock : STDOUT_FILENO; //lmoa autism
	FD_ZERO(&read_fds);
	FD_SET(STDOUT_FILENO, &read_fds);
	FD_SET(listen_sock, &read_fds);
	for (int const &sock: client_socks){
		FD_SET(sock, &read_fds);
		if (sock > max_d)
			max_d = sock;
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
			add_client(t_sock, t_addr, t_addr_len);
		}
	}
	if (FD_ISSET(STDOUT_FILENO, &read_fds)){
		handle_input();
	}
	for (std::size_t i = 0; i != client_cnt; i++)
		if (FD_ISSET(client_socks[i], &read_fds))
			handle_client(i);
}

void Server::print_client(int sd, struct sockaddr_in addr){
	char *tmp = inet_ntoa(addr.sin_addr);
	printf("\tsocket : %d\n"
		   "\tip:port: %s:%d\n",
		   sd, tmp, addr.sin_port);
}

void Server::print_clients() const{
	for (std::size_t i = 0; i != client_cnt; i++)
		print_client(client_socks[i], client_addr[i]);
}
// TODO: add parser and parse (duh) received message if it is
//complete
void Server::handle_client(std::size_t ind){
	std::size_t recv_cnt;
	recv_cnt = recv(client_socks[ind], &client_buffer[ind][0],
					client_buffer[ind].capacity() -1, 0);
	if (recv_cnt == 0){
		remove_client(ind);
		return;
	}
	client_buffer[ind][recv_cnt] = 0;
	printf("received string:\n"
		   "%s\n", client_buffer[ind].data());
}

void Server::handle_input(){
	static char stdin_buffer[stdin_buffer_size];
	static ssize_t read_cnt;
	read_cnt = read(STDOUT_FILENO, stdin_buffer,
					stdin_buffer_size-1);
	if (read_cnt < 1){
		fprintf(stderr, "error reading from stdin\n");
		return;
	}
	stdin_buffer[read_cnt] = 0;
	if (read_cnt >= 3){ 
		if (0 == strncmp("log", stdin_buffer, 3))
			printf("trying to log:%s", stdin_buffer+3);
		if (0 == strncmp("end", stdin_buffer, 3))
			listening = false;
	}
	if (0 == strncmp("print", stdin_buffer, 5)){
		print_clients();
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
	if (-1 != listen_sock){
		shutdown(listen_sock, SHUT_RDWR);
		close(listen_sock);
	}
	for (auto const &sock : client_socks){
		shutdown(sock, SHUT_RDWR);
		close(sock);
	}
	client_socks.clear();
}

