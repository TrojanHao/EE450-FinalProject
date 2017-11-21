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

#define TCPPORT "25537"
#define UDPPORT "24537"
#define SERVERA "21537"
#define SERVERB "22537"
#define SERVERC "23537"
#define BACKLOG 10


//This function is used to caculate the final result within x1,x2,x3,x4,x5,x6
static float caculator(int func,float x1,float x2,float x3,float x4,float x5,float x6){
	float cacuValue=0;
	if (func==0){
		cacuValue=-x1-x2/2-x3/3-x4/4-x5/5-x6/6;}
	if (func==1){
		cacuValue=1+x1+x2+x3+x4+x5+x6;}
	return cacuValue;


}


//This function writen by Beej
void sigchld_handler(int s)
{
	while(waitpid(-1,NULL,WNOHANG)>0);
}

//For get IPV4 or IPV6 sockaddr: From Beej's guideline
void *get_in_addr(struct sockaddr *sa)
	{
	     if (sa->sa_family==AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
		}
	     return &(((struct sockaddr_in6*)sa)->sin6_addr);
	}


//Main function, the part of initialize TCP and Udp, Tcp listen() accept() and Udp sendto() and recvfrom() writen based on Beej's guide 
int main(void)
{
int sockfd, new_fd; // use sock_fd to listen, new_FD is new connection
struct addrinfo hints, *servinfo, *p;
struct sockaddr_storage their_addr,their_Udp_addr; //connection address
socklen_t sin_size;
struct sigaction sa;
int yes=1;
char s[INET6_ADDRSTRLEN];
int rv;
socklen_t addr_len;
float numberInput;
char functionName[3];// function name was sent from client
int functionFlag;//Since client sent 0 or 1 stand for LOG or DIV, functionFlag was used to identify function
int numbytes,numbytes_A,numbytes_B,numbytes_C;
float x2;
float x4;
float x3;
float x5;
float x;
float x6;
float tmpp1;// tmpp# is used for temporary store the value of recvfrom, since I will use them to compare the value, and pass value to actual x#
float tmpp2;
float tmpp3,tmpp5,tmpp6,tmpp4;
float result;//float result, value of final result which will be sent to client


//Tcp initialize from Beej's code
memset(&hints, 0, sizeof hints);
hints.ai_family=AF_UNSPEC;
hints.ai_socktype=SOCK_STREAM;

if ((rv=getaddrinfo("127.0.0.1",TCPPORT,&hints,&servinfo))!=0){
	fprintf(stderr,"getaddrinfo:%s\n",gai_strerror(rv));
	return 1;
	}
	
	for (p=servinfo; p!=NULL;p=p->ai_next){
	if ((sockfd=socket(p->ai_family, p->ai_socktype,p->ai_protocol))==-1){
	perror("server:socket");
	continue;
	}

	if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR, &yes, sizeof(int))==-1){
	perror("setsockopt");
	exit(1);
	}

	if (bind(sockfd,p->ai_addr,p->ai_addrlen)==-1){
	close(sockfd);
	perror("server:bind");
	continue;
	}

	break;
	}

	if (p==NULL){
	fprintf(stderr, "server:falied to bind\n");
	return 2;
	}
	
	freeaddrinfo(servinfo);

	
	if (listen(sockfd,BACKLOG)==-1){
	perror("listen");
	exit(1);
	}
	printf("The aws is up and running.\n");
	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
}
	
//End the Tcp initialization	

