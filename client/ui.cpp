#include "client.h"
#include "ui.h"

bool Ui::is_map_border(int x, int y) 
{
    return y == 0 || x == 0 || x == MAP_WIDTH || y == MAP_HEIGHT;
}

int Ui::is_text(int x, int y)
{
	for(int i = 0; i < 13; i++)
	{
		if(text_list[i].x == x && text_list[i].y == y)
		{
			return (i);
		}
		else if(text_list[i].y == y && text_list[i].x < x && text_list[i].x + text_list[i].contain.length() > x)
		{
			return (-1);
		}
		else if(i == 8 && is_login == 1 && text_list[i].y == y && text_list[i].x < x && text_list[i].x + text_list[i].contain.length() + to_string(money).length() > x)
		{
			return (-1);
		}
		else if(i == 9 && is_login == 1 && text_list[i].y == y && text_list[i].x < x && text_list[i].x + text_list[i].contain.length() + to_string(user_num).length() > x)
		{
			return (-1);
		}
	}
	return (-2);
}

int Ui::is_msg(int x, int y, account* account_list)
{
	if (is_login == 1 && x >= 31 && x < MAP_WIDTH && y > 4 && y <= 4 + user_num)
	{
		string output = account_list[y-4-1].name;
		for(int i = account_list[y-4-1].name.length(); i < 7; i++)
		{
			output += " ";
		}
		output += " ";
		output += account_list[y-4-1].ip;
		for(int i = account_list[y-4-1].ip.length(); i < 15; i++)
		{
			output += " ";
		}
		output += " ";
		output += to_string(account_list[y-4-1].port_num);
		for(int i = to_string(account_list[y-4-1].port_num).length(); i < 10; i++)
		{
			output += " ";
		}
		if(x == 31)
		{
			cout << output;
			return (1);
		}
		else if(x > 31)
		{
			return (1);
		}
	}
	return (0);
}

void Ui::draw()
{
	system("clear");
	//printf("\r\e");
	for (int i(0); i <= MAP_HEIGHT; i++) 
	{
        for (int j(0); j <= MAP_WIDTH; j++) 
		{
            if (is_map_border(j, i)) 
			{
                cout << '#' << flush;

            }
			else if (is_text(j, i) != -2)
			{
				cout << text_list[is_text(j,i)].contain;
				if (is_text(j, i) == 8 && is_login == 1)
				{
					cout << money;
				}
				else if(is_text(j, i) == 9 && is_login == 1)
				{
					cout << user_num;
				}
			}
			else if (is_text(j, i) == -1)
			{
				cout << "";
			}
			else if (is_login == 1 && is_msg(j, i, user->account_list))
			{
				cout << "";
			}
			else 
			{
                cout << ' ' << flush;
            }
        }
        cout << "\n";
    }
}

Ui::Ui()
{
	port = -1;
	money = 0;
	user_num = 0;
	login_port = -1;
	MAP_WIDTH = 65;
	MAP_HEIGHT = 20;
	text_num = 8;
	status = 1;
	is_login = 0;
	text_list = new text[13];
	text_list[0].x = 8;
	text_list[0].y = 4;
	text_list[0].contain = "[v]NAME:";
	text_list[1].x = 8;
	text_list[1].y = 6;
	text_list[1].contain = "[ ]IP:";
	text_list[2].x = 8;
	text_list[2].y = 8;
	text_list[2].contain = "[ ]PORT:";
	text_list[3].x = 8;
	text_list[3].y = 10;
	text_list[3].contain = "[ ]LOGIN PORT:";
	text_list[4].x = 12;
	text_list[4].y = 18;
	text_list[4].contain = "[ ]LIST";
	text_list[5].x = 22;
	text_list[5].y = 18;
	text_list[5].contain = "[ ]LOGIN";
	text_list[6].x = 34;
	text_list[6].y = 18;
	text_list[6].contain = "[ ]LOGOUT";
	text_list[7].x = 46;
	text_list[7].y = 18;
	text_list[7].contain = "[ ]EXIT";
	text_list[8].x = 8;
	text_list[8].y = 12;
	text_list[8].contain = "Money:";
	text_list[9].x = 8;
	text_list[9].y = 14;
	text_list[9].contain = "Online User:";
	text_list[10].x = 31;
	text_list[10].y = 4;
	text_list[10].contain = "Name";
	text_list[11].x = 39;
	text_list[11].y = 4;
	text_list[11].contain = "IP";
	text_list[12].x = 55;
	text_list[12].y = 4;
	text_list[12].contain = "Port";
}

void Ui::up()
{
	if (status == 1)
	{
		text_list[status - 1].contain[1] = ' ';
		status = text_num;
		text_list[status - 1].contain[1] = 'v';
	}
	else
	{
		text_list[status - 1].contain[1] = ' ';
		status--;
		cout << status << "\n";
		text_list[status - 1].contain[1] = 'v';
	}

	draw();
}

void Ui::down()
{
	if (status == text_num)
	{
		text_list[status - 1].contain[1] = ' ';
		status = 1;
		text_list[status - 1].contain[1] = 'v';
	}
	else
	{
		text_list[status - 1].contain[1] = ' ';
		status++;
		cout << status << "\n";
		text_list[status - 1].contain[1] = 'v';
	}
	draw();
}

int Ui::enter()
{

	switch(status)
	{
		case 5:
			return (5);
			break;
		case 6:
			return (6);
			break;
		case 7:
			return (7);
			break;
		case 8:
			return (8);
			break;
		case 1:
		case 2:
		case 3:
		case 4:
			if (is_login == true)
			{
				return (-1);
			}
			string input;
			switch(status)
			{
				case 1:
					input = text_list[status-1].contain.substr(8);
					break;

				case 2:
					input = text_list[status-1].contain.substr(6);
					break;
				case 3:
					input = text_list[status-1].contain.substr(8);
					break;
				case 4:
					input = text_list[status-1].contain.substr(14);
					break;
			}
			char type_in;
			type_in = 0;
			while(type_in != 10)
			{
				char new_in = 0;
				new_in = getchar();
				//cout << getchar();
				if(new_in)
				{
					if(new_in > 32 && new_in < 126)
					{
						input += new_in;
						text_list[status-1].contain += new_in;
						type_in = new_in;
						draw();
					}
					else if(new_in == 127)
					{
						if(input.length())
						{
							input = input.erase(input.length() - 1);
							text_list[status-1].contain = text_list[status-1].contain.erase(text_list[status-1].contain.length()-1);
							draw();
						}
					}
					type_in = new_in;
				}
			}
			switch(status)
			{
				case 1:
					name = input;
					break;
				case 2:
					ip = input;
					break;
				case 3:
					if (input.length() != 0)
					{
						port = stoi(input);
					}
					break;
				case 4:
					if (input.length() != 0)
					{
						login_port = stoi(input);
					}
					break;
			}
			break;
	}
	draw();
	return (0);
}

