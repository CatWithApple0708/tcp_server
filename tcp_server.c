#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#define BUFF_SIZE 1024
int socket_server;
int socket_client_id;
static void my_handler(int sig){ // can be called asynchronously
 close(socket_server);
 close(socket_client_id);
 sleep(1);
 exit(0);
}

void accept_thread(){



}

int main(int argc, char* argv[])
{
	signal(SIGINT, my_handler);
	if(argc != 3) {
		printf("Usage : ./h_c, ip, port\n");
		return -1;
	}
	int local = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	 int val = 1;
	 setsockopt(local,SOL_SOCKET,SO_REUSEADDR,(void *)&val,sizeof(int));
	if(local < 0) {
		perror("socket create");
		return -1;
	}

	socket_server = local;

	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[2]));
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	socklen_t len = sizeof(struct sockaddr_in);

	int ret = bind(local, (struct sockaddr*)&serv_addr, len);
	if(ret < 0) {
		perror("bind error");
		return -1;
	}

	if(listen(local, 5) < 0) {
		perror("listen error");
		return -1;
	}

	while(1) {
		struct sockaddr_in peer;
		
		int new_fd = accept(local, (struct sockaddr*)&peer, &len);
		if(new_fd < 0) {
			perror("accept error");
			continue;
		}
		printf("new client connect\n");
socket_client_id = new_fd;

		while(1) {
			char recv_buff[BUFF_SIZE] = {0};
			ret = recv(new_fd, recv_buff, BUFF_SIZE - 1, 0);
			if(ret <= 0) {
				if(ret == 0) {
					printf("client shutdown\n");
					break;
				}
				else if(errno == EINTR || errno == EAGAIN) {
					continue;
				}
				break;
			}
			printf("Clien say > %s(%d)\n", recv_buff,strlen(recv_buff));

			char send_buff[BUFF_SIZE] = {0};
			fflush(stdout);
			send(new_fd, recv_buff, strlen(recv_buff)+1, 0);
		}
		close(new_fd);
	}
	close(local);
	return 0;
}
