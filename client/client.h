#ifndef CLIENT_H
# define CLIENT_H
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
using namespace std;

extern int BUFFER_SIZE;

struct account
{
	string name;
	string ip;
	int port_num;
};

class Client
{
	private:
		struct sockaddr_in addr;
		string ip;
		string name;
		int port;
		int sd;
		int port_num;
		bool login;
		bool send_msg(string msg);
		string receive();
		bool client_register(string name);
		void update_data(string respond);

	public:
		Client(const string ip,const int port, const string name, const int port_num);
		account* account_list;
		bool connect_server();
		bool client_login();
		bool get_status();
		bool exit();
		const bool is_login();
		void print_list();
		int user_num;
		int money;
};

#endif
