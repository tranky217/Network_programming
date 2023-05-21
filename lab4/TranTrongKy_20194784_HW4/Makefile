all: client server
client: client.c acc.c
	gcc client.c acc.c -o client
server: acc_server.c acc.c 
	gcc acc_server.c acc.c -o server
