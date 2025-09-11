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
#define SHM_SIZE 331072
#define BUFFER_SIZE 300000

#define SOCKET_PORT 9000

#define FORKING

#define FILENAME "/var/tmp/aesdsocketdata"
#define FILENAMENEW "/var/tmp/aesdsocketdatanew"
#define INTERVAL_SECONDS 10
#define BUFFER_T 3000

//#define APPENDWRITE
// http://gnu.cs.utah.edu/Manuals/glibc-2.2.3/html_chapter/libc_16.html
//
//

// NOTE: using AF_INET is not bidirectional


int shmid;
char *shm_addr;

pid_t pid;

int shmid_bufferposition;
int *bufferposition;

struct sockaddr_in sockaddrs[FD_SETSIZE];

char *file_pointer_new;
//int bufferposition[FD_SETSIZE];
 
//char outbuffer[131072*6];
 
//int lastBufferPosition = 0;
 
int shmid_lastBufferPosition;

int* lastBufferPosition;

sem_t mutex;

struct entry {
    pid_t pid;
    int fd;
    TAILQ_ENTRY(entry) entries;
};

TAILQ_HEAD(tailq_head, entry);

void sig_handler(int signo)
{
    if (signo == SIGINT) {
        printf("Caught signal, exiting");
        exit(0);
    }
    if (signo == SIGTERM) {
        printf("Caught signal, exiting");
        exit(0);
    }
}




void log_and_print_a(int priority, char* fmt, ...) {
    va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("Errno: %d\n", errno);
    perror("Error:");
    fflush(stdout);

    va_start(args, fmt);
    vsyslog(priority, fmt, args);
    va_end(args);
}

void log_and_print(char* fmt) {
    log_and_print_a(LOG_ERR, fmt);
}

/*
 * Note: this is concurrent but simply blocking on file open
 * improvements could write to file at seek position given a
 * file descriptor, ie: seek to position 1000 and write over
 * for 1 file descriptor and seek to position 3000 for another
 * file descriptor and write over
 */
    int
read_from_client (const int filedes, char* buffer, int nbytes)
{

    char fbuffer[BUFFER_SIZE+1];
    int sbytes;

    FILE *file_pointer;

    bzero(fbuffer, BUFFER_SIZE+1);

    if (nbytes == 1) {
        // received a ""
        free(buffer);
        return 0;
    }
    else
    {

        // move fopen out of function

#ifdef APPENDWRITE
        file_pointer = fopen(FILENAME, "a");

        // seek to position in file corresponding with fd

        if ( file_pointer == NULL ){
            log_and_print("Error writing to file.\n");
            return -1;
        }
#endif

        if ((bufferposition = shmat(shmid_bufferposition, NULL, 0)) == (int *) -1) {
             perror("shmat child");
             exit(1);
        }


        // locking mechanism needed here
        if (bufferposition[filedes] == -1){
            
            sem_wait(&mutex);

            if ((lastBufferPosition = shmat(shmid_lastBufferPosition, NULL, 0)) == (int *) -1) {
                perror("shmat child");
                exit(1);
            }

            bufferposition[filedes] = *lastBufferPosition + BUFFER_T;
            *lastBufferPosition = bufferposition[filedes];
            
            if (shmdt(lastBufferPosition) == -1) {
                perror("shmdt child");
                exit(1);
            }

            sem_post(&mutex);
            
        }

        printf("filedes: %d\n", filedes);
        printf("bufferposition[filedes]: %d\n", bufferposition[filedes]);
        printf("nbytes: %d\n", nbytes);
        fwrite(buffer, sizeof(char), nbytes, stdout);

        //char* outbuffptr = &outbuffer[bufferposition[filedes]];

        if ((shm_addr = shmat(shmid, NULL, 0)) == (char *) -1) {
            perror("shmat child");
            exit(1);
        }

        //strncpy(outbuffptr, buffer, nbytes);

        char* shmbuffptr = &shm_addr[bufferposition[filedes]];

        strncpy(shmbuffptr, buffer, nbytes);

        if (shmdt(shm_addr) == -1) {
            perror("shmdt child");
            exit(1);
        }


        // this needs lock then
        bufferposition[filedes] = bufferposition[filedes] + (nbytes * sizeof(char));
        //buffer, file_pointer
        
#ifdef APPENDWRITE
        if (fputs(buffer, file_pointer) == EOF) {
            perror("Error writing to file");
            fclose(file_pointer);
            return -1;
        }




        if (fclose(file_pointer) == EOF) {
            perror("Error closing the file");
            return -11;
        }

        FILE* file = fopen("/var/tmp/aesdsocketdata", "r");

        if (file == NULL) {
            perror("Error opening file");
            return 1;
        }

        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        fseek(file, 0, SEEK_SET);

        size_t bytes_read = fread(fbuffer, 1, file_size, file);
        if (bytes_read != (size_t)file_size) {
            perror("Error reading file");
            fclose(file);
            return 1;
        }

        fbuffer[file_size] = '\0';

        fclose(file);

        sbytes = write(filedes, fbuffer, file_size);
#else

        sbytes = write(filedes, "ACK", 3);

#endif

        if (sbytes < 0){
            perror("write");
            exit(EXIT_FAILURE);
        }

        if (shmdt(bufferposition) == -1) {
            perror("shmdt child");
            exit(1);
        }

        free(buffer);
        return 0;
    }
}




    int
