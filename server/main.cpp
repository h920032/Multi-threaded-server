#include "server.h"
#include "threadpool.h"

int main(int argc , char *argv[]) //main函數
{
	try
	{
		string input;
		if (argc == 2)
		{
			input = argv[1];
		}
		else if(argc < 2)
		{
			throw invalid_argument("...");
		}
		else
		{
			throw connect_error();
		}
		if(!(input.find_first_not_of( "0123456789" ) == std::string::npos))
		{
			throw connect_error();
		}
		int port = stoi(input);
		Server test(port); //創建server
		if (test.create_socket() == 0)	//開啟socket
		{
			throw connect_error();
		}
		test.accept_loop();
		if (test.close_server())
			return (0);
		exit(1);
		return (0);
	}
	catch (connect_error& e)
	{
		printf("fail binding");
		return (0);
	}
	catch (invalid_argument& e)
	{
		printf("please type ./server serverPort\n");
		return (0);
	}
}
