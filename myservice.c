/*myservice
  This file define the service 
*/
#include "myqueue.h"
#include "myhandle.h"
#include "myservice.h"
#include "myaux.h"

//create a service 
struct serviceInfo* service_create(char*name,callbackFunc cb,createFunc create,initFunc init,releaseFunc release,int type){
	struct serviceInfo* service = NULL;
	service 			= malloc(sizeof(struct serviceInfo));
	if(service == NULL) return NULL;
	service->cb 		= cb;
	service->create 	= create;
	service->init 		= init;
	service->release 	= release;
	service->type		= type;
	if(strlen(name)<MAX_NAME_LEN)
		sprintf(service->name, "%s" , name);
	else{
		DBG_PRINT("service_create,para name lenght should less then %d,name = %s\n",MAX_NAME_LEN ,name);
		strncpy(service->name,name,MAX_NAME_LEN-1);
	}
	int handle = myhandle_registerService(service);
	if (handle<0){ 
		DBG_PRINT("error-------- service_create,handle<0\n");
		//pthread_mutex_lock(&service_mutex);
		//free(service);
		//service = NULL;
		//pthread_mutex_unlock(&service_mutex);
		return NULL;
	}
	DBG_PRINT(L_RED"============service_create,name = %s,handle = %d\n"NONE,service->name,handle);
	service->handle = handle;
	struct msgQueue* mq = gMsgQueue_create_msgQueue();
	gMsgQueue_init_msgQueue(mq,handle,cb,service->name,service);
	service->mq = mq;
	//gMsgQueue_put_msgQueue(mq); //the mq is empty and needn't put into globe queue
	return service;
}

//init a service
void service_init(struct serviceInfo* service){
	service->init();
}

//release a service
void service_release(struct serviceInfo* service){
	DBG_PRINT(L_RED"============service_release,name = %s,handle = %d\n"NONE,service->name,service->handle);
	pthread_mutex_lock(&service_mutex);
	if(service != NULL){
		DBG_PRINT(L_RED"============service_release,name = %s,handle = %d\n"NONE,service->name,service->handle);
		if (service->release!=NULL)
			service->release();
		if(service->mq != NULL){
			gMsgQueue_release_msgQueue(service->mq);
		}
		myhandle_releaseHandle(service->handle);
		DBG_PRINT(L_RED"====service_release ==service=%p,service-2=%x,service=%x,service->handle=%d===========\n"NONE,service,*((int*)service-2),*((int*)service),service->handle);
		if (service->handle > 10000 || service->handle<-10000 || service->handle == 0){
			DBG_PRINT_2(L_GREEN"====service_release ==service=%p,service-2=%x,service=%x,service->handle=%d===========\n"NONE,service,*((int*)service-2),*((int*)service),service->handle);
			//int i=1;
			//i = i/0;
		}
		//else
			free(service); //for debug
		DBG_PRINT(L_RED"============service_release ================\n"NONE);
		service = NULL;
	}
	pthread_mutex_unlock(&service_mutex);
}

//release a service by handle
void service_release_by_handle(int handle){
	struct serviceInfo* service = myhandle_getServiceByHandle(handle);
	service_release(service);
}

//release a service by name
void service_release_by_name(char *name){
	int handle = myhandle_find_handle_byName(name);
	if (handle>=0)
		service_release_by_handle(handle);
}

//init myservice
void myservice_init(){
	pthread_mutex_init(&service_mutex,NULL);
}