#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#define AWSPORT "25537" //For client connecting PORT
#define MAXDATASIZE 512 //Maxium number of bytes

//For get IPV4 or IPV6 sockaddr: From Beej's guideline
void *get_in_addr(struct sockaddr *sa)
	{
	     if (sa->sa_family==AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
		}
	     return &(((struct sockaddr_in6*)sa)->sin6_addr);
	}



int main (int argc, char *argv[])
{
	
	char* numberInput;
	int sockfd, numbytes;
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];
	float number;
	int functionFlag;//I will pass this int value stand for function identifier to aws, 0 stand for LOG, and 1 stand for DIV
	char functionName[3];
	float numberRecv;
	
	if (argc!=3){
		printf("input need to be <Function> and <number>\n");
		return 1;
	}
	if (argc==3){
		if (strcmp(argv[1],"LOG")==0){
		functionFlag=0;
		strcpy(functionName,"LOG");}
		if (strcmp(argv[1],"DIV")==0) {functionFlag=1;
		strcpy(functionName,"DIV");}
		numberInput=argv[2];
		number=atof(numberInput);
		}
	//from Beej's guideline, initialize the tcp connection, and TCP connect
	memset(&hints, 0, sizeof hints);
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM;
        
	if ((rv=getaddrinfo("127.0.0.1", AWSPORT, &hints, &servinfo))!=0){
	fprintf(stderr,"getaddrinfo:%s\n",gai_strerror(rv));
	return 1;
	}
	
	for (p=servinfo; p!=NULL;p=p->ai_next){
	if ((sockfd=socket(p->ai_family, p->ai_socktype,p->ai_protocol))==-1){
	perror("client:socket");
	continue;
	}

	if (connect(sockfd, p->ai_addr,p->ai_addrlen)==-1){
	close(sockfd);
	perror("client:connect");
	continue;
	}

	break;
	}

	if (p==NULL){
	fprintf(stderr, "client:falied to conncet\n");
	return 2;
	}
	
	freeaddrinfo(servinfo);
	printf("The client is up and running\n");

	//Send the function identifier to aws, and then send input value to aws
	//TCP is reliable protocol, so I send function name and value one by one
	if(send(sockfd,&functionFlag,sizeof(int),0)==-1){
	perror("send");}

	if(send(sockfd,&number, sizeof(float),0)==-1){
	perror("send");}
	
	printf("The client sent <%g> and %s to AWS\n",number,functionName);
	
	//recv the final caculated value from aws, and then close connection, and quit client
	if(recv(sockfd,&numberRecv,sizeof(float),0)==-1){
	perror("recieve");
	exit(1);}
	
	printf("According to AWS, %s on <%g>:<%f>\n",functionName,number,numberRecv);
	
	
	
	close(sockfd);
	return 0; 
	
	

}