//For initialize UDP's socket, came from Beej's code 
	int sockfd_Udp;//this sockfd_Udp is stand for aws Udp socket to sendto and recvfrom function
	struct addrinfo hints_Udp, *servinfo_Udp, *p_Udp;
	int rv_Udp;
	int numbytes_Udp;
	
	

	memset(&hints_Udp, 0, sizeof hints_Udp);
	hints_Udp.ai_family = AF_UNSPEC;
	hints_Udp.ai_socktype = SOCK_DGRAM;

	if ((rv_Udp = getaddrinfo("127.0.0.1", UDPPORT, &hints_Udp, &servinfo_Udp)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv_Udp));
		return 1;
	}

	// loop through all the results and make a socket
	for(p_Udp = servinfo_Udp; p_Udp != NULL; p_Udp = p_Udp->ai_next) {
		if ((sockfd_Udp= socket(p_Udp->ai_family, p_Udp->ai_socktype,
				p_Udp->ai_protocol)) == -1) {
			perror("talker: socket");
			continue;
		}

		break;
	}

	if (p_Udp == NULL) {
		fprintf(stderr, "talker: failed to create socket\n");
		return 2;
	}
	freeaddrinfo(servinfo_Udp);
	//end initialize Socket of AWS UDP function within static port	

//For initialize servA's socket, was edited from Beej's talker.c and listener.c
	int sockfd_A;//socket for serverA, but not used in aws.c
	struct addrinfo hints_A, *servinfo_A, *p_A, *test_A;
	int rv_A;
	
	
	

	memset(&hints_A, 0, sizeof hints_A);
	hints_A.ai_family = AF_UNSPEC;
	hints_A.ai_socktype = SOCK_DGRAM;


	if ((rv_A = getaddrinfo("127.0.0.1", SERVERA, &hints_A, &servinfo_A)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv_A));
		return 1;
	}
	for(p_A = servinfo_A; p_A != NULL; p_A = p_A->ai_next) {
		if ((sockfd_A= socket(p_A->ai_family, p_A->ai_socktype,
				p_A->ai_protocol)) == -1) {
			perror("talker: socket");
			continue;
		}

		break;
	}

	// loop through all the results and make a socket
	p_A = servinfo_A;
	if (p_A == NULL) {
		fprintf(stderr, "talker: failed to create socket\n");
		return 2;
	}
	
	
	//end initialize serverA	
	
//For initialize servB's socket, was edited from Beej's talker.c and listener.c 
	int sockfd_B;//socket for serverB, but not used in aws.c
	struct addrinfo hints_B, *servinfo_B, *p_B;
	int rv_B;
	
	
	

	memset(&hints_B, 0, sizeof hints_B);
	hints_B.ai_family = AF_UNSPEC;
	hints_B.ai_socktype = SOCK_DGRAM;

	if ((rv_B = getaddrinfo("127.0.0.1", SERVERB, &hints_B, &servinfo_B)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv_B));
		return 1;
	}
	
	// loop through all the results and make a socket
	for(p_B = servinfo_B; p_B != NULL; p_B = p_B->ai_next) {
		if ((sockfd_B= socket(p_B->ai_family, p_B->ai_socktype,
				p_B->ai_protocol)) == -1) {
			perror("talker: socket");
			continue;
		}

		break;
	}
	
	 

	if (p_B == NULL) {
		fprintf(stderr, "talker: failed to create socket\n");
		return 2;
	}
	
	//end initialize serverB	