make_socket (uint16_t port)
{
    int sock;
    struct sockaddr_in name;

    sock = socket (PF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror ("socket");
        exit (EXIT_FAILURE);
    }

    int rcvBufferSize = 0;
    socklen_t optlen = sizeof(rcvBufferSize);

    if (getsockopt(sock, SOL_SOCKET, SO_RCVBUF, &rcvBufferSize, &optlen) == -1) {
        perror("getsockopt failed");
        close(sock);
        return -1;
    }

    printf("Current receive buffer size: %d\n", rcvBufferSize);

    name.sin_family = AF_INET;
    name.sin_port = htons (port);
    name.sin_addr.s_addr = htonl (INADDR_ANY);
    if (bind (sock, (struct sockaddr *) &name, sizeof (name)) < 0)
    {
        perror ("bind");
        exit (EXIT_FAILURE);
    }

    return sock;
}



void *safe_malloc(size_t n)
{
    void *p = malloc(n);
    if (p == NULL) {
        log_and_print("Fatal: failed to allocate bytes.\n");
        abort();
    }
    return p;
}



int append_time(void) {

    FILE *fp;
    time_t raw_time;
    struct tm *gmt_time;
    char timestamp_buf[256];

    time(&raw_time);

    // 2. Convert raw time to GMT broken-down time.
    // RFC 2822 uses GMT (UTC).
    gmt_time = gmtime(&raw_time);
    if (gmt_time == NULL) {
        perror("gmtime");
        return 1;
    }

    // 3. Format the GMT time as an RFC 2822 compliant string.
    // Example format: "Mon, 08 Sep 2025 04:30:00 +0000"
    strftime(timestamp_buf, 256, "%a, %d %b %Y %H:%M:%S +0000", gmt_time);

    // 4. Open the file in append mode.
    // Creates the file if it doesn't exist.
    fp = fopen(FILENAME, "a");
    if (fp == NULL) {
        perror("fopen");
        return 1;
    }

    // 5. Append the formatted timestamp to the file.
    fprintf(fp, "timestamp:%s\n", timestamp_buf);

    // 6. Close the file to ensure the data is written.
    fclose(fp);

    // 7. Print to console for confirmation.
    //printf("Appended timestamp: %s\n", timestamp_buf);

    // 8. Wait for 10 seconds before the next iteration.
    //sleep(INTERVAL_SECONDS);

}


