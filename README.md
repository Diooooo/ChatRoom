# ChatRoom
UB 2018Fall CSE589 project 1

Develop the client and server components of a text chat application, consisting of one chat server and multiple chat clients over TCP connections.

## Developer
 * Bilin Shi
 * Wenqi Li

## Detailed Description
In this project, TCP sockets are ONLY allowed to be used both for sending and broadcasting, select() function is used for handling multiple socket connections.  
Commands that need to be implemented are listed below:
  * AUTHOR
  * IP
  * PORT
  * LIST
  * STATISTICS
  * BLOCKED <client-ip>
  * [EVENT]: Message Relayed
  * LOGIN <server-ip> <server-port>
  * REFRESH
  * SEND <client-ip> <msg>
  * BROADCAST <msg>
  * BLOCK <client-ip>
  * UNBLOCK <client-ip>
  * LOGOUT
  * EXIT
  * [EVENT]: Message Received
## Requirement
Linux/Unix: g++ 4.8 or newer version
## How to start
  After download the project, enter the main directory and execute the following commands:
  ```
  mkdir object
  make
  ```
  Then run
  ```
  ./assignment1 [s/c] [port]
  ```
  Here the first parameter means the type of terminal you want to start, s--server and c--client, the second is the port number you want to bind.
  After starting the program, enter the commands described above.
