# Haystack storage
The program implements the concept of data storage that keeps all files in one big file (BLOB - binary large object) and
information about them (name, size, first byte in BLOB-file) in other file (BLIN).

For now its only for UNIX systems.

Commands
----
* add "_filepath_" - add file from _filepath_ to storage
* get "_filename_" "_filepath_" - get file with _filename_ from storage to _filepath_
* list - show all files in storage
* exit - terminate program

The program keeps only the name of file, not full path that was entered. For example:
```
add home/user/Pictures/background.jpg
get background.jpg /home/user/Downloads/picture.jpg
```
