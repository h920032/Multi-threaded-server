# Computer Networks Programming Assignment
## Client
1. 執行環境：Ubuntu(Linux) 18.x
> $ ./client
> 
2. 程式語言：c++
3. 需求檔案：client 執行檔
4. 編譯方式：
*  先確認當前目錄有：main.cpp、client.cpp、client.h、ui.cpp、ui.h、Makefile這六個檔案
*  確認環境位於Ubuntu(Linux) 18.x下
*  執行：
> $ make all
>
![](https://i.imgur.com/UYXbobk.png)



5. 執行過程：
*  執行：
> $ ./client
>
![](https://i.imgur.com/rZjxXVf.png)



*  操作畫面：
![](https://i.imgur.com/tCDafPn.png)



*  透過W與S鍵可以操作勾選位置
*  透過enter可以開始輸入以及確認輸入
*  完成輸入後選到LOGIN的地方按enter可以開始登入
*  顯示範例：
![](https://i.imgur.com/TuJsSI7.png)


*  選擇LOGOUT可以登出帳號
*  選擇EXIT可以結束程式

---

## Server

1. 執行環境：Ubuntu(Linux) 18.x
> $ ./server <port_num>
>
2. 程式語言：c++
3. 需求檔案：server 執行檔
4. 編譯方式：
*  先確認當前目錄有：main.cpp、server.cpp、server.h、ui.cpp、threadpool.h、Makefile這六個檔案
*  確認環境位於Ubuntu(Linux) 18.x下
*  執行：
> $ make all
>
![](https://i.imgur.com/LRlJPnR.png)


5. 執行過程：
*  執行：假設port設為5000
> $ ./server 5000
>
![](https://i.imgur.com/Y83RL28.png)


*  操作畫面：
![](https://i.imgur.com/cd9czFj.png)

*  Account List下會列出已註冊的名單
*  Online List會列出目前在線者的名單
*  Server Log下會顯示過去Server的提示訊息
*  顯示範例：
![](https://i.imgur.com/dhiM50p.png)

6. 互動訊息格式：與Programming Assignment說明文件相同

