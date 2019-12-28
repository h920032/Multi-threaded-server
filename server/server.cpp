#include "server.h"
#include "threadpool.h"

/*=======================client=============================*/
client::client(int *_forClientSockfd,string ip, Server *_server)
{
	is_login = 0;
	connect_msg = "connection accepted\n";
	forClientSockfd = _forClientSockfd;
	client_info.ip = ip;
	server = _server;
}

string client::receive() //用來接收訊息轉成字串
{
	string msg;
	char *buffer = new char[BUFFER_SIZE];
	recv(*forClientSockfd,buffer,BUFFER_SIZE,0);
	msg = buffer;
	return msg;
}

bool client::send_msg(string msg) //用來把字串發出去
{
	int n = msg.length();
	char *buf = new char[BUFFER_SIZE];
	strcpy(buf, msg.c_str());
	int success = 0;
	success = send(*forClientSockfd,buf,n,0);
	if(success == -1)
		return (0);
	return (1);
}

bool client::client_register(string name) //call server裡的client_register 
{
	account input;
	input.name = name;
	input.money = 10000;
	return server->client_register(input);
}

bool client::check_msg(string input)
{
	string name = input.substr(0, input.find("#"));
	string port = input.substr(input.find("#")+1,input.length() -input.find("#") - 2);
	if ((int)input.find("#") == -1)
	{
		return (0);
	}
	else if (!(port.find_first_not_of( "0123456789" ) == std::string::npos))
	{
		return (0);
	}
	else
	{
		return (1);
	}
}

bool client::client_login(string name, int port_num) //call server裡的client_login函數
{
	online input;
	input.name = name;
	input.ip = client_info.ip;
	input.port_num = port_num;
	
	if (server->client_login(input))
	{
		client_info.name = name;
		client_info.port_num = port_num;
		is_login = 1;
		return (1);
	}
	return (0);
}

void client::list_status()
{
	string output = server->get_status(client_info.name);
	send_msg(output);	
}

void client::close_client() //關閉這個client
{
	is_login = 0;
	close(*forClientSockfd);
}

bool client::create_connect() //建立一個給client的連線
{
	if (*forClientSockfd == -1)
		return (0);
	send_msg(connect_msg);
	server->main_ui->update("Connection accepted");
	while(*forClientSockfd != -1)
	{
		string input;
		input = receive();
		//printf("%s",input.c_str()); //印出收到的訊息
		if (input.compare("Exit\n") == 0) //離開
		{
			string bye = "Bye\n";
			if (is_login == 1)
			{
				//移除登入名單
				if (server->remove_online(client_info.name))
				{
					is_login = 0;
				}
			}
			//printf("Enter response: Bye\n");
			server->main_ui->update(input.substr(0,input.length()-1));
			server->main_ui->update("Enter response: Bye");
			send_msg(bye);
			break;
		}
		else if(!is_login && input.substr(0, input.find("#")).compare("REGISTER") == 0 && input.find("#")+1 != input.length()-1)
			//註冊帳號
		{
			string name = input.substr(input.find("#")+1,input.length() -input.find("#") - 2); //看是否名字符合
			if (client_register(name))
			{
				string ok = "100 OK\n";
				server->main_ui->update(input.substr(0,input.length()-1));
				send_msg(ok);
			}
			else
			{
				string fail = "210 FAIL\n";
				server->main_ui->update(input.substr(0,input.length()-1));
				send_msg(fail);
			}
		}
		else if(!is_login && check_msg(input)) //確認字串格式是否是登入
		{
			string name = input.substr(0, input.find("#"));
			string port = input.substr(input.find("#")+1,input.length() -input.find("#") - 2);
			if (client_login(name,stoi(port)))
			{
				server->main_ui->update(input.substr(0,input.length()-1));
				list_status();
			}
			else
			{
				string fail = "220 AUTH_FAIL\n";
				server->main_ui->update(input.substr(0,input.length()-1));
				send_msg(fail);
			}
		}
		else if (is_login && input.compare("List\n") == 0) //列出清單
		{
			server->main_ui->update(input.substr(0,input.length()-1));
			list_status();
		}
		else if (is_login)
		{
			string fail = "please type the right option number!\n";
			if (input.compare("\n") != 0)
				server->main_ui->update(input.substr(0,input.length()-1));
			send_msg(fail);
		}
		else
		{
			string fail = "210 FAIL\n"; //錯誤訊息
			if (input.compare("\n") != 0)
				server->main_ui->update(input.substr(0,input.length()-1));
			send_msg(fail);
		}
	}
	close_client();
	return (1);
}

