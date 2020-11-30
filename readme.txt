trf40 : tfp15

Testcase #1:
	./detector stuff
	
	Directory "stuff" has inside of it two more folders and a single text file. The thread recursively opens the two subdirectories in new threads, and the file is opened in a new thread that tokenizes the file.
	
Testcase #2:
	./detector stuff
	
	This time, the program opens up subdirectories recursively until it gets to subdirectory "stuff/all/the/stuff" and opens up a file within the subdirectory there.
	
Testcase #3: 
	./detector stuff
	
	Within the stuff directory, there exists an executable a.out. This is tokenized without issue.
	
Testcase #4:
	./detector stuff/all/forbidden
	
	The "forbidden" directory has no permissions whatsoever; the thread handling directories is able to gracefully exit. If instead "stuff/all" is the input, a thread is opened up for "forbidden" but is gracefully exited, and the rest of the directory is explored through without issue.

Testcase #5:
	./detector ~/Documents
	
	The detector is allowed to go through the entire Documents folder on my local machine, and everything within is tokenized without issue or segfault, though it takes a few moments to finish.
