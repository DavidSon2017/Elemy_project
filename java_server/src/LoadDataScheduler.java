package newpr;

import org.quartz.Job;
import org.quartz.JobExecutionContext;
import org.quartz.JobExecutionException;

public class LoadDataScheduler implements Job{
	public void execute(JobExecutionContext arg0) throws JobExecutionException{
		//매일 클러스터링 결과 불러오기
		ConnectData.loadClusResult();
	}	
}
