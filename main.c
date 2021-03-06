/* Jack Swenson
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <string.h>
#include "messy.h"

int main(int argc, char ** argv){

    messy_network* network = messy_make_network("messynet", "Dr. User");

    messy_make_chan(network,"chan1");
    messy_join_chan(network, "chan1");

    messy_set_username(network, "user1");
    messy_join_chan(network, "chan1");

    messy_set_username(network, "user2");
    messy_join_chan(network, "chan1");

    messy_cleanup(network);

    return 0;
}

