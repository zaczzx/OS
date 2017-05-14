# cs352-os

## cs352 project 1 wsh (Wimpy SHell)

### Overview
* The shell uses a command line interface, 
  and supports foreground (interactive) processing, background (batch) processing, redirection, pipes, 
  and job control. Your shell must support only a very small number of built-in commands
  
### Features
* Foreground jobs

  A "foreground job" is a command of the form ARG0 ARG1 ... ARGn . 

  Examples:
  
  cp file1 file2
  
  cp -R dir1 dir2
  
  rm -f -R dir1 dir2 file1 file2
  
  xterm
  
* Background jobs

  A "background job" is a command of the form ARG0 ARG1 ... ARGn & . 
  
  Examples:
  
  cp file1 file2 &
  
  cp -R dir1 dir2 &
  
  rm -f -R dir1 dir2 file1 file2 &
  
  xterm &
  
* Redirection of standard input and output

  Both background and foreground jobs should be able to redirect standard input and output using "<" and ">", respectively. 

  For example,

  ls -a -l -F > file1

  ls -a -l -F > file2 &

* Built-in commands

  The main difference between your shell and an industrial-strength shell is the lack of built-in commands. 
  Your shell must handle the following commands internally, without creating a new process:

  • cd newdir 

  Change the current working directory to "newdir" 

  • wait jobnum 

  Wait for background job with specified job number to terminate. 

  • exit 

  Exit the shell.
  
* Pipes

  be able to handle a command pipeline of the form job1 | job2 | ... | jobn , where each job is a sequence of arguments (like a foreground job). 
  
  Examples:
  cat -n file | less
  
  ls -alF | sort -n -r +4 | head -5
  
  cat -n file1.cc file2.cc | grep #include | sed -e s/include/exclude/ | head -5
  
  yes + | head -15 | cat -n | sed s/10/10x/ | tr -d \n | tr x \n | head -1 | bc
