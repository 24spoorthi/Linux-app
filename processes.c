#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <sys/uio.h>
#include "server.h"

static int get_uid_gid(pid_t pid, uid_t* uid, gid_t* gid)
{
  char d_name[64];
  struct stat d_info;
  int rval;
  snprintf(d_name,sizeof(d_name),"/proc/%d",(int) pid);
  rval = stat(d_name,&d_info);
  if(rval != 0)
    return 1;
  assert(S_ISDIR(d_info.st_mode));
  *uid = d_info.st_uid;
  *gid = d_info.st_gid;
  return 0;
}

static char* get_username(uid_t uid)
{
  struct passwd* entry;
  entry = getpwuid(uid);
  if(entry == NULL)
    system_error("getpwuid");
  return xstrdup(entry->pw_name);
}

static char* get_groupname(gid_t gid)
{
  struct group* entry;
  entry = getgrgid(gid);
  if(entry == NULL)
    system_error("getgrgid");
  return xstrdup(entry->gr_name);
}

static char* get_program_name (pid_t pid)
{
  char f_name[64];
  char st_info[256];
  int fd;
  int rval;
  char* open_paren;
  char* close_paren;
  char* res;
  snprintf (f_name, sizeof (f_name), "/proc/%d/stat", (int) pid);
  fd = open(f_name,O_RDONLY);
  if(fd == -1)
    return NULL;
  rval = read (fd, st_info, sizeof (st_info) - 1);
  close(fd);
  if(rval <= 0)
    return NULL;
  st_info[rval] = '\0';
  open_paren = strchr (st_info, '(');
  close_paren = strchr (st_info, ')');
  if(open_paren == NULL || close_paren == NULL || close_paren < open_paren)
    return NULL;
  res = (char*) xmalloc (close_paren - open_paren);
  strncpy(res,open_paren + 1, close_paren - open_paren - 1);
  res[close_paren - open_paren - 1] = '\0';
  return res;
}

static int get_rss(pid_t pid)
{
  char file_name[64];
  int fd;
  char mem_info[128];
  int rval;
  int rss;
  snprintf(file_name, sizeof (file_name), "/proc/%d/statm", (int) pid);
  fd = open(file_name, O_RDONLY);
  if(fd == -1)
    return -1;
  rval = read (fd, mem_info, sizeof (mem_info) - 1);
  close(fd);
  if(rval <= 0)
    return -1;
  mem_info[rval] = '\0';
  rval = sscanf(mem_info,"%*d %d",&rss);
  if(rval != 1)
    return -1;
  return rss * getpagesize() /1024;
}
static char* format_process_info(pid_t pid)
{
  int rval;
  uid_t uid;
  gid_t gid;
  char* u_name;
  char* g_name;
  int rss;
  char* p_name;
  size_t res_len;
  char* res;
  rval = get_uid_gid(pid, &uid,&gid);
  if(rval != 0)
    return NULL;
  rss = get_rss(pid);
  if(rss == -1)
    return NULL;
  p_name = get_program_name(pid);
  if(p_name == NULL)
    return NULL;
  u_name = get_username (uid);
  g_name = get_groupname (gid);
  res_len = strlen(p_name)+strlen(u_name)+strlen(g_name)+128;
  res = (char*) xmalloc(res_len);
  snprintf(res,res_len,"<tr><td align=\"right\">%d</td><td><tt>%s</tt></td><td>%s</td>""<td>%s</td><td align=\"right\">%d</td></tr>\n",(int) pid,p_name,u_name,g_name,rss);
  free (p_name);
  free (u_name);
  free (g_name);
  return res;
}

static char* page_start =
  "<html>\n"
  " <body>\n"
  " <table cellpadding=\"4\" cellspacing=\"0\" border=\"1\">\n"
  "<thead>\n"
  "<tr>\n"
  "<th>PID</th>\n"
  "<th>Program</th>\n"
  "<th>User</th>\n"
  "<th>Group</th>\n"
  "<th>RSS&nbsp;(KB)</th>\n"
  "</tr>\n"
  "</thead>\n"
  "<tbody>\n";

static char* page_end =
  "</tbody>\n"
  " </table>\n"
  " </body>\n"
  "</html>\n";

void module_generate(int fd)
{
  size_t i;
  DIR* proc_list;
  size_t vec_length = 0;
  size_t vec_size = 16;
  struct iovec* vec = (struct iovec*)xmalloc(vec_size* sizeof(struct iovec));
  vec[vec_length].iov_base = page_start;
  vec[vec_length].iov_len = strlen(page_start);
  ++vec_length;
  proc_list = opendir("/proc");
  if(proc_list == NULL)
    system_error("opendir");
  while(1){
    struct dirent* proc_entry;
    const char* name;
    pid_t pid;
    char* pro_info;
    proc_entry = readdir(proc_list);
    if(proc_entry == NULL)
      break;
    name = proc_entry->d_name;
    if(strspn(name, "0123456789") != strlen(name))
      continue;
    pid = (pid_t) atoi(name);
    pro_info = format_process_info(pid);
    if(pro_info == NULL)
      pro_info = "<tr><td colspan=\"5\">ERROR</td></tr>";
    if(vec_length == vec_size - 1){
      vec_size *= 2;
      vec = xrealloc(vec,vec_size * sizeof(struct iovec));
    }
    vec[vec_length].iov_base = pro_info;
    vec[vec_length].iov_len = strlen(pro_info);
    ++vec_length;
  }
  closedir(proc_list);
  vec[vec_length].iov_base = page_end;
  vec[vec_length].iov_len = strlen (page_end);
  ++vec_length;
  writev(fd,vec,vec_length);
  for(i = 1 ; i < vec_length-1 ; i++)
    free(vec[i].iov_base);
  free(vec);
}
  
    

  
