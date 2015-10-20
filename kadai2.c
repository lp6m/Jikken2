#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

#define NUMSTR 3
#define GET_URL_LENGTH 500

char scheme[GET_URL_LENGTH], host[GET_URL_LENGTH];
char port[GET_URL_LENGTH], path[GET_URL_LENGTH];

int GetSocketParamsByURL(char *url){
	/*fgetsで文字数制限しているので文字数があふれることは考慮しない*/
	int url_len = strlen(url);
	char url2[url_len]; //urlのコピー
	char tmp[url_len];
	char *tp;
	int PortExistflag = 0;
	int i;
	int MojiCounter = 0;		//パスの切り出し開始位置用
	
	/*入力の最後の改行を消去*/
	for(i = 0; url[i] != '\n' || i < strlen(url); i++);
	if(i < strlen(url)) url[i] = '\0';

	/*strtokで破壊されるので別変数にコピー*/
	strcpy(url2,url);

	/*冒頭6文字がhttp://でなければエラー*/
	if(strlen(url2) < 8){
		return -1;
	}else{
		strncpy(tmp, url2, 8);
		tmp[7] = '\0';
		if(strcmp(tmp, "http://") != 0) return -1;
	}

	/*ポート番号が含まれるかどうか調べる（:がhttp://以降にあるか調べる）*/
	for(i = 8; url2[i] != '\0' || i < strlen(url2); i++) if(url2[i] == ':') PortExistflag = 1;

	/*スキーム,ホスト,ポート,の切り出し*/
	tp = strtok(url, ":/");
	strcpy(scheme, tp);

	MojiCounter += strlen(scheme);

	/*ポート番号が含まれるかどうかで処理を分岐*/
	if(PortExistflag){
		for(i = 0; i < 2; i++){
			tp = strtok(NULL,":/");
			if(tp == NULL){
				return -1;
				break;
			}
			if(i == 0) strcpy(host, tp);
			else if(i == 1) strcpy(port, tp);
			MojiCounter += strlen(tp);
		}
		/*ポート番号に数字以外の文字が含まれていたらエラー*/
		for(i = 0; port[i] != '\0'; i++){
			if(port[i] < '0' || port[i] > '9') return -1;
		}
		MojiCounter += 5; //切り出した文字数をカウント
	}else{
		for(i = 0; i < 1; i++){
			tp = strtok(NULL,":/");
			if(tp == NULL){
				return -1;
				break;
			}
			if(i == 0) strcpy(host, tp);
			MojiCounter += strlen(tp);
		}
		MojiCounter += 4; //切り出した文字数をカウント
		/*ポート番号が含まれない場合ポート番号は80*/
		strcpy(port, "80");
	}
	/*パスの切り出し*/
	memset(path, '\0', sizeof(path));
	strncpy(path, url2+MojiCounter, strlen(url2)-MojiCounter+1);	
	return 1;
}

int main(int argc, char* argv[])
{
	FILE *fp;
	char hostname[128];
	int i, s;
	struct hostent *hp;
	struct sockaddr_in sin;
	char buf[128];
	char url[GET_URL_LENGTH];
	/*標準入力からURLを受け取る*/
	while(printf("Input URL:\n"),fgets(url,GET_URL_LENGTH,stdin)){
		/*ホスト,ポート,パスを切り出す*/
		if(GetSocketParamsByURL(url)!=1){
			/*URLが無効であれば即終了*/
			fprintf(stderr, "Invalid URL\n");
			exit(1);
		}
		/*ホストが無効であれば終了*/
		if ((hp = gethostbyname(host)) == NULL) {
			fprintf(stderr, "%s: unknown host.\n", hostname);
			exit(1);
		}
		/*ソケット通信の準備*/
		s = socket(AF_INET, SOCK_STREAM, 0);

		bzero(&sin, sizeof(sin));
		sin.sin_family = AF_INET;
		sin.sin_port = htons(atoi(port));
		memcpy(&sin.sin_addr, hp->h_addr, hp-> h_length);

		if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
			perror("client connect()");
			exit(1);	
		}
		fp = fdopen(s, "r");
		
		//サーバに送信するリクエストを作成
		char reqlines[NUMSTR][GET_URL_LENGTH];
		sprintf(reqlines[0],"GET /%s HTTP/1.1\r\n",path);
		sprintf(reqlines[1],"Host: %s\r\n",host);
		sprintf(reqlines[2],"\r\n");

		//サーバーにリクエストを送信
		for(i = 0; i < NUMSTR; i++) {
		  send(s, reqlines[i], strlen(reqlines[i]), 0);
		}

		//サーバ送られてきたデータを表示
		while (fgets(buf, sizeof(buf), fp) != NULL) {
		  printf("%s", buf);
		}
		
		close(s);
	}
	return 0;
}

 
