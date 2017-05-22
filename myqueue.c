/*myqueue
  This file define the queue
*/
#include "pthread.h"
#include "myqueue.h"
#include "myaux.h"
#include "myservice.h"

static struct gMsgQueue gMQ;	//the gMsgQueue control value

//init the program
void msgModule_init(){
	gMQ.head = NULL;
	gMQ.tail = NULL;
	pthread_mutex_init(&gMQ.mutex,NULL);
	gMQ.queueLen = 0;
	return;
}

//send msg api
void send_msg(int source,int target,struct message_data *msgdata){
	//DBG_PRINT("enter send_msg,source=%d,target=%d,new_msg_data->code = %d\n",source,target,msgdata->code);
	struct msgQueue* mq = NULL;
	int handle = target;
	mq =  myhandle_getMqByHandle(handle);
	if(mq != NULL){
		msgQueue_put_msg(mq,source,msgdata);
	}
}

//send msg by name api
void send_msg_byName(int source,char* name,struct message_data *msgdata){
	//DBG_PRINT("enter send_msg_byName,source=%d,name=%s,new_msg_data->code = %d\n",source,name,msgdata->code);
	int handle = myhandle_find_handle_byName(name);
	if (handle>=0)
		send_msg(source,handle,msgdata);
}

//put a msg into the message queue
void msgQueue_put_msg(struct msgQueue* mq,int source,struct message_data* msg_d){
	struct msgQueue_msg* msg = NULL;
	if (mq == NULL) {
		DBG_PRINT("enter msgQueue_put_msg,error mq = null\n");
		return;
	}
	if(msg_d != NULL)
		DBG_PRINT(GREEN">>>>>>>>>>> msgQueue_put_msg,source=%d,target=%d,mq->inGlobeQueue=%d,mq->queueLen=%d,msg_d->code=%d\n"NONE,source,mq->handle,mq->inGlobeQueue,mq->queueLen,msg_d->code);
	else
		DBG_PRINT(GREEN">>>>>!!!!!!>>>>>> msgQueue_put_msg,source=%d,target=%d,mq->inGlobeQueue=%d,mq->queueLen=%d,msg_d = NULL\n"NONE,source,mq->handle,mq->inGlobeQueue,mq->queueLen);		
	msg = msgQueue_create_msg();
	if (msg!=NULL){
		pthread_mutex_lock(&mq->mutex);
		msg->source  = source;
		msg->msgData = msg_d;
		msg->next    = NULL;
		if (mq->head == NULL){
			mq->head = mq->tail = msg;
			mq->queueLen = 1;
		}
		else{
			mq->tail->next = msg;
			mq->tail       = msg;
			mq->queueLen ++;
		}
		if(mq->inGlobeQueue==FALSE)
			gMsgQueue_put_msgQueue(mq);   //when get a new message mq should be put into the globe queue
		pthread_mutex_unlock(&mq->mutex);
		return ;
	}else{
		//int i;
		//i = 1/0;		
		return ;
	}
		
}

//get a msg from the message queue
struct msgQueue_msg* msgQueue_get_msg(struct msgQueue* mq){
	pthread_mutex_lock(&mq->mutex);
	struct msgQueue_msg* result = mq->head;
	if (result != NULL){
		DBG_PRINT(GREEN"<<<<<<<<<<< msgQueue_get_msg,source=%d,target=%d,mq->inGlobeQueue=%d,mq->queueLen=%d,msg_d->code=%d\n"NONE,result->source,mq->handle,mq->inGlobeQueue,mq->queueLen,result->msgData->code);
		mq->head     = mq->head->next;
		result->next = NULL;
		mq->queueLen --;
		if (mq->tail==result) mq->tail = NULL;
		if(mq->head != NULL )
			gMsgQueue_put_msgQueue(mq);   //if mq is not empty, it should be put back into the globe queue
	}else{
		mq->tail = NULL;
		mq->queueLen = 0;
	}
	pthread_mutex_unlock(&mq->mutex);
	if (result == NULL || mq->head == NULL){
		DBG_PRINT(GREEN"<<<<<<<<<<< msgQueue_get_msg,target=%d,mq->inGlobeQueue=%d,mq->queueLen=%d\n"NONE,mq->handle,mq->inGlobeQueue,mq->queueLen);
		if (mq->parentService != NULL && mq->parentService->type == S_TYPE_ONCE){
			service_release(mq->parentService);
		}
	}
	return result;
}

//dispatch a message, need call the cbFunc defined in the mq
void msgQueue_dispatch_message(int srcHandle,int tarHandle,struct msgQueue* mq, struct message_data * msg_d){

}

//create a message
struct msgQueue_msg* msgQueue_create_msg(){
	struct msgQueue_msg* msg = malloc(sizeof(struct msgQueue_msg));
	if(msg == NULL) return NULL;
	msg->source = -1;
	msg->target = -1;
	msg->next   = NULL;
	if(msg != NULL)
		DBG_PRINT("msgQueue_create_msg msg=%p,msglen=%d\n",msg,sizeof(struct msgQueue_msg));
	return msg;
}

