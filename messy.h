#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>

#define MAX_CHANS 100
#define MAX_MEMBERS 100

typedef struct messy_chan{
    char* name;
    char* path;
    struct messy_network* network;
    char** members;
    int* members_pipes;
    int* mypipe;
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

int _make_dir(const char *path);
void removeWhitespace(char **str);

char* _prefix = "/tmp/";
int _prefixlen = 5;
int _mem_size;

int messy_send_network(messy_network* network, const char* msg){
    return 1;
}

int messy_send_chan(messy_network *network, const char* chan_name, 
        const char* msg){
}

int messy_send_user(messy_network *network, const char* chan_name, 
        const char* username, const char* msg){

    messy_chan* chan = NULL;
    int c;

    for(c = 0; chan == NULL && c < network.num_chans; c++)
        if(strcmp(network->chans[c]->name, chan_name) == 0)
            chan = network->chans + c;

    for(c = 0; c < chan->num_members; c++){
        if(strcmp(chan->members[c], username) == 0){
            //constructing full msg
            char *tmp = malloc(sizeof(char) * 100);
            
            strcpy(tmp, network->username);
            strcat(tmp, ": ");
            strcat(tmp, msg);
            strcat(tmp, "\0");
            printf("full msg: %s", tmp);
            write(chan->members_pipes[c], tmp, strlen(tmp));
            return 0;
        }
    }


}

void messy_get_members_list(const messy_network *network, 
                         const char* chan_name, char **list, int *size){

    for(int i = 0; i < network->num_chans; i++){
        if(strcmp(network->chans[i]->name, chan_name) == 0){
            *list = network->chans[i]->members;
            *size = network->chans[i]->num_members;
        }
    }

    return NULL; 
}

void removeWhitespace(char **str){
    int len = strlen(*str);
    char *tmp = malloc(sizeof(char) * (len + 5));
    for(int j = 0, k = 0; j <= len; j++)
        if((*str)[j] != ' ') 
            tmp[k++] = (*str)[j];
    free(*str);
    *str = tmp;
}

messy_chan* messy_make_chan(messy_network *network, const char* chan_name){
    
    messy_chan* chan = malloc(sizeof(messy_chan));
    int name_len = strlen(chan_name),
        //net_name_len = strlen(network->name),
        net_path_len = strlen(network->path),
        chan_path_len = net_path_len + name_len + 1;

    network->chans[network->num_chans] = chan;
    network->num_chans++;
    chan->network = network;

    chan->num_members = 0;
    chan->members = malloc(sizeof(char*) * MAX_MEMBERS);
    chan->members_pipes = malloc(sizeof(int) * MAX_MEMBERS);
    memset(chan->members_pipes, 0, sizeof(int) * MAX_MEMBERS);

    chan->name = malloc(sizeof(char) * (name_len + 1));
    strcpy(chan->name, chan_name);
    chan->path = malloc(sizeof(char) * (net_path_len + name_len + 2));
    strcpy(chan->path, network->path);

    if(chan->path[net_path_len - 1] != '/'){
        chan->path[net_path_len] = '/';
        chan->path[net_path_len + 1] = '\0';
    }
    strcat(chan->path, chan_name);
    //printf("creating chan at %s\n", chan->path);

    _make_dir(chan->path);

    chan->memfile = malloc(sizeof(char) * (chan_path_len + 10));
    strcpy(chan->memfile, chan->path);
    strcat(chan->memfile, "/.members");

    FILE* file = fopen(chan->memfile,"w"); 
    if(file == NULL){
        perror("Error opening channel");
        return NULL;
    }

    //num_members written as char
    fprintf(file, "0");

    fclose(file);

    return chan;
}


int messy_add_member(messy_chan *chan, const char* username){

    struct stat s;
    stat(chan->memfile, &s);
    int len = strlen(username),
        read,
        num_members;
    char *memlist = malloc(s.st_size + len + 3);
    char *tmp = malloc(sizeof(char) * 50);

    FILE *file = fopen(chan->memfile, "r+");
    read = fscanf(file, "%i", &num_members);
    num_members++;

    //read current contents into memlist
    sprintf(memlist, "%i", num_members);
    while(!feof(file)){
        fscanf(file, "%s", tmp);
        sprintf(memlist + strlen(memlist), "\n%s", tmp);
    }
    sprintf(memlist + strlen(memlist), "\n%s", username);
    printf("memlist:\n%s\n~\n", memlist);

    //close read file
    fclose(file);

    //write edited contents
    file = fopen(chan->memfile, "w");
    fwrite(memlist, strlen(memlist), 1, file);
    fclose(file);

    //make pipe for this user
    free(tmp);
    tmp = malloc(sizeof(char) 
            * (strlen(chan->path) + strlen(chan->network->username) + 2));
    strcpy(tmp, chan->path);
    strcat(tmp, "/");
    strcat(tmp, chan->network->username);
    mkfifo(tmp, S_IRUSR | S_IWUSR | S_IWGRP | S_IWOTH);
    int fd[2] = open(tmp, O_RDWR);
    //close write end
    close(fd[1]);
    chan->mypipe = fd[0];

    free(tmp);
    free(memlist);

    return 0;
}

int messy_join_chan(messy_network *network, const char* chan_name){

    messy_chan *chan = NULL;

    for(int i = 0; i < network->num_chans; i++){
        if(strcmp(network->chans[i]->name, chan_name) == 0){
            chan = network->chans[i]; 
            break;
        }
    }
    if(chan){
        messy_add_member(chan, network->username);
        //need to open other users pipes
        
    }
    else
        return 1;
}

void messy_set_username(messy_network *network, const char* username){
    if(network->username)
        free(network->username);

    network->username = malloc(sizeof(char) * (strlen(username) + 1));
    strcpy(network->username, username);
}

messy_network* messy_make_network(const char* network_name, 
                                  const char* username){

    int len = strlen(network_name);
    
    messy_network* network = malloc(sizeof(messy_network));
    network->username = NULL;
    messy_set_username(network, username);
    strcpy(network->username, username);
    removeWhitespace(&(network->username));
    network->chans = malloc(sizeof(messy_chan*) * MAX_CHANS); 
    network->num_chans = 0;

    //make network hidden if not already
    if(network_name[0] != '.'){
        network->name = malloc(sizeof(char) * (++len + 1));
        network->name[0] = '.';
        network->name[1] = '\0';
    }
    else{
        network->name = malloc(sizeof(char) * (len + 1));
        network->name[0] = '\0';
    }

    network->path = malloc(sizeof(char) * (len + _prefixlen + 1));

    strcat(network->name, network_name);
    strcpy(network->path, _prefix);
    strcat(network->path, network->name);

    //if network does not exist, create it
    int existed = _make_dir(network->path);
    /*
    DIR* dir = opendir(network->path);
    if(dir)
        closedir(dir);
    else if(errno == ENOENT){
        mkdir(network->path, 0777);
    }
    */

    if(existed)
        printf("%s exists", network->name);

    return network;
}

void _touch_file(const char *path){
    int fd = open(path, O_RDWR | O_CREAT);
    close(fd);
}

int _make_dir(const char *path){
    DIR* dir = opendir(path);
    if(dir){
        closedir(dir);
        return 0;
    }
    else if(errno == ENOENT){
        mkdir(path, 0777);
        return 1;
    }
    return 2;
}

/*
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
*/

void _free_messy_chan(messy_chan* c){
    for(int i = 0; i < c->num_members; i++)
        free(c->members[i]);
    free(c->name);
    free(c->path);
    for(int i = 0; i < c->num_members; i++)
        free(c->members[i]);
    free(c->members);
    free(c);
}

void messy_cleanup(messy_network *network){
    free(network->username);
    free(network->name);
    free(network->path);
    for(int i = 0; i < network->num_chans; i++)
        _free_messy_chan(network->chans[i]);
}

void _grow(void* array, int size, int newsize){

   printf("%i\n", (int)sizeof(array)); 
}

