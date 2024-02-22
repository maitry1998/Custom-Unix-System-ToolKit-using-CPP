// Readme file
About My Unix Systems Toolkit (mytoolkit)

Project Statement:
The objective of this project was to develop a simple Unix systems toolkit for process and filesystem management. The toolkit's functionality resembles Unix shells like bash and Unix systems forensic toolkits such as Sleuth Kit.

Project Objectives:
Practice Unix system calls.
Understand Unix process management, synchronization, and inter-process communication (IPC).
Implement filesystem management tools.

Functionality Overview:
Upon initialization, mytoolkit displays a dollar sign ($) and a space, awaiting user input. The toolkit supports various commands, including "myexit", "mycd", and "mypwd". Additionally, it allows execution of external Unix commands, supports pipes, and enables I/O redirection.

Implemented Commands:

Internal Commands:
myexit: Terminates the toolkit.
mycd: Changes the current working directory. Syntax: mycd dir.
mypwd: Prints the absolute path of the current working directory.

External Commands:
All existing external Unix commands are supported, including:
mytree.x
mymtimes.x
mytimes.x
mytimeout.x: Runs a command with a time limit. Syntax: mytimeout.x snds cmd [cmd-args].

Notes:
Handling End of File: The toolkit terminates upon encountering the end of file character (CTRL-D).
No Zombie Processes: Long-term zombie processes are not created, ensuring proper process handling.
Makefile: A makefile is provided for easy compilation.
README: This document explains the implementation choices, limitations, and special requirements of the toolkit commands.

Limitations:
Special character expansion (e.g., *) is not supported.
Pipe and I/O redirection cannot appear together in any command line.
The "system()" library routine is not used.

Conclusion:
My Unix systems toolkit (mytoolkit) provides a comprehensive environment for process and filesystem management on Unix systems. It adheres to project specifications, handles process management efficiently, and enables seamless execution of both internal and external commands.

description of all files:
mainterminal.cpp : main source code that has whole code for running terminal.
mytree.cpp : source code for mytree program that mimics tree program.
mytime.cpp : source code for mytime program that mimics time program.
mytimes.cpp : source code for mytimes.cpp.
mytimeout.cpp : source code for mytimeout.cpp that mimics timeout.
terminal : object/ executable file for running the terminal
mytree : object / executable file for running mytree program
mytime : object / executable file for running mytime program
mytimes : object / executable file for running mytimes program
mytimeout : object / executable file for running mytimeout program


How to run the terminal?
GIST : command:
1. make
2. terminal

PLEASE NOTE:
While running the object file aka terminal , if it doesn't work , please try ./terminal , same goes for mytree, mytime, mytimes, and mytimeout. 
ALso the executable files for three external commands , i.e mytree , mytime and mymtimes is NOT mytree.x, mymtimes.x and mytime.x but it is just mytree, mytime and mymtimes to make it more command like when executing in toolkit terminal. The source code for these three program is same as last time because there were no functional changes. 


Explanation:
compile all the codes. To compile just run 'make' command. The make command will call makefile , which will compile all the source codes which includes mytree.cpp , mytime.cpp , mytimes.cpp , mytimeout.cpp and mainterminal.cpp. The object files will be mytree, mytime , mytimes , mytimeout and terminal. Now , once the code is compiled , to run the terminal , just type 'terminal' as this is the object file name for mainterminal.cpp

how to run individual external command outside terminal?
1. make
2. mytree or mytime or mytimes or mytimeout (with appropriate commands)

you can also compile individual files with make and filenname.


to clen all the executables , run make clean.


// assumption
I am able to run external programs in my terminal as i have set the path.

//Scope Of this terminal
this terminal supports all external commands like mv , mkdir , ls , etc.  it supports internal commands cd as mycd , exit as myexit and pwd as mypwd. It also supports io redirection and inter process communication through pipes.

// limitations
My toolkit only supports two pipes. It does not support more than two pipes.
