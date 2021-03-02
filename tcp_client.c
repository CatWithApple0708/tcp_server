#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFF_SIZE 1024

int main(int argc, char* argv[])
{
	if(argc != 3) {
		printf("Usage : ./h_c, ip, port\n");
		return -1;
	}
	int cli_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(cli_fd < 0) {
		perror("socket create");
		return -1;
	}

	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[2]));
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);

	int ret = connect(cli_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	if(ret < 0) {
		perror("connect error");
		return -1;
	}

	while(1) {
		//struct sockaddr_in peer;
		printf("Please Enter > ");
		char send_buff[BUFF_SIZE] = {0};
		fflush(stdout);
		scanf("%s", send_buff);
		send(cli_fd, send_buff, strlen(send_buff), 0);

		char recv_buff[BUFF_SIZE] = {0};
		ret = recv(cli_fd, recv_buff, BUFF_SIZE - 1, 0);
		if(ret <= 0) {
			if(ret == 0) {
				printf("peer shutdown\n");
				close(cli_fd);
				return -1;
			}
			perror("recv error");
			continue;
		}
		printf("Server say > %s\n", recv_buff);
	}
	close(cli_fd);
	return 0;
}
