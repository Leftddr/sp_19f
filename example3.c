#include<sys/stat.h>
#include<dirent.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

static int indent=0;
static int filecount=0;
static int dircount=0;
#define max 100
#define buf 128
#define block_size 4194304

char result_name[max][buf];
int result_size[max];
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
        struct stat sb;

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
        else{
            if(stat(items[i]->d_name,&sb)==-1){perror("stat"); return 1;}
            strcpy(result_name[filecount],items[i]->d_name);
            if(sb.st_blocks%block_size==0){
                result_size[filecount++]=sb.st_blocks/block_size;
            }else{
                result_size[filecount++]=(sb.st_blocks/block_size)+1;
            }
        }     
    }
    indent--;
    chdir("..");
}

void sort(){
    for(int i=0;i<filecount-1;i++){
        for(int j=i+1;j<filecount;j++){
            if(result_size[i]<result_size[j]){
                int temp=result_size[i];
                result_size[i]=result_size[j];
                result_size[j]=temp;
                char ch[buf];
                strcpy(ch,result_name[i]);
                strcpy(result_name[i],result_name[j]);
                strcpy(result_name[j],ch);
            }
        }
    }
}

int main(int argc, char *argv[]){
    Scandir(argv[1],myfunc,0);
    for(int i=0;i<filecount;i++){
        printf("%s\n",result_name[i]);
    }
}