#include <vector>
#include <string>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>

using namespace std;

/*
 * NOTE: I have taken inspiration of this program from professor's lecture's example:
 * the example is : https://www.cs.fsu.edu/~duan/classes/cop5570/examples/lect6/example6.c
 * */


void sigaction_handler(int signum) {
    exit(1);
}

int main(int argc, char* argv[]) {

    int timeout = 0;
    int stat;

    vector<string> commandarg;


    // checking if there are adequate number of arguments for doing timeout
    if (argc <= 2) {
        cout<<"$ please enter appropriate command for timeout"<< endl;
        exit(1);
    }

    // stoi is used here to convert from string to integer,
    // note that the first argument will be an integer value for timeout

    timeout = stoi(argv[1]);


    // start from second element as first will
    // be execu name and second is stored in the integer timeout
    int elem = 2;

    while(elem < argc)
    {
        // I was using pushback , but for c++11 , emplace_back seems to be more promising.
        // reference: https://en.cppreference.com/w/cpp/container/vector/emplace_back
        commandarg.emplace_back(argv[elem]);
        elem ++;
    }

    // creating process to execute the command other than
    // the timeout seconds
    pid_t pid = fork();
    if (pid == -1) {
        cerr << "Failed to fork child process" << endl;
        return 1;
    } else if (pid == 0) {

        // +1 for nullptr , as required by execvp.
        char* arraytosend[commandarg.size() + 1];

        for (int i = 0; i < commandarg.size(); i++) {
            arraytosend[i] = &commandarg[i][0];
        }

        //last argument needs to be null pointer for execvp
        arraytosend[commandarg.size()] = nullptr;

        // calling the execvp function to execute
        // the command on which timeout will happen
        execvp(arraytosend[0], arraytosend);

        // sometinh is gravely wrong
        cerr << "Failed to execute command" << endl;
        return 1;
    } else {
        // Parent process - set timeout and wait for child process

        struct sigaction to_sa;
        to_sa.sa_handler = sigaction_handler; // this method is called when signal is recieved
        sigemptyset(&to_sa.sa_mask);
        to_sa.sa_flags = 0; // using default flags for signal handler

        if (sigaction(SIGALRM, &to_sa, nullptr) == -1) {
            cerr << "Error: sigaction() failed with errno " << errno << std::endl;
            return 1;
        }

        /* Major breakthrough : child process also recieves the signals that parent process recieves
        I had been playing around with alarm to send SIGALRM in child process to stop it
        I was not able to find a way as child process is executing the command and cannot
         send signal at the same time,I later on got realization that child process also gets the SIGALRM that
         the parent process aka this process has set via alarm function
         */

        // here a timer is set at timeout seconds, after that seconds have
        //elaspsed , a signal SIGALRM will be set
        alarm(timeout);

        waitpid(pid, &stat, 0); // waiting on child process
    }
}

