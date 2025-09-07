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

#define SOCKET_PORT 19000

int
make_socket (uint16_t port)
{
  int sock;
  struct sockaddr_in name;

  /* Create the socket. */
  sock = socket (PF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    {
      perror ("socket");
      exit (EXIT_FAILURE);
    }

  /* Give the socket a name. */
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


int main(void) {
	printf("Server program assignment 5\n");

    //size_t s_size;

    struct in_addr my_s_addr;

    struct sockaddr_in sock_address;
    
    //my_s_addr = inet_addr("127.0.0.1");
    inet_pton(AF_INET, "127.0.0.1", &my_s_addr);

    //inet_aton("127.0.0.1", &sock_address.s_addr);
    
    sock_address.sin_port = SOCKET_PORT;

    //strncpy(&sock_address.sin_port, "10000\0", 6);
    sock_address.sin_family = AF_INET;
    sock_address.sin_addr = my_s_addr;    

    // socket()
    //
    //
    //

    int s_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (s_fd < 0) {
        log_and_print(LOG_ERR, "Unable to create socket.\n", NULL);
        return -1;
    }


    //int b_rval = bind(s_fd, (struct sockaddr *) &sock_address, sizeof(struct sockaddr_in));

    //if ( b_rval < 0 ) {
    //    log_and_print(LOG_ERR, "Unable to bind to port.\n", NULL);
    //}

    // bind (sockfd, sockaddr-sever)
    //
    //
    //

    int c_rval = connect( s_fd, (struct sockaddr *) &sock_address, sizeof(sock_address));

    if (c_rval < 0) {
        log_and_print(LOG_ERR, "Unable to connect.\n", NULL);
        return -1;
    }

    char buffer[512] = "Hello, world!\0";

    write(s_fd, buffer, 100);

    //char responsebuffer[512] = "                                                               \0";

    // read(s_fd, buffer, 100);

    //printf("Client Received: %s\n", responsebuffer);
    // accept
    //
    //
    //






}
