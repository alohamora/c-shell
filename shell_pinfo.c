#include "stdio.h"
#include "unistd.h"
#include "string.h"
#include "stdlib.h"
#include "sys/types.h"
#include "dirent.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "pwd.h"
#include "grp.h"
#include "time.h"

int pinfo(char **tokenlist)
{
        char randstr[100],readstr[10004],randstr1[100],readstr1[1000];
        char *argvs[100],*argvs1[100];
        int pid =getpid();
        if(tokenlist[1]==NULL)
        {
                sprintf(randstr,"/proc/%d/stat",pid);
                sprintf(randstr1,"/proc/%d/exe",pid);
        }
        else
        {
                sprintf(randstr,"/proc/%s/stat",tokenlist[1]);
                sprintf(randstr1,"/proc/%s/exe",tokenlist[1]);
        }
        int fd = open(randstr,O_RDONLY);
        int fd1 = open(randstr1,O_RDONLY);
        int numbyte = read(fd,readstr,10000);
        int numbyte1 = readlink(randstr1,readstr1,1000);
        char * wrd = strtok(readstr," ");
        int i=0;
        while(wrd!=NULL)
        {
                argvs[i++]=wrd;
                wrd=strtok(NULL," ");
        }
        printf("pid:\t\t %s\nProcess Status:  %s\nMemory: \t %s\nExecutable Path: %s\n",argvs[0],argvs[2],argvs[22],readstr1);
        return 0;
}
