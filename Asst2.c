#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

//Function to actually recursively look through 
void * dirRecurse(void * inputDir){
	DIR * dir = opendir(inputDir);
	
	//Check to make sure the opendir() worked; if it didn't, returns
	if(dir == NULL){
		printf("\033[1;31m");
		printf("Directory %s is inaccessible\n", (char*)inputDir);
		printf("\033[0m");
		return NULL;
	}
	
	printf("\033[1;31m");
	printf("Opening directory %s\n", (char*)inputDir);
	printf("\033[0m");
	
	//Create a string to hold the path, a dirent struct for readdir, and loop while there's more to read within the directory
	char path[1000];
	struct dirent * dirRead;
	while((dirRead = readdir(dir)) != NULL){
		//Check to see if the file is a regular file, print the path, file name, and file length in green
		if(dirRead->d_type == DT_REG){
			
			strcpy(path, (char*)inputDir);
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
				strcpy(path, (char*)inputDir);
				strcat(path, "/");
				strcat(path, dirRead->d_name);
				
				printf("\033[94m");
				printf("%s\n", path);
				printf("\033[0m");
				
				//Create a new thread for following subdirectories. ignore .git because it's annoying
				if(strcmp(dirRead->d_name, ".git") != 0){
					pthread_t thread;
					pthread_create(&thread, NULL, dirRecurse, path);
					
					//Join the thread once it's done
					pthread_join(thread, NULL);
				}
			}
		}
	}
	
	//Close the DIR pointer
	closedir(dir);
	
	return NULL;
}

int main(int argc, char * argv[]){
	if(argc == 2){
		//Create a thread for the initial run; next threads will be opened by the function itself
		pthread_t thread;
		pthread_create(&thread, NULL, dirRecurse, argv[1]);
		
		//Join the thread once it's done
		pthread_join(thread, NULL);
		
	} else printf("Incorrect number of inputs. Program expects a single directory path as stdin.\n");
	
	return 0;
}
