#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <cstdlib>
#include <sstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>



using namespace std;

char* edit_number(char *filechar, int numchange);                                            // function to edit the number in file

int main(int argc, char *argv[])
{
    int N = strtol (argv[2], NULL, 10);                                                     // number of child process, store third main argument, convert to int
    int i = 0;
    int fd[2], bytes;                                                                       // initialize pipe
    pipe(fd);
    string text;
    char infilechar[2048];
    char outfilechar[2048];
    //printf("Creating %d children\n", N);                                                  // testing purpose for child and parent ID
    //printf("PARENT PROCESS\nMy pid is:%d \n",getpid() );
    ifstream inputStream(argv[1]);                                                          // opens file name from second main argument,
    string contents((istreambuf_iterator<char>(inputStream)), istreambuf_iterator<char>()); // copies content in file to content
    strcpy (infilechar, contents.c_str());                                                  // copies content to char array = infilechar
    remove("outputfile.txt");                                                               // removes the outputfile if the process was ran previously

    pid_t parent = getpid();                                                                // saves the first process id into parent

    for(i = 0; i < N - 1; i++)                                                              // number of times child process is created
    {

        pid_t pid = fork();                                                                 // child creation
        if(pid < 0)                                                                         // fork error
        {
             perror("Fork error\n");
             return 1;
        }
        else if (pid == 0)                                                                  // child process
        {
            close (fd[0]);                                                                  // close write end pipe
            //printf("CHILD My pid is:%d  my parent pid is %d\n",getpid(), getppid() );     // testing purpose to detect child and parent process ID
            edit_number(infilechar, atoi (argv[3]));                                        // number to change by using fourth main argument
            write(fd[1], infilechar, (strlen(infilechar)) + 1);                             // writes to pipe the changed char arrays
        }
        else                                                                                // parent process
        {
          close(fd[1]);                                                                     // close read end of pipe
          int returnStatus;
          waitpid(getpid(), &returnStatus, 0);                                              // parent wait for child process to complete before exiting

          if (getpid() == parent)                                                           // only the first parent runs; excludes child turning into parents
          {
            for(int a = 0; a < N - 1; a++) {                                                // empties pipe from read end
              bytes = read(fd[0], outfilechar, sizeof(outfilechar));
            }

                cout << outfilechar;


          }
          exit(0);
        }

    }
    return 0;
}

char* edit_number(char *filechar, int numchange)
{
  ofstream myfile;                                              // creates a copy of original file as outputfile.txt from initial char array
  myfile.open("outputfile.txt");                                // for editing purposes
  myfile << filechar;
  myfile.close();

  fstream infile;
  infile.open("outputfile.txt");
  int a = 0;
  string word;


  while( infile >> word) {                                      // stores individual string separated by space into word
    char teststring[1024];
    for (int i = 0; i < word.length(); i++)
      teststring[i] = word[i];                                  // stores word in char array

    int number;

    if(isdigit(teststring[0]) || (teststring[0] == '-' && isdigit(teststring[1]))) {        // tests if char array is an int and adds from argument 4 if it is

      number = atoi(teststring);
      number += numchange;
      ostringstream stringout;                                  // stringout is used to turn int back into string

      if (teststring[word.length() - 1] == ',')                 // puts back ', .' if it was taken out
        stringout << number << ",";
      else if (teststring[word.length() - 1] == '.')
        stringout << number << ".";
      else
        stringout << number;
      word = stringout.str();                                   // word gets the changed value of number -> stringout -> back into word
    }

    for (int i = 0; i < word.length(); i++) {                   // stores word into filechar
      filechar[a] = word[i];
      a++;
    }
    filechar[a++] = ' ';                                        // puts back the removed space back into filechar

  }
  ofstream fout("outputfile.txt");                              // saves over "outputfile.txt"
  fout << filechar;
  return filechar;                                              // returns file char
}
