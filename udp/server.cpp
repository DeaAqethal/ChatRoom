#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFLEN 100

const char* IP = "127.0.0.1";
const unsigned int SERV_PORT = 7777;
void Chat(int sockfd);

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in s_addr, c_addr;
    char buf[BUFLEN];
    socklen_t len;

    /*建立socket*/
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
        perror("socket");
        exit(errno);
    }

    /*设置服务器ip*/
    bzero(&s_addr, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(SERV_PORT);
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    /*把地址和端口帮定到套接字上*/
    if((bind(sockfd, (struct sockaddr*) &s_addr,sizeof(struct sockaddr))) == -1){
        perror("bind");
        exit(errno);
    }
    printf("*****************server start***************\n");

    len = sizeof(struct sockaddr);

    while(1)
    {
        int n = 0;

        /******接收消息*******/
        bzero(buf,BUFLEN);
        n = recvfrom(sockfd, buf, BUFLEN, 0, (struct sockaddr*)&c_addr, &len);
        if(n > 0)
            printf("receive massage:%s\n",buf);
        else
        {
            if(n < 0 )
                printf("receive failed\n");
            else//服务器调用close函数后，系统阻塞函数调用，返回0
                printf("client stop\n");
            break;        
        }

    _retry:
        /******发送消息*******/
        bzero(buf,BUFLEN);
        printf("enter your words:");
        /*fgets函数：从流中读取BUFLEN-1个字符*/
        fgets(buf,BUFLEN,stdin);

        if(!strncasecmp(buf,"quit",4))
        {
            printf("server stop\n");
            break;
        }

        /*如果输入的字符串只有"\n"，即回车，那么请重新输入*/
        if(!strncmp(buf,"\n",1))
        {
            goto _retry;
        }    

        /*如果buf中含有'\n'，那么要用strlen(buf)-1，去掉'\n'*/            
        if(strchr(buf,'\n'))
        {
            n = sendto(sockfd, buf, strlen(buf)-1, 0, (struct sockaddr*)&c_addr, len);
        }
        /*如果buf中没有'\n'，则用buf的真正长度strlen(buf)*/    
        else
        {
            n = sendto(sockfd,buf,strlen(buf),0, (struct sockaddr*)&c_addr, len);
        }

        if(n > 0)
            printf("send successful\n");            
        else{
            printf("send failed\n");
            break;            
        }


        }
    /*关闭已连接套接字*/
    close(sockfd);
    return 0;
}
