# INFO0940 - Operating Systems - Project 1

## How it works
When in the project directory, type in the terminal 
```bash
make
```
which will compile the codes. Then, use
```bash
[valgrind] ./oshell
```
Where `valgrind` is used for debugging. 

## TO DO
- [ ] Usage of `execvp` to execute any command of the terminal.
- [ ] Defining the built-in commands (along with their errors handling) :[cd], loadmem, memdump and showlist.
- [ ] In these commands, have a trace of the 3-tuple (`name`, `pid`, `exit_status`).
- [ ] Implement the parallel execution of these commands (in function `executeCmd`).
- [ ] Set a 5 seconds timeout in sequential mode. 
