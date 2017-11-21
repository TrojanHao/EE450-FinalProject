#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MYPORT "22537"	// the port users will be connecting to serverB

#define MAXBUFLEN 100

// get sockaddr, IPv4 or IPv6: Most part of Udp initialization and recvfrom sendto function came from Beej's code
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
	struct sockaddr_storage their_addr;
	float recv;
	char buf[MAXBUFLEN];
	socklen_t addr_len;
	char s[INET6_ADDRSTRLEN];
	float xCube;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
	hints.ai_socktype = SOCK_DGRAM;
	

	if ((rv = getaddrinfo("127.0.0.1", MYPORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("listener: socket");
			continue;
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("listener: bind");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "listener: failed to bind socket\n");
		return 2;
	}

	freeaddrinfo(servinfo);

	printf("The serverB is up and running using UDP on port 22537\n");
	//since it will always running, I used while(1) to keep serverB running
	while(1){
	addr_len = sizeof their_addr;
	if ((numbytes = recvfrom(sockfd, &recv,sizeof(float) , 0,
		(struct sockaddr *)&their_addr, &addr_len)) == -1) {
		perror("recvfrom");
		exit(1);
	}
	//caculate x's cube
	xCube=recv*recv*recv;
	printf("The serverB received input <%g>\n",recv);
	printf("The serverB caculated cube:<%g>\n",xCube);
	//buf[numbytes] = '\0';
	//send caculated value back to aws
	if ((numbytes = sendto(sockfd, &xCube,sizeof(float) , 0,
		(struct sockaddr *)&their_addr, addr_len)) == -1) {
		perror("recvfrom");
		exit(1);
	}
	printf("The serverB finished sending the output to AWS\n");

}

	close(sockfd);

	return 0;
}
