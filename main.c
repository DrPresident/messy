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

    int messychans;

    init_messy("messychans");
    //join_channel("hello");

    make_channel("chan1");

    return 0;
}

