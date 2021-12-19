#include <fcntl.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "server.h"

static char* page_start =
  "<html>\n"
  " <body>\n"
  "  <pre>\n";

static char* page_end =
  "  </pre>\n"
  " </body>\n"
  "</html>\n";

static char* error_page =
  "<html>\n"
  " <body>\n"
  " <p>Error: Could not open /proc/issue.</p>\n"
  " </body>\n"
  "</html>\n";

static char* error_message = "Error reading /proc/issue.";

void module_generate(int fd)
{
  int i_fd;
  struct stat f_info;
  int rval;
  i_fd = open("/etc/issue", O_RDONLY);
  if(i_fd == -1)
    system_error("open");
  rval = fstat(i_fd,&f_info);
  if(rval == -1)
    write(fd, error_page, strlen(error_page));
  else{
    int rval;
    off_t offset = 0;
    write(fd,page_start,strlen(page_start));
    rval = sendfile(fd,i_fd,&offset,f_info.st_size);
    if(rval == -1)
      write(fd, error_message,strlen(error_message));
    write(fd,page_end,strlen(page_end));
  }
  close(i_fd);
}
    
	       
  


  





