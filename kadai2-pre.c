#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

#define NUMSTR 3

char *reqlines[NUMSTR] = {
  "GET /le2soft/ HTTP/1.1\r\n",
  "Host: www.fos.kuis.kyoto-u.ac.jp\r\n",
  "\r\n",
};

int main(int argc, char* argv[])
{
	FILE *fp;
	char hostname[128];
	int i, s, port;
	struct hostent *hp;
	struct sockaddr_in sin;
	char buf[128];

	port = 80;
	if ((hp = gethostbyname("www.fos.kuis.kyoto-u.ac.jp")) == NULL) {
		fprintf(stderr, "%s: unknown host.\n", hostname);
		exit(1);
	}
	s = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	memcpy(&sin.sin_addr, hp->h_addr, hp-> h_length);
		printf("hoge\n");
	if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
		perror("client connect()");
		exit(1);	
	}
	
	fp = fdopen(s, "r");
	
	// send request to server
	for(i = 0; i < NUMSTR; i++) {
	  send(s, reqlines[i], strlen(reqlines[i]), 0);
	}
	printf("hoge\n");
	// receive contents from server
	while (fgets(buf, sizeof(buf), fp) != NULL) {
	  printf("%s", buf);
	}
	
	close(s);
	return 0;
}

 
