#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>

int echo_quote(char buf[]){
    int len,i=0,j,k=0,end,start = 0;
    char quote,temp[10000];
    len = strlen(buf);
    end = len - 1;
    while(buf[start]==' ' || buf[start]=='\t'){
        start++;
    }
    start+=4;
    while(buf[start]==' ' || buf[start]=='\t'){
        start++;
    }
    // printf("%c",buf[start]);
    while(buf[end]==' ' || buf[end]=='\t')
        end--;
    // printf("%c",buf[end]);
    if(buf[start]=='\'' || buf[start]=='\"'){
        if(buf[end]!=buf[start]){
            printf("echo : string invalid\n");
            return 0;
        }
        else{
            quote = buf[start];
            for(j=start;j<=end;j++){
                if(buf[j]!=quote){  
                    temp[k] = buf[j];
                    k++;
                }
            }
            temp[k]='\0';
        }
        printf("%s\n",temp);
    }
    else    
        printf("%s\n",&buf[i]);
    return 0;
}

int echo(char **tokenlist,int index){
    int i;
    for(i=1;i<index;i++)
        printf("%s ",tokenlist[i]);
    printf("\n");
    return 0;
}
