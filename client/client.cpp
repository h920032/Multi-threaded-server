#include "client.h"

#include <time.h>
void delay(int sec)
{
   time_t _t;
   _t = time (NULL);    // 讀取系統時間
   while(time (NULL) <= _t + sec){};  // 比對時間差
}

Client::Client(string ip_, int port_, string name_, int port_num_)
{
	ip = ip_;
	name = name_;
	port = port_;
	sd = -1;
	money = 0;
	user_num = 0;
	login = 0;
	account_list = NULL;
	port_num = port_num_;
	bzero(&addr,sizeof(addr));
	addr.sin_family = PF_INET;
	addr.sin_addr.s_addr = inet_addr(ip.c_str());
	addr.sin_port = htons(port);
}

string Client::receive()
{
	string msg;
	char *buffer = new char[BUFFER_SIZE];
	
	//char buffer[BUFFER_SIZE];
	//delay(1);
	int n = recv(sd,buffer,BUFFER_SIZE,0);
	//cout << "num" << n << "\n";
	//buffer[n] = '\0';
	//cout << buffer << "\n";
	msg = buffer;
	//cout << msg;
	return msg;
}

bool Client::send_msg(string msg)
{
	int n = msg.length();
	char *buf = new char[BUFFER_SIZE];
	strcpy(buf, msg.c_str());
	//buf[n] = '\0';
	int success = 0;
	success = send(sd,buf,n,0);
	if(success == -1)
		return (0);
	return (1);
}

bool Client::connect_server()
{
	sd = socket(AF_INET , SOCK_STREAM , 0);
	if (sd == -1)
	{
        return (0);
    }

	int err = connect(sd,(struct sockaddr *)&addr,sizeof(addr));
	if(err==-1)
	{
		return (0);
    }
	string respond = this->receive();
	if(respond.compare("connection accepted\n") == 0)
	{
		return (1);
	}
	else
	{
		return (0);
	}
}

bool Client::client_register(string name)
{
	string output_msg = "REGISTER#" + name + "\n";
	//cout << output_msg;
	if(!this->send_msg(output_msg))
	{
		return (0);
	}
	string respond = this->receive();
	//cout << respond << "respond";
	if(respond.compare("100 OK\n") == 0)
	{
		return (1);
	}
	else if(respond.compare("210 FAIL\n") == 0)
	{
		return (0);
	}
	else
	{
		return (0);
	}
}

bool Client::client_login()
{
	string output_msg = name + "#" + to_string(port_num) + "\n";
	if(!this->send_msg(output_msg))
	{
		return (0);
	}
	delay(1);
	string respond = this->receive();
	if(respond.compare("220 AUTH_FAIL\n") == 0)
	{
		if(!this->client_register(name))
		{
			return (0);
		}
		else
		{
			return client_login();
		}
	}
	//delay(1);
	//respond += this->receive();
	login = 1;
	this->update_data(respond);
	return (1);
}

bool Client::exit()
{
	char exit[] = "Exit\n";
	int test = send(sd,exit,sizeof(exit),0);
	if(test == -1)
	{
		return (0);
	}
	string respond = this->receive();
	//cout << respond;
	if(respond.compare("Bye") == 0)
	{
		return (1);
	}
	close(sd);
	login = 0;
	return (0);
}

bool Client::get_status()
{
	string output_msg = "List\n";
	if(!this->send_msg(output_msg))
	{
		return (0);
	}
	delay(1);
	string respond = this->receive();
	if(respond.compare("220 AUTH_FAIL\n") == 0)
	{
		return (0);
	}
	//respond += this->receive();
	this->update_data(respond);
	return (1);
}

const bool Client::is_login()
{
	return login;
}

void Client::update_data(string respond)
{
	cout << respond;
	string delimiter = "\n";
	size_t pos = 0;
	string token;
	pos = respond.find(delimiter);
	token = respond.substr(0, pos); //取除價值
	respond.erase(0, pos + delimiter.length());
	money = stoi(token);

	delimiter = ":";
	pos = respond.find(delimiter);
	respond.erase(0, pos + delimiter.length());

	delimiter = "\n";
	pos = respond.find(delimiter);
	token = respond.substr(0, pos);
	respond.erase(0, pos + delimiter.length());
	user_num = stoi(token);

	delete[](account_list);
	account_list = new account[user_num];
	string token2;
	int count = 0;
	while ((pos = respond.find(delimiter)) != string::npos && count < user_num)
	{
		token = respond.substr(0, pos);
		respond.erase(0, pos + delimiter.length());
		string d = "#";
		pos = token.find(d);
		token2 = token.substr(0, pos);
		token.erase(0, pos + d.length());
		account_list[count].name = token2;
		
		pos = token.find(d);
		token2 = token.substr(0, pos);
		token.erase(0, pos + d.length());
		account_list[count].ip = token2;
		account_list[count].port_num = stoi(token);
		count++;
	}
}

void Client::print_list()
{
	for(int i = 0; i < user_num; i++)
	{
		cout << account_list[i].name << "#" << account_list[i].ip << "#" << account_list[i].port_num << "\n";
	}
}
