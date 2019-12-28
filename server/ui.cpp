#include "server.h"

ui::ui(int _width, int _height, vector<online> *_onlne_list, vector<account> *_account_list)
{
	width = _width;
	height = _height;
	online_list = _onlne_list;
	account_list = _account_list;
	text title1(8,3,"Account List");
	title.push_back(title1);
	text title2(38,3,"Online List");
	title.push_back(title2);
	text title3(80,3,"Server Log");
	title.push_back(title3);
	text title4(8,5,"NAME");
	title.push_back(title4);
	text title5(20,5,"BALANCE");
	title.push_back(title5);
	text title6(38,5,"NAME");
	title.push_back(title6);
	text title7(50,5,"IP");
	title.push_back(title7);
	text title8(65,5,"PORT");
	title.push_back(title8);
}

bool ui::is_border(int x, int y) //判斷是不是邊框
{
	if(x == 0 || y == 0 || x == width - 1 || y == height - 1)
		return (1);
	return (0);
}

bool ui::is_msg(int x, int y) //判斷這個座標點上有沒有訊息 需不需要輸出空格
{
	for (int i = 0; i < (int)title.size(); i++)
	{
		if (title[i].x == x && title[i].y == y)
		{
			printf("%s",title[i].contain.c_str());
			return (1);
		}
		if (title[i].y == y && x > title[i].x && x < title[i].x + title[i].len)
		{
			return (1);
		}
	}

	for (int i = 0; i < (int)register_data.size(); i++)
	{
		if (register_data[i].x == x && register_data[i].y == y)
		{
			printf("%s",register_data[i].contain.c_str());
			return (1);
		}
		if (register_data[i].y == y && x > register_data[i].x && x < register_data[i].x + register_data[i].len)
		{
			return (1);
		}
	}

	for (int i = 0; i < (int)online_data.size(); i++)
	{
		if (online_data[i].x == x && online_data[i].y == y)
		{
			printf("%s",online_data[i].contain.c_str());
			return (1);
		}
		if (online_data[i].y == y && x > online_data[i].x && x < online_data[i].x + online_data[i].len)
		{
			return (1);
		}
	}

	for (int i = 0; i < (int)log_data.size(); i++)
	{
		if (log_data[i].x == x && log_data[i].y == y)
		{
			printf("%s",log_data[i].contain.c_str());
			return (1);
		}
		if (log_data[i].y == y && x > log_data[i].x && x < log_data[i].x + log_data[i].len)
		{
			return (1);
		}
	}

	return (0);
}

void ui::draw() //畫出圖形
{
	system("clear");
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			if (is_border(x, y))
				printf("#");
			else if(is_msg(x, y))
			{}
			else
				printf(" ");
		}
		printf("\n");
	}
}

void ui::update(string msg) //把server中的account listt和online list中的資料updat成文字
{
	register_data.clear();
	online_data.clear();
	for (int i = 0; i < (int)account_list->size(); i++)
	{
		string output;
		output += account_list->at(i).name;
		for (int j = 0; j < 12 - (int)account_list->at(i).name.length(); j++)
		{
			output += " ";
		}
		output += to_string(account_list->at(i).money);
		text temp(8,6 + i, output);
		register_data.push_back(temp);
	}
	for (int i = 0; i < (int)online_list->size(); i++)
	{
		string output;
		output += online_list->at(i).name;
		for (int j = 0; j < 12 - (int)online_list->at(i).name.length(); j++)
		{
			output += " ";
		}
		output += online_list->at(i).ip;
		for (int j = 0; j < 15 - (int)online_list->at(i).ip.length(); j++)
		{
			output += " ";
		}
		output += to_string(online_list->at(i).port_num);
		text temp(38,6 + i, output);
		online_data.push_back(temp);
	}
	if (log_data.size() >= 17)
	{
		log_data.erase(log_data.begin());
		for (int i = 0; i < (int)log_data.size(); i++)
		{
			log_data[i].y--;
		}
		text temp(80,5 + log_data.size(),msg);
		log_data.push_back(temp);
	}
	else
	{
		text temp(80,5 + log_data.size(),msg);
		log_data.push_back(temp);
	}
	draw();
}
