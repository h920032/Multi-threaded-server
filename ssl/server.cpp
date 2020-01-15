#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <algorithm>
#include <deque>
#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

#define NUM_THREADS 8
#define CERT_FILE "server-cert.pem"
#define KEY_FILE "server-key.pem"
#define USE_SSL 1
using namespace std;

/* global to killme; */
bool killme = 0;

struct ConnData 
{
    BIO *bio;
    string name;
    string ip;
    string port;
};

struct UserData 
{
    string name;
    int balance;
    ConnData *conn;
};

struct rmsg 
{
    string msg;
    bool retry;
};
deque<UserData> UserD;
pthread_mutex_t UDm;
deque<ConnData> ConnD;
pthread_mutex_t CDm;

void error(string msg) 
{
    cout << msg << endl;
    exit(0);
}

void send(BIO *bio, string msg) 
{
    if (BIO_write(bio, msg.c_str(), msg.length()) <= 0)
        error("Error writing message\n");
}

int userExist(string name) 
{
    pthread_mutex_lock(&UDm);
    for (uint i = 0; i < UserD.size(); i++) 
	{
        if (UserD.at(i).name == name) {
            pthread_mutex_unlock(&UDm);
            return i;
        }
    }
    pthread_mutex_unlock(&UDm);
    return -1;
}

string listInfo(string name) 
{
    string info;
    int balance;
    pthread_mutex_lock(&UDm);
    for (uint i = 0; i < UserD.size(); i++) 
	{
        if (UserD.at(i).name == name)
            balance = UserD.at(i).balance;
        else if (UserD.at(i).conn != NULL) 
		{
            info += UserD.at(i).name + "#" + UserD.at(i).conn->ip + "#" +
                    UserD.at(i).conn->port + "\n";
        }
    }
    pthread_mutex_unlock(&UDm);
    return "AccountBalance: " + to_string(balance) + "\nUsers Online: " + to_string(UserD.size()) + "\n\n" + info;
}

rmsg receive(BIO *bio) 
{
    char buffer[1024];
    bool retry = 0;
    memset(buffer, 0, 1024);
    if (BIO_read(bio, buffer, 1023) <= 0) 
	{
        if (BIO_should_retry(bio) == true)
            retry = 1;
        else
            error("Error reading message\n");
    }
    string msg(buffer);
    cout << msg;
    rmsg r{msg, retry};
    return r;
}

void *worker(void *data) 
{
    while (true) 
	{
        if (killme) pthread_exit(NULL);

        pthread_mutex_lock(&CDm);
        if (ConnD.size() == 0) 
		{
            pthread_mutex_unlock(&CDm);
            sleep(5);
            continue;
        }
        ConnData user = ConnD.front();
        ConnD.pop_front();
        pthread_mutex_unlock(&CDm);

        bool addBack = 1;
        rmsg r = receive(user.bio);
        if (r.retry == 0) 
		{
            if (r.msg.substr(0, 9) == "REGISTER#") 
			{
                int pos = r.msg.find('#', 9);
                string tname = r.msg.substr(9, pos - 9);
                int balance =
                    stoi(r.msg.substr(pos + 1, r.msg.length() - pos - 1));
                if (userExist(tname) != -1) 
				{
                    send(user.bio, "210 FAIL\n");
                } 
				else 
				{
                    UserData nuser{tname, balance, NULL};
                    pthread_mutex_lock(&UDm);
                    UserD.push_back(nuser);
                    pthread_mutex_unlock(&UDm);
                    send(user.bio, "100 OK\n");
                }
            } 
			else if (r.msg == "List\n") 
			{
                send(user.bio, listInfo(user.name));
            } 
			else if (r.msg == "Exit\n") 
			{
                send(user.bio, "Bye\n");
                addBack = false;
                for (uint i = 0; i < UserD.size(); i++) 
				{
                    if (UserD.at(i).name == user.name) 
					{
                        pthread_mutex_lock(&UDm);
                        UserD.at(i).conn = NULL;
                        pthread_mutex_unlock(&UDm);
                    }
                }
                BIO_free(user.bio);
                continue;
            }
			else if (count(r.msg.begin(), r.msg.end(), '#') == 1) 
			{
                int hash = r.msg.find("#");
                string tname = r.msg.substr(0, hash);
                if (userExist(tname) != -1) 
				{
                    int tempfd;
                    BIO_get_fd(user.bio, &tempfd);
                    struct sockaddr addr;
                    socklen_t addr_len = sizeof(addr);
                    getpeername(tempfd, &addr, &addr_len);
                    char ipstr[INET_ADDRSTRLEN];
                    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
                    inet_ntop(AF_INET, &s->sin_addr, ipstr, INET_ADDRSTRLEN);
                    /* end get ip */
                    user.ip = string(ipstr);
                    user.name = tname;
                    user.port = r.msg.substr(hash + 1, r.msg.size() - hash - 2);
                    /* set UserD association */
                    int pos = userExist(user.name);
                    pthread_mutex_lock(&UDm);
                    UserD.at(pos).conn = &user;
                    pthread_mutex_unlock(&UDm);
                    send(user.bio, listInfo(user.name));
                }
            } 
			else if (count(r.msg.begin(), r.msg.end(), '#') == 2) 
			{
                int hash = r.msg.find("#", 0);
                int hash2 = r.msg.find("#", hash + 1);
                int fromUser = userExist(r.msg.substr(0, hash));
                int toUser = userExist(
                    r.msg.substr(hash2 + 1, r.msg.size() - hash2 - 2));
                int amount =
                    stoi(r.msg.substr(hash + 1, hash2 - hash - 1), NULL);
                pthread_mutex_lock(&UDm);
                UserD.at(fromUser).balance -= amount;
                UserD.at(toUser).balance += amount;
                pthread_mutex_unlock(&UDm);
            }
        }
        if (addBack) 
		{
            pthread_mutex_lock(&CDm);
            ConnD.push_back(user);
            pthread_mutex_unlock(&CDm);
        }
    }
}

