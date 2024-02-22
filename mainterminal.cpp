#include <iostream>
#include <fstream>
#include <regex>
#include <sys/wait.h>
#include <fcntl.h>
#include <cerrno>
#include <sstream>
#include <functional>
#include <unordered_map>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <string>

using namespace std;

// this map is used to to map to the integer equivalent of the commands so that
// they can be used in switch case.
// i had to do this because cpp does not support string cases , as string is a third party lib.
const static unordered_map<string,int> stringtointeger{
        {"myexit",1},
        {"mypwd",2},
        {"mycd",3},
        {"mytree",4},
        {"mytime",5},
        {"mytimes",6},
        {"mytimeout",7},
};


/*
 * Arguments : no arguments
 * return : void
 * This function: getting the current working directory with the command mypwd
 */
void mypwd()
{
   char printwd[2000]; // where we will store results of mypwd

   if (getcwd(printwd, sizeof(printwd)) == nullptr)
   {
       cout << "There is some issue with fetching current working directory!!\n";
   }
   cout<< printwd <<endl;
}

/*
 * Arguments :
 * firstcmd:  the command to chaneg the directory to
 * return : void
 * This function: called for changing the directory
 */
void mycd(string firstcmd)
{
    int execcheck;
    execcheck = chdir(firstcmd.c_str());// actual code where directory is changed

    // checking for errors if chdir was not successfull.
    if (execcheck != 0) {
        switch (errno) {
            case ENOENT:
                cerr << "Error: The given directory does not exists" << endl;
                break;
            case ENOTDIR:
                cerr << firstcmd << " is not a directory, Please Check" << endl;
                break;
            case EACCES:
                cerr << "Permission denied for directory " << firstcmd << endl;
                break;
            default:
                cerr << "Error: Failed to change directory. errno=" << errno << endl;
                break;
        }
    }
}

/*
 * Arguments :
 * firstcmd:  the command to exit the terminal
 * return : void
 * This function: // this is  a simple exit function, with status 0.
 */
void myexit()
{
 exit(0);
}


/*
 * Arguments :
 * commandarg:  the vector that has all the arguments of commands seperated by white spaces in string format
 * return : int , mostly status.
 * This function: to call external commands like mv , mkdir  , ls and so on
 */
void external_command(vector<string> commandarg)
{
    char** chararray = new char*[commandarg.size() + 1]; // +1 for null pointer
    int childpros = fork(); // creating child process
    if (childpros == -1) {
        cerr << "Error forking process" << endl;
        exit(1);
    }
    // going inside child process by checking its id
    if (childpros == 0) {
//        cout<<"here1";
        // inside child process

            // converting vector string to char array to fit to syntax of execvp
            for (int i = 0; i < commandarg.size(); i++) {
                chararray[i] = &commandarg[i][0];
            }
            chararray[commandarg.size()] = nullptr; // null pointer because to terminate the char array

            // actually executing the external command through child process process
            execvp(chararray[0], chararray);
            // If we get here, there was an error with exec
            cerr << "Error in " << commandarg[1] << " command, Please Check!" << endl;
            exit(1);
        }
    // this is parent process
    int st;
    waitpid(childpros, &st, 0); // waiting to finish  chid process, after that we return back to while loop in main.
}


/*
 * Arguments :
 * arg:  the vector that has all the arguments of commands seperated by white spaces in string format
 * return : int , mostly status.
 * This function: to call external programs like mytree mytimout mytimes and mytime
 * Note that the difference between this function and the function above is
 * just the first command , we could have also reused the above function , as the code is similar to above function
 * but i wanted to keep distincion and maintain modularity between general external commands and commands called externally.
 */
