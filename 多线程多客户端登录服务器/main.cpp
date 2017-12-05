#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include"server.h"
using namespace std;

//主函数入口
int main()
{
	Server server;
	server.WaitForClient();
	system("pause");
	return 0;
}