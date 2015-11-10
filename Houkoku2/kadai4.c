#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
//kadai4.cでの操作方法
// ./a.out 300001
//kadai2.cでの操作方法
//http://localhost:300001/hoge.html

const char *SuccessMessage = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=us-ascii\n";
const char *FailMessage = "HTTP/1.1 404 Not Fonund\nContent-Type: text/html; charset=us-ascii\n\n<HTML><HEAD>Not Found</HEAD>\n<BODY>\nThe requested URL ";
const char *FailMessage2 = " was not found on this server.\n</BODY></HTML>\n";

int main(int argc, char* argv[])
{
	char buf[128];
	char path[2000];
	char host[2000];
	int RequestValdFlag = 0;
	FILE *fp;
	int i, s, ns, port, pid;

	struct sockaddr_in sin, fsin;

	socklen_t fromlen = sizeof(struct sockaddr_in);
	
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
	
	//TIME_WAIT状態のポートが存在していてもbindができるようになる
	//参考: http://www.geekpage.jp/programming/winsock/so_reuseaddr.php
	int yes = 1;
	setsockopt(s,SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes));
	if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
		perror("server bind()");
		exit(1);	
	}
	
	if (listen(s, 128) == -1) {
		perror("server listen()");
		exit(1);	
	}

	for(;;){
		if ((ns = accept(s, (struct sockaddr *)&fsin, &fromlen)) == -1) {
			perror("server accept()");	
			exit(1);
		}
		pid = fork();//プロセスを分岐
		if(pid == 0){
			//子プロセスでクライアントの要求を待つ
			fp = fdopen(ns, "r");
			
			//クライアントからの応答を待つ \r\nで待機終了
			int cnt = 0;
			while (fgets(buf, sizeof(buf), fp) != NULL) {
				if(strcmp(buf,"\r\n") == 0) break;
				if(cnt == 0){
					if(sscanf(buf,"GET /%s HTTP/1.1\r\n", path) != EOF){
			  			cnt++;
			  		}
			  	}else if(cnt == 1){
			  		if(sscanf(buf,"Host: %s\r\n", host) != EOF){
			  			cnt++;
			  			RequestValdFlag = 1;
			  		}
			  	}
			}

			//リクエストが不正な場合はエラーを出して終了
			if(!RequestValdFlag){
				fprintf(stderr, "Bad Request\n");
				break;
			}

			//ファイルが存在するか調べる
			FILE *fp2;
			fp2 = fopen(path,"r");
			if(fp2 == NULL){
				//ファイルが存在しない場合はNot Foundを返す
				send(ns, FailMessage, strlen(FailMessage), 0);
				send(ns, path, strlen(path), 0);
				send(ns, FailMessage2, strlen(FailMessage), 0);
			}else{
				//ファイルの内容を送信
				send(ns, SuccessMessage, strlen(SuccessMessage), 0);
				char c;
				while( (c = getc(fp2)) != EOF ){
		    		send(ns, &c, (size_t)1, 0);
		  		}
		  		fclose(fp2);
			}
			close(ns);
			fclose(fp);
			exit(0);
		}else{
			//親プロセスは次のクライアントからの要求を待つ
			close(ns);
			fclose(fp);
		}
	}
	return 0;
}
