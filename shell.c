#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<signal.h>
#include <termios.h>
#include<pwd.h>
#include "shell_cd.h"
#include "shell_pwd.h"
#include "shell_echo.h"
#include "shell_ls.h"
#include "shell_pinfo.h"
#include "foreground.h"
#include "background.h"
#include "nightswatch.h"
int arr[100],ind = 0,varindex,status[10]={0};
char name[100][100];
extern char **environ;
int printv(int varindex);
int builtin_fn(char **tokenlist,int index,char home[],char temp[]);
int exec_fn(char *tokenlist[],int index);
int tokenize(char **tokenlist,char *token,char buf[],char del[]);
int call_fn(char *tokenlist[],int index,char home[],char temp[],char *builtins[]);
void child_terminate();
int print_prompt(char home[],char *user);
int push_front(int index_arr);
int print_jobs();
void handler(){
    exit(0);
}
int main(){
    char *inp=NULL,*token,**tokenlist,home[1024],multiple;
    char *buf2,*buf,**tokenlist2,*temp;
    char del[] = ";\n";
    char del2[] = " \t\r\n;";
    int index,flag,i,index2,j,ret;
    char *builtins[] = {
        "cd","pwd","ls","pinfo","echo","nightswatch","setenv","printenv","unsetenv","jobs","kjob","fg","bg","overkill"
    };
    uid_t uid;
    struct passwd *n;
    uid = getuid();
    n = getpwuid(uid);
    varindex = 0;
    while(environ[varindex])   varindex++;
    getcwd(home,sizeof(home));
    ssize_t bufsize = 0;
    signal(SIGINT,handler);
    signal(SIGCHLD, child_terminate);
    while(1){
        ret = 0;
        flag = 0;
        index = 0;
        print_prompt(home,n->pw_name);
        getline(&inp, &bufsize, stdin);
        buf = (char *)malloc(sizeof(char)*1024);
        strcpy(buf,inp);
        if(strcmp(buf,"\n")==0)
            continue;
        tokenlist = (char **)malloc(sizeof(char *)*1024);
        index = tokenize(tokenlist,token,buf,del);
        for(i=0;i<index;i++){
            temp = (char *)malloc(sizeof(char)*1024);
            buf2 = (char *)malloc(sizeof(char)*1024);
            strcpy(buf2,tokenlist[i]);
            strcpy(temp,buf2);
            tokenlist2 = (char **)malloc(sizeof(char *)*1024);
            index2 = tokenize(tokenlist2,token,buf2,del2);
            ret = call_fn(tokenlist2,index2,home,temp,builtins);
            if(ret == -1)
                break;
            free(buf2);
            free(tokenlist2);
            free(temp);
        }
        // printf("%s",name[0]);                        
        //printf("%s",tokenlist[0]);                        
        if(ret==-1)
            break;
        free(buf);
    }
    return 0;
}

int tokenize(char *tokenlist[],char *token,char buf[],char del[]){
    int index = 0;
    token = strtok(buf,del);
    while(token!=NULL){
        tokenlist[index] = token;
        token = strtok(NULL,del);
        index++;
    }
    tokenlist[index]=NULL;
    return index;
}

int call_fn(char *tokenlist[],int index,char home[],char temp[],char *builtins[]){
    int flag = 0,j;
    if(tokenlist[index-1][0]=='&'){
        flag = 1;
        exec_fn(tokenlist,index);
    }
    if(flag==0){
        if(strcmp(tokenlist[0],"quit")==0){
            return -1;
        }
        for(j=0;j<14;j++){
            if(strcmp(tokenlist[0],builtins[j])==0){
                builtin_fn(tokenlist,index,home,temp);
                break;
            }
        }
        if(j==14)
            exec_fn(tokenlist,index);
    }
    return 0;
}

int exec_fn(char *tokenlist[],int index){
    int pid = 0,i;
    if(tokenlist[index-1][0] == '&'){
        tokenlist[index-1] = NULL;
        strcpy(name[ind],tokenlist[0]);
        for(i=1;i<index-1;i++){
            strcat(name[ind]," ");
            strcat(name[ind],tokenlist[i]);
        }
        pid = bg(tokenlist);
        arr[ind] = pid;
        ind++;
    }
    else{
        fg(tokenlist);
    }
    return 0;
}

