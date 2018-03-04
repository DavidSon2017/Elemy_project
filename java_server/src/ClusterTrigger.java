package newpr;

import org.quartz.CronScheduleBuilder;
import org.quartz.JobBuilder;
import org.quartz.JobDetail;
import org.quartz.JobKey;
import org.quartz.Scheduler;
import org.quartz.SchedulerFactory;
import org.quartz.Trigger;
import org.quartz.TriggerBuilder;
import org.quartz.impl.StdSchedulerFactory;

public class ClusterTrigger {
	ClusterTrigger(){
		try{		
			SchedulerFactory schedulerFactory = new StdSchedulerFactory();
			Scheduler scheduler = schedulerFactory.getScheduler();
			scheduler.start();
			
			
			//���� ������ ȣ�� �Ǵ� Ŭ�����͸� ���α׷�
			JobKey jobKey1 = new JobKey("job1","group1");
			JobDetail jobDetail1 = JobBuilder.newJob(ClusterScheduler.class)
					.withIdentity(jobKey1)
					.build();
			
			Trigger trigger1 = TriggerBuilder.newTrigger()
					.withIdentity("trigger1","triggerGroup1")
					.withSchedule(CronScheduleBuilder.cronSchedule("0 0 1 ? * 2")) //���� ������ ���� 1��
					.build();
			
			/*
			 * ���� �Ͽ��� ���� 9�ÿ� ���� 0 0 9 ? * 1
			 * ���� 5�� 24�п� 0 24 17 * * ?
			 * 2�ʸ��� ���� 0/5 * * * * ?
			 * ���� ���� 9�ÿ� ���� 0 0 9 * * ?
			 */
			
			//���� ȣ��Ǵ� Ŭ�����͸��� ���α׷�
			JobKey jobKey2 = new JobKey("job2","group2");
			JobDetail jobDetail2 = JobBuilder.newJob(LoadDataScheduler.class)
					.withIdentity(jobKey2)
					.build();
			
			Trigger trigger2 = TriggerBuilder.newTrigger()
					.withIdentity("trigger2","triggerGroup2")
					.withSchedule(CronScheduleBuilder.cronSchedule("0 0 1 * * ?")) //���� ���� 1��
					.build();
			
			scheduler.scheduleJob(jobDetail1,trigger1);
			scheduler.scheduleJob(jobDetail2,trigger2);
		}catch(Exception e){
			e.printStackTrace();
		}
	}
}
