#include "client.h"
#include "input.h"
#include "ui.h"

int BUFFER_SIZE = 10000;

int main(int argc, char **argv)
{
	//cout << BUFFER_SIZE;
	fd_set rfds;
	timeval timeout;
	
	timeout.tv_sec = 0;
    timeout.tv_usec = 0;
	Ui test;
	test.draw();
	input_on();
	Client *user = NULL;
	//Client test("127.0.0.1", 5001, "test", 1234);
	//test.connect_server();
	//cout << test.client_login() << "\n";
	//cout << test.get_status();
	//test.print_list();
	//test.exit();
	
	while (true)
	{
		FD_ZERO(&rfds);
		FD_SET(STDIN_FILENO, &rfds);
		if (/*select(1, &rfds, NULL, NULL, &timeout) > 0 && */FD_ISSET(STDIN_FILENO, &rfds)) 
		{	
			//cout << getchar();
            switch (getchar()) {

                case 100:
                case 68:
				//	test.right();
					break;

                case 83:
                case 115: 
					test.down(); 
					break;

                case 65:
                case 97:
				//	test.left();
					break;

                case 119:
                case 87:
					test.up();
					break;


				case 10:
				{
					int type = test.enter();
					if(type == 8)
					{
						if(test.is_login == 1)
						{
							user->exit();
							delete(user);
							test.is_login = 0;
							test.is_login = 0;
							test.user_num = 0;
							test.user = NULL;
							user = NULL;
							test.draw();
						}
						return (0);
					}
					else if(type == 5 && test.is_login == 1 && user != NULL)
					{
						user->get_status();
						test.money = user->money;
						test.user_num = user->user_num;
						test.user = user;
					}
					else if(type == 6 && test.is_login == 0 && user == NULL && test.name.length() > 0 && test.ip.length() > 0 && test.port > 0 && test.login_port > 0)
					{
						user = new Client(test.ip,test.port,test.name,test.login_port);
						test.user = user;
						if (user->connect_server())
						{
							if (user->client_login())
							{
								test.is_login = 1;
								test.money = user->money;
								test.user_num = user->user_num;
							}
							else
							{
								delete(user);
								user = NULL;
								cout << "fail\n";
							}
						}
						else
						{
							delete(user);
							user = NULL;
							cout << "fail\n";
						}
					}
					else if(type == 7 && test.is_login == 1 && user != NULL)
					{
						user->exit();
						delete(user);
						test.is_login = 0;
						test.money = 0;
						test.user_num = 0;
						user = NULL;
						test.user = NULL;
					}
					test.draw();
				}
					break;
                default:break;
            }
        }
		//cout << DIRECTION;
	}
	input_off();
	return (0);
}