void other_my(vector<string>  vectorcommands)
{
//    arg[0] = "/Users/maitrychauhan/CLionProjects/cpdpproj2/"+arg[0];
    // assuming that path is already set
    char** chararray = new char*[vectorcommands.size() + 1]; // +1 for null pointer
    for (int i = 0; i < vectorcommands.size(); i++) {
        chararray[i] = &vectorcommands[i][0];
    }
    chararray[vectorcommands.size()] = nullptr; // termintae the array


    // creating child process to run external commands
    pid_t chprocess = fork();
    if (chprocess == 0) {
        // in child process
        if (execvp(chararray[0],chararray) == -1) {
            // If execvp returns -1, an error occurred
            cerr << "Error in execution of external command" << endl;
            exit(1);
        }
    } else if (chprocess == -1) {
        cerr << " Error forking a child process" << endl;
        exit(1);
    } else {
        // in parent process
        int status;
        waitpid(chprocess, &status, 0);
    }
}

/*
 * Arguments :
 * arg:  the vector that has all the arguments of commands seperated by white spaces in string format
 * return : int.
 * This function: to call external programs and external commands.
 */
int myCommands(vector<string> wholeinput) {
    // if the string command does not match with any of the map keys , call it as an external command
    // eg ls , mkdir
    if (stringtointeger.count(wholeinput[0]) <= 0)
    {
        external_command(wholeinput);
    }
    else
    {
        // converting from string to integer with stringtointeger map declared at the start.
        // switch case is to make the code more organised and we can easily scale it
        switch (stringtointeger.at(wholeinput[0])) {
            case 1:
                myexit(); // to exit
                break;
            case 2:
                mypwd(); // call mypwd
                break;
            case 3:
                mycd(wholeinput[1]); // change dir
                break;
            case 4: // mytree
            case 5: // mytime
            case 6: // mytimes
            case 7: // mytimeout
                other_my(wholeinput); // all 4,5,6,7 cases will call this.
                break;
            default:
                break;
        }
    }
 return 0;
}

/*
 * Arguments :
 * commandarg:  the vector that has all the arguments of commands seperated by white spaces in string format ,
 * index1: the integer value whichis the position of first | occuring in command arg
 * index2 :  the integer value whichis the position of second | occuring in command arg
 * return : void.
 * This function: to execute pipe commands that have two pipes.
 */
