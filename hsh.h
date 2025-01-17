#ifndef HSH_H
#define HSH_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>

#define COMMAND_MAX_LEN 1024
#define FUNCTION_MAX_TYPE 7

char command[COMMAND_MAX_LEN];
char cwd[COMMAND_MAX_LEN];
char output[COMMAND_MAX_LEN];
char errmsg[COMMAND_MAX_LEN];
extern char helpmsg[];

void self_define(char *command);
void fast_connect_workstation(char *command);
void fast_push_to_git(char *command);
void help(char *command);
void remove_hsh(char *command);
void change_directory(char *command);
void exit_shell(char *command);
void execute(char *command);

typedef struct {
    char *name;
    void (*func)(char *);
} function;

typedef struct PCB{
    char name[30];
    pid_t pid;
    time_t start_time;
    struct PCB *next;
} pcb;

extern function func_mappings[];
extern void sigchild_handler(int signo);
extern char userpath[COMMAND_MAX_LEN];
extern char passwdpath[COMMAND_MAX_LEN];
extern char datapath[COMMAND_MAX_LEN];

pcb *head, *tail;
int pcb_num;

#endif