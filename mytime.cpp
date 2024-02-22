
#include <sys/resource.h>
#include <unistd.h>
#include <time.h>
#include <iomanip>
#include <sys/wait.h>
#include <iostream>
using namespace std;


/*
 * This method displays the time as per the format of time command.
 * Arguments: Taking real elapsed time , user CPU time and System CPU time
 * Displaying in the format like time command
 */
void displaytimedata(double realtime, double usertime, double systemtime)
{

    cout << "real\t" << setprecision(3)<< realtime << "s" << endl;
    cout << "user\t" <<setprecision(3)<< usertime << "s" << endl;
    cout << "sys \t" <<setprecision(3) << systemtime << "s" << endl;
}

/**
 * This is main function that calculates the actual clocktime passed , and other two CPU times.
 * @param argc : the argument counts
 * @param argv : the arguments in character array
 * @return
 */
int main(int argcount, char* argument[]) {
    const double MICRO_SEC = 1000000.0;
    struct rusage usage;
    int status;
    if (argcount < 2) {
        displaytimedata(0.00,0.00,0.00);
        return 1;
    }
    struct ::timespec startrecord, stoprecord;
    pid_t pid = fork();
    if (pid == 0) {
        // child process
        execvp(argument[1], argument + 1);
        cerr << "execvp failed due to error in execution, check your command:" <<argument[1]<< endl;
        return 1;
    } else if (pid > 0) {
        // this is parent process
        // STOP watch LAP 1
        clock_gettime(CLOCK_MONOTONIC,&startrecord);
        // waiting for child process to complete,
        // note that the successfull return statement by execvp is 0, in which case the program will execute successfully
        // if for some resaons , the process is not executed by execvp, it gets returned
        wait(&status);
        // STOP watch LAP 2
        clock_gettime(CLOCK_MONOTONIC,&stoprecord);

        //checking successfull execution of child process
        if (status==0)
        {
            // this is a call used to retrieve information regarding the process running,
            // In our case, child process, so appropriate Macro is used.
            getrusage(RUSAGE_CHILDREN, &usage);
            //fetching system cpu time
            double sctime;
            // calculating micro second time for the process to execute, dividing it by 10 Pow 6 to convert it to seconds
            double msctime = (double) usage.ru_stime.tv_usec / MICRO_SEC;
            // adding the seconds elapsed for system time with the microsecond elapsed
            sctime = (double)usage.ru_stime.tv_sec + msctime;

            // fetching user CPU time
            double uctime;
            // calculating micro second time for the process to execute, dividing it by 10 Pow 6 to convert it to seconds
            double muctime = (double)usage.ru_utime.tv_usec / MICRO_SEC;
            // adding the seconds elapsed for cpu time with the microsecond elapsed
            uctime = (double)usage.ru_utime.tv_sec + muctime;

            // calculating the clock time difference of stop watch lap 2 and lap 1
            double etime;
            // calculating the elapsed time in nano seconds , dividing my 10 pow 9 to convert it to second
            double entime = (stoprecord.tv_nsec - startrecord.tv_nsec) / 1000000000.0;

            // adding the time passed in seconds to that of nano seconds(which is converted to seconds in above step)
            etime = (stoprecord.tv_sec - startrecord.tv_sec)+  entime;

            // display actual stats
            displaytimedata(etime,uctime,sctime);
            return 0;
        }
        else
        {
            // if invalid unix command is given , execvp will fail, causing the status to return non zero,
            // in which case the 0 time shpuld be displayed
            displaytimedata(0.00,0.00,0.00);
            exit(1);
        }
    } else {
        cout << "Error in creating child process!" << endl;
        return 1;
    }
}


