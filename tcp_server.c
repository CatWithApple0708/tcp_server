#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define BUFF_SIZE 1024
int socket_server;
int socket_client_id;
bool hasAcceptClient = false;
static void my_handler(int sig) { // can be called asynchronously
  close(socket_server);
  close(socket_client_id);
  sleep(1);
  exit(0);
}

void *accept_thread_handler(void *_null) {
  while (1) {
    struct sockaddr_in peer;
    socklen_t len;
    int new_fd = accept(socket_server, (struct sockaddr *)&peer, &len);
    if (new_fd < 0) {
      perror("accept error");
      continue;
    }
    printf("new client connect\n");
    socket_client_id = new_fd;
    hasAcceptClient = true;
  }
  return NULL;
}

void *receive_thread_handler(void *_null) {
  while (1) {

    if (!hasAcceptClient) {
      sleep(1);
    }

    char recv_buff[BUFF_SIZE] = {0};
    int ret = recv(socket_client_id, recv_buff, BUFF_SIZE - 1, 0);
    if (ret <= 0) {
      if (ret == 0) {
        printf("client shutdown\n");
        sleep(1);
        continue;
      } else if (errno == EINTR || errno == EAGAIN) {
        printf("client EINTR EAGAIN\n");
        continue;
      }
      printf("some other error%d\n", ret);
      perror("recv error");
      continue;
    }
    recv_buff[ret] = 0;
    printf("Clien say > %s(%d,%d)\n", recv_buff, (int)strlen(recv_buff), ret);
    // char send_buff[BUFF_SIZE] = {0};
    // fflush(stdout);
    // send(new_fd, recv_buff, strlen(recv_buff) + 1, 0);
  }
  return NULL;
}

int main(int argc, char *argv[]) {
  signal(SIGINT, my_handler);
  if (argc != 3) {
    printf("Usage : ./h_c, ip, port\n");
    return -1;
  }
  int local = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  int val = 1;
  setsockopt(local, SOL_SOCKET, SO_REUSEADDR, (void *)&val, sizeof(int));
  if (local < 0) {
    perror("socket create");
    return -1;
  }

  socket_server = local;

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(atoi(argv[2]));
  serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
  socklen_t len = sizeof(struct sockaddr_in);

  int ret = bind(local, (struct sockaddr *)&serv_addr, len);
  if (ret < 0) {
    perror("bind error");
    return -1;
  }

  if (listen(local, 5) < 0) {
    perror("listen error");
    return -1;
  }

  pthread_t accept_thread;
  pthread_t rec_thread;
  pthread_create(&accept_thread, NULL, accept_thread_handler, NULL);
  pthread_create(&rec_thread, NULL, receive_thread_handler, NULL);

  while (1) {
    int ret;
    scanf("%d", &ret);
    printf("input %d\n", ret);
    const char *cmd;
    if (ret == 0) {
      cmd =
          "{\"cmd\":\"device_status_request\",\"sn\":\"123456\",\"packet_id\":"
          "\"packet_id_1\"}";
    } else if (ret == 1) {
      cmd = "{\"cmd\":\"lock_device\",\"sn\":\"123456\",\"packet_id\":"
            "\"packet_id_1\"}";
    } else if (ret == 2) {
      cmd = "{\"cmd\":\"unlock_device\",\"sn\":\"123456\",\"packet_id\":"
            "\"packet_id_1\"}";
    } else if (ret == 3) {
      cmd = "{\"cmd\":\"stop_working\",\"sn\":\"123456\",\"packet_id\":"
            "\"packet_id_1\"}";
    }

    printf("send %s\n", cmd);

	if(hasAcceptClient){
		 send(socket_client_id, cmd, strlen(cmd) + 1, 0);
	}


  }
  close(local);
  return 0;
}
