#ifndef UI_H
# define UI_H
# include "server.h"

struct text
{
	int x;
	int y;
	string contain;
};

class Ui
{
	private:
		int MAP_WIDTH;
		int MAP_HEIGHT;
	public:	
		void draw();
};

#endif
