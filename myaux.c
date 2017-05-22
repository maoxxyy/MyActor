#include "myaux.h"

void my_trace(const char *cmd, ...)  
{  
    printf("%s %s ", __DATE__, __TIME__); 
    /* 
    va_list args;       //定义一个va_list类型的变量，用来储存单个参数  
    va_start(args,cmd); //使args指向可变参数的第一个参数  
    vprintf(cmd,args);  //必须用vprintf等带V的  
    va_end(args);       //结束可变参数的获取  
    //*/
}  
void dumyFnc(const char * _Format, ...){

}
