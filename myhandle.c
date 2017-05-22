/*myhandle
  This file define the relationship between handle and msgQueue
*/
#include "myqueue.h"
#include "myhandle.h"
#include "myservice.h"
#include "myaux.h"

//init the handle control var
void myhandle_init(){
	pthread_mutex_init(&shMap.mutex,NULL);
	shMap.cap  = INIT_MAP_ITEM_NUM;
	shMap.head = 0;
	shMap.tail = shMap.cap-1;
	servicehandle  = malloc(sizeof(struct serviceHandleItem)*INIT_MAP_ITEM_NUM);
}

//get a empty handle index
int myhandle_getIdleHandle(){
	return 0;
}

//release a handle index
void myhandle_releaseHandle(int handle){
	if (handle<0 ||handle>shMap.cap) return;
	int h = handle % shMap.cap;
	pthread_mutex_lock(&shMap.mutex);
	servicehandle[h].service = NULL;
	pthread_mutex_unlock(&shMap.mutex);
}

//register a serviceInfo and return the handle
int myhandle_registerService(struct serviceInfo* service){
	DBG_PRINT("enter myhandle_registerService\n");
	int result =-1;
	pthread_mutex_lock(&shMap.mutex);
	int initIndex = shMap.head;
	while(1){
		if(servicehandle[shMap.head].service==NULL){
			result = shMap.head;
			servicehandle[shMap.head].service = service;
			shMap.head = (shMap.head+1) % shMap.cap;
			break;
		}
		shMap.head = (shMap.head+1) % shMap.cap;
		if (initIndex == shMap.head) break;
	}
	//if handle map is full enlarge the cap
	if(initIndex == shMap.head){
		result = myhandle_enlarge_map_cap(service);
	}
	pthread_mutex_unlock(&shMap.mutex);
	return result;
}

//find the handle by name
//the design now is bad, should be upgrade
int myhandle_find_handle_byName(char* name){
	int result = -1;
	pthread_mutex_lock(&shMap.mutex);
	int initIndex = shMap.head;
	int i;
	for(i=0;i<shMap.cap;i++){
		if(servicehandle[i].service!=NULL){
			if(strcmp(servicehandle[i].service->name, name) == 0){
				result = i;
				break;
			}
		}
	}
	pthread_mutex_unlock(&shMap.mutex);
	return result;
}


//get the serviceInfo by handle
struct serviceInfo* myhandle_getServiceByHandle(int handle){
	//printf("enter myhandle_getServiceByHandle handle = %d\n",handle);
	struct serviceInfo* result = NULL;
	if (handle<0 ||handle>shMap.cap) return NULL;
	int h = handle % shMap.cap;
	if (servicehandle[h].service!=NULL){
		if (servicehandle[h].service->handle == handle)
			result = servicehandle[h].service;
	}
	return result;
}

//get the msgQueue by handle
struct msgQueue* myhandle_getMqByHandle(int handle){
	//printf("enter myhandle_getMqByHandle handle = %d\n",handle);
	struct msgQueue* result = NULL;
	struct serviceInfo* service = myhandle_getServiceByHandle(handle);
	if (service!=NULL)
		result = service->mq;
	return result;
}

//enlarge the handle serviceInfo map cap
int myhandle_enlarge_map_cap(struct serviceInfo* service){
	DBG_PRINT("enter myhandle_enlarge_map_cap\n");
	struct serviceHandleItem* new_servicehandle   = malloc(sizeof(struct serviceHandleItem)*shMap.cap*2);
	int i = 0;
	for(i=0;i<shMap.cap;i++){
		new_servicehandle[i] = servicehandle[i];
	}
	free(servicehandle);
	servicehandle  = NULL;
	servicehandle  = new_servicehandle;
	int index = shMap.cap;    	//this is the place mq should put
	servicehandle[index].service = service;	//set the mq
	shMap.head = index + 1;		//head should be the index + 1
	shMap.cap  = shMap.cap * 2;	//enlarge the cap value
	return index;
}

/* old version

//init the handle control var
void myhandle_init(){
	pthread_mutex_init(&mqHM.mutex,NULL);
	mqHM.cap  = INIT_MAP_ITEM_NUM;
	mqHM.head = 0;
	mqHM.tail = mqHM.cap-1;
	mqhandle  = malloc(sizeof(struct mqHandleItem)*INIT_MAP_ITEM_NUM);
}

//get a empty handle index
int myhandle_getIdleHandle(){
	return 0;
}

//release a handle index
void myhandle_releaseHandle(int handle){
	if (handle<0 ||handle>mqHM.cap) return;
	int h = handle % mqHM.cap;
	mqhandle[h].mq = NULL;
}

//register a msgQueue and return the handle
int myhandle_registerMq(struct msgQueue* mq){
	printf("enter myhandle_registerMq\n");
	int result =-1;
	pthread_mutex_lock(&mqHM.mutex);
	int initIndex = mqHM.head;
	while(1){
		if(mqhandle[mqHM.head].mq==NULL){
			result = mqHM.head;
			mqhandle[mqHM.head].mq = mq;
			mqHM.head = (mqHM.head+1) % mqHM.cap;
			break;
		}
		mqHM.head = (mqHM.head+1) % mqHM.cap;
		if (initIndex == mqHM.head) break;
	}
	//if handle map is full enlarge the cap
	if(initIndex == mqHM.head){
		result = myhandle_enlarge_map_cap(mq);
	}
	pthread_mutex_unlock(&mqHM.mutex);
	return result;
}

//find the handle by name
//the design now is bad, should be upgrade
int myhandle_find_handle_byName(char* name){
	int result = -1;
	pthread_mutex_lock(&mqHM.mutex);
	int initIndex = mqHM.head;
	int i;
	for(i=0;i<mqHM.cap;i++){
		if(mqhandle[i].mq!=NULL){
			if(strcmp(mqhandle[i].mq->name, name) == 0){
				result = i;
				break;
			}
		}
	}
	pthread_mutex_unlock(&mqHM.mutex);
	return result;
}


//get the msgQueue by handle
struct msgQueue* myhandle_getMqByHandle(int handle){
	printf("enter myhandle_getMqByHandle handle = %d\n",handle);
	struct msgQueue* result = NULL;
	if (handle<0 ||handle>mqHM.cap) return NULL;
	int h = handle % mqHM.cap;
	if (mqhandle[h].mq!=NULL){
		if (mqhandle[h].mq->handle == handle)
			result = mqhandle[h].mq;
	}
	return result;
}

//enlarge the handle msgQueue map cap
int myhandle_enlarge_map_cap(struct msgQueue* mq){
	printf("enter myhandle_enlarge_map_cap\n");
	struct mqHandleItem* new_mqhandle   = malloc(sizeof(struct mqHandleItem)*mqHM.cap*2);
	int i = 0;
	for(i=0;i<mqHM.cap;i++){
		new_mqhandle[i] = mqhandle[i];
	}
	free(mqhandle);
	mqhandle  = new_mqhandle;
	int index = mqHM.cap;    	//this is the place mq should put
	mqhandle[index].mq = mq;	//set the mq
	mqHM.head = index + 1;		//head should be the index + 1
	mqHM.cap  = mqHM.cap * 2;	//enlarge the cap value
	return index;
}

//*/