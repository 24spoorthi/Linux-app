#include <assert.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include "server.h"
static char* page_template =
  "<html>\n"
  " <head>\n"
  "  <meta http-equiv=\"refresh\" content=\"1\">\n"
  " </head>\n"
  " <body>\n"
  "  <p>The current time is %s.</p>\n"
  " </body>\n"
  "</html>\n";

void module_generate(int fd)
{
  struct timeval tv;
  struct tm* ptm;
  char ts[40];
  FILE* fp;
  gettimeofday(&tv, NULL);
  ptm = localtime(&tv.tv_sec);
  strftime(ts,sizeof(ts),"%H:%M:%S",ptm);
  fp = fdopen(fd, "w");
  assert(fp != NULL);
  fprintf(fp,page_template,ts);
  fflush(fp);
}
