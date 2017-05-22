#ifndef  MYSYSENV_H
  #define MYSYSENV_H

#define WORK_THREAD_NUM 8

static pthread_mutex_t sysenv_mutex;
static pthread_cond_t  sysenv_cond;

//sys env init func
void mysysenv_init();

//start the sys work thread
void mysysenv_startwork();

//resume the work thread
void mysysenv_resumework();

#endif