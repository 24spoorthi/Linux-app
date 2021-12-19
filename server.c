#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include "server.h"

static char* ok_response =
  "HTTP/1.0 200 OK\n"
  "Content-type: text/html\n"
  "\n";

static char* bad_request_response =
  "HTTP/1.0 400 Bad Request\n"
  "Content-type: text/html\n"
  "\n"
  "<html>\n"
  " <body>\n"
  " <h1>Bad Request</h1>\n"
  " <p>This server did not understand your request.</p>\n"
  " </body>\n"
  "</html>\n";

static char* not_found_response_template =
  "HTTP/1.0 404 Not Found\n"
  "Content-type: text/html\n"
  "\n"
  "<html>\n"
  " <body>\n"
  " <h1>Not Found</h1>\n"
  " <p>The requested URL %s was not found on this server.</p>\n"
  " </body>\n"
  "</html>\n";

static char* bad_method_response_template =
  "HTTP/1.0 501 Method Not Implemented\n"
  "Content-type: text/html\n"
  "\n"
  "<html>\n"
  " <body>\n"
  " <h1>Method Not Implemented</h1>\n"
  " <p>The method %s is not implemented by this server.</p>\n"
  " </body>\n"
  "</html>\n";

static void clean_up_process(int signal_num)
{
  int status;
  wait(&status);
}

static void handle_get(int conn_fd, const char* page)
{
  ser_mod* module = NULL;
  if(*page == '/' && strchr(page + 1,'/') == NULL){
    char mod_f_name[64];
    snprintf(mod_f_name,sizeof(mod_f_name),"%s.so",(page + 1));
    module = module_open(mod_f_name);
  }
  if(module == NULL){
    char res[1024];
    snprintf (res, sizeof (res), not_found_response_template, page);
    write(conn_fd,res,strlen(res));
    
  }
  else{
    write (conn_fd, ok_response, strlen (ok_response));
    (*module->generate_funtion)(conn_fd);
    module_close(module);
  }
}

static void handle_connection(int conn_fd)
{
  char buff[256];
  ssize_t bytes_read = read(conn_fd,buff,sizeof(buff) - 1);
  //printf("%d\n",bytes_read);
  if(bytes_read > 0){
    // printf("%s\n",buff);
    char method[sizeof (buff)];
    char url[sizeof (buff)];
    char protocol[sizeof (buff)];
    buff[bytes_read] = '\0';
    sscanf(buff,"%s %s %s",method,url,protocol);
    //  printf("%s\n",method);
    //    printf("%s\n",url);
    //    printf("%s\n",protocol);
    while (strstr (buff, "\r\n\r\n") == NULL)
      bytes_read = read(conn_fd,buff,sizeof(buff));
    if(bytes_read == -1){
      close(conn_fd);
      return;
    }
    if(strcmp(protocol, "HTTP/1.0") && strcmp(protocol,"HTTP/1.1")){
      write (conn_fd, bad_request_response,sizeof (bad_request_response));
    }
    else if(strcmp(method,"GET")){
      char res[1024];
      snprintf(res,sizeof(res),bad_method_response_template, method);
      write(conn_fd, res, strlen (res));
    }
    else
      handle_get(conn_fd,url);
  }
  else if(bytes_read == 0)
    ;
  else
    system_error("read");
}

void server_run (struct in_addr local_address, uint16_t port)
{
    struct sockaddr_in sock_addr;
    int rval;
    struct sigaction sa;
    int ser_sock;
    memset (&sa, 0, sizeof (sa));
    sa.sa_handler = &clean_up_process;
    sigaction(SIGCHLD, &sa, NULL);
    ser_sock = socket (PF_INET, SOCK_STREAM, 0);
    if(ser_sock == -1)
      system_error("socket");
    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = port;
    sock_addr.sin_addr = local_address;
    rval = bind(ser_sock, &sock_addr, sizeof(sock_addr));
    if(rval != 0){
      printf("%d\n",rval);
      system_error("bind");
    }
    rval = listen(ser_sock,10);
    if(rval != 0)
      system_error("listen");
    if(verbose){
      socklen_t add_len;
      add_len = sizeof(sock_addr);
      rval = getsockname(ser_sock,&sock_addr,&add_len);
      assert(rval == 0);
      printf("server listening on %s : %d\n",inet_ntoa(sock_addr.sin_addr), (int)ntohs(sock_addr.sin_port));
    }
    while(1){
      struct sockaddr_in rem_add;
      socklen_t add_len;
      int conn;
      pid_t child_pid;
      add_len = sizeof(rem_add);
      conn = accept(ser_sock, &rem_add, &add_len);
      if(conn == -1){
	if(errno == EINTR)
	  continue;
	else
	  system_error("accept");
      }
      if(verbose){
	socklen_t add_len;
	add_len = sizeof(sock_addr);
	rval = getpeername(conn, &sock_addr, &add_len);
	assert(rval == 0);
	printf("connection accepted from %s\n",inet_ntoa (sock_addr.sin_addr));
      }
      child_pid = fork();
      if(child_pid == 0){
	close (STDIN_FILENO);
	close (STDOUT_FILENO);
	close (ser_sock);
	handle_connection(conn);
	close(conn);
	exit(0);
      }
      else if(child_pid > 0)
	close(conn);
      else
	system_error("fork");
    }
}
	
	
      
	
		      
			
