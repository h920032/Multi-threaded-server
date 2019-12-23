#ifndef UI_H
# define UI_H
# include "client.h"

struct text
{
	int x;
	int y;
	//bool show = 1;
	string contain;
};

class Ui
{
	private:
		int MAP_WIDTH;
		int MAP_HEIGHT;
		text* text_list;
		int text_num;
	public:		
		string name;
		string ip;
		int port;
		int login_port;
		bool is_login;
		int status;
		Ui();
		bool is_map_border(int x, int y);
		int is_text(int x, int y);
		int is_msg(int x, int y, account* account_list);
		void draw();
		void up();
		void down();
		void right();
		void left();
		int enter();
		int money;
		int user_num;
		Client* user;
};

#endif
