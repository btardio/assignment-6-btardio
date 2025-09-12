#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <syslog.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/queue.h>
#include <time.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <semaphore.h>


void sig_handler(int signo);

void log_and_print_a(int priority, const char* fmt, ...);

void log_and_print(const char* fmt);

int read_from_client (const int filedes, char* buffer, int nbytes);

int make_socket (uint16_t port);

void *safe_malloc(size_t n);

void append_time(void);

int shmid;

//int pmain(void);

int dummy;

char *shm_addr;
 
pid_t pid;
 
int shmid_bufferposition;
int *bufferposition;
 
struct sockaddr_in sockaddrs[FD_SETSIZE];
 
char *file_pointer_new;
 
int shmid_lastBufferPosition;
 
int* lastBufferPosition;
 
sem_t mutex;
 
struct entry {
    pid_t pid;
    int fd;
};




