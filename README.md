# Shell Command With Process Management

# Author: Wenda Zhao

#blazersh

blazersh is a C file for running Linux shell command.

## Installation

Downlod Project3_Wenda.tar

tar -cvf Project3_Wenda.tar

make

## Usage

./blazersh

After running blazersh, you can enter any shell command.

Such as:
	pwd/ls/hostname/etc.
	
To get help of using blazersh, enter:
help

To quit blazersh, type:
quit

To check the current files, enter:
list

To show the input history, enter:
history

To test I/O rederiction, enter:
./myprog < inputfile > outputfile

To interupt child process, press:
Ctrl-C

To quit blazersh, press: 
Ctrl-\

To stop child process, press: 
Ctrl-Z

blazersh generates log to record commands input, the log file name is:
blazersh.log

To show the input history, enter: 
history

