#include "client.h"

#include <time.h>
void delay(int sec)
{
   time_t _t;
   _t = time (NULL);    // 讀取系統時間
   while(time (NULL) <= _t + sec){};  // 比對時間差
}

void error(string msg) 
{
    cout << msg << endl;
    exit(0);
}

BIO *setup_connection(string servername) 
{
    BIO *bio;

    if (USE_SSL) 
	{
        /* ssl context */
        SSL_CTX *ctx = SSL_CTX_new(SSLv23_client_method());
        SSL *ssl;

        /* connect with ssl */
        bio = BIO_new_ssl_connect(ctx);
        BIO_set_conn_hostname(bio, servername.c_str());
        BIO_get_ssl(bio, &ssl);
        if (!ssl) error("error finding ssl pointer");

        /* error checks */
        if (SSL_get_verify_result(ssl) != X509_V_OK)
            error("error verifying ssl connection");
        if (bio == NULL) error("error setting up connection\n");
        if (BIO_do_connect(bio) <= 0) error("error connection failed\n");
        if (BIO_do_handshake(bio) <= 0) error("error handshake failed\n");
    } 
	else 
	{
        /* unsecured connect */
        bio = BIO_new_connect(servername.c_str());
    }
    return bio;
}

void send_o(BIO *bio, string msg) 
{
    if (BIO_write(bio, msg.c_str(), msg.length()) <= 0)
        error("Error writing message\n");
}

string receive_o(BIO *bio) 
{
    char buffer[1024];
    memset(buffer, 0, 1024);
    if (BIO_read(bio, buffer, 1023) < 0) 
		error("Error reading message\n");
    string msg(buffer);
    return msg;
}

vector<vector<string> > parser(string entry) 
{
    /* ignore int/uint warnings because find overflows uint */
    int pos = entry.find("\n");
    string balance = entry.substr(0, pos);
    int x = pos + 1;
    pos = entry.find("\n", x);
    string usersOnline = entry.substr(x, pos - x);
    /* find the extra newline */
    pos = entry.find("\n", pos + 1);
    vector<vector<string> > userList;
    int pos2 = entry.find('#', pos - 2);
    pos++;
    while (pos2 != string::npos) 
	{
        vector<string> temp;
        temp.push_back(entry.substr(pos, pos2 - pos));
        pos = pos2 + 1;
        pos2 = entry.find('#', pos);
        temp.push_back(entry.substr(pos, pos2 - pos));
        pos = pos2 + 1;
        pos2 = entry.find("\n", pos);
        temp.push_back(entry.substr(pos, pos2 - pos));
        userList.push_back(temp);
        pos = pos2 + 1;
        pos2 = entry.find('#', pos);
    }
    /* also print it out */
	/*
    cout << balance << endl << usersOnline << endl;
    cout << left << setw(6) << "No:" << left << setw(25) << "Name" << left
         << setw(18) << "Address" << left << setw(8) << "Port" << endl;
    for (uint i = 0; i < userList.size(); i++) {
        cout << left << setw(6) << i << left << setw(25) << userList[i][0]
             << left << setw(18) << userList[i][1] << left << setw(8)
             << userList[i][2] << endl;
    }
	*/
    return userList;
}

void *listener(void *tdata) 
{
    /* data receives a port and a bio */
    workerData *data = (workerData *)tdata;

    /* server bio setup */
    BIO *bio = BIO_new_accept(to_string(data->port).c_str());
    if (bio == NULL) error("error setting up connection\n");

    /* pre declared because scope issues */
    BIO *connection;
    SSL_CTX *ctx;

    if (USE_SSL) {
        /* setup ssl ctx */
        ctx = SSL_CTX_new(SSLv23_server_method());
        if (ctx == NULL) error("error loading ctx context");

        /* load certificates */
        if (!SSL_CTX_use_certificate_file(ctx, CERT_FILE, SSL_FILETYPE_PEM))
            error("error loading certificate file");
        if (!SSL_CTX_use_PrivateKey_file(ctx, KEY_FILE, SSL_FILETYPE_PEM))
            error("error loading private key file");
        if (!SSL_CTX_check_private_key(ctx))
            error("error checking private key");

        /* setup BIO chain */
        connection = BIO_new_ssl(ctx, 0);
        BIO_set_accept_bios(bio, connection);
    }

    if (BIO_do_accept(bio) <= 0) error("error connection failed 2\n");

    while (true) {
        /* accept then pass to secured bio */
        if (BIO_do_accept(bio) <= 0)
            error("error listener connection failed 2\n");
        connection = BIO_pop(bio);
        string msg = receive_o(connection);

        /* handle exit, becasue using a select is too hard */
        if (msg == "Exit") break;

        send_o(data->bio, msg);
    }
    /* free up resources */
    if (USE_SSL) SSL_CTX_free(ctx);
    BIO_free(bio);
    BIO_free(connection);
    pthread_exit(NULL);
}


