#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

//Function to actually recursively look through 
void dirRecurse(char * dir){
	DIR * dir = opendir(dir);
	
	//Check to make sure the opendir() worked
	if(dir == NULL) return;
	
	//Create a string to hold the path, a dirent struct for readdir, and loop while there's more to read within the directory
	char path[1000];
	struct dirent * dirRead;
	while((dirRead = readdir(dir)) != NULL){
		//Check to see if the file is a regular file, print the path, file name, and file length in green
		if(dirRead->d_type == DT_REG){
			
			strcpy(path, dir);
			strcat(path, "/");
			strcat(path, dirRead->d_name);
			
			//Open the file and lseek to find the file length
			int fd = open(path, O_RDONLY);
			int fileSize = lseek(fd, 0, SEEK_END);
			close(fd);
			
			printf("\033[92m");
			printf("%s : %dB\n", path, fileSize);
			printf("\033[0m");
			
			//Check to see if it's a directory at all
		} else if(dirRead->d_type == DT_DIR){
			//If it is, check to see if it's the current or previous directory and ignore them (".", "..", respectively), then print the path
			if(strcmp(dirRead->d_name, ".") != 0 && strcmp(dirRead->d_name, "..") != 0){
				strcpy(path, dir);
				strcat(path, "/");
				strcat(path, dirRead->d_name);
				
				printf("\033[94m");
				printf("%s\n", path);
				printf("\033[0m");
				
				//The program then recursively calls the path. Ignores .git/ directory because it's annoying
				if(strcmp(dirRead->d_name, ".git") != 0) dirRecurse(path);
			}
		}
	}
	
	//Close the DIR pointer
	closedir(dir);
	
	return;
}

int main(int argc, char * argv[]){
	dirRecurse(".");
	
	return 0;
}