void twopipesfunction(vector<string> commandarg, int index1 , int index2)
{
    // creating pipe arrays for two pipes as this function is for two pipes
    int frstpipe[2]; // first pipe in commands
    int scndpipe[2]; // second pipe in commands

    // creating a seperate vector for first command
    vector<string> command1(commandarg.begin(), commandarg.begin() + index1);

    // converting the above vector for command 1 into a character array to fit for the syntax of execvp
    char* arr_first_ex[command1.size() + 1];
    for (int i = 0; i < command1.size(); i++) {
        arr_first_ex[i] = &command1[i][0];
    }
    // creating a seperate vector for second command
    vector<string> command2(commandarg.begin() + index1 + 1, commandarg.begin() + index2);

    // converting the above vector for command 2 into a character array to fit for the syntax of execvp
    char* arr_second_ex[command2.size() + 1];
    for (int i = 0; i < command2.size(); i++) {
        arr_second_ex[i] = &command2[i][0];
    }

    // creating a seperate vector for third command
    vector<string> command3(commandarg.begin() + index2 + 1, commandarg.end());

    // converting the above vector for command 3 into a character array to fit for the syntax of execvp
    char* arr_third_ex[command3.size() + 1];
    for (int i = 0; i < command3.size(); i++) {
        arr_third_ex[i] = &command3[i][0];
    }

    // last element in arrays needs to be null pointer, so that execvp knows where command ends
    arr_first_ex[command1.size()] = nullptr;
    arr_second_ex[command2.size()] = nullptr;
    arr_third_ex[command3.size()] = nullptr;

    // calling and creating pipes for above declared pipes
    if (pipe(frstpipe) == -1 || pipe(scndpipe) == -1) {

        // if returned type is -1 , pipe was not created.
    cerr << "one or both pipes cannot be created, Some issue is encountered." << endl;
    exit(1); // exit program on failure of pipe creation
    }

    // creating first child process to execute first commad
    int frstchld = fork();
    if (frstchld == -1) {
    cerr << "Failed to fork first child process" << endl;
    exit(1);
    }

    else if (frstchld == 0)
    {
    // First child - Here the first commad is executed and output is given to the first pipe from frst command

    // closing std output because we dont want the first command's output to be displayed in stdout aka 1.
    close(STDOUT_FILENO);

    // duplicating write end of first pipe , as we closed the std out , this pipe get attached to the previously closed
    // descripter for std out, so this std out 1 is taken by this process now for writing
    dup(frstpipe[1]);


    // here we are closing read end of first pipe because the output generated from execvp will go to stdout
    // which in above steps we pointed and redirected to the write end of the pipe , so here in this process,
    // we are only writing to pipe so we dont need the read end open.
    close(frstpipe[0]); // Close write end of first pipe

    // second pipe is no where in the picture right now , we dont need them for this command, so
    // we are cloing both of its ends , i.e read and write
    close(scndpipe[0]); // read closed
    close(scndpipe[1]); // write closed


    // actually executing the first command
    if (execvp(arr_first_ex[0], arr_first_ex) == -1) {
    cerr << "Error in executing the first command of pipe, the error is:  " << errno << endl;
    exit(1);
    }
    }

    // creating second child process to execute second command
    int scndchld = fork();
    if (scndchld == -1) {
        // checking is second child process is created successfully, exiting on any error
    cerr << "Failed to fork the second child process" << endl;
    exit(1);
    } else if (scndchld == 0) {
        // second child - Here the second commad is executed and output is given to the second pipe from frst command

        // closing std input because we are not getting the input from keyboard , we are getting input from the write
        // end of first pipe.
        close(STDIN_FILENO);

        // duplicating the read end of first pipe, so that it can get the value of std input that
        // we just closed in previous step, so instead of getting input from keyboard , the command executed by
        // execvp will get input from first pipe's read end.
        dup(frstpipe[0]);

        // closing the write end of first pipe because for this pipe , at this stage ,
        // we only want to read from the pipe, the write end is not needed because we are
        // done with writing in the first child process, and we dont need the write end.
        close(frstpipe[1]);

        // closing the std out so that whatever data we fetched from the read end of first pipe ,
        // we dont write it to std out , instead we write it to the write end of second pipe
        close(STDOUT_FILENO); // Close unused read end of second pipe

        // for the reason mentioned in above step , duplicating the write end of second pipe
        // so that std out gets assigned to this pipe's descriptor, so when execvp executes for second child ,
        // the output is written to this pipe
        dup(scndpipe[1]);

        // closing the read end of second pipe because we are just writing to this pipe and not reading anything
        // righnow for this process, we will need this third child process
        close(scndpipe[0]);

        // actually executing the second command
        if (execvp(arr_second_ex[0], arr_second_ex) == -1) {
        cerr << "Error in executing the second command of pipe, the error is: " << errno << endl;
        exit(1);
        }
    }

    // creating third child process to execute third command
    int thrdchld = fork();
    if (thrdchld == -1) {
    cerr << "Failed to fork third child process" << endl;
        exit(1);
    } else if (thrdchld == 0) {
        // third child - Here the third command is executed and input is taken from second pipe

        // closing std input because we are not getting the input from keyboard , we are getting input from the write
        // end of second pipe.
        close(STDIN_FILENO);

        // duplicating the read end of second pipe, so that it can get the value of std input that
        // we just closed in previous step, so instead of getting input from keyboard , the command executed by
        // execvp will get input from second pipe's read end.
        dup(scndpipe[0]);

        // closing write end of both the pipes because we are now not writing anything to pipes , we will write to
        // stdout only
        close(scndpipe[1]);
        close(frstpipe[1]);

        // actually executing the third command
        if (execvp(arr_third_ex[0], arr_third_ex) == -1) {
            cerr << "Error in executing the third command of pipe, the error is: " << errno << endl;
            exit(1);
        }
    }

    // inside parent process
    // close all the pipes opened in child process
    close(frstpipe[0]);
    close(frstpipe[1]);
    close(scndpipe[0]);
    close(scndpipe[1]);

    // waiting for completion of all child processes
    waitpid(frstchld, nullptr, 0);
    waitpid(scndchld, nullptr, 0);
    waitpid(thrdchld, nullptr, 0);
}