int builtin_fn(char **tokenlist,int index,char home[],char temp[]){
    int i,j,length,flag = 0,pid,sig,tempindex;
    char *temp_str;
    pid_t fpid;
    if(strcmp(tokenlist[0],"cd")==0){
        if(index>2)
            printf("cd : excess arguments given");
        else{
            if(tokenlist[1]!=NULL)
                cd(tokenlist[1],home);
            else{
                tokenlist[1]="\0";
                cd(tokenlist[1],home);
            }
        }
    }
    else if(strcmp(tokenlist[0],"setenv")==0){
        if(index > 3 || index == 1 )    printf("setenv error: no of arguments given not correct\n");
        else{
            if(setenv(tokenlist[1],tokenlist[2],1)==0)
                varindex++;
            else    perror("setenv error");
        }
    }
    else if(strcmp(tokenlist[0],"unsetenv")==0){
        if(index != 2 )    printf("unsetenv error: no of arguments given not correct\n");
        else{
            temp_str = getenv(tokenlist[1]);          
            if(temp_str != NULL){
                if(unsetenv(tokenlist[1])==0)
                    varindex--;
                else perror("unsetenv error");
            }
            else{
                printf("unsetenv error: env variable not present\n");
            }
        }
    }
    else if(strcmp(tokenlist[0],"printenv")==0){
        if(index > 1) printf("printenv error: excess arguments given\n");
        else
            printv(varindex);
    }
    else if(strcmp(tokenlist[0],"jobs")==0){
        if(index != 1)  printf("jobs error: excess arguments given\n");
        else print_jobs();
    }
    else if(strcmp(tokenlist[0],"kjob")==0){
        if(index!=3)    printf("kjob error: wrong arguments given\n");
        else{
            j=1;
            pid = 0;
            sig = 0;
            for(i=strlen(tokenlist[1])-1;i>=0;i--){
                pid += j * (tokenlist[1][i]-48);
                j = j*10; 
            }
            j=1;
            for(i=strlen(tokenlist[2])-1;i>=0;i--){
                sig += j * (tokenlist[2][i]-48);
                j = j*10; 
            }
            if(pid <= ind){
                fpid = arr[pid-1];
                if(kill(fpid,sig)==-1)
                   perror("kjob error");
            }
            else
                printf("kjob error: process does not exist\n");
        }
    }
    else if(strcmp(tokenlist[0],"fg")==0){
        int j_no=0,j=1,status;
        if(index!=2){
            printf("fg error: wrong no of arguments given\n");
        }
        else{
            for(i=strlen(tokenlist[1])-1;i>=0;i--){
                j_no += j * (tokenlist[1][i] - 48);
                j = j * 10;
            }
            if(j_no <= ind){
                pid = arr[j_no-1];
                if(kill(pid,SIGCONT)==-1){
                    perror("fg error");
                }
                else{
                    printf("%s",name[j_no-1]);
                    push_front(j_no - 1);
                    while(wait(&status)!=pid);
                }
            }
            else{
                printf("fg error: process not found\n");
            }
        }
    }
    else if(strcmp(tokenlist[0],"bg")==0){
        int j_no=0,j=1;
        if(index!=2){
            printf("bg error: wrong no of arguments given\n");
        }
        else{
            for(i=strlen(tokenlist[1])-1;i>=0;i--){
                j_no += j * (tokenlist[1][i] - 48);
                j = j * 10;
            }
            if(j_no <= ind){
                pid = arr[j_no-1];
                if(kill(pid,SIGCONT)==-1){
                    perror("bg error");
                }
                else{
                    status[ j_no - 1] = 0;
                }
            }
            else{
                printf("bg error: process not found\n");
            }
        }
    }
    else if(strcmp(tokenlist[0],"overkill")==0){
        int i;
        for(i=0;i<ind;i++){
            kill(arr[i],9);
        }
    }
    else if(strcmp(tokenlist[0],"pwd")==0){
        if(index > 1)
            printf("pwd : excess arguments given");
        else
            pwd();
    }
    else if(strcmp(tokenlist[0],"ls")==0)
            ls(tokenlist,index);

    else if(strcmp(tokenlist[0],"echo")==0){
        for(i=1;i<index;i++){
            length = strlen(tokenlist[i]);
            for(j=0;j<length;j++)
                if(tokenlist[i][j] =='\"' || tokenlist[i][j]=='\''){
                    flag = 1;
                    break;
                }
        }
        if(flag!=1)
            echo(tokenlist,index);
        else
            echo_quote(temp);
    }
    else if(strcmp(tokenlist[0],"pinfo")==0)
        pinfo(tokenlist);
    else if(strcmp(tokenlist[0],"nightswatch")==0)
        nightswatch(tokenlist,index);
    return 0;
}

void child_terminate()
{
        union wait wstat;
        pid_t   pid;
        int index = ind;
        char *pname;
        while (1) {
            pid = wait3 (&wstat, WNOHANG, (struct rusage *)NULL );
            if (pid == 0)
                return;
            else if (pid == -1)
                return;
            else
            {
                int i,temp_stop = ind;
                for(i=0;i<ind;i++)
                {
                    if(arr[i]==pid){
                        pname = name[i];
                        index = i;
                    }
                } 
                fprintf (stderr,"%s with pid: %d terminated %s\n",pname,pid,(wstat.w_retcode==0)?"normally":"abnormally");
                push_front(index);
            }
        }
}

int printv(int varindex){
    int i=0;
    printf("loda");
    for(i=0;i<varindex;i++){
        printf("%s\n",environ[i]);
    }
    return 0;
}

int print_jobs(){
    int i;
    for(i=0;i<ind;i++){
        printf("[%d] ",i+1);
        if(status[i]==1)    printf("Stopped ");
        else    printf("Running ");
        printf("%s[%d]\n",name[i],arr[i]);
    }
    return 0;
}
int print_prompt(char home[],char *user){
    int index;
    char host[1024],cwd[1024];
    gethostname(host,1024);
    getcwd(cwd,1024);
    if(strstr(cwd,home)){
        index = strlen(home);
        cwd[index-1] = '~';
        printf("\033[1;32m<%s@%s\033[0m:\033[1;36m%s>\033[0m$ ",user,host,&cwd[index-1]);
    }
    else
        printf("\033[1;32m<%s@%s\033[0m:\033[1;36m%s>\033[0m$ ",user,host,cwd);
    return 0;
}

int push_front(int index_arr){
    int i,len;
    for(i=index_arr;i<ind-1;i++){
        arr[i] = arr[i+1];
        len = strlen(name[i+1]);
        if(strlen(name[i+1]) > strlen(name[i]))
            memmove(name[i],name[i+1],len);
        else{
            memmove(name[i],name[i+1],len);
            name[i][len]='\0';
        }
    }
    ind--;
}