/*===========================original==================================*/

Client::Client(string ip_, int port_, string name_, int port_num_)
{
	ip = ip_;
	name = name_;
	port = port_;
	sd = -1;
	money = 10000;
	user_num = 0;
	login = 0;
	account_list = NULL;
	port_num = port_num_;
}

bool Client::connect_server()
{
	string  entry;
	bio = setup_connection(ip + ":" + to_string(port));

    /* create worker thread for p2p */
    int listenport = port_num;
    wData = new workerData{listenport, bio};
	entry = receive_o(bio);
    if (pthread_create(&worker, NULL, listener, (void *)wData))
	{
        //error("error creating listening thread");
		return (0);
	}
	return (1);
}

bool Client::client_register(string name)
{
	string entry;
	send_o(bio, "REGISTER#" + name + "#" + to_string(money) + "\n");
    entry = receive_o(bio);
    if (entry.substr(0, 3) == "100")
        return (1);
    else if (entry.substr(0, 3) == "210")
        return (0);
	return (0);
}

bool Client::client_login()
{
	/*
	string entry;
    send_o(bio, name + "#" + to_string(port_num) + "\n");
    entry = receive_o(bio);
	//cout << entry << endl;
	*/
	string entry;
	if (!this->client_register(name))
	{
		send_o(bio, name + "#" + to_string(port_num) + "\n");
		entry = receive_o(bio);
		if (entry.find("230 ") != string::npos || entry.find("220 ") != string::npos)
			return (0);
	}
	else
	{
		send_o(bio, name + "#" + to_string(port_num) + "\n");
		entry = receive_o(bio);
		if (entry.find("230 ") != string::npos || entry.find("220 ") != string::npos)
			return (0);
	}
	/*
    if (entry.find("230 ") != string::npos || entry.find("220 ") != string::npos)
	{
		//cout << entry << endl << "Login Failed" << endl;
		if(!this->client_register(name))
		{
			return (0);
		}
		else
		{
			return client_login();
		}
	}
    else 
	{*/
        login = 1;
		//cout << entry << endl;
        userList = parser(entry);
		this->update_data(entry);
		return (1);
    //}
	
	return (1);
}

bool Client::exit()
{
	string entry;
	send_o(bio, "Exit\n");
    entry = receive_o(bio);
	BIO *sbio = setup_connection("localhost:" + to_string(port_num));
    send_o(sbio, "Exit");
	pthread_join(worker, NULL);
    pthread_exit(NULL);
    BIO_free(bio);
    BIO_free(sbio);
	login = 0;
	return (1);
}

bool Client::get_status()
{
	string entry;
	send_o(bio, "List\n");
    entry = receive_o(bio);
    userList = parser(entry);
	this->update_data(entry);
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
	
	pos = token.find(": ");
	token.erase(0, pos + 2);
	cout << token << endl;
	money = stoi(token);

	delimiter = ":";
	pos = respond.find(delimiter);
	respond.erase(0, pos + delimiter.length());

	delimiter = "\n\n";
	pos = respond.find(delimiter);
	token = respond.substr(0, pos);
	respond.erase(0, pos + delimiter.length());
	user_num = stoi(token);

	delete[](account_list);
	account_list = new account[user_num - 1];
	string token2;
	int count = 0;
	delimiter = "\n";	
	cout << respond << endl;
	while ((pos = respond.find(delimiter)) != string::npos && count < user_num - 1)
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

bool Client::trans_money(int t1, int t2)
{
	if (t1 >= user_num - 1)
		return (0);
	BIO *p2pbio = setup_connection(userList.at(t1).at(1) + ":" + userList.at(t1).at(2));
    send_o(p2pbio, name + "#" + to_string(t2) + "#" + userList.at(t1).at(0) + "\n");
    BIO_free(p2pbio);
	return (1);
}

void Client::print_list()
{
	for(int i = 0; i < user_num; i++)
	{
		cout << account_list[i].name << "#" << account_list[i].ip << "#" << account_list[i].port_num << "\n";
	}
}
