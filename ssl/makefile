all: new server

client: client.cpp
	@g++ client.cpp -lpthread -lssl -lcrypto -o client

new:
	@g++ client.cpp main.cpp ui.cpp -lssl -lcrypto -pthread -o client

server: server.cpp
	@g++ server.cpp -lpthread -lssl -lcrypto -o server
