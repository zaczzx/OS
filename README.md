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
