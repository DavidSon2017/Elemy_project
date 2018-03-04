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
			
			
			//매주 월요일 호출 되는 클러스터링 프로그램
			JobKey jobKey1 = new JobKey("job1","group1");
			JobDetail jobDetail1 = JobBuilder.newJob(ClusterScheduler.class)
					.withIdentity(jobKey1)
					.build();
			
			Trigger trigger1 = TriggerBuilder.newTrigger()
					.withIdentity("trigger1","triggerGroup1")
					.withSchedule(CronScheduleBuilder.cronSchedule("0 0 1 ? * 2")) //매주 월요일 오전 1시
					.build();
			
			/*
			 * 매주 일요일 오전 9시에 수행 0 0 9 ? * 1
			 * 오후 5시 24분에 0 24 17 * * ?
			 * 2초만에 시행 0/5 * * * * ?
			 * 매일 오전 9시에 수행 0 0 9 * * ?
			 */
			
			//매일 호출되는 클러스터링층 프로그램
			JobKey jobKey2 = new JobKey("job2","group2");
			JobDetail jobDetail2 = JobBuilder.newJob(LoadDataScheduler.class)
					.withIdentity(jobKey2)
					.build();
			
			Trigger trigger2 = TriggerBuilder.newTrigger()
					.withIdentity("trigger2","triggerGroup2")
					.withSchedule(CronScheduleBuilder.cronSchedule("0 0 1 * * ?")) //매일 오전 1시
					.build();
			
			scheduler.scheduleJob(jobDetail1,trigger1);
			scheduler.scheduleJob(jobDetail2,trigger2);
		}catch(Exception e){
			e.printStackTrace();
		}
	}
}
