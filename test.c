#include "pthread.h"
#include "sys/time.h"
#include "myqueue.h"
#include "myhandle.h"
#include "myservice.h"
#include "mysysenv.h"
#include "myaux.h"

static pthread_mutex_t test_mutex;
static pthread_cond_t  test_cond;

#include<stdio.h>  
#include<stdlib.h>  
#include<signal.h>  
#include<string.h>  
#include<execinfo.h>  
  
void SystemErrorHandler(int signum)  
{  
    const int len=1024;  
    void *func[len];  
    size_t size;  
    int i;  
    char **funs;  
  
    signal(signum,SIG_DFL);  
    size=backtrace(func,len);  
    funs=(char**)backtrace_symbols(func,size);  
    fprintf(stderr,"System error, Stack trace:\n");  
    for(i=0;i<size;++i) fprintf(stderr,"%d %s \n",i,funs[i]);  
    free(funs);  
    //exit(1);  
}  
  
void Fun1()  
{  
    char *p=NULL;  
    *p = 'A';  
}  
  
void Fun()  
{  
    Fun1();  
}  


void get_abstime_wait(int microseconds, struct timespec *abstime)
{
  struct timeval tv;
  int absmsec;
  gettimeofday(&tv, NULL);
  absmsec = tv.tv_sec * 1000 + tv.tv_usec / 1000;
  absmsec += microseconds;

  abstime->tv_sec = absmsec / 1000;
  abstime->tv_nsec = absmsec % 1000 * 1000000;
}

void *thread_test(void* arg){
	DBG_PRINT("%s\n",arg);
	int i = 0;
	while(1)
	{
		struct timeval now;
		struct timespec outtime;
		pthread_mutex_lock(&test_mutex);
		gettimeofday(&now,NULL);
		DBG_PRINT("now = %u\n", now.tv_sec);
		outtime.tv_sec = now.tv_sec + 1;
		outtime.tv_nsec = now.tv_usec*1000;
		//get_abstime_wait(1000,&outtime);
		pthread_cond_timedwait(&test_cond,&test_mutex,&outtime);
		pthread_mutex_unlock(&test_mutex);

		int source = outtime.tv_sec % 10;
		int handle = now.tv_usec % 30;
		int code   = i++;
		int moduleIndex = now.tv_usec % 40;

		struct message_data *new_msg_data = malloc(sizeof(struct message_data));
		new_msg_data->code = code;
		//send_msg(source,handle,new_msg_data);
		char name[MAX_NAME_LEN];
		sprintf(name,"module-%d",moduleIndex);
		send_msg_byName(source,name,new_msg_data);
	}
	DBG_PRINT("%s-222\n",arg);
}

void send_test_msg(int source){
	struct message_data *new_msg_data = msgQueue_create_msgData();
	if(new_msg_data == NULL) return;
	new_msg_data->code = source;
	char name[MAX_NAME_LEN];
	int target = (source + 1)%9+1;
	//int target = (source + 1)%10;
	sprintf(name,"module-%d",target);
	send_msg_byName(source,name,new_msg_data);
}

void send_test_msg0(int source){
	struct message_data *new_msg_data = msgQueue_create_msgData();
	if(new_msg_data == NULL) return;
	new_msg_data->code = source;
	char name[MAX_NAME_LEN];
	int target = source;
	sprintf(name,"service-%d",target);
	send_msg_byName(source,name,new_msg_data);
}

int module_x_callback(int source,struct message_data* msg_d){
	DBG_PRINT(L_RED"xxxxxmodule_x_callback,source= %d, msg_d->code = %d \n"NONE,source,msg_d->code);
	//send_test_msg(0);
	//send_test_msg(0);
	return 0;
}
int static idx = 100;

int module_0_callback(int source,struct message_data* msg_d){
	DBG_PRINT("00000module_0_callback,source= %d, msg_d->code = %d \n",source,msg_d->code);
	//send_test_msg(0);
	char name[MAX_NAME_LEN];
	sprintf(name,"service-%d",idx++);
    service_create(name, module_x_callback, NULL, NULL, NULL,S_TYPE_ONCE);
    send_test_msg0(idx-1);
	//send_test_msg(0);
	return 0;
}

