#ifndef CLIENT_H
# define CLIENT_H
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

#define CERT_FILE "client-cert.pem"
#define KEY_FILE "client-key.pem"
#define USE_SSL 1

using namespace std;

struct account
{
	string name;
	string ip;
	int port_num;
};

struct workerData 
{
    int port;
    BIO *bio;
};

void delay(int sec);
void error(string msg);
BIO *setup_connection(string servername);
void send_o(BIO *bio, string msg);
string receive_o(BIO *bio);
vector<vector<string> > parser(string entry);
void *listener(void *tdata);

class Client
{
	private:
		BIO* bio;
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
		workerData *wData;
		pthread_t worker;
		vector<vector<string> > userList;

	public:
		Client(const string ip,const int port, const string name, const int port_num);
		account* account_list;
		bool connect_server();
		bool client_login();
		bool get_status();
		bool trans_money(int t1, int t2);
		bool exit();
		const bool is_login();
		void print_list();
		int user_num;
		int money;
};

#endif
