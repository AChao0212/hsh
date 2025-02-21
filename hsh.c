#include "hsh.h"

char helpmsg[] = "\n\nAvailable commands:\n"
                 "sdf (Will implement this func later)\n\n"

                 "fws <workstation number>\n"
                 "====can connect to workstation fast\n"
                 "!!!!WARNING: this command will NOT work for the first time\n"
                 "!!!!         when you connect to a new workstation\n"
                 "====example: /fws ws3/ or /fws 3/ can connect to ws3\n"
                 "====         (for meow1 or meow2, use /fws meow1/ or /fws meow2/)\n"
                 "====flags:\n"
                 "==== -p: set password\n"
                 "==== -u: set username\n"
                 "====example: /fws -p 123456789/ can set password\n\n"

                 "fgit <commit message>\n"
                 "====can add, commit and push to git fast\n\n"

                 "rmhsh\n"
                 "====remove hsh\n\n"

                 "cd <directory>\n"
                 "====change directory\n\n"

                 "exit\n"
                 "====exit hsh\n"
                 "====use -k to kill all running processes\n\n"
                 
                 "other commands will be executed by system\n\n\n";

function func_mappings[] = {
    {"sdf", self_define},
    {"fws", fast_connect_workstation},
    {"fgit", fast_push_to_git},
    {"help", help},
    {"rmhsh", remove_hsh},
    {"cd", change_directory},
    {"exit", exit_shell}
};

void self_define(char *command) {
    //char *args = strtok(command, " ");
    snprintf(output, COMMAND_MAX_LEN, "%s: Will implement this function later.\n", command);
    write(STDOUT_FILENO, output, strlen(output));
}

