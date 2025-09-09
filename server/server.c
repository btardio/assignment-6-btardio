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

// required to read till end of buffer - if not wait for something that reads entire buffer?
#define BUFFER_SIZE 300000

#define SOCKET_PORT 9000

#define FORKING

#define FILENAME "/var/tmp/aesdsocketdata"

#define INTERVAL_SECONDS 10

// http://gnu.cs.utah.edu/Manuals/glibc-2.2.3/html_chapter/libc_16.html

// NOTE: using AF_INET is not bidirectional
struct sockaddr_in sockaddrs[FD_SETSIZE];

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




void log_and_print(int priority, char* fmt, ...) {
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
  //char buffer[BUFFER_SIZE+1];
  //char writebuffer[512];
  //int nbytes;
  int sbytes;

  FILE *file_pointer;

  bzero(fbuffer, BUFFER_SIZE+1);
  //bzero(buffer, BUFFER_SIZE+1);


  //printf("nbytes: %d\n", nbytes);
  //printf(">>>%s\n", buffer[0]);
  //nbytes = read (filedes, buffer, BUFFER_SIZE);

  /*
  if (nbytes < 0)
    {
      // Read error.
      perror ("read");
      exit (EXIT_FAILURE);
    }
  else if (nbytes == 0) {

      printf("ending read\n");

    // End-of-file.
    free(buffer);
    return -1;
  }
  */
  if (nbytes == 1) {
      // received a ""
      free(buffer);
      return 0;
  }
  else
    {
      
      // move fopen out of function

      file_pointer = fopen(FILENAME, "a");
      
      // seek to position in file corresponding with fd

      if ( file_pointer == NULL ){
          log_and_print(LOG_ERR, "Error writing to file.\n", NULL);
          return -1;
      }

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
      

      if (sbytes < 0){
          perror("write");
          exit(EXIT_FAILURE);
      }

      char message[] = "z";
      //write(filedes, buffer, nbytes);
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
        log_and_print(LOG_ERR, "Fatal: failed to allocate %zu bytes.\n", n);
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



// Attribute
//  google AI: search term "queue.h remove all items in queue"
/*
void clear_queue(struct tailhead *head) {
    struct entry *np, *next_np;

    // Iterate safely through the queue and remove each element
    TAILQ_FOREACH_SAFE(np, head, entries, next_np) {
        TAILQ_REMOVE(head, np, entries);
        free(np); // Free the memory allocated for the element
    }
}
*/

int pmain(void) {

    //time_t last_execution_time = time(NULL); // Initialize with current time

    //const double interval_seconds = 10.0; // Desired interval in seconds

    if (signal(SIGINT, sig_handler) == SIG_ERR) {
        log_and_print(LOG_ERR, "Unable to create signal handler.\n", NULL);
    }


    if (signal(SIGTERM, sig_handler) == SIG_ERR) {
        log_and_print(LOG_ERR, "Unable to create signal handler.\n", NULL);
    }


    size_t s_size;

    struct in_addr my_s_addr;

    fd_set active_fd_set, read_fd_set;

    inet_pton(AF_INET, "127.0.0.1", &my_s_addr);

    int s_fd = make_socket(SOCKET_PORT);

    if (s_fd < 0) {
        log_and_print(LOG_ERR, "Unable to create socket.\n", NULL);
        return -1;
    }

    
    int l_rval = listen(s_fd, 3);


    if ( l_rval < 0 ) {
        log_and_print(LOG_ERR, "Unable to listen on port.\n", NULL);
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

    const double interval_seconds = 10.0; // Desired interval in seconds




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
                    log_and_print(LOG_ERR, "Unable to accept.\n", NULL);
                    
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
                    //TAILQ_INSERT_TAIL(&head, newentry, entries);
                    //printf("inserting %d\n", i);
                }

                // fork  
                n_reads = n_reads + 1;  
                //printf("%d\n", n_reads);
                //int status;
                /* Data arriving on an already-connected socket. */

                char* read_buffer = malloc(sizeof(char)*BUFFER_SIZE+1);
                bzero(read_buffer, BUFFER_SIZE+1);
                int nbytes = read (i, read_buffer, BUFFER_SIZE);
#ifdef FORKING
                pid_t pid;
                //pid_t pid = fork();
//                if (pid < 0) {
//                    fprintf(stderr, "Fork failed.\n");
//                    exit(EXIT_FAILURE);
//                } else if (pid ==0) { // forked
#endif
                    
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
                        fprintf(stderr, "fork failed\n");
                        exit(EXIT_FAILURE);
                       } else if (pid == 0) {
                           read_from_client (i, read_buffer, nbytes);
                           exit(EXIT_SUCCESS);
                           break;
                       } else {
                           // if you are not killing the parent you should be using PTHREAD
                           // however i dont think the tests like that
                           //exit(EXIT_SUCCESS);
                           //break;
                       }

                    }
#ifdef FORKING
                    //exit(EXIT_SUCCESS);
                
//                } else {
//                    newentry->pid = pid;
//                }
#endif
                // if == 0 handle the ""
                //
                // before joining there join here
                //
                //int status;
#ifdef FORKING
                //wait(&status);
/*
                pid_t endpid = waitpid(newentry->pid, &status, 0);
                if(endpid == -1){
                    perror("waitpid failed");
                    exit(EXIT_FAILURE);
                }
                if (WIFEXITED(status)) {
                    printf("Parent process: Child (PID %d) terminated with status %d.\n", endpid, WEXITSTATUS(status));
                } else {
                    printf("Parent process: Child (PID %d) terminated abnormally.\n", endpid);
                }
                */
#endif

              }
          }
      }

      wait(&status);
      
      // join
      //clear_queue(&head);
      //
      //
      //
      //struct entry *np, *next_np;

      // Iterate safely through the queue and remove each element
//      TAILQ_FOREACH_SAFE(np, &head, entries, next_np) {
//          TAILQ_REMOVE(head, np, entries);
//          free(np); // Free the memory allocated for the element
//      }

    }
}

int main(void){

    if (remove("/var/tmp/aesdsocketdata") == 0) {
    } else {
        perror("Error deleting file");
    }

    pid_t p = fork();

    if ( p == 0 ) {
        pmain();
    }
    else {

    }

}
