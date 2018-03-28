#include "Client.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>

const int PORT = 8888;
const int INP_BUFF_SIZE = 256;

Client::Client(){
	running = false;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sock){
		perror("failed to open socket\n");
		return;
	}
}

int Client::set_serv_addr(const char *serv_ip, int port){
	serv_addr.sin_family = AF_INET;
	if (0 == inet_aton(serv_ip, &(serv_addr.sin_addr))){
		perror("set_serv_addr: invalid server ip adress\n");
		return -1;
	}
	serv_addr.sin_port = htons(port);
	return 0;
}

int Client::run(){
	int return_val = 0;
	if (-1 == set_serv_addr("127.0.0.1", PORT)){
		fprintf(stderr, "run: couldn't set server address :)\n");
		return -1;
	}
	running = true;
	char in_buff[INP_BUFF_SIZE];
	
	while (running){
		scanf("%s", in_buff);
		if (0 == strncmp(in_buff, "connect", 7)){
			if (0 != connect(sock, (struct sockaddr*)&serv_addr,
							 sizeof(serv_addr))){
				perror("couldn't connect to server\n");
				running = false;
				return_val = -1;
				continue;
			}
			printf("Connected successfully!\n");
		}else if (0 == strncmp(in_buff, "exit", 4)){
			running = false;
			continue;
		}
	}
	
	return return_val;
}

Client::~Client(){
	if (-1 != sock){
		shutdown(sock, SHUT_RDWR);
		close(sock);
	}
}