int module_1_callback(int source,struct message_data* msg_d){
	//DBG_PRINT("11111module_1_callback,source= %d, msg_d->code = %d \n",source,msg_d->code);
	send_test_msg(1);
	send_test_msg0(0);
	//send_test_msg(0);
	return 0;
}
int module_2_callback(int source,struct message_data* msg_d){
	//DBG_PRINT("22222module_2_callback,source= %d, msg_d->code = %d \n",source,msg_d->code);
	send_test_msg(2);
	send_test_msg0(0);
	//send_test_msg(5);
	return 0;
}
int module_3_callback(int source,struct message_data* msg_d){
	//DBG_PRINT("33333module_3_callback,source= %d, msg_d->code = %d \n",source,msg_d->code);
	send_test_msg(3);
	send_test_msg0(0);
	//send_test_msg(1);
	return 0;
}
int module_4_callback(int source,struct message_data* msg_d){
	//DBG_PRINT("44444module_4_callback,source= %d, msg_d->code = %d \n",source,msg_d->code);
	send_test_msg(4);
	send_test_msg0(0);
	//send_test_msg(2);
	return 0;
}
int module_5_callback(int source,struct message_data* msg_d){
	//DBG_PRINT("55555module_5_callback,source= %d, msg_d->code = %d \n",source,msg_d->code);
	send_test_msg(5);
	send_test_msg0(0);
	//send_test_msg(9);
	return 0;
}
int module_6_callback(int source,struct message_data* msg_d){
	//DBG_PRINT("66666module_6_callback,source= %d, msg_d->code = %d \n",source,msg_d->code);
	send_test_msg(6);
	send_test_msg0(0);
	//send_test_msg(7);
	return 0;
}
int module_7_callback(int source,struct message_data* msg_d){
	//DBG_PRINT("77777module_7_callback,source= %d, msg_d->code = %d \n",source,msg_d->code);
	send_test_msg(7);
	send_test_msg0(0);
	//send_test_msg(5);
	return 0;
}
int module_8_callback(int source,struct message_data* msg_d){
	//DBG_PRINT("88888module_8_callback,source= %d, msg_d->code = %d \n",source,msg_d->code);
	send_test_msg(8);
	send_test_msg0(0);
	//send_test_msg(2);
	return 0;
}
int module_9_callback(int source,struct message_data* msg_d){
	//DBG_PRINT("99999module_9_callback,source= %d, msg_d->code = %d \n",source,msg_d->code);
	send_test_msg(9);
	send_test_msg0(0);
	//send_test_msg(3);
	return 0;
}


struct serviceInfo* module_x_init(callbackFunc cb,char* name){//name should short less than 20
	struct serviceInfo* service = NULL;
	service = service_create(name, cb, NULL, NULL, NULL,S_TYPE_FOREVER);
	return service;
}

void modules_init(){
	module_x_init(module_0_callback,"service-0");
	module_x_init(module_1_callback,"module-1");
	module_x_init(module_2_callback,"module-2");
	module_x_init(module_3_callback,"module-3");
	module_x_init(module_4_callback,"module-4");
	module_x_init(module_5_callback,"module-5");
	module_x_init(module_6_callback,"module-6");
	module_x_init(module_7_callback,"module-7");
	module_x_init(module_8_callback,"module-8");
	module_x_init(module_9_callback,"module-9");
	int i = 0;
	for (i=0;i<30;i++){
		char name[MAX_NAME_LEN];
		sprintf(name,"module-%d",i+10);
		module_x_init(module_x_callback,name);
	}
}



main(){
    signal(SIGSEGV,SystemErrorHandler); //Invaild memory address  
    signal(SIGABRT,SystemErrorHandler); // Abort signal  
    //Fun();  
    //return 0;

	mysysenv_init();
	modules_init();
	send_test_msg(5);
	send_test_msg0(0);
	//pthread_mutex_init(&test_mutex,NULL);
	//pthread_cond_init(&test_cond,NULL);
	//pthread_t th_t;
	//pthread_create(&th_t,NULL,thread_test,"myTest");DBG_PRINT("th_t=%u\n",th_t);
	mysysenv_startwork();
	//pthread_join(th_t,NULL);
}
