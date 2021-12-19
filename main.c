#include <assert.h>
#include <getopt.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include "server.h"

static const struct option long_options[] = {
  { "address", 1, NULL, 'a'},
  { "help", 0, NULL, 'h'},
  { "module-dir", 1, NULL, 'm'},
  { "port", 1, NULL, 'p'},
  { "verbose", 0, NULL, 'v'},
};
static const char* const short_options = "a:hm:p:v";
static const char* const usage_template =
  "Usage: %s [options]\n"
  " -a, --address ADDR        Bind to local address (by default, bind\n"
  "                           to all local addresses).\n"
  "-h, --help                 Print this information.\n"
  "-m, --module-dir DIR       Load modulea from specified directory.\n"
  "                           (by default, we use executable directory).\n"    
  "-p, --port PORT            Bind to specified port.\n"
  "-v, --verbose              Print verbose messages.\n";

static void print_usage(int is_error)
{
  fprintf(is_error ? stderr : stdout,usage_template,program_name);
  exit(is_error ? 1 : 0);
}

int main (int argc, char* const argv[])
{
  struct in_addr local_add;
  uint16_t port;
  int next_op;
  program_name = argv[0];
  local_add.s_addr = INADDR_ANY;
  port = 0;
  verbose = 0;
  module_dir = get_self_executable_directory();
  assert(module_dir != NULL);
  do{
    next_op = getopt_long(argc,argv,short_options,long_options, NULL);
    //    printf("%d\n",next_op);
    switch(next_op){
    case 'a':
      {
	struct hostent* local_host_name;
	local_host_name = gethostbyname(optarg);
	if(local_host_name == NULL || local_host_name->h_length == 0)
	  error(optarg, "INVALID HOST NAME");
	else
	  local_add.s_addr = *((int*)(local_host_name->h_addr_list[0]));
	printf("%d\n",local_add.s_addr);
      }
      break;
    case 'h':
      print_usage(0);
      
    case 'm':
      {
	struct stat dir_info;
	if(access(optarg, F_OK) != 0)
	  error(optarg, "module directory does not exist");
	if(access(optarg, R_OK | X_OK) != 0)
	  error(optarg, "module directory is not accessible");
	if(stat(optarg, &dir_info) != 0 || !S_ISDIR(dir_info.st_mode))
	  error(optarg,"not a directory");
	module_dir = strdup(optarg);
      }
      break;
    case 'p':
      {
	long val;
	char* end;
	val = strtol(optarg, &end, 10);
	if(*end != '\0')
	  print_usage(1);
	port = (uint16_t)htons(val);
      }
      break;
    case 'v':
      verbose = 1;
      break;
    case'?':
      print_usage(1);
    case -1:
      break;
    default:
      abort();
    }
  }while(next_op != -1);
  if(optind != argc)
    print_usage(1);
  if(verbose)
    printf("modules will be loaded from %s\n",module_dir);
  server_run(local_add, port);
  return 0;
}
	
      
  

  
  