/*=======================Server=============================*/

Server::Server(int _port)
{
	port = _port;
    bzero(&sin,sizeof(sin));
	bzero(&cin,sizeof(cin));
 
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(port);
	//socket設定
	main_ui = new ui(MAP_WIDTH,MAP_HEIGHT,&online_list,&account_list);
	main_ui->draw();
}

bool Server::create_socket() //開啟一個socket
{
	fs = socket(AF_INET,SOCK_STREAM,0); //服务端套接字，tcp协议
	if (fs == -1)
		return (0);
    bind(fs,(struct sockaddr*)&sin,sizeof(sin));
    listen(fs,MAX_CONNECTER_NUM);
    //printf("waiting for a connection\n");
	main_ui->update("waiting for a connection");
	len = sizeof(cin);
	return (1);
}

void Server::create_client(int *fc, string ip,Server *_server) //丟到threadpool的函數
{
	client child(fc,ip,_server);
	child.create_connect();
}

bool Server::client_login(online new_online) //用來登陸
{
	bool in_register = 0;
	for (int i = 0; i < (int)account_list.size(); i++)
	{
		if (account_list[i].name.compare(new_online.name) == 0)
			in_register = 1;
	}
	bool empty_port = 1;
	for (int i = 0; i < (int)online_list.size(); i++)
	{
		if (online_list[i].port_num == new_online.port_num || online_list[i].name.compare(new_online.name) == 0)
			empty_port = 0;
	}
	if (in_register && empty_port)
	{
		online_list.push_back(new_online);
		return (1);
	}
	return (0);
}

bool Server::client_register(account new_account) //用來註冊新帳號
{
	for (int i = 0; i < (int)account_list.size(); i++)
	{
		if(account_list[i].name.compare(new_account.name) == 0)
			return (0);
	}
	account_list.push_back(new_account);
	return (1);
}

string Server::get_status(string name) //用來回應client端的List
{
	string output;
	for (int i = 0; i < (int)account_list.size(); i++)
	{
		if (account_list[i].name.compare(name) == 0)
			output += to_string(account_list[i].money);
	}
	output += "\nnumber of accounts online:";
	output += (to_string(online_list.size()) + "\n");
	for (int i = 0; i < (int)online_list.size(); i++)
	{
		output += (online_list[i].name + "#" + online_list[i].ip + "#" + to_string(online_list[i].port_num) + "\n");
	}
	return output;
}

bool Server::remove_online(string name) //從online list中移除使用者
{
	for (int i = 0; i < (int)online_list.size(); i++)
	{
		if (online_list[i].name.compare(name) == 0)
		{
			online_list.erase(online_list.begin() + i);
			return (1);
		}
	}
	return (0);
}

void Server::accept_loop() //server啟動loop等待別人來連線
{
	if (fs == -1)
		return;
	bool wait_user = 1;
	threadpool exector{MAX_CONNECTER_NUM};
	while (wait_user)
	{
		int *fc = new int;
        *fc = accept(fs,(struct sockaddr*)&cin , &len); //建立一個連線等待client上們
		//cout << fc;
		//thread client_test(create_client,fc);
		char addr_p[INET_ADDRSTRLEN];
		memset(addr_p,0,sizeof(addr_p));
		inet_ntop(AF_INET,&cin.sin_addr,addr_p,sizeof(addr_p));
		string ip = addr_p;
		/*
		printf("client ip:%s,port:%d\n",addr_p,ntohs(cin.sin_port));
		printf("socket number:%d\n",*fc);
		*/
		//printf("Connection accepted\n");
		exector.commit(create_client,fc,ip,this);//上門的話就丟進threadpool中
		//close(*fc);
	}
}


bool Server::close_server() //關閉server
{
	if(close(fs) == -1)
		return (0);
	else
		return (1);
}

