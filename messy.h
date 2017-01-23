#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

char** get_members(const char*);

typedef struct channel{
    char * name;
    char ** members;
    int num_members;
} channel;

typedef struct messysession{
    char* username;
    char* chanserv;
    char* chanservpath;
    channel* chan;
} messysession;

messysession *messy;

void free_channel(channel* c){
    for(int i = 0; i < c->num_members; i++)
        free(c->members[i]);
    free(c->name);
    free(c);
}

channel* get_channels(const char* chanserv){

    return 0;
}

int fsize(FILE *f){
    fpos_t pos; 
    fgetpos(f,&pos); 

    fseek(f,0,SEEK_END);
    int size = ftell(f);
    fsetpos(f,&pos);

    return size;
}

channel* make_channel(const char* chan){
    
    FILE* file = fopen(messy->chanservpath,"a"); 
    channel *c = malloc(sizeof(channel));

    c->num_members = 0;
    c->members = NULL;
    c->name = malloc(sizeof(char) * strlen(chan));
    strcpy(c->name, chan);

    fprintf(file, "%s\n", c->name);

    fclose(file);

    return c;
}

int join_channel(const char* chan){

    char *path = malloc(sizeof(char) * (strlen(chan) + 6)); 
    path = strcat(path, "/tmp/");
    path = strcat(path, chan);

    free(path);
    return 0;
}

int init_messy(const char* chanserv){

    //if messychans does not exist, create it
    int len = strlen(chanserv);
    messy = malloc(sizeof(messysession));
    messy->username = NULL;
    messy->chan = NULL;
    messy->chanserv = malloc(sizeof(char) * (strlen(chanserv) + 1));
    messy->chanservpath = malloc(sizeof(char) * (strlen(chanserv) + 7));

    int i; 
    for(i = 0; chanserv[i] != '\0'; i++)
        messy->chanserv[i] = chanserv[i];
    messy->chanserv[i] = '\0';

    messy->chanservpath[0] = '\0';
    strcat(messy->chanservpath, "/tmp/");
    for(i = 0; chanserv[i] != '\0'; i++)
        messy->chanservpath[i + 5] = chanserv[i];
    messy->chanservpath[i] = '\0';
    
    printf("chanserv: %s\nchanservpath: %s\n", 
            messy->chanserv, messy->chanservpath);

    if(access(messy->chanservpath, F_OK) != 0){
        printf("creating chanserv: %s\n", messy->chanservpath);
        int messychans = creat(messy->chanservpath, O_CREAT);
        close(messychans);
    }

    return 0;
}

void cleanup_messy(){

}

