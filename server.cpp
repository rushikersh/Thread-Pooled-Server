
/* Server side C/C++ program to demonstrate Socket programming
   g++ -o s server.cpp -lpthread
   ./s

*/
#include <unistd.h>	//usleep()
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <arpa/inet.h>	// inet_addr()

#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#define PORT 9000
#include <signal.h>
#include <fcntl.h>	// fcntl()
#include <errno.h>	//error handling perror()
#include <string.h>	//strerror() 
struct threadpool
{
	int sfd=-1;
	int flag=0;
	int sid;
}*s;
int cflag=0;
void ctrl_c_exit(int sig)
{
	cflag=1;
	printf("\n interrupt signal id :%d\n ",cflag);
}

pthread_t tid[10];
int i=0;

int g=1;
void * func(void * s)
{
	threadpool * p =(threadpool *)s;
	char buffer[1024] = {0};
	char hello[1024] = {0};
	int valread;
	while(cflag==0)
	{
		if(p->flag<=0)
		{
			usleep(200); // cpu utilization
			continue;
		} 
		else if(p->flag==1)
		{
			usleep(200);
			int x =pthread_self();
			recv(p->sfd  , buffer, 1024,0);
			if(strcmp(buffer,"\0")==0)

			{
				close(p->sfd);  //	handle file descriptor leak
				p->sfd=-1;	
				p->flag=0;
				usleep(200); // cpu utilization
				continue;
			} 
			else{
				printf("recv msg from client %d :'%s'\n",p->sid,buffer );
				sprintf(hello,"Msg from Primary Server :'%s'",buffer);
				send(p->sfd , hello ,1024,0);
				printf("echo msg to client %d :%s\n\n",p->sid,hello);
				bzero(hello,sizeof(hello));
				bzero(buffer,sizeof(buffer));
				close(p->sfd);  //	handle file descriptor leak
				p->sfd=-1;	
				p->flag=0;
			}
		}
	}	
	return NULL;	
}

int main(int argc, char const *argv[])
{
	int server_fd, new_socket, valread,pid; 
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);

	char buffer[1024] = {0};
	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	else
	{
		perror("server ");
	}

	// Forcefully reuse socket to the port 8080
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
				&opt, sizeof(opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	else
	{
		perror("setsocket ");
	}
	address.sin_family = AF_INET;
	//	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_port = htons( PORT );

	// Forcefully attaching socket to the port 8080
	if (bind(server_fd, (struct sockaddr *)&address, 
				sizeof(address))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	else
	{
		perror("bind ");
	}
	// malloc allocation of structure s * 10
	if((	s=(threadpool *)malloc(sizeof(threadpool)*10))!=NULL);
	{
		perror("malloc size allloction ");
	}

	printf("sizeof s =%ld\n ",sizeof(s));
	//listen
	if (listen(server_fd, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	else
	{
		perror("listen ");
	}
	//signal
	signal(SIGINT,ctrl_c_exit);
	//pthread create
	s[0].sid=0;
	for(int i=0;i<10;i++)
	{
		s[i].sid=i+1;
		if(	pthread_create(&tid[i], NULL, func,(void *)&s[i])==0)
			printf(" thread %d create successfully now waiting \n",i);
	}
	// server_fd non block
	fcntl(server_fd, F_SETFL, O_NONBLOCK);//       fcntl - manipulate file descriptor
	//	Set the file status flags to 

	//If a process  tries  to perform an incompatible access (e.g., read(2) or write(2)) on a file region that has an incompatible mandatory lock, then the result depends upon whether  the  O_NONBLOCK  flag  is enabled for its open file description.  If the O_NONBLOCK flag is not enabled, then the system call is blocked until the lock is removed or converted to a mode that is compatible with the access.  If the O_NONBLOCK flag is enabled, then the system call fails with the error EAGAIN.


	while(cflag==0)
	{		//accept			
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
						(socklen_t*)&addrlen))<0)
		{
			if(errno == EAGAIN)	//The operation is prohibited because the file has been memory-mapped by another process.

			{
				usleep(500);
				continue;
			}
			perror("accept");
			exit(EXIT_FAILURE);
		}
		else
		{

			while(cflag==0)
			{
				for(int i=0;i<10;i++)
				{

					if(s[i].flag==0)
					{
						s[i].flag=1;
						s[i].sfd=new_socket;
						usleep(5000);
						//	printf("accept value :sfd: %d\tflag: %d\tsid: %d \n",s[i].sfd,s[i].flag,s[i].sid);
						break;
					}
				}
				if(i<10)
				{
					break;	
				}
			}		
		}
	}

	printf("return 0 to main \n") ;
	return 0;
}

/*
   You probably have a file descriptor leak.
   If your code keeps opening files, never closing them, you will reach the limit sooner or later, and then fopen will fail. max reach 1024.


*/

