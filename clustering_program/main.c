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
#define DB_NAME "clus" //���̺�� : clus.datas , clus.result
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
void clearResult();	// ���� ������ ����
void loadData(int flag);	// ���� Ȥ�� �ָ������͸� �о��
void clustering();
void uploadData(int flag);	// ��� ���̺� ������ ����
char* chartime(double t);	// ��, ��, �ʸ� ���ڷ� ȯ��
int inttime(char * str);	// ��, ��, �ʸ� ������ ȯ��

int main(void)
{
	connectDB(); //DB�� ����

				 //clearResult(); //������ clustering result ���̺� ����

	loadData(0); //���ϵ����� �о��
	clustering(); //Ŭ�����͸� ����
	uploadData(0); //���ϰ�� ����

	free(datas);

	//loadData(1); //�ָ������� �о��
	//clustering(); //Ŭ�����͸� ����
	//uploadData(1); //�ָ���� ����

	//free(datas);

	mysql_close(connection); //DB ���� ����
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

	//���� ������ ���� ��
	sprintf(query, "truncate result");
	query_stat = mysql_query(connection, query);
	if (query_stat != 0) {
		fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
		return 1;
	}

}

//���ϵ������о����(0), �ָ��������о����(1)
void loadData(int flag) {
	char query[255];
	char buf[255];

	int c_floor; //ȣ��� ��
	int c_time;  //ȣ��� �ð�
	int is_weekend = flag; //����,�ָ� ����

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
	printf("�����Ͱ��� : %d -> ����k���� : %d\n", dc, kc);

	datas = (data*)malloc(sizeof(data)*dc); //�����Ͱ�����ŭ d �迭 �����Ҵ�

	//�����͹迭�� ������ ���� ������ �� ��� ������ �ҷ�����. �ּ� Ǯ�� ���
	//sprintf(buf, "select * from datas where '%d-%d-%d'<call_day and call_day<'%d-%d-%d' and is_weekend=%d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday - 7, t->tm_year + 1900, t->tm_mon + 1, t->tm_mday - 1,is_weekend);

	//�ּ�
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

		datas[idx].time = inttime(sql_row[1]); //�ð� ����
		datas[idx].floor = atoi(sql_row[2]); //�� �� ����

		idx++;
	}

	//printf("����ü�� ����Ȯ��\n");
	//for (int i=0;i<dc;i++){
	//	printf("%.2f - %d\n", datas[i].time, datas[i].floor);
	//}
	//printf("----------------------------------------------\n");

	//mysql_free_result(sql_result);
}

int i, j, signal, min_j, same_count;
data *k;
data *center;
int * tmp;
clusinfo * clus;
double *sum, *count, *result;
double **distance;
double tmp_distance, min;

