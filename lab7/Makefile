all: client server
client: tcp_client.c
	gcc tcp_client.c -o client
server: tcp_server.c acc.c
	gcc tcp_server.c acc.c -pthread -o server