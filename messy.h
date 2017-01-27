#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>

#define MAX_CHANS 100
#define MAX_MEMBERS 100

struct messy_network;

typedef struct messy_chan{
    char* name;
    char* path;
    struct messy_network* network;
    char** members;
    char* memfile;
    int num_members;
} messy_chan;

typedef struct messy_network{
    char* username;
    char* name;
    char* path;
    messy_chan** chans;
    int num_chans;
} messy_network;

void _make_dir(const char *path);
messy_network* _network;
char* _prefix = "/tmp/";
int _prefixlen = 5;
int _mem_size;

void _free_messy_chan(messy_chan* c){
    for(int i = 0; i < c->num_members; i++)
        free(c->members[i]);
    free(c->name);
    free(c);
}

messy_chan* messy_get_chanlist(const messy_network* network){

    FILE* file = fopen(network->path, "r");
    int num_chans = fgetc(file);
    messy_chan *list = malloc(sizeof(messy_chan) * num_chans);

    return 0;
}

int _fsize(FILE *f){
    fpos_t pos; 
    fgetpos(f,&pos); 

    fseek(f,0,SEEK_END);
    int size = ftell(f);
    fsetpos(f,&pos);

    return size;
}

int messy_send_network(const char* network){
    return 1;
}

int messy_send_chan(const char* chan){
    return 1;
}

int messy_send_user(const char* chan){
    return 1;
}

void messy_set_username(const char* name){
    if(name){
        int len = strlen(name);
        if(len == 0)
           perror("cannot set name to an empty string\n"); 
        else{
            _network->username = malloc(sizeof(char) * len + 1);
            strcpy(_network->username, name);
        }
    }
    else{
       perror("cannot set name to an null string\n"); 
    }
}

char** messy_get_members(const char* messy_chan){
    return NULL; 
}

messy_chan* messy_make_chan(messy_network *network, const char* chan_name){
    
    messy_chan* chan = malloc(sizeof(messy_chan));
    int name_len = strlen(chan_name),
        net_name_len = strlen(network->name),
        net_path_len = strlen(network->path),
        chan_path_len = net_path_len + name_len + 1;

    network->chans[network->num_chans++] = chan;
    chan->network = network;

    chan->num_members = 0;
    chan->members = malloc(sizeof(char*) * MAX_MEMBERS);
    chan->name = malloc(sizeof(char) * (name_len + 1));
    strcpy(chan->name, chan_name);
    chan->path = malloc(sizeof(char) * (net_path_len + name_len + 2));
    strcpy(chan->path, network->path);
    if(chan->path[net_path_len - 1] != '/'){
        chan->path[net_path_len] = '/';
        chan->path[net_path_len + 1] = '\0';
    }
    strcat(chan->path, chan_name);
    printf("creating chan at %s\n", chan->path);

    _make_dir(chan->path);

    chan->memfile = malloc(sizeof(char) * (chan_path_len + 10));
    strcpy(chan->memfile, chan->path);
    strcat(chan->memfile, "/.members");

    FILE* file = fopen(chan_memfile,"w"); 
    if(file == NULL){
        perror("Error opening channel");
        return NULL;
    }

    //num_members written as char
    fprintf(file, "0");

    fclose(file);
    free(chan_memfile);

    return chan;
}

int messy_join_chan(messy_network *network, const char* chan_name){

    messy_chan *chan;

    for(int c = 0; c < network->num_chans; c++){
        if(strcmp(network->chans[i]->name, chan)){
            chan = network->chans + c;
        }
    }

    FILE *file = fopen(chan->memfile, "r+");

    fclose(file);


    free(path);
    return 0;
}

messy_network* messy_make_network(const char* network_name, 
                                  const char* username){

    //if network does not exist, create it
    int len = strlen(network_name);
    
    _network = malloc(sizeof(_network));
    _network->chans = malloc(sizeof(messy_chan*) * MAX_CHANS); 
    _network->num_chans = 0;

    //make network hidden if not already
    if(network_name[0] != '.'){
        _network->name = malloc(sizeof(char) * (++len + 1));
        _network->name[0] = '.';
        _network->name[1] = '\0';
    }
    else{
        _network->name = malloc(sizeof(char) * (len + 1));
        _network->name [0] = '\0';
    }

    _network->path = malloc(sizeof(char) * (len + _prefixlen + 1));

    strcat(_network->name, network_name);
    strcpy(_network->path, _prefix);
    strcat(_network->path, _network->name);

    DIR* dir = opendir(_network->path);
    if(dir)
        closedir(dir);
    else if(errno == ENOENT){
        mkdir(_network->path, 0777);
    }

    /*
    if(access(_network->path, F_OK) != 0){
        printf("creating network: %s\n", _network->path);
        int network_folder = open(_network->path, O_RDWR);
        close(network_folder);
    }
    */

    return _network;
}

void _touch_file(const char *path){
    int fd = open(path, O_RDWR | O_CREAT);
    close(fd);
}

void _make_dir(const char *path){
    DIR* dir = opendir(path);
    if(dir)
        closedir(dir);
    else if(errno == ENOENT){
        int status = mkdir(path, 0777);
    }
}

void NOTWORKING_make_hidden_dir(const char *path){
    int len = strlen(path);
    char *str = malloc(sizeof(char) * (len + 2)); 
    for(int i = len - 2; i > 0; i++){
        if(path[i] == '/'){
            if(path[i + 1] != '.'){
                //strncpy(
            }
            
        }
    }
}

void messy_cleanup(){

}

void _grow(void* array, int size, int newsize){

   printf("%i\n", sizeof(array)); 
}

