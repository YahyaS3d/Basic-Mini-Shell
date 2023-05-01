//ex1.c mini shell
//Name: Yahya Saad
//-------------------------------some include for usage------------------------------
#include <stdlib.h>

#include <stdio.h>

#include <unistd.h>

#include <sys/wait.h>

#include <sys/types.h>

#include <pwd.h>

#include <string.h>
 //-------------------------------some methods and globals parameters\macros------------------------------
pid_t pid;
#define MAX 510
#define MAX_ARGS 10
// this method is for the prompt line
void DisplayPrompt(int command_counter, int args_count);
//remove double quotes
void removeQuotes(char * str);
//tokenizes the command string into individual words
int ParsingCommand(char * command, char ** countW, int * index);
//execute the command if it's already exist in the operating system 
void RunCommand(char ** countW);
//-------------------------------main program------------------------------
int main() {
  //command counter to print it later 
  //commands length for testing usage only 
  int command_counter = 0, commands_length = 0;
  char command[MAX];
  char ** commandArr; //main array to store the parsed output
  int empty_lines = 0; // number of consecutive empty lines
  int totArgs = 0;
  while (1) {
    int countArgs = 0; // reset the count of arguments for each command
    DisplayPrompt(command_counter, totArgs); // print the prompt line with the number of arguments
    fgets(command, MAX, stdin); // receives a command

    if (strcmp(command, "\n") == 0 != 0) { // if empty line is entered
      empty_lines++;
      if (empty_lines == 3) { // if three consecutive empty lines are entered
        printf("Exiting the program!!");
        exit(EXIT_FAILURE);
      }

      continue;
    }

    empty_lines = 0; // reset consecutive empty lines count

    //        command_counter++;
    command[strlen(command) - 1] = '\0'; // delete the \n
    commands_length += strlen(command); // add the length of the current command to the sum

    char * command_list[MAX_ARGS];
    int num_commands = 0;

    char * command_token = strtok(command, ";");
    while (command_token != NULL) {
      command_list[num_commands] = command_token;
      num_commands++;
      command_token = strtok(NULL, ";");
    }
    command_counter += num_commands;

    for (int j = 0; j < num_commands; j++) {
      commandArr = (char ** ) malloc(sizeof(char * ) * MAX); // allocate memory for the array who hold the command words
      int index = 0;

      ParsingCommand(command_list[j], commandArr, & index); // use a method to split the command into a double array
      //now commandArr holds all the arguments received

      if (strcmp(commandArr[0], "cd") == 0) {
        printf("command not supported\n");
      } else { //if it needs to run the command
        RunCommand(commandArr);
        countArgs += index; // add the number of arguments to the total count of arguments

      }

      for (int i = 0; i < index; i++) // free the arrays of the words
        free(commandArr[i]);
      free(commandArr);
    }

    totArgs += countArgs; // add the total number of arguments for the current line to the overall count

  }
  return 0;
}
//-------------------------------Helpful functions------------------------------
int ParsingCommand(char * command, char ** countW, int * index) {
  // split the command to command name, arguments
  char * argument = strtok(command, " "); //copy the commandArr argument(command name) to an array
  *(index) = 0;
  int arg_size;
  while (argument != NULL) { //fill the array countW with the arguments
    if ( * index >= MAX_ARGS) { // check if the number of arguments is bigger than 10
      printf("Too many arguments.\n");
      exit(EXIT_FAILURE);
    }
    arg_size = strlen(argument);
    countW[ * index] = (char * ) malloc(sizeof(char) * (arg_size + 1)); //allocate an array for the argument
    if (countW[ * index] == NULL) { // check that the memory allocated
      printf("An array for part of the command didnt allocate");
      exit(EXIT_FAILURE);
    }
    strcpy(countW[ * index], argument); //copy the argument
    ( * index) ++;
    argument = strtok(NULL, " ");
  }
  countW[ * index] = NULL; // if it's the end of the command
  free(argument);
  return * index;
}

void DisplayPrompt(int command_counter, int args_count) {
  struct passwd * pw; // will hold the username
  char * cwd; // will hold the path to current directory
  pw = getpwuid(getuid());
  cwd = getcwd(NULL, 0); // get the path
  //show the path
  printf("#cmd:%d|#args:%d@%s%s> ", command_counter, args_count, pw -> pw_name, cwd); //print the open line
  free(cwd);
}
void removeQuotes(char * str) {
  char * src, * dst;
  for (src = dst = str;* src != '\0'; src++) {
    if ( * src != '"') {
      * dst++ = * src; //remove the quotes 
    }
  }
  * dst = '\0';
}

void RunCommand(char ** countW) {
  pid_t x = fork(); // create a thread that will run the command
  if (x < 0) {
    perror("couldn't create new process\n");
    exit(EXIT_FAILURE);
  } else if (x == 0) { // son process
    setpgrp(); // pull it out of the parent group, so he will act the way we want

    if (strcmp(countW[0], "echo") == 0) { // if the command is echo
      int i = 1;
      while (countW[i] != NULL) { // iterate through all the words after "echo"
        removeQuotes(countW[i]);
        printf("%s ", countW[i]); // print each word separated by a space
        i++;
      }
      printf("\n"); // add a newline character after all the words are printed
      exit(EXIT_SUCCESS);
    } else { // if the command is not echo
      execvp(countW[0], countW); //activate the command
      exit(EXIT_SUCCESS);
    }
  }
  // father process
  else {
    pid = x;
    int status;
    waitpid(pid, & status, 0); // wait for child process to complete
  }
}