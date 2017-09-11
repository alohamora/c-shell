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

int ls(char **tokenlist,int num)
{
        char cwdnm[100];
        if(getcwd(cwdnm,100)==NULL)
        {
             perror("ls Error");
                return 0;
        }
        // DIR* pdir=opendir(cwdnm);
        struct dirent** nmlist;
        struct stat filedet;
        struct passwd* pw;
        struct group* grp ;
        struct tm* tmdet;
        int n;
        // if( (n= scandir(cwdnm,&nmlist,NULL,alphasort)) == -1)
        //         return 0;

        // int i;
        // printf("%s",nmlist[0]->d_name);
        // scanf("%[^\n]s",cmdstr);
        // char* wrd = strtok(cmdstr," ");
        // if(wrd == NULL)
        // {
        //         perror(Error: No command entered);
        //         return (-1);
        // }
        // while(wrd != NULL)
        // {
        //         argvs[i++]=wrd;
        //         wrd = strtok(NULL," ");
        // }
        int i=0,j=0,flaga=0,flagl=0;
        int dflag=0,idx;
        for(j=1;j<num;j++)
        {
                if(tokenlist[j][0] != '-')
                {
                        dflag=1;
                        idx=j;
                }
                int k=0;
                for(;k<strlen(tokenlist[j]);k++)
                {
                        if(tokenlist[j][k]=='a' && tokenlist[j][0] == '-')
                                flaga=1;
                        if(tokenlist[j][k]=='l' && tokenlist[j][0] == '-')
                                flagl=1;
                }
        }
        if(dflag == 1)
        {
                if( (n = scandir(tokenlist[idx],&nmlist,NULL,alphasort)) == -1)
               {
                    perror("ls Error");
                    return 0;
               }
        }
        else
        {
                if( (n = scandir(cwdnm,&nmlist,NULL,alphasort)) == -1)
                {
                         perror("ls Error");
                        return 0;
                }

        }
        // printf("flagl is %d  and flaga is %d\n",flagl,flaga);
        // for(i=0;i<n;i++)
        //         printf("\n%s\n",nmlist[i]->d_name);
        i=0;
        if((!flaga && !flagl) || (!flagl && flaga))
        {
        for(i=0;i<n;i++)
        {
                int fd;
                if (dflag)
                {
                        char opstr[1000];
                        sprintf(opstr, "%s/%s",tokenlist[idx],nmlist[i]->d_name);
                        if ((fd = open(opstr,O_RDONLY)) == -1)
                        {
                                perror("ls Error ");
                                return 0;
                         }
                }
                else{
                if ((fd = open(nmlist[i]->d_name,O_RDONLY)) == -1)
                {
                        perror("ls Error");
                        return 0;
                }
                }
                if (fstat(fd,&filedet) == -1)
                {
                        perror("ls Error");
                        return 0;
                }
                else{
                char dirnm[100];
                strcpy(dirnm,nmlist[i]->d_name);
                if((!flaga && !flagl && (dirnm[0] != '.')) || (!flagl && flaga))
                {
                    if(!S_ISDIR(filedet.st_mode))
                        printf("%s ",nmlist[i]->d_name);
                    else
                        printf("\033[1;36m%s\033[0m ",nmlist[i]->d_name);

                }
                }
        }
        printf("\n");
        }
        else
        {
        for(i=0;i<n;i++)
        {
                int fd;
                if (dflag)
                {
                        char opstr[1000];
                        sprintf(opstr, "%s/%s",tokenlist[idx],nmlist[i]->d_name);
                        if ((fd = open(opstr,O_RDONLY)) == -1)
                        {
                                perror("ls Error");
                                return 0;
                        } 
               }
                else{
                if ((fd = open(nmlist[i]->d_name,O_RDONLY)) == -1)
                {
                        perror("ls Error");
                        return 0;
                }
                }
                char dirnm[100];
                strcpy(dirnm,nmlist[i]->d_name);
                if ((flaga && flagl) || (flaga == 0 && flagl==1 && (dirnm[0]!='.') && (strcmp(nmlist[i]->d_name,".."))))
                {
                if (fstat(fd,&filedet) == -1)
                {
                        perror("ls Error");
                        return 0;
                }
                char tstr[30];
                off_t samstr = filedet.st_size;
                pw = getpwuid(filedet.st_uid);
                grp = getgrgid(filedet.st_gid);
                tmdet = localtime(&(filedet.st_mtime));
                strftime(tstr,30,"%b %d %H:%M",tmdet);
                printf((S_ISDIR(filedet.st_mode)? "d":"-"));
                printf((filedet.st_mode & S_IRUSR)? "r":"-");
                printf((filedet.st_mode & S_IWUSR)? "w":"-");
                printf((filedet.st_mode & S_IXUSR)? "x":"-");
                printf((filedet.st_mode & S_IRGRP)? "r":"-");
                printf((filedet.st_mode & S_IWGRP)? "w":"-");
                printf((filedet.st_mode & S_IXGRP)? "x":"-");
                printf((filedet.st_mode & S_IROTH)? "r":"-");
                printf((filedet.st_mode & S_IWOTH)? "w":"-");
                printf((filedet.st_mode & S_IXOTH)? "x":"-");
                int arg1=filedet.st_nlink,arg2=filedet.st_size;
                if(!S_ISDIR(filedet.st_mode))
                    printf(" %d %s %s %d %s %s",arg1,pw->pw_name,grp->gr_name,arg2,tstr,nmlist[i]->d_name);
                else
                    printf(" %d %s %s %d %s \033[1;36m%s\033[0m",arg1,pw->pw_name,grp->gr_name,arg2,tstr,nmlist[i]->d_name);
                printf("\n");
                }
        }
        printf("\n");
        }
        return 0;
}
