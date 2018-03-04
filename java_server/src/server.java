package newpr;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.ServerSocket;
import java.util.HashMap;
import java.util.Iterator;

import java.sql.*;
import java.util.Calendar;
import java.util.StringTokenizer;

public class server {
	static int clusfloor;
	
	int port = 8888;
	
	ServerSocket serverSock = null; // ServerSocket 생성   
	Socket clientSock = null; // 클라이언트와의 통신을 하게 될 Socket 생성       
	HashMap<Socket, PrintWriter> map = new HashMap<Socket, PrintWriter>();// 서버에 접속된 클라이언트를 구별할 역할의 HashMap 생성

	public server() {
		try {
			serverSock = new ServerSocket(port); // 포트번호로 요청 기다림
			System.out.println("소켓준비완료... ");
		} catch (IOException e) {// TODO Auto-generated catch block            
			e.printStackTrace();
			System.out.println("1");
		}
	}

	public void Loop() {
		while (true) { // 무한루프           
			try {
				clientSock = serverSock.accept(); // 클라이언트의 연결요청을 받아들여 새로운 포트 번호를 가지는 소켓 만듬
				System.out.println("클라이언트연결완료.");

				client client = new client(clientSock);// clientSock의 client 인스턴스 생성
				client.start(); // client 쓰레드 실행                           
			} catch (Exception e) {
				e.printStackTrace();
				System.out.println("2");
			}
		}
	}

	public static void main(String[] args) throws IOException {
		//클러스터링함수 매주 월요일마다 실행. 클러스터링층수 매일마다 실행.
		new ClusterTrigger();
		
		ConnectData.loadClusResult();
		
		//통신
		new server().Loop(); // serverPC객체를 생성해서 클라이언트 연결을 기다리며 무한반복 
	}

	public class client extends Thread {
		private Socket socket; // 서버와 통신을 할 클라이언트 소켓 생성       
		PrintWriter out = null; // 소켓을 통해 밖으로 나갈 문자열 객체 생성       
		BufferedReader in = null; // 소켓을 통해 얻어올 버퍼 객체 생성               

		public client(Socket socket) {
			this.socket = socket;
		} // 인자로 전달된 소켓을 생성한 소켓으로 지정해주는 생성자               

		@Override
		public void run() {
			try {
				in = new BufferedReader(new InputStreamReader(socket.getInputStream())); // 소켓으로부터 값을 읽어옴
				out = new PrintWriter(socket.getOutputStream(), true); // 소켓을 통해 값을 보냄
				map.put(socket, out);// 소켓을 key로, 소켓을 통해 보낼 값을 내용으로 hashMap에 저장 

				while (true) {
					String inputData;
					inputData = in.readLine(); // 값을 읽어와 inputData String에 저장 후 
					System.out.println("클라이언트로부터받은문자열> " + inputData); // 출력

					Iterator<Socket> i = map.keySet().iterator(); // hashMap의 내용을 지시할 지시자 생성					
					Iterator<Socket> j = map.keySet().iterator(); // hashMap의 내용을 지시할 지시자 생성		
					
					//데이터 받을 때의 시간.층 수 알고 데이터베이스에 저장하는 부분
					if(inputData.equals("0 "
							+ "")){ //안드로이드에서 호출 한 층
						inputData = in.readLine();
						System.out.println("클라이언트로부터받은문자열> " + inputData); // 출력
						ConnectData.saveData(inputData);
						
						while (i.hasNext()) {
							Socket temp = i.next();
							if (temp.equals(socket))
								continue;
							PrintWriter outiter = map.get(temp);
							outiter.println(inputData);
						}	
					}
					//getclusfloor 함수 시행해서 클러스터링 층 수 알아온다음, "0이 아닐경우에" 엘리베이터로 보낸다.
					else if(inputData.equals("stop")){
					//else if(inputData.equals("99")){

						//스마트폰으로 현재층수 보내기
						//inputData="77";
						inputData = in.readLine(); //아두이노로부터 받아온 현재층수 받아오기
						System.out.println("클라이언트로부터받은문자열> " + inputData); // 출력
						while (i.hasNext()) {
							Socket temp = i.next();
							if (temp.equals(socket))
								continue;
							PrintWriter outiter = map.get(temp);
							outiter.println(inputData); //현재 층 수 전달
						}					
						
						sleep(2000);
						
						//아두이노로 clusfloor 보내기
						clusfloor = ConnectData.getclusfloor();
						//clusfloor = 40;
						if(clusfloor != 0){
							//엘리베이터로 보냄 (stop 문자열을 보낸 쪽으로 전달)
							System.out.println("clusfloor: " + clusfloor); // 출력
							while (j.hasNext()) {
								Socket temp = j.next();
								if (temp.equals(socket)){
									PrintWriter outiter = map.get(temp);
									outiter.println(clusfloor);
								}
								else
									continue;
							}
						}
					}
					else if(inputData.equals("\0")){
						System.out.println("여기이따!!!!!!!!!!!!");
					}
					else{
						// 자신을 제외한 나머지 클라이언트에게 문자열 전달 \
						while (i.hasNext()) {
							Socket temp = i.next();
							if (temp.equals(socket))
								continue;
							PrintWriter outiter = map.get(temp);
							outiter.println(inputData);
						}	
					}

					// 소켓으로부터 읽어온 데이터가 99999999이면 종료                    
					if (inputData.equals("99999999")) {
						System.out.println("See u later~!");
						break;
					}
				}//while

				out.close();
				in.close();
				socket.close(); // 소켓종료
			} catch (Exception e) {
				e.printStackTrace();
				System.out.println("3");
			}
		}// run
	}// client Thread
}// public class server
