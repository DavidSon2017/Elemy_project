#define _CRT_SECURE_NO_WARNINGS

#include <mysql.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define DB_HOST "127.0.0.1"
#define DB_USER "root"
#define DB_PASS "1234"
#define DB_NAME "clus" //테이블명 : clus.datas , clus.result
#define RoundOff(x, dig) (floor((x) * pow(10,dig) + 0.5) / pow(10,dig))

typedef struct data {
	double time;
	int floor;
	int cluster;
}data;

typedef struct clus {
	int min_time;
	int max_time;
	int avgfloor;
}clusinfo;

MYSQL       *connection = NULL, conn;
MYSQL_RES   *sql_result;
MYSQL_ROW   sql_row;
int       query_stat;
data * datas;
int dc, kc;

void connectDB();
void clearResult();		// 이전 데이터 삭제
void loadData(int flag);	// 평일 혹은 주말데이터를 읽어옴
void clustering();
void uploadData(int flag);	// 결과 테이블에 데이터 삽입
char* chartime(double t);	// 시, 분, 초를 문자로 환산
int inttime(char * str);	// 시, 분, 초를 정수로 환산

int main(void)
{
	connectDB(); //DB에 접속

	//clearResult(); //이전의 clustering result 테이블 삭제

	loadData(0); //평일데이터 읽어와
	clustering(); //클러스터링 시행
	uploadData(0); //평일결과 저장

	free(datas);

	//loadData(1); //주말데이터 읽어와
	//clustering(); //클러스터링 시행
	//uploadData(1); //주말결과 저장

	//free(datas);

	mysql_close(connection); //DB 접속 종료
}

void connectDB() {
	mysql_init(&conn);
	connection = mysql_real_connect(&conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3306, (char *)NULL, 0);

	if (connection == NULL) {
		fprintf(stderr, "Mysql connection error : %s", mysql_error(&conn));
		return 1;
	}
}

void clearResult() {
	char query[256];

	//이전 데이터 삭제 후
	sprintf(query, "truncate result");
	query_stat = mysql_query(connection, query);
	if (query_stat != 0) {
		fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
		return 1;
	}
}

//평일데이터읽어오기(0), 주말데이터읽어오기(1)
void loadData(int flag) {
	char query[255];
	char buf[255];

	int c_floor; //호출된 층
	int c_time;  //호출된 시간
	int is_weekend = flag; //평일,주말 여부

	time_t timer = time(NULL);
	struct tm *t = localtime(&timer);

	sprintf(buf, "select count(*) from datas where is_weekend=%d", is_weekend);

	query_stat = mysql_query(connection, buf);
	if (query_stat != 0) {
		fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
		return 1;
	}
	sql_result = mysql_store_result(connection);

	while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
	{
		dc = atoi(sql_row[0]);
	}

	mysql_free_result(sql_result);

	kc = ceil(sqrt(dc / 2));
	printf("데이터갯수 : %d -> 군집k갯수 : %d\n", dc, kc);

	datas = (data*)malloc(sizeof(data)*dc); //데이터갯수만큼 d 배열 동적할당

	//데이터배열에 저장을 위한 일주일 전 모든 데이터 불러오기. 주석 풀고 사용
	//sprintf(buf, "select * from datas where '%d-%d-%d'<call_day and call_day<'%d-%d-%d' and is_weekend=%d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday - 7, t->tm_year + 1900, t->tm_mon + 1, t->tm_mday - 1,is_weekend);

	//주석
	sprintf(buf, "select * from datas where is_weekend=%d", is_weekend);

	query_stat = mysql_query(connection, buf);
	if (query_stat != 0) {
		fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
		return 1;
	}

	sql_result = mysql_store_result(connection);

	printf("[time]\t\t[floor]\n");

	int idx = 0;

	while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
	{
		//printf("%s\t%s\n", sql_row[1], sql_row[2]);

		datas[idx].time = inttime(sql_row[1]); //시간 저장
		datas[idx].floor = atoi(sql_row[2]); //층 수 저장

		idx++;
	}
}

int i, j, signal, min_j, same_count;
data *k;
data *center;
int * tmp;
clusinfo * clus;
double *sum, *count, *result;
double **distance;
double tmp_distance, min;

