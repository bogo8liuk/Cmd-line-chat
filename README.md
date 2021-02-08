# Cmd-line-chat
### How it works
Cmd-line-chat is a simple project that allows two terminals to communicate
via a TCP channel. It is thought for more than two terminals. It has two
executable files: `cl_reply` and `cl_request`. The first one has to be executed
passing it the ip address and the port number that represent the point to
connect to:
```bash
$ cl_reply <ip_address> <port_number>
```
The second one has to be executed passing it the same ip address and the same
port number of the terminal that executed `cl_reply` in order to connect to it:
```bash
$ cl_request <ip_address> <port_number>
```
At this point, the two terminals are connected and they can "chat".
A terminal can have two modes:
* The _command mode_, where you can type a command, but you cannot write messages;
the possible commands are: 
1. 'w' to enter the insert mode; 
2. 'e' to exit from the
chat and to close the connection; 
3. 'h' to prompt a help message that describes the possible commands you can do.
* The _insert mode_, where you can write a message (of the max size of 4096
characters). This typed message will be sent to the other point only when the
newline character will be typed. Be aware that when you are in this mode, you
cannot receive and see messages from the other terminal.
### Requirements (on Ubuntu 20.04)
This project has been developed with C11. The CMake required version is the 3.16.
### Build
To build the project, simply run the following commands:
```bash
$ cmake .
$ make
```