/*
 * Arguments :
 * commandarg:  the vector that has all the arguments of commands seperated by white spaces in string format ,
 * index: the integer value whichis the position of | occuring in command arg
 * return : void.
 * This function: to execute pipe commands that have only one pipe.
 */
void singlepipfunction(vector<string> commandarg , int index)
{

    // creating two char arrays to fit the syntax of execvp , as the command we have are stored in vectors
    char** frst_arr_char = new char*[index + 1]; // +1 for null pointer
    char** scnd_arr_char = new char*[commandarg.size() - index]; // argument 2 will be all the data after |

    // creating pipe for inter process communication, aka between two child process
    int pipefiled[2];


    // creating pipe to transfer the output of first command
    if (pipe(pipefiled) == -1) {
        cerr << "Failed to create pipe" << endl;
        exit(1);
    }

    // creating first child process
    int chld = fork();
    if (chld == -1) {
        cerr << "Failed to fork first child process" << endl;
        exit(1);
    } else if (chld == 0) {
        // First child - Here the first commad is executed and output is given to the first pipe from frst command

        // closing std output because we dont want the first command's output to be displayed in stdout aka 1.
        close(STDOUT_FILENO);

        // duplicating write end of  pipe , as we closed the std out , this pipe get attached to the previously closed
        // descripter for std out, so this std out 1 is taken by this process now for writing
        dup(pipefiled[1]);

        // here we are closing read end of pipe because the output generated from execvp will go to stdout
        // which in above steps we pointed and redirected to the write end of the pipe , so here in this process,
        // we are only writing to pipe so we dont need the read end open.
        close(pipefiled[0]);

        // converting the string vector for command 1 into a character array to fit for the syntax of execvp
        for (int i = 0; i < index; i++) {
            frst_arr_char[i] = &commandarg[i][0];
        }

        // last element in array needs to be null pointer, so that execvp knows where command ends
        frst_arr_char[index] = nullptr;

        // actually executing the first command
        execvp(frst_arr_char[0], frst_arr_char);
        cerr << "Error in executing the first command of pipe, the error is: " << errno << endl;
        exit(1);
    }

    // Fork the second child process
    int chld2 = fork();
    if (chld2 == -1) {
        cerr << "Failed to fork second child process" << endl;
        exit(1);
    } else if (chld2 == 0) {
        // First child - Here the first commad is executed and output is given to the first pipe from frst command

        // closing std input because we are not getting the input from keyboard , we are getting input from the write
        // end of pipe.
        close(STDIN_FILENO);

        // duplicating the read end of pipe, so that it can get the value of std input that
        // we just closed in previous step, so instead of getting input from keyboard , the command executed by
        // execvp will get input from  pipe's read end.
        dup(pipefiled[0]);

        // closing write end of the pipe because we are now not writing anything to pipes , we will write to
        // stdout only
        close(pipefiled[1]);

        // converting the string vector for command 1 into a character array to fit for the syntax of execvp
        for (int i = index + 1; i < commandarg.size(); i++) {
            scnd_arr_char[i - index - 1] = &commandarg[i][0];
        }

        // last element in array needs to be null pointer, so that execvp knows where command ends
        scnd_arr_char[commandarg.size() - index - 1] = nullptr;

        // actually executing the second command
        execvp(scnd_arr_char[0], scnd_arr_char);
        cerr << "Error in executing the second command of pipe, the error is: " << errno << endl;
        exit(1);
    }

    // inside parent process
    // close all the pipes opened in child process
    close(pipefiled[0]);
    close(pipefiled[1]);

    // waiting for completion of all child processes
    waitpid(chld, nullptr, 0);
    waitpid(chld2, nullptr, 0);
}

