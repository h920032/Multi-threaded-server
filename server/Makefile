all:clean server

CC = g++
INCLUDE = server.h threadpool.h
FILE = main.cpp server.cpp ui.cpp
CFLAGS = -Wall -Werror -Wextra
THREAD = -pthread
NAME = server

server:
	$(CC) $(THREAD) $(CFLAGS) -o $(NAME) $(FILE)


clean:
	@rm -f server

fclean:clean
