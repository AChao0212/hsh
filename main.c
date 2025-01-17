#include "hsh.h"

char welcome[] = "============================\n"
                 "Welcome to Hao's shell (hsh)\n"
                 "============================\n"
                 "Type 'help' to see the available commands\n";
char userpath[COMMAND_MAX_LEN], passwdpath[COMMAND_MAX_LEN], datapath[COMMAND_MAX_LEN];

bool check_first_time() {
    int fd = open(userpath, O_RDONLY);
    if (fd < 0) {
        return true;
    }
    if (lseek(fd, 0, SEEK_END) == 0) {
        close(fd);
        return true;
    }
    close(fd);

    fd = open(passwdpath, O_RDONLY);
    if (fd < 0) {
        return true;
    }
    if (lseek(fd, 0, SEEK_END) == 0) {
        close(fd);
        return true;
    }
    close(fd);

    return false;
}

void setup(){
    char input[COMMAND_MAX_LEN];
    write(STDOUT_FILENO, "Please enter your username (example: b10902099): ", 49);
    fgets(input, COMMAND_MAX_LEN, stdin);
    input[strcspn(input, "\n")] = '\0';
    int fd = open(userpath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    fcntl(fd, F_SETLK, F_WRLCK);
    write(fd, input, strlen(input) + 1);
    fcntl(fd, F_SETLK, F_UNLCK);
    close(fd);
    write(STDOUT_FILENO, "Please enter your password: ", 28);
    fgets(input, COMMAND_MAX_LEN, stdin);
    input[strcspn(input, "\n")] = '\0';
    fd = open(passwdpath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    fcntl(fd, F_SETLK, F_WRLCK);
    write(fd, input, strlen(input) + 1);
    fcntl(fd, F_SETLK, F_UNLCK);
    close(fd);
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    write(STDOUT_FILENO, welcome, strlen(welcome));
    head = NULL;
    tail = NULL;

    snprintf(userpath, COMMAND_MAX_LEN, "%s/.hsh/user", getenv("HOME"));
    snprintf(passwdpath, COMMAND_MAX_LEN, "%s/.hsh/password", getenv("HOME"));
    snprintf(datapath, COMMAND_MAX_LEN, "%s/.hsh", getenv("HOME"));
    
    if(check_first_time()) {
        setup();
    }

    while (1) {
        getcwd(cwd, COMMAND_MAX_LEN);
        snprintf(output, COMMAND_MAX_LEN, "hsh [%s] > ", cwd);
        write(STDOUT_FILENO, output, strlen(output));
        if (!fgets(command, COMMAND_MAX_LEN, stdin)) {
            perror("fgets");
            break; 
        }

        command[strcspn(command, "\n")] = '\0';
        int handled = 0;
        for (int i = 0; i < FUNCTION_MAX_TYPE; i++) {
            if (strncmp(command, func_mappings[i].name, strlen(func_mappings[i].name)) == 0) {
                func_mappings[i].func(command);
                handled = 1;
                break;
            }
        }

        if (!handled) {
            execute(command);
            system(command);
            //snprintf(errmsg, COMMAND_MAX_LEN, "Unknown command: %s\n", command);
            //write(STDOUT_FILENO, errmsg, strlen(errmsg));
            //errmsg[0] = '\0';
        }
        else if (errmsg[0] != '\0') {
            write(STDOUT_FILENO, errmsg, strlen(errmsg));
            errmsg[0] = '\0';
        }
        memset(output, 0, COMMAND_MAX_LEN);
    }

    return 0;
}