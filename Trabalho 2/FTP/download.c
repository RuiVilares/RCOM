/*
 * download.c -- a ftp client
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#define PORT "21" // the port client will be connecting to
#define MAXDATASIZE 10000 // max number of bytes we can get at once

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

//abstraction to send user and pass cmd
void sendCmdAndWaitResponse(int fd, char *cmd, int length) {

	char buf[MAXDATASIZE];

	if (write(fd, cmd, length) == -1) {
		perror("send");
		exit(1);
	}
	if (read(fd, buf, MAXDATASIZE-1) > 0) {
		printf("%s\n", buf);
	}
}

//enter passive mode. it returns the port where the data will came out
//server_address will be the ip of the server
int enterPassive(int sockfd, char *server_address) {
	char response[MAXDATASIZE];
	write(sockfd, "pasv\r\n", strlen("pasv\r\n"));
	read(sockfd, response, MAXDATASIZE-1);

	printf("%s\n", response);

	int ipPart1, ipPart2, ipPart3, ipPart4;
	int port1, port2;
	sscanf(response, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)", &ipPart1,
			&ipPart2, &ipPart3, &ipPart4, &port1, &port2);

	sprintf(server_address, "%d.%d.%d.%d", ipPart1, ipPart2, ipPart3, ipPart4);

	return port1 * 256 + port2;
}

//parse information of the user
void parseUser(char *argv, char *sendUser, char *sendPass, char *host, char *path, char *userInfo) {

	char *username = strchr(argv+strlen("ftp://"), ':');
	if (username == NULL || username > userInfo) {
		fprintf(stderr, "Check the user information\n");
		exit(1);
	}

	char *field = argv+strlen("ftp://");
	strcpy(sendUser, "USER ");
	strncat(sendUser, field, abs(username - field));
	strncat(sendUser, "\r\n", 2);

	field = username + 1;
	strcpy(sendPass, "PASS ");
	strncat(sendPass, field, abs(userInfo - field));
	strncat(sendPass, "\r\n", 2);

	userInfo = userInfo + 1;
	field = strchr(userInfo, '/');
	strncpy(host, userInfo, abs(field - userInfo));
	strcat(path, field+1);
	strncat(path, "\r\n", 2);
}

//parse filename
void parseFilename(char *argv, char *filename) {
	strcpy(filename, ".");

	char *name = strrchr(argv, '/');

	strcat(filename, name);
}

//para testar
//ftp://user:pass@ftp.funet.fi/pub/standards/RFC/rfc959.txt
//ftp://ftp.up.pt/pub/CPAN/RECENT-1M.json
int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr,"usage: download ftp://[<user>:<password>@]<host>/<url-path>\n");
		return 1;
	}
	if (strncmp("ftp://", argv[1], strlen("ftp://")) != 0) {
		fprintf(stderr,"The link must start with ftp://\n");
		return 1;
	}

	char *userInfo = strchr(argv[1], '@');
	char sendUser[1000]; char sendPass[1000]; char host[1000];
	char path[1000]; strcpy(path, "RETR ");
	//verfica se existe um utilizador
	if (userInfo != NULL) {
		parseUser(argv[1], sendUser, sendPass, host, path, userInfo);
	}
	else {
		char *begin = argv[1]+strlen("ftp://");
		char *field = strchr(begin, '/');
		strncpy(host, begin, abs(field - begin));
		strcat(path, field+1);
		strncat(path, "\r\n", 2);
	}

	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	if ((rv = getaddrinfo(host, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	
	// loop through all the results and connect to the first we can
	//one host can have multiple ip
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
			p->ai_protocol)) == -1) {
			perror("client: socket");
		continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client: connect");
			continue;
		}
		break;
	}

	//unsuccessful connection
	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}
	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
		s, sizeof s);

	//printf("client: connecting to %s\n", s);
	freeaddrinfo(servinfo); // all done with this structure

	//read the welcome message
	char buf[MAXDATASIZE];
	if (read(sockfd, buf, MAXDATASIZE-1) > 0) {
		printf("%s\n", buf);
	}

	//send user and pass cmd if required
	if (userInfo != NULL) {
		sendCmdAndWaitResponse(sockfd, sendUser, strlen(sendUser));
		sendCmdAndWaitResponse(sockfd, sendPass, strlen(sendPass));
	}
	else {
		strcpy(sendUser, "USER ftp");
		strncat(sendUser, "\r\n", 2);
		strcpy(sendPass, "PASS pass");
		strncat(sendPass, "\r\n", 2);
		sendCmdAndWaitResponse(sockfd, sendUser, strlen(sendUser));
		sendCmdAndWaitResponse(sockfd, sendPass, strlen(sendPass));
	}

	char server_address[20];
	int server_port = enterPassive(sockfd, server_address);

	//send the file requested
	write(sockfd, path, strlen(path) + 1);

	//starts opening the address of the data
	struct	sockaddr_in server_addr;	
	//server address handling
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(server_address);	//32 bit Internet address network byte ordered
	server_addr.sin_port = htons(server_port);		//server TCP port must be network byte ordered
    
    int sockfd_server;
	//open an TCP socket
	if ((sockfd_server = socket(AF_INET,SOCK_STREAM,0)) < 0) {
    		perror("socket()");
        	exit(1);
    	}

	//connect to the server
    	if(connect(sockfd_server, 
	           (struct sockaddr *)&server_addr, 
		   sizeof(server_addr)) < 0){
        	perror("connect()");
		exit(1);
	}

	//open the file
	char filename[MAXDATASIZE];
	parseFilename(argv[1], filename);
	int file = open(filename, O_WRONLY | O_CREAT, 0777);

	//read to the file
	char print;
	while (read(sockfd_server, &print, 1) == 1) {
		write(file, &print, 1);
	}

	//all done with the transmission
	close(file);
	close(sockfd_server);
	close(sockfd);

	printf("Transfer completed!\n");
	return 0;
}