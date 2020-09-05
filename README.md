# Virtual-File-System

This project is used to emulate all functionalities provided by File systems.

#### Platform required
Windows NT platform OR Linux Distributions

#### Architectural requirement
Intel 32 bit processor

#### User Interface
Command User Interface

#### Technology used
C programming

### Description
- - - -
* In this project we emulate all data structures which are used by operating system to
manage File system oriented tasks.
* As the name suggest its virtual because we maintain all records in Primary storage.
* In this project we create all data structures which required for File Subsystems as
Inode Inode Table, File Table, UAREA, User File Descriptor Table, Super block, Disk
Inode List Block, Data Block, Boot Block etc.
* We provide all implementations of necessary system calls and commands of File
subsystem as Open, Close, Read, Write, Lseek, Create, RM, LS, Stat, Fstat etc.
* While providing the implementations of all above functionality we use our own data
structures by referring Algorithms of UNIX operating system.
* By using this project we can get overview of UFS (UNIX File System) on any platform.
- - - -

### Commands implemented using this project
Command | Description
------- | ------------------------------------------
ls      | To list out all the files
clear   | To clear the console
create  | Create a new file
open    | Open specific file
close   | Close specific file
closeall| Close all the opened files
read    | To read contents from the file
write   | To write contents into the file
truncate| To remove all the data from the file
rm      | To delete the file
stat    | Display information about the file
fstat   | Display information using the File Descriptor
exit    | To terminate the File System

#### Reference
Linux System Programming by Robert Love
