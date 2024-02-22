#include <dirent.h>
#include <ctime>
#include <algorithm>
#include <sys/stat.h>
#include <cstring>
#include <unistd.h>
#include <iostream>
using namespace std;


//this function is used to print the data in the following format
/*
 * Day Month Date HoursBack(in Current hour to last 24 hours) Year : Count of grouped files
 * Arguments : filecounter : Array that stores grouped file count per hours , with hour number as index
 *             timenow     : current time
 */
void printfiles(int * filecounter,time_t timenow)
{
    time_t disp_hour;
    int eachiter = 23;
    char bfr[100];
    while (eachiter >= 0) {
        // fetching timerepresenation of last 24 hours
        disp_hour = timenow - (eachiter * 60 * 60);
        // this function is used to convert the time in a required string format
        strftime(bfr, sizeof(bfr), "%a %b %d %T %Y", localtime(&disp_hour));
        cout << bfr << ": " << filecounter[eachiter]  << "\n";
        eachiter--;
    }
}

/*
 * This is controller logic which calculates the filecounts grouped by hour modification
 * Arguments: dir_path    : path of current file in the function call
 *            timenow     : current time
 *            filecounter : array that stores the count of files per hour modification as index
 */
void groupfiles(const string &dir_path,
                time_t timenow,int * filecounter) {
    DIR *currdir = opendir(dir_path.c_str());
    struct dirent *curritem;
    struct stat path_stat;
    string path;
    double hours;

    if (currdir == nullptr)
    {return;}

    // this loops executes until there is an entry in directory to process
    while( (curritem =readdir(currdir)) != nullptr)
    {
        // entry->d_name returns the subsequent file name under the directory
        if (strcmp(curritem->d_name, ".") != 0 && (strcmp(curritem->d_name, "..") != 0)) {
            // appening full path to the retrived file so as to check its status
            path = dir_path + "/" + curritem->d_name;
            //checking if file is valid and a regular file
            if ((lstat(path.c_str(), &path_stat) == 0) && (S_ISREG(path_stat.st_mode))) {
                //finding the nth hour in which file was modified
                hours = (timenow - path_stat.st_mtime) / 3600;
                // checking if files modified are in past 24 hours
                if (hours < 24) {
                    // increasing counter in group of last modified file
                    filecounter[int(hours)] = filecounter[int(hours)] + 1;

                }
            }
            //if file is directory , do recursion call to do the same jobs for files under that directory
            if (S_ISDIR(path_stat.st_mode)) {
                groupfiles(path, timenow, filecounter);
            }
        }
    }
    closedir(currdir);
}

int main(int no_arg, char *argument[]) {
    string sdir;
    int filecounter[24]= {0};
    char currentwdirectory[2000];
    time_t timenow = time(nullptr);
    struct stat buf;

    // checking if there is a directory mentioned in the command line argument
    if (no_arg == 2) {
        // checking if directory is valid
        if (lstat(argument[1], &buf) < 0) {
            cout << "Please check, "<< argument[1] << " , encountered issue with retrival \n";
            exit(0);
        }
        else if (!S_ISDIR(buf.st_mode)) // checking if the argument is directory or not
        {
            cout << "The given argument is not a directory";
            ::exit(0);
        }
        else
        {
            sdir = argument[1];// if a  valid directory is given , use that
        }
    }
    else
    {
        // if no arguments are given in command line , use current working directory
        getcwd(currentwdirectory, sizeof(currentwdirectory));
        sdir = currentwdirectory;
    }
    // function call to generate an array that counts the file per modification time
    groupfiles(sdir, timenow, filecounter);
    // display the time in the spectrum of last 24 hours along with the count
    printfiles(filecounter,timenow);
    return 0;
}