void fast_connect_workstation(char *command) {
    char workstation[6] = {"???\0"};
    char *args = strtok(command, " ");
    args = strtok(NULL, " ");
    int cpwd = -1, cuser = -1, num = 1;
    while(args != NULL) {
        if (((args[0] - '0') > 0) && ((args[0] - '0') < 8)) {
            strcpy(workstation, "ws");
            workstation[2] = args[0];
            workstation[3] = '\0';
        }
        else if (args[0] == '-'){
            int flaglen = strlen(args);
            if(flaglen > 3 || flaglen < 2) {
                snprintf(errmsg, COMMAND_MAX_LEN, "fws: unknown flag\n");
                return;
            }
            for(int i = 1; i < flaglen; i++) {
                if(args[i] == 'p') {
                    cpwd = num;
                    num++;
                }
                else if(args[i] == 'u') {
                    cuser = num;
                    num++;
                }
                else {
                    snprintf(errmsg, COMMAND_MAX_LEN, "fws: unknown flag\n");
                    return;
                }
            }
        }
        else if (cpwd > 0 || cuser > 0){
            if((cuser > 0 && cuser < cpwd && cpwd > 0) || (cuser > 0 && cpwd < 0)) {
                cuser = -1;
                int fd = open(userpath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                fcntl(fd, F_SETLK, F_WRLCK);
                write(fd, args, strlen(args));
                fcntl(fd, F_SETLK, F_UNLCK);
                close(fd);
            }
            else {
                cpwd = -1;
                int fd = open(passwdpath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                fcntl(fd, F_SETLK, F_WRLCK);
                write(fd, args, strlen(args));
                fcntl(fd, F_SETLK, F_UNLCK);
                close(fd);
            }
        }
        else if (strncmp(args, "meow", 4) == 0){
            if((args[4] - '0') > 0 && (args[4] - '0') < 3) {
                strncpy(workstation, "meow", 4);
                workstation[4] = args[4];
                workstation[5] = '\0';
            }
            else {
                snprintf(errmsg, COMMAND_MAX_LEN, "fws: unknown workstation\n");
                return;
            }
        }
        else if(strncmp(args, "ws", 2) == 0) {
            if((args[2] - '0') > 0 && (args[2] - '0') < 8) {
                strcpy(workstation, "ws");
                workstation[2] = args[2];
                workstation[3] = '\0';
            }
            else {
                snprintf(errmsg, COMMAND_MAX_LEN, "fws: unknown workstation\n");
                return;
            }
        }
        else {
            snprintf(errmsg, COMMAND_MAX_LEN, "fws: unknown argument\n");
            return;
        }
        args = strtok(NULL, " ");
    }
    char address[40];
    char passwd[40];
    int fd = open(userpath, O_RDONLY);
    fcntl(fd, F_SETLK, F_RDLCK);
    read(fd, address, 40);
    fcntl(fd, F_SETLK, F_UNLCK);
    close(fd);
    fd = open(passwdpath, O_RDONLY);
    fcntl(fd, F_SETLK, F_RDLCK);
    read(fd, passwd, 40);
    fcntl(fd, F_SETLK, F_UNLCK);
    close(fd);
    strncat(address, "@\0", 2);
    strncat(address, workstation, ((workstation[0] == 'm')? 6 : 4));
    strcat(address, ".csie.ntu.edu.tw");
    char syscmd[COMMAND_MAX_LEN];
    snprintf(syscmd, COMMAND_MAX_LEN, "sshpass -p %s ssh %s", passwd, address);
    if(workstation[0] != '?') {
        system(syscmd);
    }
    else {
        snprintf(output, COMMAND_MAX_LEN, 
        "changed successfully\nnew address: %s\nnew password: %s\n"
        , address, passwd);
        write(STDOUT_FILENO, output, strlen(output));
    }
}

void fast_push_to_git(char *command) {
    char *args = strtok(command, " ");
    args = strtok(NULL, " ");
    if (args == NULL) {
        snprintf(errmsg, COMMAND_MAX_LEN, "fgit: missing argument\n");
        return;
    }
    char syscmd[COMMAND_MAX_LEN];
    snprintf(syscmd, COMMAND_MAX_LEN, "git add . && git commit -m \"%s\" && git push", args);
    system(syscmd);
}

void help(char *command) {
    char *args = strtok(command, " ");
    args = strtok(NULL, " ");
    if (args != NULL) {
        snprintf(errmsg, COMMAND_MAX_LEN, "help: too many arguments\n");
        return;
    }
    write(STDOUT_FILENO, helpmsg, strlen(helpmsg));
}

bool tryexec(char *command){
    if(command[0] == '~' && command[1] == '/') {
        char tmp[COMMAND_MAX_LEN];
        strcpy(tmp, getenv("HOME"));
        strcat(tmp, command + 1);
        strcpy(command, tmp);
    }
    int length = strlen(command), befflag;
    while(command[length] != '/')
        length--;
    command[length] = '\0';
    befflag = length + 1;
    while(command[befflag] != '\0' && command[befflag] != ' ')
        befflag++;
    command[befflag] = '\0';        
    struct dirent *entry;
    DIR *dp = opendir(command);
    if(dp == NULL){
        command[length] = '/', command[befflag] = ' ';
        snprintf(errmsg, COMMAND_MAX_LEN, "No such file or directory (@NO DIR)\n");
        return false;
    }
    while((entry = readdir(dp)) != NULL) {
        if(strcmp(entry->d_name, command + length + 1) == 0) {
            closedir(dp);
            command[length] = '/';
            struct stat st;
            stat(command, &st);
            uid_t uid = getuid();
            gid_t gid = getgid();
            bool permission = false;
            if(uid == st.st_uid) {
                if(st.st_mode & S_IRUSR) {
                    permission = true;
                }
            }
            if(gid == st.st_gid) {
                if(st.st_mode & S_IRGRP) {
                    permission = true;
                }
            }
            else{
                if(st.st_mode & S_IROTH) {
                    permission = true;
                }
                else {
                    snprintf(errmsg, COMMAND_MAX_LEN, "Permission denied\n");
                }
            }
            command[befflag] = ' ';
            return permission;
        }
    }
    closedir(dp);
    command[befflag] = ' ';
    snprintf(errmsg, COMMAND_MAX_LEN, "No such file or directory (@NO FILE)\n");
    return false;
}

void execute(char *command) {
    if(!tryexec(command)) {
        return;
    }
    char *args = strtok(command, " ");
    char execname[COMMAND_MAX_LEN];
    int i = strlen(args) - 1;
    while(command[i] != '/')
        i--;
    strcpy(execname, command + i + 1);
    pcb *new_pcb = (pcb *)malloc(sizeof(pcb));
    pcb_num++;
    strcpy(new_pcb->name, execname);
    char *argfc[100];
    i = 0;
    while(args != NULL) {
        argfc[i] = args;
        args = strtok(NULL, " ");
        i++;
    }
    argfc[i] = NULL;
    new_pcb->start_time = time(NULL);
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGCHLD);
    sigprocmask(SIG_BLOCK, &sigset, NULL);
    pid_t childpid = fork();
    if (childpid == 0) {
        sigprocmask(SIG_UNBLOCK, &sigset, NULL);
        execvp(argfc[0], argfc);
        exit(0);
    }
    else {
        new_pcb->pid = childpid;
        if (head == NULL) {
            head = new_pcb;
            tail = new_pcb;
        }
        else {
            tail->next = new_pcb;
            tail = new_pcb;
        }
        if(argfc[i - 1][0] != '&') {
            bool keep_waiting = true;
            while(keep_waiting) {
                sigset_t nothing;
                sigemptyset(&nothing);
                if(sigsuspend(&nothing) == -1 && errno == EINTR) {
                    pid_t returnpid = waitpid(-1, NULL, WNOHANG);
                    if(returnpid == childpid) {
                        keep_waiting = false;
                        sigprocmask(SIG_UNBLOCK, &sigset, NULL);
                    }
                    if(returnpid > 0) {
                        pcb *tmp = head, *prev = NULL;
                        while(tmp != NULL) {
                            if(tmp->pid == returnpid) {
                                time_t end_time = time(NULL);
                                snprintf(output, COMMAND_MAX_LEN,
                                "Process %s terminated. (pid: %d, runtime: %ld seconds)\n", 
                                tmp->name, tmp->pid, end_time - tmp->start_time);
                                write(STDOUT_FILENO, output, strlen(output));
                                if(prev == NULL) {
                                    head = tmp->next;
                                }
                                else {
                                    prev->next = tmp->next;
                                }
                                if(tmp == tail) {
                                    tail = prev;
                                }
                                free(tmp);
                                pcb_num--;
                                break;
                            }
                            prev = tmp;
                            tmp = tmp->next; 
                        }
                    }
                }
            }
        }
    }
}

void change_directory(char *command) {
    char *args = strtok(command, " ");
    args = strtok(NULL, " ");
    if(args[0] == '~') {
        char tmp[COMMAND_MAX_LEN];
        strcpy(tmp, getenv("HOME"));
        strcat(tmp, args + 1);
        args = tmp;
    }
    if (args == NULL) {
        snprintf(errmsg, COMMAND_MAX_LEN, "cd: missing argument\n");
        return;
    }
    if (chdir(args) == -1) {
        snprintf(errmsg, COMMAND_MAX_LEN, "cd: %s: %s\n", args, strerror(errno));
    }
}

void exit_shell(char *command) {
    char *args = strtok(command, " ");
    args = strtok(NULL, " ");
    if(args != NULL && strcmp(args, "-k") == 0) {
        args = strtok(NULL, " ");
        if(args != NULL && strcmp(args, "-r") != 0) {
            snprintf(errmsg, COMMAND_MAX_LEN, "exit: too many argument\n");
            return;
        }
        while(head != NULL) {
            kill(head->pid, SIGKILL);
            pcb *tmp = head;
            head = head->next;
            time_t end_time = time(NULL);
            snprintf(output, COMMAND_MAX_LEN,
            "Process %s terminated. (pid: %d, runtime: %ld seconds)\n",
            tmp->name, tmp->pid, end_time - tmp->start_time);
            free(tmp);
            pcb_num--;
        }
        if(args != NULL){
            return;
        }
    }
    else if(args != NULL) {
        snprintf(errmsg, COMMAND_MAX_LEN, "exit: unknown flag\n");
        return;
    }
    snprintf(output, COMMAND_MAX_LEN, "Exit gracefully.");
    write(STDOUT_FILENO, output, strlen(output));
    exit(0);
}

void remove_hsh(char *command) {
    char *args = strtok(command, " ");
    args = strtok(NULL, " ");
    if (args != NULL) {
        snprintf(errmsg, COMMAND_MAX_LEN, "rmhsh: too many arguments\n");
        return;
    }
    write(STDOUT_FILENO, "Are you sure to remove hsh? (y/n):", 35);
    char yn[5];
    fgets(yn, 5, stdin);
    if(yn[0] == 'y' || yn[0] == 'Y' || strcmp(yn, "yes") == 0) {
        printf("unlinking /usr/local/bin/hsh\n");
        system("sudo rm -f /usr/local/bin/hsh");
        printf("unlinking %s\n", datapath);
        system("rm -rf ~/.hsh");
        strncpy(command, "exit -k -r\0", 12);
        exit_shell(command);
        write(STDOUT_FILENO, "hsh removed.\n", 13);
        exit(0);
    }
}