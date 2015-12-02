import java.net.*;
import java.io.*;
import java.util.*;
import java.util.regex.*;
import java.util.ArrayList;

class Browse{
	public String urlstr,contents;
	public ArrayList<String> links;
	public Boolean successflag;
	public Stack<String> backStack;
	public Stack<String> forwardStack;

	//constructor
	public Browse(){
		urlstr = "";
		links = new ArrayList<String>();
		backStack = new Stack<String>();
		forwardStack = new Stack<String>();
		contents = "";
		successflag = false;

	}
	public void updatePage(String nexturl) throws Exception{
		//ページを更新する
		try{
			contents = "";
			links.clear();
			urlstr = nexturl;
			URL url = new URL(urlstr);
			int port;
			if (url.getPort() == -1) port = 80;
			else port = url.getPort();
			Socket s = new Socket(url.getHost(), port);
			BufferedReader in = new BufferedReader(new InputStreamReader(s.getInputStream()));
			PrintWriter out = new PrintWriter(s.getOutputStream(),true);
			String buf;

			//サーバにリクエストを送信する
			out.print("GET " + url.getPath() + " HTTP/1.1\r\n");
			out.print("Host: " + url.getHost() + " \r\n");
			out.println("\r\n");

			//サーバからデータを受け取ってその内容を表示する

			//1行目はHTTPのレスポンスを調べる 空白で切り出して2つめの文字がHTTPステータスコード
			//このプログラムでは、2xxであれば成功、それ以外は失敗として扱うことにしている
			String firstline = in.readLine();
			int responsecode = -1;
			if(firstline != null){
				System.out.println(firstline);
				String[] httpresponse = firstline.split("[\\s]+");
				if(httpresponse.length >= 2) responsecode = Integer.parseInt(httpresponse[1]);
			}
			if(responsecode >= 200 && responsecode <= 299) successflag = true;
			else successflag = false;

			//残りの行はそのまま受け取ったデータを表示する
			while (null != (buf = in.readLine())) {
				contents += buf;
			    System.out.println(buf);
			}
			in.close();
			out.close();
			s.close();

			//成功した場合のみリンクを取得して表示する
			if(successflag){
				collectLinks();
				printLinks();
			}else{
				//失敗時はステータスコードを表示する
				if(firstline != null) System.out.println(firstline);
			}
		} catch (MalformedURLException e){
			System.out.println("Bad URL.");
			System.exit(1);
		} catch (UnknownHostException e) {
			System.out.println("Unknown host.");
			System.exit(1);
		} catch (IOException e) {
			System.out.println("IO exception.");
			System.exit(1);
		}
	}

	public void collectLinks(){
		try{
			//contentsの中のリンクを抽出しlinksを更新
			links.clear();
			URL url = new URL(urlstr);
			//Pattern ptn = Pattern.compile("</a/i>.*?/href/i=\"(.*?)\".*?>(.*?)<//a/i>", Pattern.DOTALL);
			Pattern ptn = Pattern.compile("(?i)<a.*? href=\"(.*?)\".*?>(.*?)</a>", Pattern.DOTALL);
			Matcher matcher = ptn.matcher(contents);
			System.out.println("[list of links]");
			while (matcher.find()) {
				String href = matcher.group(1);
			 	String text = matcher.group(2);
			 	//リンク先URLを現在のURLをもとに補完する
			 	URL complementUrl = new URL (url, href);
			 	href = complementUrl.toString();
				links.add(href);
			}
		} catch (MalformedURLException e){
			System.out.println("Bad URL.");
			System.exit(1);
		}
	}

	public void printLinks(){
		//linksの内容をナンバリングしながら表示
		for(int i = 0; i < links.size(); i++){
			System.out.println(String.valueOf(i+1) + ". " + getLink(i));
		}
	}

	public String getLink(int n){
		//links中のn番目のURLを返す
		return links.get(n);
	}
	public void gotoURL(String nexturl) throws Exception{
		//指定されたURLに移動する
		//前ページに戻れるように以前のURLをスタックに積む
		if(urlstr != "" && urlstr != null) backStack.push(urlstr);
		updatePage(nexturl);
	}

	public void pageBack() throws Exception{
		//前ページに戻る
		//前ページのデータを取り出して、「進む」ができるように次ページのスタックに積む
		if(backStack.empty()) return;
		forwardStack.push(urlstr);
		updatePage((String)backStack.pop());
	}
	public void pageForward() throws Exception{
		//次ページに進む
		//次ページのデータを取り出して、「戻る」ができるように前ページのスタックに積む
		if(forwardStack.empty()) return;
		backStack.push(urlstr);

		updatePage(forwardStack.pop());
	}

}
class MyClient {
	public static void main(String[] args) throws Exception{
		Browse browse = new Browse();
		BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
		browse.gotoURL(br.readLine());
		
		while(true){
			try{
				//ユーザにメッセージを表示
				System.out.println("\n現在閲覧しているURL: " + browse.urlstr);
				System.out.println("コマンドを指定してください.");
				if(browse.links.size() != 0) System.out.println("1〜" + String.valueOf(browse.links.size()) + ": 指定した番号のリンク先に移動します.");
				if(!browse.backStack.empty()) System.out.println("back: 前のページに戻ります");
				if(!browse.forwardStack.empty()) System.out.println("forward: 先のページに進みます");
				System.out.print("コマンド:");

				//コマンドを受け取る
				String buf = br.readLine();
				int selectlinkindex = -1; 

				//コマンドに応じて処理をする
				System.out.print(buf);
				if(buf.equals("back") && !browse.backStack.empty()) browse.pageBack();
				else if(buf.equals("forward") && !browse.forwardStack.empty()) browse.pageForward();
				else  selectlinkindex = Integer.parseInt(buf);
				if(selectlinkindex <= 0 || selectlinkindex > browse.links.size()) throw new NumberFormatException();
				browse.gotoURL(browse.getLink(selectlinkindex - 1));
			} catch (NumberFormatException e){
				System.out.println("コマンドが不正です");

			}
		}
	}
}
