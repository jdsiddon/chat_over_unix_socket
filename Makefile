server: server.cpp
	g++ server.cpp -o chatserve 

client: client.c
	gcc -o chatclient client.c 

clean:
	rm -r chatserve chatclient
