package newpr;

import org.quartz.Job;
import org.quartz.JobExecutionContext;
import org.quartz.JobExecutionException;

public class LoadDataScheduler implements Job{
	public void execute(JobExecutionContext arg0) throws JobExecutionException{
		//���� Ŭ�����͸� ��� �ҷ�����
		ConnectData.loadClusResult();
	}	
}
