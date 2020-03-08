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
const int SERV_PORT = 7777;

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in send_addr; //服务器套接字地址
    struct sockaddr_in recv_addr;
    socklen_t send_len, recv_len;
    char send_buf[BUFLEN], recv_buf[BUFLEN];    

    /*建立socket*/
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
        perror("socket");
        exit(errno);
    }

    /*设置服务器ip*/
    bzero(&send_addr, sizeof(send_addr));
    send_addr.sin_family = AF_INET;
    send_addr.sin_port = htons(SERV_PORT);

    if(inet_aton(IP, (struct in_addr*)&send_addr.sin_addr) == 0)
    {
        perror("IP error");
        exit(errno);
    }

    printf("*****************client start***************\n");

    send_len = sizeof(struct sockaddr);

    while(1)
    {
        int n = 0;
    _retry:    
        /******发送消息*******/    
        bzero(send_buf,BUFLEN);
        printf("enter your words:");
        /*fgets函数：从流中读取BUFLEN-1个字符*/
        fgets(send_buf,BUFLEN,stdin);

        if(!strncasecmp(send_buf,"quit",4))
        {
            printf("client stop\n");
            break;
        }


        /*如果输入的字符串只有"\n"，即回车，那么请重新输入*/
        if(!strncmp(send_buf,"\n",1))
        {
            goto _retry;
        }


        /*如果buf中含有'\n'，那么要用strlen(buf)-1，去掉'\n'*/    
        if(strchr(send_buf,'\n'))
        {
            n = sendto(sockfd,send_buf,strlen(send_buf)-1,0, (struct sockaddr*)&send_addr, send_len);
        }
        /*如果buf中没有'\n'，则用buf的真正长度strlen(buf)*/    
        else
        {
            n = sendto(sockfd,send_buf,strlen(send_buf),0, (struct sockaddr*)&send_addr, send_len);
        }


        if(n == -1)
        {
            printf("send failed\n");            
            break;
        }
        else
        {
            printf("send successful\n");
        }

        /******接收消息*******/
        bzero(recv_buf,BUFLEN);
        n = recvfrom(sockfd,recv_buf,BUFLEN,0, (struct sockaddr*)&recv_addr, &recv_len);

        if(n > 0)
        {
            printf("receive massage:%s\n",recv_buf);
        }
        else
        {
            if(n < 0 )
                printf("receive failed\n");
            else
                printf("server stop\n");
            break;    
        }
    }
    /*关闭连接*/
    close(sockfd);

    return 0;
}
