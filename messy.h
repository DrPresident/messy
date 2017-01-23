#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

char** get_members(const char*);
char** get_channels(const char*);

char* _messychanserv;

typedef struct channel{
    char * name;
    char ** members;
    int num_members;
} channel;

void free_channel(channel* c){
    for(int i = 0; i < numMembers; i++)
        free(members[i]);
    free(name);
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
    
    FILE* file = fopen(_messychanserv,"a"); 
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
    _messychansserv = malloc(sizeof(char) * len);
    
    for(int i = 0; i <= len; i++)
        _messychansserv[i] = chanserv[i];

    if(access("/tmp/messychans", F_OK) != 0){
        messychans = creat("/tmp/messychans", O_CREAT);
        close(messychans);
    }

    return 0;
}

