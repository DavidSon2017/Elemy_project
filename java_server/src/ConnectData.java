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
	
	//�Ϸ縶�� �׳��� Ŭ�����͸��� ������ �ҷ�����(ù�ð�~���ð� : ����)
	public static void loadClusResult(){
		//isweekend = 0 ����, isweekend = 1 �ָ�
		int isweekend = getDayofWeek();
		
		try {
			Class.forName("com.mysql.jdbc.Driver");
			System.out.println("����̹� �˻� ����!!");
		} catch (ClassNotFoundException e) {
			System.out.println("����̹� �˻� ����!");
		}

		Connection con = null;
		String url = "jdbc:mysql://127.0.0.1:3306/clus";
		String user = "root";
		String pass = "1234";

		try {
			con = DriverManager.getConnection(url, user, pass);
			System.out.println("My-SQL ���� ����!!");
		} catch (SQLException e) {
			System.out.println("My-SQL ���� ����");
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
				// "+resultfloor+"��");
			} // while
		} catch (SQLException e) {
			System.out.println("SQLException:" + e.getMessage());
		}

		System.out.println("-----------------------------------");
		for (int i = 0; i < count; i++) {
			System.out.println(chartime(datas[i].start_time) + " ~ " + chartime(datas[i].finish_time) + " -> " + datas[i].result_floor + "��");
		}
		System.out.println("-----------------------------------");
	}
	
	//ȣ�� ���� ��, ȣ���ȣ ������. getclusfloor �Լ� �����ؼ� Ŭ�����͸� �� �� �˾ƿ´���, "0�� �ƴҰ�쿡" ���������ͷ� ������.
	public static int getclusfloor(){
		String timestr = getCurrentTime(); //����ð�
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
		
		System.out.println("���� �ð��� " + timestr + "(" + timeint + ")" + "�̹Ƿ� Ŭ�����͸� �� ���� \"" + clusfloor + "\" �Դϴ�.");		
		return clusfloor;
	}
	
	//������ ������ ���� ���� �ð�.
	public static String getCurrentTime() {
		String str;
		Calendar aCalendar = Calendar.getInstance();
		int hour = aCalendar.get(Calendar.HOUR_OF_DAY);
		int min = aCalendar.get(Calendar.MINUTE);
		int sec = aCalendar.get(Calendar.SECOND);

		str = hour + ":" + min + ":" + sec;
		return str;
	}
	
	//������ ������ ���� ���� ��¥.
	public static String getCurrentDay(){
		String str;
		Calendar aCalendar = Calendar.getInstance();
		int year = aCalendar.get(Calendar.YEAR);
		int month = aCalendar.get(Calendar.MONTH)+1;
		int date = aCalendar.get(Calendar.DATE);

		str = year + "-" + month + "-" + date;
		return str;
	}
	
	//������ ������ ���� ���� ����. (������ 0, �ָ��� 1)
	public static int getDayofWeek(){
		Calendar aCalendar = Calendar.getInstance();
		int dayofweek = aCalendar.get(Calendar.DAY_OF_WEEK);
		//DAY_OF_WEEK : �Ͽ����� 1, �������� 2 , ,,, ������� 7 ���
		
		if(dayofweek == 1 || dayofweek ==7)
			return 1;
		else
			return 0;
	}

	//���ڿ��ð��� ��Ʈ���ð�����. (9:12 -> 178)
	public static int inttime(String str) {
		int time = 0;
		int cnt = 1;

		StringTokenizer s = new StringTokenizer(str, ":");

		while (s.hasMoreTokens()) {
			String ss = s.nextToken();
			if (cnt == 1)
				time += (Integer.parseInt(ss) * 3600); // �� ó��
			else if (cnt == 2)
				time += (Integer.parseInt(ss) * 60); // �� ó��
			else
				time += Integer.parseInt(ss); // �� ó��

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
	
	//������ ����.
	public static void saveData(String data){
		int floor = Integer.parseInt(data);
		
		try {
			Class.forName("com.mysql.jdbc.Driver");
			//System.out.println("����̹� �˻� ����!!");
		} catch (ClassNotFoundException e) {
			System.out.println("����̹� �˻� ����!");
		}

		Connection con = null;
		String url = "jdbc:mysql://127.0.0.1:3306/clus";
		String user = "root";
		String pass = "1234";

		try {
			con = DriverManager.getConnection(url, user, pass);
			//System.out.println("My-SQL ���� ����!!");
		} catch (SQLException e) {
			System.out.println("My-SQL ���� ����");
		}

		try {
			Statement stmt = con.createStatement();
			
			String currentDay = ConnectData.getCurrentDay();
			String currentTime = ConnectData.getCurrentTime();
			int dayofweek = ConnectData.getDayofWeek();

			String sql = "insert into datas values('"+currentDay+"','"+
					currentTime+"','"+floor+"','"+dayofweek+"','0')";
			
			if(stmt.executeUpdate(sql)>0){
				System.out.println("Data �߰� : '"+currentDay+"','"+
						currentTime+"','"+floor+"','"+dayofweek+"','0'");
			}
			else
				System.out.println("Data �߰� ����");
		} catch (SQLException e) {
				System.out.println("SQLException:" + e.getMessage());
		}		
	}
}
