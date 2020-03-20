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
When the prompt `OShell>` appears, type the bash command you want to use. Then, follow the instructions.

## TO DO
- [X] Usage of `execvp` to execute any command of the terminal => add the wait function of the parent process.
- [X] Defining the built-in commands (along with their errors handling) :
    - [X] cd
    - [X] loadmem
    - [X] memdump
    - [X] showlist
- [X] Have a trace of the 3-tuple (`name`, `pid`, `exit_status`) of non built-in commands (all except the last 4).
- [X] Parallel execution of non built-in functions
- [X] Set a 5 seconds timeout in sequential mode.
