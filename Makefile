server: server.cpp
	g++ server.cpp -o server 

client: client.c
	gcc -o client client.c 

clean:
	rm -r server client
