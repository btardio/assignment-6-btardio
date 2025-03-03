#include <stdio.h>
#include <stdlib.h>
//#include <sys/socket.h>
#include <sys/types.h>
#include <syslog.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
//#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>


// http://gnu.cs.utah.edu/Manuals/glibc-2.2.3/html_chapter/libc_16.html

// NOTE: using AF_INET is not bidirectional


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



int
read_from_client (int filedes)
{
  char buffer[512];
  int nbytes;
//  int sbytes;

  FILE *file_pointer;


  nbytes = read (filedes, buffer, 512);

  if (nbytes < 0)
    {
      /* Read error. */
      perror ("read");
      exit (EXIT_FAILURE);
    }
  else if (nbytes == 0)
    /* End-of-file. */
    return -1;
  else
    {
      
      fprintf (stderr, "%s", buffer);

      //return 0;
      // write to same FD
      
      
      //sbytes = write(filedes, buffer, 512);


      //if (sbytes < 0){
      //    perror("write");
      //    exit(EXIT_FAILURE);
      //}


      // end write to same FD

      file_pointer = fopen("/var/tmp/aesdsocketdata", "a");

      if ( file_pointer == NULL ){
          log_and_print(LOG_ERR, "Error writing to file.\n", NULL);
          return -1;
      }

      if (fputs(buffer, file_pointer) == EOF) {
          perror("Error writing to file");
          fclose(file_pointer);
          return -1;
      }

      if (fputs("\n", file_pointer) == EOF) {
          perror("Error writing to file");
          fclose(file_pointer);
          return -1;
      }
/*
      if (fflush(file_pointer) == EOF) {
          perror("Error flushing.");
          return -1;
      }
*/
      if (fclose(file_pointer) == EOF) {
          perror("Error closing the file");
          return -11;
      }

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




//struct sockaddr sock_address = { AF_INET, "10005" };

void *safe_malloc(size_t n)
{
    void *p = malloc(n);
    if (p == NULL) {
        log_and_print(LOG_ERR, "Fatal: failed to allocate %zu bytes.\n", n);
        abort();
    }
    return p;
}


int pmain(void) {

    

    if (signal(SIGINT, sig_handler) == SIG_ERR) {
        log_and_print(LOG_ERR, "Unable to create signal handler.\n", NULL);
    }


    if (signal(SIGTERM, sig_handler) == SIG_ERR) {
        log_and_print(LOG_ERR, "Unable to create signal handler.\n", NULL);
    }


    //printf("Server program assignment 5\n");

    size_t s_size;

    struct in_addr my_s_addr;

    //struct sockaddr_in sock_address;

    fd_set active_fd_set, read_fd_set;

    //my_s_addr = inet_addr("127.0.0.1");
    inet_pton(AF_INET, "127.0.0.1", &my_s_addr);

    //inet_aton("127.0.0.1", &sock_address.s_addr);
    
    //sock_address.sin_port = 9000;

    //strncpy(&sock_address.sin_port, "10000\0", 6);
    //sock_address.sin_family = AF_INET;
    //sock_address.sin_addr = my_s_addr;    

    // socket()
    //
    //
    //

    //int s_fd = socket(PF_INET, SOCK_STREAM, 0);
    int s_fd = make_socket(9000);

    if (s_fd < 0) {
        log_and_print(LOG_ERR, "Unable to create socket.\n", NULL);
        return -1;
    }


//    int b_rval = bind(s_fd, (struct sockaddr *) &sock_address, sizeof(struct sockaddr_in));

//    if ( b_rval < 0 ) {
//        log_and_print(LOG_ERR, "Unable to bind to port.\n", NULL);
//    }

    // bind (sockfd, sockaddr-sever)
    //
    //
    //
    
    int l_rval = listen(s_fd, 3);



    if ( l_rval < 0 ) {
        log_and_print(LOG_ERR, "Unable to listen on port.\n", NULL);
    }
    // listen (sockfd)
    //
    //
    // 

    struct sockaddr_in addr_connector;

//    struct socklen_t _s_addr_and_len;

//    struct socklen_t* s_addr_and_len = &s_addr_and_len;


    // null null, don't want to deal with privilege mode and traps?

    s_size = sizeof (addr_connector);
    // = 0;


  /* Initialize the set of active sockets. */
  FD_ZERO (&active_fd_set);
  FD_SET (s_fd, &active_fd_set);

  while (1)
    {
      /* Block until input arrives on one or more active sockets. */
      read_fd_set = active_fd_set;
      if (select (FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0)
        {
          perror ("select");
          exit (EXIT_FAILURE);
        }

      /* Service all the sockets with input pending. */
      for (int i = 0; i < FD_SETSIZE; ++i)
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
                }

//                new = accept (sock,
//                              (struct sockaddr *) &clientname,
//                              &size);
//                if (new < 0)
//                  {
//                    perror ("accept");
//                    exit (EXIT_FAILURE);
//                  }
/*
                fprintf (stderr,
                         "Server: connect from host %s, port %hd.\n",
                         inet_ntoa (addr_connector.sin_addr),
                         ntohs (addr_connector.sin_port));
                         */
                FD_SET (new, &active_fd_set);
              }
            else
              {
                /* Data arriving on an already-connected socket. */
                if (read_from_client (i) < 0)
                  {
                    close (i);
                    FD_CLR (i, &active_fd_set);
                  }
              }
          }
    }

    /*
    for(;;) {
    
        // remember: sebastian files, ritchie ports - ritchie you need a pocket protector - trap
        printf("i am blocking\n");
        int a_fd = accept(s_fd, (struct sockaddr*) &addr_connector, (unsigned int *) &s_size); //(struct sockaddr *) &addr_connector, NULL);
         
        if ( a_fd == -1 ) {
            log_and_print(LOG_ERR, "Unable to accept.\n", NULL);
        }


        //if (FD_ISSET (i, &read_fd_set))

        read_from_client (s_fd + 1 + 1 + 1);
        // TODO: Logs message to the syslog “Accepted connection from xxx” where XXXX is the IP address of the connected client. 
//        vsyslog(LOG_ERR, "Accepted connection from %s\n", inet_ntoa(addr_connector.sin_addr));
                        fprintf (stderr,
                         "Server: connect from host %s, port %hd.\n",
                         inet_ntoa (addr_connector.sin_addr),
                         ntohs (addr_connector.sin_port));

    }
*/

    // accept
    //
    //
    //






}

int main(void){

    pid_t p = fork();

    if ( p == 0 ) {
        pmain();
    }
    else {
        //waitpid(p, NULL, 0);

    }

}
