#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

#define NUMSTR  3

char *strs[NUMSTR] = {
	"1\n",
	"2\n",
	"3\n"
};

int main(int argc, char* argv[])
{
	/*クライアントとの接続用*/
	FILE *fp;
	int i, s, ns, port;
	struct sockaddr_in sin, fsin;
	char buf[128];
	
	socklen_t fromlen = sizeof(struct sockaddr_in);
	printf("%d\n",argc );
	if((argc <= 1) || ((port = atoi(argv[1])) == 0)) {
	  perror("no port number");
	  exit(1);		
	}

	if((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("client socket()");
		exit(1);		
	}
	
	bzero(&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = INADDR_ANY;
	
	if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
		perror("server bind()");
		exit(1);	
	}
	
	if (listen(s, 128) == -1) {
		perror("server listen()");
		exit(1);	
	}
	
	if ((ns = accept(s, (struct sockaddr *)&fsin, &fromlen)) == -1) {
		perror("server accept()");	
		exit(1);
	}
	
	fp = fdopen(ns, "r");
	
	/*要求をおくるための準備*/
	
	//クライアントからの要求が\r\nになるまで受信し続ける
	while (fgets(buf, sizeof(buf), fp) != NULL) {
	  if(strcmp(buf,"\r\n") == 0) break;
	  //受信したデータをそのまま要求する
	  
	}

	// send contents to client
	for (i = 0; i < NUMSTR; i++){
		send(ns, strs[i], strlen(strs[i]), 0);
	}
	
	close(ns);
	close(s);
	return 0;
}

