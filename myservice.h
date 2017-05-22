/*myservice
  This file define the service 
*/
#ifndef MYSERVICE_H
 #define MYSERVICE_H
#include "myqueue.h"
#include "myhandle.h"

//service callback function
typedef int(*callbackFunc)(int source,struct message_data* msg_d); 
//service create function
typedef int(*createFunc)(int source,struct message_data* msg_d); 
//service init function
typedef int(*initFunc)(); 
//service release function
typedef int(*releaseFunc)(); 

#define S_TYPE_ONCE    1
#define S_TYPE_FOREVER 2

pthread_mutex_t 		service_mutex;		//service mutex

struct serviceInfo{
	int 				handle;				//handle index
	char 				name[MAX_NAME_LEN];	//service name
	callbackFunc 		cb;					//service callbackFunc
	createFunc			create;				//service createFunc
	initFunc			init;				//service initFunc
	releaseFunc			release;			//service releaseFunc
	struct msgQueue*	mq;					//msgQueue
	int 				type;				//service type
};


//create a service 
struct serviceInfo* service_create(char*name,callbackFunc cb,createFunc create,initFunc init,releaseFunc release,int type);

//init a service
void service_init(struct serviceInfo* service);

//release a service
void service_release(struct serviceInfo* service);

//release a service by handle
void service_release_by_handle(int handle);

//release a service by name
void service_release_by_name(char *name);

//init myservice
void myservice_init();

#endif