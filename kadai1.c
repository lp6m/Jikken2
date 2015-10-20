#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*ArchLinuxをインストールしようとしたらkadai1.cとkadai2.cがパーティションごと飛んで行ってすごくつらかった*/

#define GET_URL_LENGTH 500
int main(){
	char url[GET_URL_LENGTH];  //strtokによって破壊される
	char url2[GET_URL_LENGTH]; //urlのコピー
	char scheme[GET_URL_LENGTH], host[GET_URL_LENGTH];
	char port[GET_URL_LENGTH], path[GET_URL_LENGTH];
	char *tmp, *tp;
	int PortExistflag = 0;
	int SuccessFlag = 1;
	int i;
	int MojiCounter = 0;		//パスの切り出し開始位置用

	/*標準入力からURLを受け取る*/
	printf("Input URL:\n");
	fgets(url,GET_URL_LENGTH,stdin);
	/*入力の最後の改行を消去*/
	for(i = 0; url[i] != '\n' || i < strlen(url); i++);
	if(i < strlen(url)) url[i] = '\0';
	/*strtokで破壊されるので別変数にコピー*/
	strcpy(url2,url);

	/*冒頭6文字がhttp://でなければエラー*/
	if(strlen(url2) < 8){
		SuccessFlag = 0;
	}else{
		tmp = malloc(sizeof(char)*8);
		strncpy(tmp, url2, 8);
		tmp[7] = '\0';
		if(strcmp(tmp, "http://") != 0) SuccessFlag = 0;
		free(tmp);
	}

	/*ポート番号が含まれるかどうか調べる（:がhttp://以降にあるか調べる）*/
	for(i = 8; url2[i] != '\0'|| i < strlen(url2); i++) if(url2[i] == ':') PortExistflag = 1;

	/*スキーム,ホスト,ポート,の切り出し*/
	if(SuccessFlag){
		tp = strtok(url, ":/");
		strcpy(scheme, tp);
		MojiCounter += strlen(scheme);
		/*ポート番号が含まれるかどうかで処理を分岐*/
		if(PortExistflag){
			for(i = 0; i < 2; i++){
				tp = strtok(NULL,":/");
				if(tp == NULL){
					SuccessFlag = 0;
					break;
				}
				if(i == 0) strcpy(host, tp);
				else if(i == 1) strcpy(port, tp);
				MojiCounter += strlen(tp);
			}
			/*ポート番号に数字以外の文字が含まれていたらエラー*/
			for(i = 0; port[i] != '\0'; i++){
				if(port[i] < '0' || port[i] > '9') SuccessFlag = 0;
			}
			MojiCounter += 5; //切り出した文字数をカウント
		}else{
			for(i = 0; i < 1; i++){
				tp = strtok(NULL,":/");
				if(tp == NULL){
					SuccessFlag = 0;
					break;
				}
				if(i == 0) strcpy(host, tp);
				MojiCounter += strlen(tp);
			}
			MojiCounter += 4; //切り出した文字数をカウント
			/*ポート番号が含まれない場合ポート番号は80*/
			strcpy(port, "80");
		}
	}

	/*パスの切り出し*/
	//printf("%d %d\n",strlen(url2)-MojiCounter+1),MojiCounter;
	if(SuccessFlag){
		memset(path, '\0', sizeof(path));
		strncpy(path, url2+MojiCounter, strlen(url2)-MojiCounter+1);	
	}

	/*結果の表示*/
	if(SuccessFlag){
		printf("host -> %s\n", host);
		printf("port -> %s\n", port);
		printf("path -> %s\n", path);	
	}else{
		printf("Error\n");
	}
	return 0;	
}