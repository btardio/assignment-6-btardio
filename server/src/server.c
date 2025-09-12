#include "server.h"

int main(void){

    if (remove("/var/tmp/aesdsocketdata") == 0) {
    } else {
        perror("Error deleting file");
    }

    pid_t p = fork();

    if ( p == 0 ) {
        pmain();
    } else {

    }

}
