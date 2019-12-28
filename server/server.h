#ifndef SERVER_H
# define SERVER_H
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <string>
# include <arpa/inet.h>
# include <iostream>
# include <thread>
# include <vector>
# include <exception>
# include <stdexcept>

# include "threadpool.h"
using namespace std;

//# define MAX_THREAD_NUM 100
# define MAX_CONNECTER_NUM 16
# define BUFFER_SIZE 10000
# define MAP_WIDTH 120
# define MAP_HEIGHT 25

struct connect_error : public exception
{
   const char * what () const throw ()
   {
      return "Connection Error";
   }
};

struct online
{
	string name;
	string ip;
	int port_num;
};

struct account
{
	string name;
	int money;
};

/*=================UI=======================*/

class text
{
	public:
		int x;
		int y;
		string contain;
		int len;
		text(int _x, int _y, string _contain)
		{
			x = _x;
			y = _y;
			contain = _contain;
			len = contain.length();
		}
};

class ui
{
	private:
		int width;
		int height;
		vector<online> *online_list;
		vector<account> *account_list;
		bool is_border(int x, int y);
		bool is_msg(int x, int y);
		vector<text> title;
		vector<text> register_data;
		vector<text> online_data;
		vector<text> log_data;
	public:
		ui(int _width, int _height, vector<online> *_onlne_list, vector<account> *_account_list);
		void draw();
		void update(string msg);
};

/*==================Server====================*/

class Server //Server物件用來表示該server的連線狀況
{
	private:
		vector<online> online_list;
		vector<account> account_list;
		int port;
		struct sockaddr_in sin;
		struct sockaddr_in cin;
		int fs;  //创建套接字用，服务端和客户端各一个
		socklen_t len;
	public:
		Server(int _port);
		bool create_socket();
		static void create_client(int *fc, string ip, Server *_server);
		void accept_loop(); //執行迴圈，不斷的等待連線
		bool close_server();
		bool client_register(account);
		bool client_login(online);
		string get_status(string name);
		bool remove_online(string name);
		ui *main_ui;
};

/*=================client======================*/

class client
{
	private:
		int *forClientSockfd;
		Server *server;
		string connect_msg;
		online client_info;
		bool check_msg(string input);
		bool client_register(string name);
		bool client_login(string name, int port_num);
		void list_status();
		bool is_login;
		string receive();
		bool send_msg(string msg);
		void close_client();
	public:
		client(int *_forClientSockfd,string ip, Server *_server);
		bool create_connect();
};

#endif