/*
 * Arguments :
 * commandarg:  the vector that has all the arguments of commands seperated by white spaces in string format ,
 * chboth:  bool value to true if redirectaion has both <  and >
 * chg :   bool value to true if redirectaion has >
 * chl : bool value to true if redirectaion has <
 * return : void.
 * This function: to execute the comands with redirection operators
 */
void redirectionfunction(bool chl,bool chg , bool chboth , vector<string> commandarg)
{
    // we know the original values od in and op , we just didnt want to assume
    int inputoriginal = STDIN_FILENO; // will be 0
    int outputoriginal = STDOUT_FILENO; // will be 1
    int fd = 0;

    // this array is maninly to fit into the syntax of execvp
    char** arr_cmd = new char*[commandarg.size()-2];
    int index = -1;

    // creating child process and fetching its pid, if pid is zero , we are in child process
    // if pid is > 0 , we are in parent process
    int chld = fork();
//    string filename;
    if (chld == -1) {
        cerr << "Error forking process" << endl;
        exit(1);
    }
    if (chld == 0) {
        // Find the index of the target element
        string symbol;
        if (chg) // for only greater than operator for redireaction
        {
            symbol = ">";
            for (int i = 0; i < commandarg.size(); i++) {
                if (commandarg[i] == symbol) {
                    index = i;
                    break;
                }
            }

            // here fd is file descriptor , it can be any value other than o 1 2
            // we are opening the file to get its file descriptor  value
            // not that if file is not present , it will be created using O_CREAT
            // also remeber that we are using > and < opertaor and not >> and <<
            // so we dont need to append to the files , we simply need to overwrite to the files
            // so i have used O_TRUNC to overwrite if file already exists.
            fd = open(commandarg[index + 1].c_str(), O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);

            // Important //
            /*
               Here , dup function is used which copies the file descriptor entry to STDOUT_FILENO
             * Note that STDOUT_FILENO is nothing but 1, so the integer value of fd whicever it is
             * which is fetched during open operation of file, let say fd was given value 4 after
             * doing open function , now after dup , it copies this value of 4 to STDOUT_FILENO, so now
             * instead of 1, stdout_fileno has value 4. we could have also used fd1 = dup(STDOUT_FILENO)
             * but that will become chaotic as we will also have to close standard output so dup2
             * seemed more easy and less confusing to implement. we do this because we dont want the output to go to
             * terminal , which was 1(original value of standard op), we want it to go to the file.
             *
             * Major breakthrough : Also note that original value
             * does not get reassigned after doing close on fd. I had a wrong impression that
             * close function restores the FD's for standard IN/OP. close just closes the fd so it can be used
             * by some other process. we will discuss later how std inp and oup values are restored
             */
            dup2(fd, STDOUT_FILENO);
        }
        if (chl || chboth) // for  < operator or both redirecation operator
        {
            symbol = "<";
            for (int i = 0; i < commandarg.size(); i++) {
                if (commandarg[i] == symbol) {
                    index = i;
                    break;
                }
            }

            // similar working to what explained above , but this is only readonly because
            // if the file isnt present, it should raise eror for reading , we are reading the file here
            fd = open(commandarg[index + 1].c_str(), O_RDONLY);

            /*
             * dup2 is explained above , the only differnece here is lets say the value of
             * fd here is 8 , stdin_fileno has value 0 , after doing dup2, it becomes 8, we do this becuase
             * we dont want the input from keyboard , instead we want it from the file pointed by fd here.
             * */
            dup2(fd, STDIN_FILENO);

        }
        if (chboth)
        {
//            int index1 = ;
            int fdnew;
            fdnew = open(commandarg[index + 3].c_str(), O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
            dup2(fdnew, STDOUT_FILENO);
            close(fdnew);
        }
        if (fd == -1) {
            cerr << "Failed to open file" << endl;
            exit(1);
        }

        // close the fd so that it can be reused by other process.
        close(fd);

        // converting the vectors to char array which can fit the execvp syntax
        // we have used index -1 because index is the position of first redirect operator and
        // the actual command is just to the left of index
        for (int i = 0; i < commandarg[index-1].size(); i++) {
            arr_cmd[i] = &commandarg[i][0];
        }


        if (chboth)
        {
            // here we have subtracted from 4 becasue , there will be 4 extra elements in array
            // other than the first command
            // i.e > < and two file names
            arr_cmd[commandarg.size()-4] = nullptr;
        }
        else
        {
            // here we have subtracted from 2 because , there will be 2 extra elements in array
            // other than the first command
            // i.e > or < and one file name
            arr_cmd[commandarg.size()-2] = nullptr;

        }

        // actually calling/executing the first command
        if (execvp(arr_cmd[0], arr_cmd) == -1)
        {
            cerr << "The given command didnt work!";
            exit(1);
        }

    }
    int st;
    waitpid(chld, &st, 0); // waiting for child process to end.
    /*
     *
     *
     * here is where the std inp and output values are restored, we saved the original
     * values in a local variable shown in first arg here and then we manually reassign them
     * note that they would have reassigned when we would restart the prog,
     * but as we are using recurring terminal , which is one big process and various
     * child process , we need our standard ip and op in order.
     *
     * Update : i realized later on  that parent dosnt get affected by canges of values
     * of standard input and output's descriptors in child process
     * so the below lines dont make any changes
     * */
    dup2(inputoriginal,STDIN_FILENO);
    dup2(outputoriginal,STDOUT_FILENO);
}



// main function, not taking ang cmd line args because we are not using any here
int main() {
    // infinite loop unles user exits with myexit or ctrld
    while (true) {
        string cmd; // whole input is taken in this string
        vector<string> allcmds; // vector of strings that have all the commands
//        size_t pos = 0;
//        string token;
        cout << "$ "; // printing $ for terminal
        getline(cin, cmd); // taking input from user
        bool chl = false ,chg = false, chboth= false; // variables for redirection
//        cout<<cmd<<endl;
        string inter;
        if (cin.eof()) {
            cout<<"End of file is encountered"<<endl;
            break;
        }
        istringstream strstream(cmd); // split the string by " "
        while (getline(strstream, inter, ' ')) {
//            token = cmd.substr(0, pos);
            if (inter != "") // this is checked to avoid adding elements for extra spaces
            {
                allcmds.push_back(inter);

            }
        }
        // check for redirection
        if ((cmd.find('>') != string::npos)  && (cmd.find('<') != string::npos))
        {
            redirectionfunction(chl=false, chg = false , chboth = true, allcmds);
            continue;
        }
        if (cmd.find('<') != string::npos)  {
            redirectionfunction(chl=true, chg = false , chboth = false , allcmds);
            continue;
        }
        if (cmd.find('>') != string::npos)
        {
            redirectionfunction(chl=false, chg = true , chboth = false, allcmds);
            continue;
        }

        // check for pipe
        if ((cmd.find('|') != string::npos))
        {
            string symbol = "|";
            int index = 0; // first index place for |
            int index1 = 0;// second index place for |
            for (int i = 0; i < allcmds.size(); i++) {
                if (allcmds[i] == symbol) {
                    if (index == 0)
                    {
                        index = i;
                    }
                    else
                    {
                        index1 = i;
                        break;
                    }
                }
            }
            // checking to see if there are two pipes in cmds
            if (index1 == 0)
            {
                singlepipfunction(allcmds , index); // when only one pipe is there
                continue;
            }
            else
            {
                twopipesfunction(allcmds, index, index1); // when two pipes are there
                continue;
            }
        }

        // all the external commands are here
        myCommands(allcmds);
    }
    return 0;
}


