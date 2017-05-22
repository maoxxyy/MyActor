#include "pthread.h"
#include "sys/time.h"
#include "myqueue.h"
#include "myhandle.h"
#include "myservice.h"
#include "mysysenv.h"
#include "myaux.h"

void *thread_work(void* arg){
	DBG_PRINT("thread_work ----------------- %s\n",arg);
	char thread_name[MAX_NAME_LEN];
	strcpy(thread_name,arg);
	while(1)
	{
		struct msgQueue_msg* msg = NULL;
		struct msgQueue * mq ;
		mq = gMsgQueue_get_msgQueue();
		if(mq!=NULL){
			//pthread_mutex_lock(&mq->mutex);
			//DBG_PRINT("%s,get mq, mq->handle= %d, mq->name= %s\n",thread_name,mq->handle,mq->name);
			msg = msgQueue_get_msg(mq);
			if(msg!=NULL){
				DBG_PRINT("%s,get msg msg->msgData->code = %d \n",thread_name,msg->msgData->code);
				if(mq->cb != NULL){
					mq->cb(msg->source,msg->msgData);
				}
				msgQueue_release_msg(msg);
				//DBG_PRINT("%s,get msg ------------ \n",thread_name);
			}else{
				//static int i = 0;
				//i++;
				//if(i>10)
				//	i = 1/0;
				DBG_PRINT("%s,get msg --!!!!!---------- \n",thread_name);
			}
			//pthread_mutex_unlock(&mq->mutex);
		}else{
			pthread_mutex_lock(&sysenv_mutex);
			pthread_cond_wait(&sysenv_cond, &sysenv_mutex);
			pthread_mutex_unlock(&sysenv_mutex);
		}
	}
}

//resume the work thread
void mysysenv_resumework(){
	pthread_mutex_lock(&sysenv_mutex);
	pthread_cond_broadcast(&sysenv_cond);
	pthread_mutex_unlock(&sysenv_mutex);
}

//sys env init func
void mysysenv_init(){
	msgModule_init();	//globe queue init
	myhandle_init();	//init the handle service map
	myservice_init();
}

//start the sys work thread
void mysysenv_startwork(){
	DBG_PRINT("enter mysysenv_startwork\n");
	pthread_t th[WORK_THREAD_NUM];
	pthread_mutex_init(&sysenv_mutex,NULL);
	pthread_cond_init(&sysenv_cond,NULL);
	int i;
	for (i=0;i<WORK_THREAD_NUM;i++){
		//pthread_mutex_lock(&mutex);
		char str[MAX_NAME_LEN];
		sprintf(str, "thread-%d" , i);
		pthread_create(&th[i],NULL,thread_work,str);
		sleep(1);
		//DBG_PRINT("th_t[%d]=%u\n",i,th[i]);
		//pthread_mutex_unlock(&mutex);
	}
	for (i=0;i<WORK_THREAD_NUM;i++){
		pthread_join(th[i],NULL);
	}
}
