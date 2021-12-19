# Linux-app
**Implements a complete GNU/Linux program**
The example program is part of a system for monitoring a running GNU/Linux system. It includes these features:
-> The program incorporates a minimal Web server. Local or remote clients access system information by requesting Web pages from the server via HTTP.
-> The program does not serve static HTML pages. Instead, the pages are generated on the fly by modules, each of which provides a page summarizing one aspect of the system’s state.
-> Modules are not linked statically into the server executable. Instead, they are loaded dynamically from shared libraries. Modules can be added, removed, or replaced while the server is running.
-> The server services each connection in a child process.This enables the server to remain responsive even when individual requests take a while to complete, and it shields the server from failures in modules.
-> The server does not require superuser privilege to run (as long as it is not run on a privileged port). However, this limits the system information that it can collect.
