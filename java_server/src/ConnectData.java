package newpr;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Calendar;
import java.util.StringTokenizer;

public class ConnectData {

	static class data {
		public int start_time;
		public int finish_time;
		public int result_floor;

		data(int st, int ft, int rf) {
			start_time = st;
			finish_time = ft;
			result_floor = rf;
		}
	}
	
	static data[] datas;
	static int count = 0;
	
	//하루마다 그날의 클러스터링된 데이터 불러오기(첫시간~끝시간 : 층수)
	public static void loadClusResult(){
		//isweekend = 0 평일, isweekend = 1 주말
		int isweekend = getDayofWeek();
		
		try {
			Class.forName("com.mysql.jdbc.Driver");
			System.out.println("드라이버 검색 성공!!");
		} catch (ClassNotFoundException e) {
			System.out.println("드라이버 검색 실패!");
		}

		Connection con = null;
		String url = "jdbc:mysql://127.0.0.1:3306/clus";
		String user = "root";
		String pass = "1234";

		try {
			con = DriverManager.getConnection(url, user, pass);
			System.out.println("My-SQL 접속 성공!!");
		} catch (SQLException e) {
			System.out.println("My-SQL 접속 실패");
		}

		try {
			Statement stmt = con.createStatement();
			
			String buf ="select count(*) cnt from result where is_weekend="+isweekend;
			ResultSet rs = stmt.executeQuery(buf);
			while(rs.next()){
				count = rs.getInt("cnt");
			}
			datas = new data[count];

			int idx = 0;

			buf ="select * from result where is_weekend="+isweekend+" order by start_time";
			rs = stmt.executeQuery(buf);
						
			while (rs.next()) {
				String starttime = rs.getString("start_time");
				String finishtime = rs.getString("finish_time");
				int resultfloor = rs.getInt("result_floor");

				int st = inttime(starttime);
				int ft = inttime(finishtime);
				
				datas[idx] = new data(st, ft, resultfloor);

				idx++;
				// System.out.println(starttime+"~"+finishtime+" ->
				// "+resultfloor+"층");
			} // while
		} catch (SQLException e) {
			System.out.println("SQLException:" + e.getMessage());
		}

		System.out.println("-----------------------------------");
		for (int i = 0; i < count; i++) {
			System.out.println(chartime(datas[i].start_time) + " ~ " + chartime(datas[i].finish_time) + " -> " + datas[i].result_floor + "층");
		}
		System.out.println("-----------------------------------");
	}
	
	//호출 했을 때, 호출신호 보내고. getclusfloor 함수 시행해서 클러스터링 층 수 알아온다음, "0이 아닐경우에" 엘리베이터로 보낸다.
	public static int getclusfloor(){
		String timestr = getCurrentTime(); //현재시간
		//String timestr = "17:50:30";
		
		int timeint = inttime(timestr);
		int clusfloor = 0;
		for (int i = 0; i < count; i++) {
			if (datas[i].start_time <= timeint && timeint <= datas[i].finish_time) {
				clusfloor = datas[i].result_floor;
				break;
			} else
				clusfloor = 0;
		}
		
		clusfloor = 10;
		
		System.out.println("현재 시간은 " + timestr + "(" + timeint + ")" + "이므로 클러스터링 된 층은 \"" + clusfloor + "\" 입니다.");		
		return clusfloor;
	}
	
	//데이터 저장을 위한 현재 시간.
	public static String getCurrentTime() {
		String str;
		Calendar aCalendar = Calendar.getInstance();
		int hour = aCalendar.get(Calendar.HOUR_OF_DAY);
		int min = aCalendar.get(Calendar.MINUTE);
		int sec = aCalendar.get(Calendar.SECOND);

		str = hour + ":" + min + ":" + sec;
		return str;
	}
	
	//데이터 저장을 위한 현재 날짜.
	public static String getCurrentDay(){
		String str;
		Calendar aCalendar = Calendar.getInstance();
		int year = aCalendar.get(Calendar.YEAR);
		int month = aCalendar.get(Calendar.MONTH)+1;
		int date = aCalendar.get(Calendar.DATE);

		str = year + "-" + month + "-" + date;
		return str;
	}
	
	//데이터 저장을 위한 현재 요일. (평일은 0, 주말은 1)
	public static int getDayofWeek(){
		Calendar aCalendar = Calendar.getInstance();
		int dayofweek = aCalendar.get(Calendar.DAY_OF_WEEK);
		//DAY_OF_WEEK : 일요일은 1, 월요일은 2 , ,,, 토요일은 7 출력
		
		if(dayofweek == 1 || dayofweek ==7)
			return 1;
		else
			return 0;
	}

	//문자열시간을 인트형시간으로. (9:12 -> 178)
	public static int inttime(String str) {
		int time = 0;
		int cnt = 1;

		StringTokenizer s = new StringTokenizer(str, ":");

		while (s.hasMoreTokens()) {
			String ss = s.nextToken();
			if (cnt == 1)
				time += (Integer.parseInt(ss) * 3600); // 시 처리
			else if (cnt == 2)
				time += (Integer.parseInt(ss) * 60); // 분 처리
			else
				time += Integer.parseInt(ss); // 초 처리

			cnt++;
		}

		return time;
	}
	
	public static String chartime(int t){
		String str;
		
		int hour = t / 3600;
		int minute = (t - (hour * 3600)) / 60;
		int second = t - (3600 * hour) - (60 * minute);

		str = hour + ":"+minute+":"+second;
		return str;
	}
	
	//데이터 저장.
	public static void saveData(String data){
		int floor = Integer.parseInt(data);
		
		try {
			Class.forName("com.mysql.jdbc.Driver");
			//System.out.println("드라이버 검색 성공!!");
		} catch (ClassNotFoundException e) {
			System.out.println("드라이버 검색 실패!");
		}

		Connection con = null;
		String url = "jdbc:mysql://127.0.0.1:3306/clus";
		String user = "root";
		String pass = "1234";

		try {
			con = DriverManager.getConnection(url, user, pass);
			//System.out.println("My-SQL 접속 성공!!");
		} catch (SQLException e) {
			System.out.println("My-SQL 접속 실패");
		}

		try {
			Statement stmt = con.createStatement();
			
			String currentDay = ConnectData.getCurrentDay();
			String currentTime = ConnectData.getCurrentTime();
			int dayofweek = ConnectData.getDayofWeek();

			String sql = "insert into datas values('"+currentDay+"','"+
					currentTime+"','"+floor+"','"+dayofweek+"','0')";
			
			if(stmt.executeUpdate(sql)>0){
				System.out.println("Data 추가 : '"+currentDay+"','"+
						currentTime+"','"+floor+"','"+dayofweek+"','0'");
			}
			else
				System.out.println("Data 추가 실패");
		} catch (SQLException e) {
				System.out.println("SQLException:" + e.getMessage());
		}		
	}
}
