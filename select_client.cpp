#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include<thread>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include<iostream>
using namespace std;
int main()
{
    int sockfd=socket(AF_INET,SOCK_STREAM,0);
    sockaddr_in sock_addr;
    memset(&sock_addr,0,sizeof(sock_addr));
    sock_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    sock_addr.sin_family=AF_INET;
    sock_addr.sin_port=htons(55555);

    if(connect(sockfd,(sockaddr * )&sock_addr,sizeof(sockaddr))==-1)
    {
        perror("connect()");
        return 0;
    }
    char buffer[256];
    int len=recv(sockfd,buffer,200,0);
    buffer[len]='\0';
    cout<<buffer<<endl;
    auto func=[](int sk){
       while(true){
	  char buffer[256];
          int len=recv(sk,buffer,200,0);
           if(len<=0){
	     cout<<"服务器断开连接"<<endl;
	     return ; 
	  }
           buffer[len]='\0';
           cout<<buffer<<endl;
	}
         return ;
    };
    thread th(func,sockfd);

    th.detach();
    while(true){
        cin>>buffer;
        send(sockfd,buffer,strlen(buffer),0);
    }
    close(sockfd);

    return 0;
}
