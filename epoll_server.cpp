#include <sys/select.h>
#include<sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include<thread>
#include<sys/poll.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include<iostream>
using namespace std;
#define BUFSIZE 256
#define MAX_EVENTS 20
char buf[BUFSIZE];
int main()
{
    int listenfd,clientfd,sockfd,epollfd,nfds;
    int nready;
    int n;
    socklen_t clilen;
    sockaddr_in clientaddr,serveraddr;
    epoll_event ev,events[MAX_EVENTS];
    listenfd=socket(AF_INET,SOCK_STREAM,0);
    bzero(&serveraddr,sizeof(serveraddr));
    serveraddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(55555);

    bind(listenfd,(sockaddr *)&serveraddr,sizeof(sockaddr));
    listen(listenfd,50);

    epollfd = epoll_create(20);
    ev.data.fd=listenfd;
    ev.events=EPOLLIN;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,listenfd,&ev);

    while(true){
      nready = epoll_wait(epollfd,events,MAX_EVENTS,-1);
      if(nready==-1)break;
      printf("ready %d\n",nready);
      for(int i=0;i<nready;i++){

        if(events[i].data.fd==listenfd){
            clilen=sizeof(clientaddr);
            if((clientfd=accept(listenfd,(sockaddr*)&clientaddr,&clilen))<0){
                    perror("accept()");
                    continue;
            }
            ev.events = EPOLLIN|EPOLLET;
            ev.data.fd = clientfd;
            if(epoll_ctl(epollfd,EPOLL_CTL_ADD,clientfd,&ev)<0){
                    perror("epoll_ctr()");
                    break;
            }
            send(clientfd,"okk",3,0);
        }else if(events[i].events&EPOLLIN){
            sockfd = events[i].data.fd;
            int len=recv(sockfd,buf,BUFSIZE,0);
            if(len<0){
                close(sockfd);
                perror("read error");
                break;
            }else if(len==0){
                close(sockfd);
                epoll_ctl(epollfd,EPOLL_CTL_DEL,sockfd,NULL);
                continue;
            }else{
                buf[len]='\0';
                cout<<"buf("<<sockfd<<"):"<<buf<<endl;
                send(sockfd,buf,len,0);
            }
        }
      }
    }
    close(epollfd);
    return 0;
}
