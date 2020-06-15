# INFO0940 - Operating Systems - Project 3

## Prerequisites
One need a Virtual Machine image of Ubuntu 16.04 with 32bits architecture to make this project work. Otherwise, some strange behavior can appear inside the kernel. Get started after having intalled the right configuration. 
I am not responsible for the consequences that this project could have on the machine of a person wanting to test it. 

## How it works
This project is designed to be tested on the students' Virtual Machine in order to preserve the Montefiore submission platform from kernel panics. 
When the components are set up (see Prerequisites), then a git init chould be performed at the root of the file system.
```bash
cd / && git init
``` 
The first commit should be the entire kernel.
```bash
git add . && git commit -m "first commit"
```
After this step (that can take some time to be done), one can copy the patch folder at the root of the file system before to install the dependencies : 
```bash
sudo apt-get install -y gcc bison flex git-core make bc libssl-dev
```
Then, the patches can be applied.
```bash 
git am patch/*
```
```bash
make -j4 ARCH=i386
```
```bash
sudo make -j4 module_install ARCH=i386
```
```bash
sudo make install ARCH=i386
```
Which will respectively apply the patch, compile the kernel, install kernel modules and intall the kernel.
After a reboot a the machine, the system calls can be called in C programs for instance.

## Results
This part of the project has been marked 18 over 20.
