#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>

int bg(char **tokenlist){
    pid_t pid;
    pid = fork();
    int status;
    if(pid < 0)
        perror("shell error");
    else if(pid==0){
        if(execvp(tokenlist[0],tokenlist) == -1)
            perror("shell error");
    }
    else{
        printf("pid : %d\n",pid);
    }
    return pid;
}