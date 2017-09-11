# UNIX Shell

## To run the shell:
- make
- ./shell

## Shell Features:
- The directory from which the shell is invoked will be the home directory of the shell represented by ~.
- Supports semi-colon seperated list of commands.
- 'pwd' ,'ls', 'cd' and 'echo' are built in the shell itself (builtin commands)
- All other commands are executed by forking parent process.
- Background Processes.

## Commands Used : 
- fork : To run a child process under the parent process.
- execvp : Used to execute a command along with the arguments.
- strtok : Used to tokenize the command.
- wait : Used to wait for the child process to end.
- perror : Used for various errors.
- getcwd : Used for current directory
- gethostname : Used to get host name of the OS.
