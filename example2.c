#include<sys/stat.h>
#include<dirent.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

static int indent=0;
static int filecount=0;
static int dircount=0;

char cdir[256];
void myfunc(char *file){
    printf("%s%s\n",getcwd(cdir,256),file);
}

void Scandir(char *wd, void(*func)(char*), int depth){
    struct dirent **items;
    int nitems,i;
    if(chdir(wd)<0){
        printf("dir : %s\n",wd);
        perror("chdir");
        exit(1);
    }
    nitems=scandir(".",&items,NULL,alphasort);

    for(int i=0;i<nitems;i++){

        struct stat fstat;

        if(!strcmp(items[i]->d_name,".")||(!strcmp(items[i]->d_name,".."))){
            continue;
        }
        func(items[i]->d_name);

        lstat(items[i]->d_name,&fstat);
        if((fstat.st_mode & S_IFDIR)==S_IFDIR){
            dircount++;
            if(indent<(depth-1) || depth==0){
                indent++;
                Scandir(items[i]->d_name,func,depth);
            }
        }
        else filecount++;
    }
    indent--;
    chdir("..");
}

int main(int argc, char *argv[]){
    Scandir(argv[1],myfunc,0);
    printf("filecount = %d\n",filecount);
    printf("dircount = %d\n",dircount);
}