void clustering() {
	double *count_Group;   //군집 그룹
	count_Group = (double*)malloc(sizeof(double)*kc);

	k = (data*)malloc(sizeof(data)*kc);
	center = (data*)malloc(sizeof(data)*kc);
	distance = (double**)malloc(sizeof(double*)*kc);
	tmp = (int *)malloc(sizeof(int)*kc);
	clus = (clusinfo*)malloc(sizeof(clusinfo)*kc);

	// 결과값 계산변수  
	sum = (double*)malloc(sizeof(int)*kc);
	count = (double*)malloc(sizeof(int)*kc);
	result = (double*)malloc(sizeof(int)*kc);

	for (int i = 0; i < kc; i++) {
		clus[i].min_time = 86500;
		clus[i].max_time = 0;

		sum[i] = 0;
		count[i] = 0;
		result[i] = 0;
	} //초기화

	for (i = 0; i<kc; i++) {
		distance[i] = (double*)malloc(sizeof(double)*dc);
	}

	printf("----------------------------------------\n");

	srand(time(NULL));   //난수 seed값 사용

	printf("---------초기랜덤값-----------\n");
	for (int i = 0; i < kc; i++) {
		tmp[i] = (rand() % dc);
		for (int j = 0; j < i; j++) {
			if (tmp[i] == tmp[j]) {
				i--;
				break;
			}
		}
	}

	for (int i = 0; i < kc; i++) {   //k개 개체 임의 추출
		k[i] = datas[tmp[i]];
		center[i].floor = datas[tmp[i]].floor;
		center[i].time = datas[tmp[i]].time;

		printf(" 랜덤추출값 ) ");
		char * str = chartime(center[i].time);
		//printf("%s\n", str);
		//printf("%.2lf %d\n", center[i].time, center[i].floor);
	}
	printf("------------------------------\n");

	//while 탈출키 signal
	signal = 1;

	while (signal)
	{

		for (i = 0; i<kc; i++) {   //중심값을 0으로 초기화
			center[i].time = 0;
			center[i].floor = 0;
			count_Group[i] = 0;
		}

		for (i = 0; i<dc; i++) {   //각각의 모든 데이터에 대하여
			for (j = 0; j<kc; j++) {   //군집의 중심과 데이터와의 거리 계산
				tmp_distance = sqrt(pow(k[j].time - datas[i].time, 2));   //거리 계산 식
				distance[j][i] = tmp_distance;   //거리값 최신화
								 //printf(" %d 거리: %.2f \n",i,distance[j][i]);
			}
		}

		for (i = 0; i<dc; i++) {   			//각각의 모든 데이터에 대하여
			min = distance[0][i];  			//하나의 데이터와 첫번째 군집과의 거리를 최소화
			min_j = 0; 				//데이터가 속한 군집 설정
			for (j = 1; j<kc; j++) {  		//두번째 군집에 대하여
				if (min > distance[j][i]) {  	//첫번째 군집과 두번째 군집간의 거리 비교
					min = distance[j][i];  	//두번째 군집이 더 가깝다면 최소값 변경
					min_j = j;  		//데이터를 두번째 군집으로 설정
				}
			}

			center[min_j].time = center[min_j].time + datas[i].time;  //시간 중심값 누적
			count_Group[min_j]++; 	//군집안에 들어있는 데이터 갯수 증가
						//printf(" %d  %.2f \n",center[min_j].floor,center[min_j].time);
		}
		same_count = 0; //소속군집의 변화를 알기위한 변수 선언
		printf("--------------------\n");
		for (i = 0; i<kc; i++) {
			if (count_Group[i] != 0) { //군집안의 데이터의 개수가 0 이 아니면
				if (center[i].time / count_Group[i] == k[i].time) //현재 계산된 군집별 중심값이 기존의 군집별 중심값(K배열)과 같다면
					same_count++; //군집이 바뀌지 않았다는 것을 나타내는 변수값 증가
				k[i].time = center[i].time / count_Group[i];
			}

			if (same_count == kc) { //모든 데이터의 소속군집의 변화가 없으면
				signal = 0; //무한루프를 빠져나감
			}

			printf(" 중심값 ) ");
			char * str = chartime(k[i].time);
			printf("%s\n", str);
		}
	}// 루프끝

	 //각 데이터의 소속군집을 출력함
	for (i = 0; i<dc; i++) {
		min = distance[0][i];
		min_j = 0;
		for (j = 1; j<kc; j++) {
			if (min > distance[j][i]) {
				min = distance[j][i];
				min_j = j;
			}
		}
		//printf(" %d ", min_j); //소속된 군집(0 or 1) 출력
		datas[i].cluster = min_j;

		count[min_j]++;
		sum[min_j] += datas[i].floor;

		//clusinfo 구조체에 cluster에 따라 시간 초기화
		if (clus[min_j].min_time > datas[i].time) {
			clus[min_j].min_time = datas[i].time;
		}
		if (clus[min_j].max_time < datas[i].time) {
			clus[min_j].max_time = datas[i].time;
		}
	}
	//층 평균 계산
	for (i = 0; i < kc; i++) {
		result[i] = RoundOff((sum[i] / count[i]), 0);
		clus[i].avgfloor = result[i];
	}

	printf("--------------------\n");
	printf("\n---------------- 결과값 -----------------\n");

	for (i = 0; i<dc; i++) {
		//printf(" %d번째 %d층 %.2f초  군집: %d \n", i + 1, datas[i].floor, datas[i].time, datas[i].cluster);
	}

	printf("-----------------------------------------\n");
	printf("\n------------- 최종 결과값 계산 ----------\n");

	for (i = 0; i < kc; i++) {
		char * tmp1, *tmp2;
		tmp1 = chartime(clus[i].min_time);
		tmp2 = chartime(clus[i].max_time);

		printf(" [%d군집] %s ~ %s : %d층\n", i, tmp1, tmp2, clus[i].avgfloor);
	}
	printf("-----------------------------------------\n");
}

void uploadData(int flag) {
	int is_weekend = flag;
	char query[255];
	//char buf[255];

	for (int i = 0; i < kc; i++) {
		//클러스터링 결과 재삽입
		sprintf(query, "insert into result values ('%s','%s','%d','%d')", chartime(clus[i].min_time), chartime(clus[i].max_time), clus[i].avgfloor, is_weekend);
		query_stat = mysql_query(connection, query);
		if (query_stat != 0) {
			fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
			return 1;
		}
	}
}

char* chartime(double t) {
	char * str = malloc(sizeof(char) * 255);

	int hour = t / 3600;
	int minute = (t - (hour * 3600)) / 60;
	int second = t - (3600 * hour) - (60 * minute);

	sprintf(str, "%d:%d:%d", hour, minute, second);

	return str;
}


int inttime(char * str) {
	int time = 0;
	int cnt = 1;
	char * tok;

	tok = strtok(str, ":");

	while (tok != NULL) {
		if (cnt == 1) {
			time += (atoi(tok) * 3600);//시 처리
		}
		else if (cnt == 2) {
			time += (atoi(tok) * 60);//분 처리
		}
		else {
			time += atoi(tok);//초 처리
		}
		tok = strtok(NULL, ":");
		cnt++;
	}
	return time;
}
