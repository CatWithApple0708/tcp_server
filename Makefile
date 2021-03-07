all:t_s t_c
t_s:tcp_server.c
	gcc -Wall tcp_server.c -o t_s -lpthread
t_c:tcp_client.c
	gcc -Wall tcp_client.c -o t_c
.PHONY:clean
clean:
	rm -rf t_s
	rm -rf t_c
