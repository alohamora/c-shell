#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>

int cd(char *string,char *home){
    int ret,index;
    char path[2048],pre[2048],tilda[1024];
    strcpy(path,string);
    strcpy(pre,home);
    // printf("%s",path);
    if(path[0]==0)
        ret = chdir(pre);
    else if(path[0] == '~'){
        strcat(pre,&path[1]);
        ret = chdir(pre);
    }
    else if(path[0] == '/')
        ret = chdir(path);
    else{
        if(path[0] == '.')
            ret = chdir(path);
        else{
            getcwd(tilda,1024);
            strcat(tilda,path);
            ret = chdir(path);            
        }
    }
    if(ret != 0)
        perror("cd error");
    return 0;
}
