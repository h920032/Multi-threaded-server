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
