/*myqueue
  This file define the queue
*/
#ifndef MYQUEUE_H
#define MYQUEUE_H
#include "pthread.h"

#ifndef TRUE
  #define TRUE 1
#endif
#ifndef FALSE
  #define FALSE 0
#endif


#define MAX_NAME_LEN 20

//message data struct
struct message_data
{
	int   code;
	char* data;
};

//message struct
struct msgQueue_msg
{
	int    source;		//handle index
	int    target;		//handle index
	struct message_data *msgData;
	struct msgQueue_msg *next;
};

//message callback function
typedef int(*cbFunc)(int source,struct message_data* msg_d); 

//need a message queue, should include following info
struct msgQueue
{
	int 					handle;				//handle index
	pthread_mutex_t 		mutex;				//queue mutex
	struct serviceInfo*		parentService;		//parent Service
	cbFunc 					cb;					//message callbackFunc
	char                    name[MAX_NAME_LEN];	//instant name
	struct msgQueue_msg*	head;				//queue head
	struct msgQueue_msg*	tail;				//queue tail
	struct msgQueue*		next;				//next msgQueue
	int    	                inGlobeQueue;		//mark msgQueue is in globe queue or not
	int                 	queueLen;			//queue len
};
typedef struct msgQueue* msgQueueP;

//init the program
void msgModule_init();

//send msg api, send a message to target handler
void send_msg(int source,int target,struct message_data *msgdata);

//put a msg into the message queue
void msgQueue_put_msg(struct msgQueue* mq,int source,struct message_data* msg_d);

//get a msg from the message queue
struct msgQueue_msg* msgQueue_get_msg(struct msgQueue* mq);

//dispatch a message, need call the cbFunc defined in the mq
void msgQueue_dispatch_message(int srcHandle,int tarHandle,struct msgQueue* mq, struct message_data * msg_d);

//create a message 
struct msgQueue_msg* msgQueue_create_msg();

//create a message data
struct message_data* msgQueue_create_msgData();

//release a msg
void msgQueue_release_msg(struct msgQueue_msg* msg);

//globe message queue
struct gMsgQueue{
	pthread_mutex_t 	mutex;	//mutex
	struct msgQueue*	head;	//queue head
	struct msgQueue*	tail;	//queue tail
	int                 queueLen;//queue len
};

//put a message queue at the end of gMsgQueue 
void gMsgQueue_put_msgQueue (struct msgQueue* mq);

//get a message queue from the head of gMsgQueue
struct msgQueue* gMsgQueue_get_msgQueue();

//create a message queue
struct msgQueue* gMsgQueue_create_msgQueue();

//release a message queue
void gMsgQueue_release_msgQueue();

//init a message queue
void gMsgQueue_init_msgQueue(struct msgQueue* mq,int handle,cbFunc cb,char* name,struct serviceInfo* service);

#endif