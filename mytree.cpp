#include <unistd.h>
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <string>
using namespace std;


/*
 * This Function displays the content to be displayed.
 * It forms the files in a hirerarchal format.This function does not form the data all at once.
 * Arguments: printname : the file name
 *            descend : the herierchal level of the file
 */
void formdata(string printname,int descend){
    int a = 0;
    while (a < descend)
    {
        cout << "|   ";
        // for the levels which are less than actual level
        // of the file "printname", It should display "|   " for maintining the hirearchy like structure
        a = a+1;
    }
    // Once the hirearchy is established, It should then display the actual file in the desired format
    cout << "|---" << printname  << "\n";
}

/*
 * This function is called recursively to iterate over all the
 * directories and subdirectories to fetch all the files.
 * Arguments: descend: this is the hirearchal level of the individual files
 *            dir :: this is the directory the function is working under currently
 * */
void controlStructure(int descend, string dir) {
    DIR* dicpointer= opendir(dir.data()); // opening directory and setting pointer to the directory
    struct dirent* dirp;
    struct stat filetype;
    string fullpath;
    if(dicpointer == nullptr) {return;}
    dirp = readdir(dicpointer); // reading the directory which stores the pointer to the next item in directory

    // Looping over untill there are no files in the directory
    // also reading another entry from directory evertime loop executes
    while ((dirp =readdir(dicpointer)) != nullptr) {
        // fetching the absolute path of the file in iteration, This is used to get the status of file.
        fullpath = dir + "/" + dirp->d_name;

        // checking the status of file to see if it is successfully retrieved.
        if (lstat(fullpath.c_str(), &filetype) != 0) {
            cout << "an error occured in retrieving the file"<<"\n";
        }

        // checking if the type is a regular file
        if (S_ISREG(filetype.st_mode)) {
            // for the case when the encountered item is file,
            formdata(dirp->d_name,descend);
        } else {
            // this condition is executed everytime when a directory is encountered
            // . and .. represents the current and parent directory's entry
            if ((string(dirp->d_name) != ".") && (string(dirp->d_name) != "..") && (S_ISDIR(filetype.st_mode)) )  {
                formdata(dirp->d_name,descend);
                // recursively calling the function for the new directory that dirp->d_name encountered
                controlStructure(descend + 1,fullpath);
            }
        }

    }
    //closing the dir
    closedir(dicpointer);
}

int main(int argc, char* argv[]) {
    string dir;
    struct stat buf;
    if (argc >= 2 ) {
        // checking if directory is valid
        if (lstat(argv[1], &buf) < 0) {
            cout << "Error in retrieval, please check if directory exists."<<"\n";
            exit(0);
        }
            // check if the argument is a directory
        else if (!S_ISDIR(buf.st_mode))
        {
            cout << "The given argument is not a directory";
            exit(0);
        }
        else
        {
            // if everything is okay, take the argument as directory
            dir = argv[1];
            cout<<dir<<"\n";
        }
    } else {
        // if no argument is present, take current working directory
        char currentdir[2000];
        getcwd(currentdir, sizeof(currentdir));
        dir = currentdir;
        cout<<"."<<"\n";
    }
    // calling the method with initial directory
    controlStructure(0,dir);
    return 0;
}