void *worker2(void *data) 
{
    BIO *bio = BIO_new_accept((char *)data);
    if (bio == NULL)
		error("error setting up connection\n");
    BIO *connection;
    SSL_CTX *ctx;

    if (USE_SSL) 
	{
        ctx = SSL_CTX_new(SSLv23_server_method());
        if (ctx == NULL) 
			error("error loading ctx context");
        if (!SSL_CTX_use_certificate_file(ctx, CERT_FILE, SSL_FILETYPE_PEM))
            error("error loading certificate file");
        if (!SSL_CTX_use_PrivateKey_file(ctx, KEY_FILE, SSL_FILETYPE_PEM))
            error("error loading private key file");
        if (!SSL_CTX_check_private_key(ctx))
            error("error checking private key");

        connection = BIO_new_ssl(ctx, 0);
        BIO_set_accept_bios(bio, connection);
    }

    if (BIO_do_accept(bio) <= 0)
		error("error connection failed\n");

    while (true) 
	{
        /* accept and push to queue */
        if (BIO_do_accept(bio) <= 0)
			error("error worker2 connection failed\n");
        if (killme)
			break;

        connection = BIO_pop(bio);
        ConnData user{connection, "", "", ""};
        send(connection, "successful connection");
        pthread_mutex_lock(&CDm);
        ConnD.push_back(user);
        pthread_mutex_unlock(&CDm);
    }

    if (USE_SSL) SSL_CTX_free(ctx);
    BIO_free(bio);
    BIO_free(connection);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) 
{
    SSL_load_error_strings();
    ERR_load_BIO_strings();
    OpenSSL_add_all_algorithms();
    SSL_library_init();

    if (argc < 2) 
		error("usage: " + string(argv[0]) + " serverPort\n");

    pthread_mutex_init(&CDm, NULL);
    pthread_mutex_init(&UDm, NULL);

    pthread_t workers[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&workers[i], NULL, worker, NULL))
            error("error creating threads");
    }

    pthread_t lonelyWorker;
    if (pthread_create(&lonelyWorker, NULL, worker2, (void *)argv[1]))
        error("error creating listener thread");

    string com;
    while (com != "exit") 
		cin >> com;

    killme = 1;
    string myself = string("localhost:" + string(argv[1]));
    BIO *sbio = BIO_new_connect(myself.c_str());
    if (sbio == NULL)
		error("error setting up connection\n");
    if (BIO_do_connect(sbio) <= 0)
		error("error connection failed\n");

    pthread_join(lonelyWorker, NULL);
    BIO_free(sbio);
    pthread_exit(NULL);
    pthread_mutex_destroy(&CDm);
    pthread_mutex_destroy(&UDm);
    return 0;
}
