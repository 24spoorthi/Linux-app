#include<errno.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include"server.h"

const char* program_name;
int verbose;

void* xmalloc (size_t size)
{
  void* ptr = malloc (size);
  // Abort if the allocation failed.
  if (ptr == NULL)
    abort ();
  else
    return ptr;
}
void* xrealloc (void* ptr, size_t size)
{
  ptr = realloc (ptr, size);
  // Abort if the allocation failed.
  if (ptr == NULL)
    abort ();
  else
    return ptr;
}
char* xstrdup (const char* s)
{
  char* copy = strdup (s);
  // Abort if the allocation failed.
  if (copy == NULL)
    abort ();
  else
    return copy;
}
void system_error(const char* operation)
{
  error(operation,strerror(errno));
}
void error(const char* cause,const char* message)
{
  fprintf(stderr,"%s: error: (%s) %s\n",program_name,cause,message);
  exit(1);
}
char* get_self_executable_directory()
{
  int rval;
  char target[1024];
  char* slash;
  size_t res_len;
  char* res;
  rval = readlink("/proc/self/exe",target,sizeof(target));
  if(rval == -1)
    abort();
  else
    target[rval] = '\0';
  slash = strrchr(target,'/');
  if(slash == NULL || slash == target)
    abort();
  res_len = slash - target;
  res= (char*) xmalloc(res_len + 1);
  strncpy(res,target,res_len);
  res[res_len] = '\0';
  return res;
}
