#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include<fcntl.h>
#include <unistd.h>
#include<string.h>
int dirty();
int interrupts(int len1);
int print_first();
int nightswatch(char *tokenlist[],int index){
    struct termios initial_settings,new_settings;
    int n,t=2,i,len;
    char timer[3];
    unsigned char key;
    if(index > 4){
        printf("nightswatch error: excess arguments given");
        return 0;
    }
    if(index==3 || index==1){
        printf("nightswatch error: more arguments expected");
        return 0;
    }
    if(index==4){
        strcpy(timer,tokenlist[2]);
        t = atoi(timer);
    }
    tcgetattr(0,&initial_settings);
    new_settings = initial_settings;
    new_settings.c_lflag &= ~ICANON;
    new_settings.c_lflag &= ~ECHO;
    new_settings.c_lflag &= ~ISIG;
    new_settings.c_cc[VMIN] = 0;
    new_settings.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &new_settings);
    if(strcmp(tokenlist[index-1],"interrupt")==0)
        len = print_first();
    while(1){
        n = getchar();
        if(n != EOF){
            key = n;
            if(key == 'q'){
            break;
            }
        }
        if(strcmp("dirty",tokenlist[index-1])==0)
            dirty();
        else
            interrupts(len);
        sleep(1);
        fflush(stdout);        
    }
    tcsetattr(0, TCSANOW, &initial_settings);
    return 0;
}

int dirty(){
    char path[]= "/proc/meminfo";
    FILE *fp;
    char *line,*token,*tokenlist[4];
    char buf[1000];
    int index = 0,i;
    ssize_t len = 0;
    fp = fopen(path,"r");
    if(fp ==NULL)
        perror("nightswatch error");
    for(i=0;i<17;i++){
        getline(&line,&len,fp);
    }
    strcpy(buf,line);
    token = strtok(buf," \n");
        while(token!=NULL){
        tokenlist[index] = token;
        token = strtok(NULL," \n");
        index++;
    }
    tokenlist[index]=NULL;
    printf("%s kb\n",tokenlist[1]);
    fclose(fp);
    return 0;
}
int print_first(){
    char path[]= "/proc/interrupts";
    FILE *fp;
    char *line,*token,*tokenlist[10];
    char buf[1000];
    int index = 0,i;
    ssize_t len = 0;
    fp = fopen(path,"r");
    if(fp ==NULL)
        perror("nightswatch error");
    getline(&line,&len,fp);
    strcpy(buf,line);
    token = strtok(buf," \t\r\a\n");
    while(token!=NULL){
        tokenlist[index] = token;
        token = strtok(NULL," \t\r\a\n");
        index++;
    }
    tokenlist[index]=NULL;
    for(i=0;i<index;i++)
        printf("%s\t",tokenlist[i]);
    printf("\n");
    fclose(fp);
    return index;   
}
int interrupts(int len1){
    char path[]= "/proc/interrupts";
    FILE *fp;
    char *line,*token,*tokenlist[10];
    char buf[1000];
    int index = 0,i;
    ssize_t len = 0;
    fp = fopen(path,"r");
    if(fp ==NULL)
        perror("nightswatch error");
    for(i=0;i<3;i++){
        getline(&line,&len,fp);
    }
    strcpy(buf,line);
    token = strtok(buf," \t\r\a\n");
        while(token!=NULL){
        tokenlist[index] = token;
        token = strtok(NULL," \t\r\a\n");
        index++;
    }
    tokenlist[index]=NULL;
    for(i=1;i<=len1;i++)
        printf("%s\t",tokenlist[i]);
    printf("\n");
    return 0;   
}