int pmain(void) {

    if (sem_init(&mutex, 0, 1) != 0) {
        perror("sem_init failed");
        exit(EXIT_FAILURE);
    }

    // Create a shared memory segment
    // IPC_PRIVATE ensures a unique key, IPC_CREAT creates if it doesn't exist
    // 0666 sets read/write permissions for owner, group, and others
    if ((shmid = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

//     51 int shmid_lastBufferPosition;
// 52 int* lastBufferPosition;

     if ((shmid_lastBufferPosition = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) < 0) {
         perror("shmget_lastBufferPosition");
         exit(1);
     }

     
     if ((shmid_bufferposition = shmget(IPC_PRIVATE, FD_SETSIZE, IPC_CREAT | 0666)) < 0) {
         perror("shmget_bufferposition");
         exit(1);
     }

     if ((lastBufferPosition = shmat(shmid_lastBufferPosition, NULL, 0)) == (int *) -1) {
         perror("shmat child");
         exit(1);
     }


     if ((bufferposition = shmat(shmid_bufferposition, NULL, 0)) == (int *) -1) {
              perror("shmat child");
              exit(1);
     }

     *lastBufferPosition = -BUFFER_T;

     memset(bufferposition, -1, FD_SETSIZE);

     if (shmdt(bufferposition) == -1) {
         perror("shmdt child");
         exit(1);
     }

     if (shmdt(lastBufferPosition) == -1) {
         perror("shmdt child");
         exit(1);
     }


    //memset(outbuffer, 'A', 131072*6); //bzero(outbuffer, 131072*6);


    if (signal(SIGINT, sig_handler) == SIG_ERR) {
        log_and_print("Unable to create signal handler.\n");
    }


    if (signal(SIGTERM, sig_handler) == SIG_ERR) {
        log_and_print("Unable to create signal handler.\n");
    }


    size_t s_size;

    struct in_addr my_s_addr;

    fd_set active_fd_set, read_fd_set;

    inet_pton(AF_INET, "127.0.0.1", &my_s_addr);

    int s_fd = make_socket(SOCKET_PORT);

    if (s_fd < 0) {
        log_and_print("Unable to create socket.\n");
        return -1;
    }


    int l_rval = listen(s_fd, 3);


    if ( l_rval < 0 ) {
        log_and_print("Unable to listen on port.\n");
    }

    struct sockaddr_in addr_connector;

    s_size = sizeof (addr_connector);


    /* Initialize the set of active sockets. */
    FD_ZERO (&active_fd_set);
    FD_SET (s_fd, &active_fd_set);

    int n_reads = 0;

    struct entry *threads[65535];
    int status = 0;




    time_t last_execution_time = time(NULL); // Initialize with current time
    last_execution_time += 10;
    const double interval_seconds = 3.0; // Desired interval in seconds




    while (1)
    {

        time_t current_time = time(NULL);
        double elapsed_time = difftime(current_time, last_execution_time);

        if (elapsed_time >= interval_seconds) {
            append_time();
            last_execution_time = current_time; // Update last execution time
        }

        /* Block until input arrives on one or more active sockets. */
        read_fd_set = active_fd_set;
        if (select (FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0)
        {
            perror ("select");
            exit (EXIT_FAILURE);
        }


        /* Service all the sockets with input pending. */
        for (int i = 0; i < FD_SETSIZE; ++i)
        {
            //int status = 0;
            if (FD_ISSET (i, &read_fd_set))
            {
                if (i == s_fd)
                {
                    /* Connection request on original socket. */
                    int new;
                    s_size = sizeof (addr_connector);

                    new = accept(s_fd, (struct sockaddr*) &addr_connector, (unsigned int *) &s_size); //(struct sockaddr *) &addr_connector, NULL);
                    if ( new < 0 ) {
                        log_and_print("Unable to accept.\n");

                    } else {
                        sockaddrs[new] = addr_connector;
                    }
                    FD_SET (new, &active_fd_set);
                }
                else
                {

                    struct entry *newentry = threads[i];

                    if (newentry == NULL) {
                        newentry = malloc(sizeof(struct entry));
                        newentry->fd = i;
                    }

                    n_reads = n_reads + 1;  
                    /* Data arriving on an already-connected socket. */
                    char* read_buffer = malloc(sizeof(char)*BUFFER_SIZE+1);
                    bzero(read_buffer, BUFFER_SIZE+1);
                    printf("i: %d\n", i);
                    int nbytes = read (i, read_buffer, BUFFER_SIZE);
                    pid_t pid;

                    if (nbytes < 0)
                    {
                        // Read error.
                        perror ("read");
                        //exit (EXIT_FAILURE);
                        continue;
                    }
                    else if (nbytes == 0) {

                        close (i); 
                        FD_CLR (i, &active_fd_set);
                        //exit(EXIT_SUCCESS);
                        continue;

                    } else { 
                        pid = fork();

                        if ( pid < 0 ) { 
                            fprintf(stderr, "fork failed\n"); exit(EXIT_FAILURE);
                        } else if (pid == 0) {
                            printf("iii: %d\n", i);
                            pid_t pidd;
                            pidd = fork();
                            if ( pidd < 0 ) { fprintf(stderr, "fork failed\n"); exit(EXIT_FAILURE); }
                            else if (pidd == 0) {
                                printf("iiiiii: %d\n", i);
                                read_from_client (i, read_buffer, nbytes);
                                exit(EXIT_SUCCESS);
                            } else {
                            }
                            exit(EXIT_SUCCESS);
                            //break;

                        } else {
                            //exit(EXIT_SUCCESS);
                        }

                    }
                }
            }
        }




        //read_from_client(i, read_buffer, nbytes);

        wait(&status);

        if ((shm_addr = shmat(shmid, NULL, 0)) == (char *) -1) {
            perror("shmat child");
            exit(1);
        }

        printf("\n~~~ A: ");
        fwrite(shm_addr, sizeof(char), BUFFER_T, stdout);
        printf("\n~~~ B: ");
        fwrite(&shm_addr[BUFFER_T], sizeof(char), BUFFER_T, stdout);
        printf("\n~~~ C: ");
        fwrite(&shm_addr[BUFFER_T + BUFFER_T], sizeof(char), BUFFER_T, stdout);
        printf("\n~~~ D: ");
        fwrite(&shm_addr[BUFFER_T + BUFFER_T + BUFFER_T], sizeof(char), BUFFER_T, stdout);
        printf("\n");
        if (shmdt(shm_addr) == -1) {
            perror("shmdt child");
            exit(1);
        }

    }
    fclose(file_pointer_new);
}

int main(void){

    if (remove("/var/tmp/aesdsocketdata") == 0) {
    } else {
        perror("Error deleting file");
    }

    //    pid_t p = fork();

    //    if ( p == 0 ) {
    pmain();
    //    }
    //    else {

    //    }

}