//create a message data
struct message_data* msgQueue_create_msgData(){
	struct message_data* msg_d = malloc(sizeof(struct message_data));
	if (msg_d == NULL){
		return NULL;
	}
	msg_d->data = NULL;
}

//release a msg
void msgQueue_release_msg(struct msgQueue_msg* msg){
	if(msg != NULL){
		if(msg->msgData != NULL){
			if(msg->msgData->data !=NULL){
				free(msg->msgData->data);
				msg->msgData->data = NULL;
			}
			free(msg->msgData); 
			msg->msgData = NULL;
		}
		free(msg);
		msg = NULL;
	}
}

//put a message queue at the end of gMsgQueue 
void gMsgQueue_put_msgQueue (struct msgQueue *mq){
	if (mq!=NULL){
		if (mq->inGlobeQueue==TRUE) return;
		DBG_PRINT("++++++++++++++++++++ gMsgQueue_put_msgQueue,gMQ.queueLen=%d,handle=%d\n",gMQ.queueLen,mq->handle);
		pthread_mutex_lock(&gMQ.mutex);
		mq->next    	 = NULL;
		mq->inGlobeQueue = TRUE;
		if (gMQ.head == NULL){
			gMQ.head = gMQ.tail = mq;
			gMQ.queueLen++;
		}
		else{
			gMQ.tail->next = mq;
			gMQ.tail 	   = mq;
			gMQ.queueLen++;
		}
		pthread_mutex_unlock(&gMQ.mutex);
		if (gMQ.queueLen==1)
			mysysenv_resumework(); //resume the work thread
	}
}

//get a message queue from the head of gMsgQueue
struct msgQueue* gMsgQueue_get_msgQueue(){
	pthread_mutex_lock(&gMQ.mutex);
	struct msgQueue* result = gMQ.head;
	if (result != NULL){
		DBG_PRINT("-------------------- gMsgQueue_get_msgQueue,gMQ.queueLen=%d,handle=%d\n",gMQ.queueLen,result->handle);
		result->inGlobeQueue = FALSE;
		gMQ.head = gMQ.head->next;
		if (gMQ.tail==result) gMQ.tail = NULL;
		result->next = NULL;
		gMQ.queueLen--;
	}else{
		gMQ.tail = NULL;
		gMQ.queueLen = 0;
	}
	pthread_mutex_unlock(&gMQ.mutex);
	return result;
}

//create a message queue
struct msgQueue* gMsgQueue_create_msgQueue(){
	struct msgQueue* mq = NULL;
	mq = malloc(sizeof(struct msgQueue));
	return mq;
}

//release a message queue
void gMsgQueue_release_msgQueue(struct msgQueue* mq){
	if(mq != NULL){
		pthread_mutex_lock(&mq->mutex);
		while(1){
			struct msgQueue_msg* tempMsg = mq->head;
			if (tempMsg != NULL){
				DBG_PRINT(GREEN"gMsgQueue_release_msgQueue,source=%d,target=%d,mq->inGlobeQueue=%d,mq->queueLen=%d,msg_d->code=%d\n"NONE,tempMsg->source,mq->handle,mq->inGlobeQueue,mq->queueLen,tempMsg->msgData->code);
				mq->head      = mq->head->next;
				tempMsg->next = NULL;
				mq->queueLen --;
				msgQueue_release_msg(tempMsg);
			}else{
				break;
			}
		}	
		pthread_mutex_unlock(&mq->mutex);
		if (mq->handle > 10000 || mq->handle<-10000 || mq->handle == 0){
			DBG_PRINT_2(L_GREEN"====gMsgQueue_release_msgQueue ==mq=%p,mq-2=%x,mq=%x,mq->handle=%d===========\n"NONE,mq,*((int*)mq-2),*((int*)mq),mq->handle);
			//int i=1;
			//i = i/0;
		}
		//else
			free(mq); //for debug

		//free(mq);  //for debug
		DBG_PRINT("gMsgQueue_init_msgQueue --- \n");
		mq = NULL;
	}
}

//init a message queue
void gMsgQueue_init_msgQueue(struct msgQueue* mq,int handle,cbFunc cb,char* name,struct serviceInfo* service){
	DBG_PRINT("gMsgQueue_init_msgQueue mq=%p\n",mq);
	if (mq != NULL){
		mq->head = mq->tail = NULL;
		mq->handle 		 = handle;
		mq->cb     		 = cb;
		mq->inGlobeQueue = FALSE;
		if(strlen(name)<MAX_NAME_LEN)
			sprintf(mq->name, "%s" , name);
		else{
			DBG_PRINT("gMsgQueue_init_msgQueue,para name lenght should less then %d,name = %s\n",MAX_NAME_LEN ,name);
			strncpy(mq->name,name,MAX_NAME_LEN-1);
		}
		mq->parentService = service;
		pthread_mutex_init(&mq->mutex,NULL);
	}
}