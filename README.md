# Linux-app
**Implements a complete GNU/Linux program**

The example program is part of a system for monitoring a running GNU/Linux system. It includes these features:

-> The program incorporates a minimal Web server. Local or remote clients access system information by requesting Web pages from the server via HTTP.

-> The program does not serve static HTML pages. Instead, the pages are generated on the fly by modules, each of which provides a page summarizing one aspect of the system’s state.

-> Modules are not linked statically into the server executable. Instead, they are loaded dynamically from shared libraries. Modules can be added, removed, or replaced while the server is running.

-> The server services each connection in a child process.This enables the server to remain responsive even when individual requests take a while to complete, and it shields the server from failures in modules.

-> The server does not require superuser privilege to run (as long as it is not run on a privileged port). However, this limits the system information that it can collect.

**To run the program**

Clone the repository into your local machine and follow the steps below:
-> Building the program is easy, from the directory containing these sources invoke make.
  %make
  
-> To run the server, simply invoke the server executable.
If you do not specify the server port number with the --port ( -p ) option, Linux will choose one for you; in this case, specify --verbose ( -v ) to make the server print out the port number in use.

If you specify a different address, it must be an address that corresponds to your computer:
  % ./server --address localhost --port 4000
  
The server is now running. Open a browser window, and attempt to contact the server at this port number.Request a page whose name matches one of the modules.

For instance, 

-> to invoke the diskfree.so module, use this URL:
  http://localhost:4000/diskfree
  
-> to invoke the issue.so module
  http://localhost:4000/issue
  
-> to invoke the processes.so module
  http://localhost:4000/processes
  
-> to invoke the time.so module
  http://localhost:4000/time
