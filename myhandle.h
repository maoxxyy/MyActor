/*myhandle
  This file define the relationship between handle and msgQueue
*/
#ifndef  MYHANDLE_H
  #define MYHANDLE_H
#include "myqueue.h"
#define INIT_MAP_ITEM_NUM  4

struct serviceHandleMap{
	pthread_mutex_t 		mutex;			//queue mutex
	int cap;
	int head;
	int tail;
};

//
struct serviceHandleItem{
	int              	handle;
	char             	name[MAX_NAME_LEN];		//instant name
	struct serviceInfo* service;				//the service info
};



static struct serviceHandleMap shMap; 
struct serviceHandleItem* servicehandle;

//init the handle control var
void myhandle_init();

//get a empty handle index
int myhandle_getIdleHandle();

//release a handle index
void myhandle_releaseHandle(int handle);

//register a msgQueue and return the handle
//int myhandle_registerMq(struct msgQueue* mq);

//register a Service and return the handle
int myhandle_registerService(struct serviceInfo* service);

//get the service by handle
struct serviceInfo* myhandle_getServiceByHandle(int handle);

//get the msgQueue by handle
struct msgQueue* myhandle_getMqByHandle(int handle);

//enlarge the handle msgQueue map cap
int myhandle_enlarge_map_cap(struct serviceInfo* service);
#endif

/* old version
#ifndef  MYHANDLE_H
  #define MYHANDLE_H
#include "myqueue.h"
#define INIT_MAP_ITEM_NUM  4

struct mqHandleMap{
	pthread_mutex_t 		mutex;			//queue mutex
	int cap;
	int head;
	int tail;
};

//
struct mqHandleItem{
	int              	handle;
	char             	name[20];		//instant name
	struct serviceInfo* service;		//the service info
} ;



static struct mqHandleMap mqHM; 
struct mqHandleItem* mqhandle;

//init the handle control var
void myhandle_init();

//get a empty handle index
int myhandle_getIdleHandle();

//release a handle index
void myhandle_releaseHandle(int handle);

//register a msgQueue and return the handle
//int myhandle_registerMq(struct msgQueue* mq);

//register a Service and return the handle
int myhandle_registerService(struct serviceInfo* service);

//get the service by handle
struct serviceInfo* myhandle_getServiceByHandle(int handle);

//get the msgQueue by handle
//struct msgQueue* myhandle_getMqByHandle(int handle);

//enlarge the handle msgQueue map cap
int myhandle_enlarge_map_cap(struct msgQueue* mq);
#endif
//*/