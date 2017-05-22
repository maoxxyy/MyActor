An actor program system. With the help of some Skynet ideas, write the code, just for study now.
Skynet: https://github.com/cloudwu/skynet

MyActor is small scale demo for actor program system. 

You can design system besed on MyActor. In MyActor all the work is done by many service, and service communicated by message.
An service is a module code with a standalone message queue.  Same module with different message queue is different service.
All the service message queue which is not empty are managed by a Globe message queue. 
If an service message queue is empty it will be put aside or it will be put back at the end of Globe message queue. If an service with empty message queue received a message, it will be put back at the end of Globe message queue too.


test.c is the demo of how to use MyActor.

When you need create a service, code like this
  service = service_create(name, cb, NULL, NULL, NULL,S_TYPE_FOREVER);

When you need send a message, code like this
  send_msg_byName(source,name,new_msg_data);



How to build:
   gcc  myqueue.c myhandle.c myservice.c mysysenv.c test.c myaux.c -o test -lpthread -w -g

Run the test:
   ./test

Know problem:
   Have some problem with mem free.