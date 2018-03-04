package newpr;

import java.util.Calendar;
import java.util.Date;
import java.util.StringTokenizer;

import org.quartz.Job;
import org.quartz.JobExecutionContext;
import org.quartz.JobExecutionException;


public class ClusterScheduler implements Job{
	public void execute(JobExecutionContext arg0) throws JobExecutionException{
		
		//Clustering 프로그램 실행
		Runtime r = Runtime.getRuntime();

		try {
			r.exec("C:\\Users\\youkyung\\Documents\\Visual Studio 2015\\Projects\\mysql_pr.c\\x64\\Debug\\mysql_pr.c.exe");
			System.out.println("Call the clustering program.");
					
		} catch (Exception e) {
			System.out.println("Clustering program call failed.");
		}
	}	
}
