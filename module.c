#include<dlfcn.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include "server.h"
char* module_dir;
ser_mod* module_open (const char* module_name)
{
  char* mod_path;
  void* handle;
  void(* module_generate) (int);
  ser_mod* module;
  mod_path = (char*) xmalloc (strlen(module_dir) + strlen(module_name) + 2);
  sprintf(mod_path,"%s/%s",module_dir,module_name);
  handle = dlopen(mod_path,RTLD_NOW);
  free(mod_path);
  if(handle == NULL)
    return NULL;
  module_generate = (void (*) (int)) dlsym(handle, "module_generate");
  if(module_generate == NULL){
    dlclose(handle);
    return NULL;
  }
  module = (ser_mod*) xmalloc(sizeof(ser_mod));
  module->handle = handle;
  module->name = xstrdup(module_name);
  module->generate_funtion = module_generate;
  return module;
}

void module_close(ser_mod* module)
{
  dlclose(module->handle);
  free((char*)module->name);
  free(module);
}
  
  
    
    
  

  
