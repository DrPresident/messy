#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>


typedef struct chan{
    char* name;
    char** members;
    int num_members;
} chan;

typedef struct messysession{
    char* username;
    char* network;
    char* networkpath;
    chan* chan;
} messysession;

messysession* messy;
char* prefix = "/tmp/";
int prefixlen = 5;

void free_chan(chan* c){
    for(int i = 0; i < c->num_members; i++)
        free(c->members[i]);
    free(c->name);
    free(c);
}

chan* get_chanlist(const char* network){

    FILE* file = fopen(network, "r");

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

int send_network(const char* network){
    return 1;
}

int send_chan(const char* chan){
    return 1;
}

int send_user(const char* chan){
    return 1;
}

void set_username(const char* name){
    if(name){
        int len = strlen(name);
        if(len == 0)
           perror("cannot set name to an empty string\n"); 
        else{
            messy->username = malloc(sizeof(char) * len + 1);
            strcpy(messy->username, name);
        }
    }
    else{
       perror("cannot set name to an null string\n"); 
    }
}

char** get_members(const char* chan){
    return NULL; 
}

chan* make_chan(const char* channame){
    
    FILE* file = fopen(messy->networkpath,"a"); 
    if(file == NULL){
        perror("Error opening network, be sure to call init_messy\n");
        return NULL;
    }
    chan* c = malloc(sizeof(chan));

    c->num_members = 0;
    c->members = NULL;
    c->name = malloc(sizeof(char) * strlen(channame));
    strcpy(c->name, channame);

    fprintf(file, "%s\n", channame);

    fclose(file);

    return c;
}

int join_chan(const char* chan){

    char *path = malloc(sizeof(char) * (strlen(chan) + 6)); 
    path = strcat(path, "/tmp/");
    path = strcat(path, chan);

    free(path);
    return 0;
}

int init_messy(const char* networkname){

    //if network does not exist, create it
    int len = strlen(networkname);
    

    messy = malloc(sizeof(messysession));
    messy->username = NULL;
    messy->chan = NULL;

    //make network hidden if not already
    if(networkname[0] != '.'){
        messy->network = malloc(sizeof(char) * (++len + 1));
        messy->network[0] = '.';
        messy->network[1] = '\0';
    }
    else{
        messy->network = malloc(sizeof(char) * (len + 1));
        messy->network[0] = '\0';
    }

    messy->networkpath = malloc(sizeof(char) * (len + prefixlen + 1));

    strcat(messy->network, networkname);
    strcpy(messy->networkpath, prefix);
    strcat(messy->networkpath, messy->network);

    if(access(messy->networkpath, F_OK) != 0){
        printf("creating network: %s\n", messy->networkpath);
        int messynetwork = open(messy->networkpath, O_RDWR);
        close(messynetwork);
    }

    return 0;
}

void cleanup_messy(){

}