//������ Ŭ�����͸�
void clustering() {
	//--------------------------------Ŭ�����͸� �Լ� �߰�
	double *count_Group;   //���� �׷�
	count_Group = (double*)malloc(sizeof(double)*kc);

	k = (data*)malloc(sizeof(data)*kc);
	center = (data*)malloc(sizeof(data)*kc);
	distance = (double**)malloc(sizeof(double*)*kc);
	tmp = (int *)malloc(sizeof(int)*kc);
	clus = (clusinfo*)malloc(sizeof(clusinfo)*kc);

	// ����� ��꺯��  
	sum = (double*)malloc(sizeof(int)*kc);
	count = (double*)malloc(sizeof(int)*kc);
	result = (double*)malloc(sizeof(int)*kc);

	for (int i = 0; i < kc; i++) {
		clus[i].min_time = 86500;
		clus[i].max_time = 0;

		sum[i] = 0;
		count[i] = 0;
		result[i] = 0;
	} //�ʱ�ȭ

	for (i = 0; i<kc; i++) {
		distance[i] = (double*)malloc(sizeof(double)*dc);
	}

	printf("----------------------------------------\n");

	srand(time(NULL));   //���� seed�� ���

	printf("---------�ʱⷣ����-----------\n");
	for (int i = 0; i < kc; i++) {
		tmp[i] = (rand() % dc);
		for (int j = 0; j < i; j++) {
			if (tmp[i] == tmp[j]) {
				i--;
				break;
			}
		}
	}

	for (int i = 0; i < kc; i++) {   //k�� ��ü ���� ����
		k[i] = datas[tmp[i]];
		center[i].floor = datas[tmp[i]].floor;
		center[i].time = datas[tmp[i]].time;

		printf(" �������Ⱚ ) ");
		char * str = chartime(center[i].time);
		//printf("%s\n", str);
		//printf("%.2lf %d\n", center[i].time, center[i].floor);
	}
	printf("------------------------------\n");

	//while Ż��Ű signal
	signal = 1;

	while (signal)
	{

		for (i = 0; i<kc; i++) {   //�߽ɰ��� 0���� �ʱ�ȭ
			center[i].time = 0;
			center[i].floor = 0;
			count_Group[i] = 0;
		}

		for (i = 0; i<dc; i++) {   //������ ��� �����Ϳ� ���Ͽ�
			for (j = 0; j<kc; j++) {   //������ �߽ɰ� �����Ϳ��� �Ÿ� ���
				tmp_distance = sqrt(pow(k[j].time - datas[i].time, 2));   //�Ÿ� ��� ��
				distance[j][i] = tmp_distance;   //�Ÿ��� �ֽ�ȭ
												 //printf(" %d �Ÿ�: %.2f \n",i,distance[j][i]);
			}
		}

		for (i = 0; i<dc; i++) {   //������ ��� �����Ϳ� ���Ͽ�
			min = distance[0][i];  //�ϳ��� �����Ϳ� ù��° �������� �Ÿ��� �ּ�ȭ
			min_j = 0; //�����Ͱ� ���� ���� ����
			for (j = 1; j<kc; j++) {  //�ι�° ������ ���Ͽ�
				if (min > distance[j][i]) {  //ù��° ������ �ι�° �������� �Ÿ� ��
					min = distance[j][i];  //�ι�° ������ �� �����ٸ� �ּҰ� ����
					min_j = j;  //�����͸� �ι�° �������� ����
				}
			}

			center[min_j].time = center[min_j].time + datas[i].time;  //�ð� �߽ɰ� ����
			count_Group[min_j]++;  //�����ȿ� ����ִ� ������ ���� ����
								   //printf(" %d  %.2f \n",center[min_j].floor,center[min_j].time);
		}
		same_count = 0; //�Ҽӱ����� ��ȭ�� �˱����� ���� ����
		printf("--------------------\n");
		for (i = 0; i<kc; i++) {
			if (count_Group[i] != 0) { //�������� �������� ������ 0 �� �ƴϸ�
				if (center[i].time / count_Group[i] == k[i].time) //���� ���� ������ �߽ɰ��� ������ ������ �߽ɰ�(K�迭)�� ���ٸ�
					same_count++; //������ �ٲ��� �ʾҴٴ� ���� ��Ÿ���� ������ ����
				k[i].time = center[i].time / count_Group[i];
			}

			if (same_count == kc) { //��� �������� �Ҽӱ����� ��ȭ�� ������
				signal = 0; //���ѷ����� ��������
			}

			printf(" �߽ɰ� ) ");
			char * str = chartime(k[i].time);
			printf("%s\n", str);
		}
	}// ������

	 //�� �������� �Ҽӱ����� �����
	for (i = 0; i<dc; i++) {
		min = distance[0][i];
		min_j = 0;
		for (j = 1; j<kc; j++) {
			if (min > distance[j][i]) {
				min = distance[j][i];
				min_j = j;
			}
		}
		//printf(" %d ", min_j); //�Ҽӵ� ����(0 or 1) ���
		datas[i].cluster = min_j;

		count[min_j]++;
		sum[min_j] += datas[i].floor;


		//clusinfo ����ü�� cluster�� ���� �ð� �ʱ�ȭ
		if (clus[min_j].min_time > datas[i].time) {
			clus[min_j].min_time = datas[i].time;
		}
		if (clus[min_j].max_time < datas[i].time) {
			clus[min_j].max_time = datas[i].time;
		}
	}

	//�� ��� ���
	for (i = 0; i < kc; i++) {
		result[i] = RoundOff((sum[i] / count[i]), 0);
		clus[i].avgfloor = result[i];
	}

	printf("--------------------\n");
	printf("\n---------------- ����� -----------------\n");

	for (i = 0; i<dc; i++) {
		//printf(" %d��° %d�� %.2f��  ����: %d \n", i + 1, datas[i].floor, datas[i].time, datas[i].cluster);
	}

	printf("-----------------------------------------\n");
	printf("\n------------- ���� ����� ��� ----------\n");

	for (i = 0; i < kc; i++) {
		char * tmp1, *tmp2;
		tmp1 = chartime(clus[i].min_time);
		tmp2 = chartime(clus[i].max_time);

		printf(" [%d����] %s ~ %s : %d��\n", i, tmp1, tmp2, clus[i].avgfloor);
	}
	printf("-----------------------------------------\n");
}

void uploadData(int flag) {
	int is_weekend = flag;
	char query[255];
	//char buf[255];

	for (int i = 0; i < kc; i++) {
		//Ŭ�����͸� ��� �����
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
			time += (atoi(tok) * 3600);//�� ó��
		}
		else if (cnt == 2) {
			time += (atoi(tok) * 60);//�� ó��
		}
		else {
			time += atoi(tok);//�� ó��
		}
		tok = strtok(NULL, ":");
		cnt++;
	}
	return time;
}