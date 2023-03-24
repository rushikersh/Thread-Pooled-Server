// Client side C/C++ program to demonstrate Socket programming
// g++ -o c client.cpp -lpthread
//    ./c IP_Address
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
//#define PORT 8080             //intermediate port
#define PORT 9000               // main ReturnToServer.cpp port

int main(int argc, char const *argv[])
{
        for(int i=1;i<=1000;i++)
        {
                usleep(20000);
                int sockfd = 0, valread;
                struct sockaddr_in serv_addr;
                char buffer[1024] = {0};
                char hello[1024] = "Hello from Client";
                if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
                {
                        printf("\n Socket creation error \n");
                        return -1;
                }
                else
                {
                        printf("\n\nsockfd \n");
                }

                serv_addr.sin_family = AF_INET;
                serv_addr.sin_port = htons(PORT);

                // Convert IPv4 and IPv6 addresses from text to binary form
//              if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
                if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
                {
                        printf("\nInvalid address/ Address not supported \n");
                        return -1;
                }

                if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
                {
                        printf("\nConnection Failed \n");
                        return -1;
                }
                else
                {
                        printf("connect \n");
                }
                sprintf(hello,"Hello Msg from Client :'%d'",i);
                send(sockfd , hello , strlen(hello) , 0 );
                printf("Client%d  msg send to server :%s\n",i,hello);
                valread = read( sockfd , buffer, 1024);
                printf("Client%d  recv echo msg :%s\n",i,buffer );

                close(sockfd);
        }
        return 0;
}
