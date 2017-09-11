#include <unistd.h>
#include <stdio.h>
#include<stdlib.h>
#include<errno.h>

int pwd(){
    char cwd[2048];
    if(getcwd(cwd, sizeof(cwd)))
        printf("Current working dir: %s\n", cwd);
    else{
        perror("pwd error");
    }
    return 0;
}
