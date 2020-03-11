#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include<iostream>
using namespace std;
void Send(string &s,int idkey[20],int cntfd,int num){
    for(int i=0;i<cntfd;i++){
            if(num!=i)
        send(idkey[i],s.c_str(),s.length(),0);
    }
}
int main()
{
     perror("SOCKET()");
    int sockfd;
    sockaddr_in my_addr;
    memset(&my_addr,0,sizeof(my_addr));
    if((sockfd = socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("SOCKET()");
        return 0;
    }
    my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    my_addr.sin_family=AF_INET;
    my_addr.sin_port=htons(55555);

    if(::bind(sockfd,(struct sockaddr *)&my_addr,sizeof(my_addr))==-1){
        perror("bind()");
        return 0;
    }
    if(listen(sockfd,5)==-1){
        perror("listen()");
        return 0;
    }
    cout<<"listen ...."<<endl;

    fd_set fd;
    int maxfd=sockfd;
    int idkey[20]{0};

    int cntfd=0;
    timeval tim;
    tim.tv_sec=0;
    tim.tv_usec=500;
    char buffer[256];
    while(true){
        FD_ZERO(&fd);
        FD_SET(sockfd,&fd);
	maxfd = sockfd;
        for(int i=0;i<cntfd;i++){
            if(idkey[i]!=0){
             maxfd =max(maxfd,idkey[i]);   
	     FD_SET(idkey[i],&fd);
            }
        }
        if(select(maxfd+1,&fd,NULL,NULL,&tim)<0){
            perror("select()");
            return 0;
        };
	
        for(int i=0;i<cntfd;i++){
            if(FD_ISSET(idkey[i],&fd)){
                int len=recv(idkey[i],buffer,200,0);
                if(len<=0){
                    close(idkey[i]);
                    FD_CLR(idkey[i],&fd);
                    idkey[i]=idkey[--cntfd];

                }else{
		   
                    buffer[len]='\0';
                    cout<<idkey[i]<<"fasong"<<buffer<<endl;
		    string s = to_string(idkey[i])+":"+buffer;
                    Send(s,idkey,cntfd,i);
                }


            }
        }
        if(FD_ISSET(sockfd,&fd)){
            sockaddr_in csock;
            socklen_t len = sizeof(csock);
            int cfd=accept(sockfd,(sockaddr*)&csock,&len);
            idkey[cntfd++]=cfd;
	    cout<<"idkey"<<cntfd-1<<idkey[cntfd-1]<<endl;
            strcpy(buffer,"connect() ok\0");
            send(cfd,buffer,strlen(buffer),0);
        }
    }
    return 0;
}

