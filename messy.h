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
void removeWhitespace(char **str);

char* _prefix = "/tmp/";
int _prefixlen = 5;
int _mem_size;

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

char** messy_get_members(const messy_network *network, 
                         const char* chan_name){

    for(int i = 0; i < network->num_chans; i++)
        if(strcmp(network->chans[i]->name, chan_name) == 0)
            return network->chans[i]->members;

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
        size,
        read;
    char *memlist = malloc(s.st_size + len + 3);
    char *tmp = malloc(sizeof(char) * 50);

    FILE *file = fopen(chan->memfile, "r+");
    read = fscanf(file, "%i", &size);
    size++;


    //read current contents into memlist
    sprintf(memlist, "%i", size);
    while(!feof(file)){
        fscanf(file, "%s\n", tmp);
        sprintf(memlist + strlen(memlist), "%s\n", tmp);
    }
    sprintf(memlist + strlen(memlist), "\n%s", username);
    printf("memlist:\n%s\n~\n", memlist);

    //write edited contents
    fclose(file);
    file = fopen(chan->memfile, "w");
    
    fwrite(memlist, strlen(memlist), 1, file);
    fclose(file);

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
    if(chan)
        return messy_add_member(chan, network->username);
    else
        return 1;
}

messy_network* messy_make_network(const char* network_name, 
                                  const char* username){

    int len = strlen(network_name);
    
    messy_network* network = malloc(sizeof(messy_network));
    network->username = malloc(sizeof(char) * (strlen(username) + 1));
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
    DIR* dir = opendir(network->path);
    if(dir)
        closedir(dir);
    else if(errno == ENOENT){
        mkdir(network->path, 0777);
    }

    return network;
}

void _touch_file(const char *path){
    int fd = open(path, O_RDWR | O_CREAT);
    close(fd);
}

void _make_dir(const char *path){
    DIR* dir = opendir(path);
    if(dir)
        closedir(dir);
    else if(errno == ENOENT)
        mkdir(path, 0777);
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

void messy_cleanup(){

}

void _grow(void* array, int size, int newsize){

   printf("%i\n", (int)sizeof(array)); 
}

