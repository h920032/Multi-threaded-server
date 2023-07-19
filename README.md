# Experimental Multi Threaded Server in C++
2019 Computer Networks Programming Assignment
## Client
1. Execution environment: Ubuntu (Linux) 18.x
> $ ./client
> 
2. Programming language: c++
3. Required files: client executable file
4. Compilation method:
  *  First, make sure that the current directory contains: main.cpp, client.cpp, client.h, ui.cpp, ui.h, Makefile these six files
  *  Ensure the environment is in Ubuntu (Linux) 18.x
  *  Execute:
> $ make all
>
![](https://i.imgur.com/UYXbobk.png)


5. Execution process:
  *  Execute: 
> $ ./client
>
![](https://i.imgur.com/rZjxXVf.png)


  *  Operating screen:
![](https://i.imgur.com/tCDafPn.png)


  *  You can operate the selection position with W and S keys
  *  You can start typing and confirm input through enter
  *  After completing the input, select LOGIN and press enter to start login
  *  Display example:
![](https://i.imgur.com/TuJsSI7.png)


  *  Select LOGOUT to log out the account
  *  Select EXIT to end the program

---

## Server

1. Execution environment: Ubuntu(Linux) 18.x
> $ ./server <port_num>
>
2. Programming language: c++
3. Required files: server executable file
4. Compilation method:
  *  First, make sure that the current directory contains: main.cpp, server.cpp, server.h, ui.cpp, threadpool.h, Makefile these six files
  *  Ensure the environment is in Ubuntu (Linux) 18.x
  *  Execute:
> $ make all
>
![](https://i.imgur.com/LRlJPnR.png)


5. Execution process:
  *  Execution: assuming port is set to 5000
> $ ./server 5000
>
![](https://i.imgur.com/Y83RL28.png)


  *  Operating screen:
  ![](https://i.imgur.com/cd9czFj.png)
  
  *  The registered list will be listed under Account List
  *  The current online list will be listed under Online List
  *  The past Server prompt messages will be displayed under Server Log
  *  Display example:
![](https://i.imgur.com/dhiM50p.png)

6. Interaction message format: same as in the Programming Assignment explanation document