//For initialize servC's socket, was edited from Beej's talker.c and listener.c 
	int sockfd_C;//socket for serverC, but not used in aws.c
	struct addrinfo hints_C, *servinfo_C, *p_C;
	int rv_C;
	
	
	

	memset(&hints_C, 0, sizeof hints_C);
	hints_C.ai_family = AF_UNSPEC;
	hints_C.ai_socktype = SOCK_DGRAM;

	if ((rv_C = getaddrinfo("127.0.0.1", SERVERC, &hints_C, &servinfo_C)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv_C));
		return 1;
	}

	// loop through all the results and make a socket
	for(p_C = servinfo_C; p_C != NULL; p_C = p_C->ai_next) {
		if ((sockfd_C= socket(p_C->ai_family, p_C->ai_socktype,
				p_C->ai_protocol)) == -1) {
			perror("talker: socket");
			continue;
		}

		break;
	}
	
	if (p_C == NULL) {
		fprintf(stderr, "talker: failed to create socket\n");
		return 2;
	}
	
	//end initialize serverC


	//main loop, Since aws will always run untill terminate it, use while(1) keep it running
	while(1){





	//Below is for TCP receiving using, was written based on Beej's code 
	sin_size=sizeof their_addr;
	new_fd=accept(sockfd, (struct sockaddr *)&their_addr,&sin_size);
	
	if (new_fd==-1){
	perror("accept");
	continue;}
	//Since tcp is reliable connction protocol, transfer value will recv in order
	//First recv the function identifier, 0 stand for LOG, 1 stand for DIV
	//Second recv the input value, which is a float number sent from client
	if(recv(new_fd,&functionFlag,sizeof(int),0)==-1){
	perror("recieve");	
	}
	if(recv(new_fd,&numberInput,sizeof(float),0)==-1){
	perror("recieve");
	}
	
	if (functionFlag==0){
	strcpy(functionName,"LOG");
	}
	if (functionFlag==1){
	strcpy(functionName,"DIV");
	}
	printf("The AWS received input <%g> and function=%s from client using TCP over port 25537\n",numberInput,functionName);
	x=numberInput;
	
	//Below is Udp function like sendto and recvfrom, as you can see, all of these functions use aws UDP socket within static port 24537
	
	addr_len = sizeof their_addr;// This is used for temporary saving Udp source destination
	//send x to serverA,B,C, and recev the caculated value from back-server
	if ((numbytes = sendto(sockfd_Udp, &numberInput, sizeof(float), 0,
			 p_A->ai_addr, p_A->ai_addrlen)) == -1) {
		perror("talker: sendto");
		exit(1);
	}
	printf("The AWS: sent <%g> to Backend-Server A\n", numberInput);
	if ((numbytes = sendto(sockfd_Udp, &numberInput, sizeof(float), 0,
			 p_B->ai_addr, p_B->ai_addrlen)) == -1) {
		perror("talker: sendto");
		exit(1);
	}
	printf("The AWS: sent <%g> to Backend-Server B\n", numberInput);
	if ((numbytes = sendto(sockfd_Udp, &numberInput, sizeof(float), 0,
			 p_C->ai_addr, p_C->ai_addrlen)) == -1) {
		perror("talker: sendto");
		exit(1);
	}
	printf("The AWS: sent <%g> to Backend-Server C\n", numberInput);
	//recvfrom backserverA,B,C within aws UDP socket with static port 24537, based on Beej's code
	if ((numbytes_A = recvfrom(sockfd_Udp, &tmpp1,sizeof(float) , 0,
		(struct sockaddr *)&their_Udp_addr, &addr_len)) == -1) {
		perror("recvfrom");
		exit(1);
	} 
	
	if ((numbytes_B = recvfrom(sockfd_Udp, &tmpp2,sizeof(float) , 0,
		(struct sockaddr *)&their_Udp_addr, &addr_len)) == -1) {
		perror("recvfrom");
		exit(1);
	}
	
	if ((numbytes_C = recvfrom(sockfd_Udp, &tmpp3,sizeof(float) , 0,
		(struct sockaddr *)&their_Udp_addr, &addr_len)) == -1) {
		perror("recvfrom");
		exit(1);
	
}	
	//Below is written by myself, since input value will be (0,1), So there will not be equal value from x2 x3 x5
	//And we know Udp is connectionless protocol, so it may receive value not in the right order, I need to compare them, and identify which one is x2,x3,x5
	//Biggest one is x2,the smallest one is x5, the middle one is x3
	if (tmpp1>tmpp2&&tmpp2>tmpp3){
	x2=tmpp1;
	x3=tmpp2;
	x5=tmpp3;
}
	if (tmpp1>tmpp3&&tmpp3>tmpp2){
	x2=tmpp1;
	x3=tmpp3;
	x5=tmpp2;
}
	
	if (tmpp2>tmpp1&&tmpp1>tmpp3){
	x2=tmpp2;
	x3=tmpp1;
	x5=tmpp3;
}
	
	if (tmpp2>tmpp3&&tmpp3>tmpp1){
	x2=tmpp2;
	x3=tmpp3;
	x5=tmpp1;
}
	
	if (tmpp3>tmpp1&tmpp1>tmpp2){
	x2=tmpp3;
	x3=tmpp1;
	x5=tmpp2;
}
	
	if (tmpp3>tmpp2&&tmpp2>tmpp1){
	x2=tmpp3;
	x3=tmpp2;
	x5=tmpp1;
}
	//Below is testcode, Nevermind
	//printf("The AWS: Treceived <%g> Backend-Server <A> using UDP over port 24537\n", tmpp1);
	//printf("The AWS: Treceived <%g> Backend-Server <B> using UDP over port 24537\n", tmpp2);
	//printf("The AWS: Treceived <%g> Backend-Server <C> using UDP over port 24537\n", tmpp3);
	
	printf("The AWS: received <%g> Backend-Server <A> using UDP over port 24537\n", x2);
	printf("The AWS: received <%g> Backend-Server <B> using UDP over port 24537\n", x3);
	printf("The AWS: received <%g> Backend-Server <C> using UDP over port 24537\n", x5);
	
	float test1=1;

	
	//After comparing we got right order and right value of x2,x3,x5
	//And next send x2 to backserverA,B
	if ((numbytes = sendto(sockfd_Udp, &x2, sizeof(float), 0,
			 p_A->ai_addr, p_A->ai_addrlen)) == -1) {
		perror("talker: sendto");
		exit(1);
	}
	printf("The AWS: sent <%g> to Backend-Server A\n",x2 );
	if ((numbytes = sendto(sockfd_Udp, &x2, sizeof(float), 0,
			 p_B->ai_addr, p_B->ai_addrlen)) == -1) {
		perror("talker: sendto");
		exit(1);
	
	
}
	printf("The AWS: sent <%g> to Backend-Server B\n", x2);
	//Recv caculated value from backserver, and same as above, it may recevied in not right order	
	if ((numbytes_A = recvfrom(sockfd_Udp, &tmpp4,sizeof(float) , 0,
		(struct sockaddr *)&their_Udp_addr, &addr_len)) == -1) {
		perror("recvfrom");
		exit(1);
	} 
	
	if ((numbytes_B = recvfrom(sockfd_Udp, &tmpp6,sizeof(float) , 0,
		(struct sockaddr *)&their_Udp_addr, &addr_len)) == -1) {
		perror("recvfrom");
		exit(1);
	}
	//Comparing these two values, and identify which one is x4,x6, bigger one should be x4, writen by myself
	if (tmpp4>tmpp6){
	x4=tmpp4;
	x6=tmpp6;}
	if (tmpp6>tmpp4){
	x4=tmpp6;
	x6=tmpp4;
	}
	printf("The AWS: received <%g> Backend-Server <A> using UDP over port 24537\n", x4);
	printf("The AWS: received <%g> Backend-Server <B> using UDP over port 24537\n", x6);

	printf("Values of power received by AWS:<%g,%g,%g,%g,%g,%g>\n",x,x2,x3,x4,x5,x6);
	//pass value and funciton identifier to caculator function, writen by myself
	result=caculator(functionFlag,x,x2,x3,x4,x5,x6);
	printf("AWS caculated %s on <%g>:<%f>\n",functionName,x,result);
	
	
	
	//Send result to client through TCP connection, and close TCP connection
	if(send(new_fd,&result, sizeof(float),0)==-1){
	perror("send");}

	

	 
	
	close(new_fd);
	printf("AWS sent <%f> to client\n",result);

}
	
	return 0;
}
