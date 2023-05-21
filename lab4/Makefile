all: client acc_server
client: client.c acc.c
	gcc client.c acc.c -o client
acc_server: acc_server.c acc.c 
	gcc acc_server.c acc.c -o acc_server
