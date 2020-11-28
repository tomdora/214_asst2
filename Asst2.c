#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

//Create a Node struct for linked lists; node is generic as possible to apply to as many types of data as possible
typedef struct Node{
	void * data;
	struct Node * next;
} Node;

//Create a Node struct for linked lists; node is generic as possible to apply to as many types of data as possible
typedef struct threadData{
	char * path;
} threadData;

void * openFile(void * path){
	int id = rand();
	
	/*printf("\033[1;31m");
	printf("Thread %d opening file %s\n", id, (char*)path);
	printf("\033[0m");*/
	
	//Open the file and lseek to find the file length
	int fd = open((char*)path, O_RDONLY);
	int fileSize = lseek(fd, 0, SEEK_END);
	close(fd);
	
	printf("\033[0;36m");
	printf("	%s : %dB, thread %d\n", (char*)path, fileSize, id);
	printf("\033[0m");
	
	
	return NULL;
}

//Function to actually recursively look through directories
void * dirTravel(void * inputDir){
	//char * inputPath = malloc(1000);
	//strcpy(inputPath, inputDir);
	int id = rand();
	
	printf("\033[1;31m");
	printf("Thread %d opening directory %s\n", id, (char*)inputDir);
	//printf("Thread %d opening directory %s\n", id, inputPath);
	printf("\033[0m");
	
	DIR * dir = opendir(inputDir);
	
	//Check to make sure the opendir() worked; if it didn't, returns
	if(dir == NULL){
		printf("\033[1;31m");
		printf("-Directory %s is inaccessible %d\n", (char*)inputDir, id);
		//printf("-Directory %s is inaccessible %d\n", inputPath, id);
		printf("\033[0m");
		return NULL;
	}
	
	/*printf("\033[1;31m");
	printf("-%s opened by thread %d\n", (char*)inputDir, id);
	printf("\033[0m");*/
	
	//Create a linked list to store all threads
	Node * threadHead = NULL;
	
	//Create a string to hold the path, a dirent struct for readdir, and loop while there's more to read within the directory
	char *path = malloc(1000);
	struct dirent * dirRead = malloc(sizeof(struct dirent));
	while((dirRead = readdir(dir)) != NULL){
		//printf("Top of while loop\n");
		//Check to see if the file is a regular file, print the path, file name, and file length in green
		if(dirRead->d_type == DT_REG){
			//printf("if\n");
			strcpy(path, (char*)inputDir);
			strcat(path, "/");
			strcat(path, dirRead->d_name);
			
			pthread_t * thread = malloc(sizeof(pthread_t));
			
			//Check to see if the thread head exists
			if(threadHead == NULL){
				threadHead = malloc(sizeof(Node));
				
				//threadHead->data = malloc(sizeof(pthread_t));
				threadHead->data = thread;
				threadHead->next = NULL;
				
				pthread_create(thread, NULL, openFile, path);
				pthread_join(*thread, NULL);
				//printf("Thread %s creating new thread %s\n", (char*)inputDir, path);
				
			} else{	//else we need to create a new node
				//printf("New node started : %s --- ", (char*)inputDir);
				Node * new = malloc(sizeof(Node));
				
				new->next = threadHead;
				threadHead = new;
				
				//new->data = malloc(sizeof(pthread_t));
				new->data = thread;
				new->next = NULL;
				
				pthread_create(thread, NULL, openFile, path);
				pthread_join(*thread, NULL);
				//printf("Thread %s creating new thread %s\n", (char*)inputDir, path);
				
			}
			
			//Check to see if it's a directory at all
		} else if(dirRead->d_type == DT_DIR){
			//printf("else if\n");
			//If it is, check to see if it's the current or previous directory and ignore them (".", "..", respectively), then print the path
			if(strcmp(dirRead->d_name, ".") != 0 && strcmp(dirRead->d_name, "..") != 0){
				//printf("else if if\n");
				strcpy(path, (char*)inputDir);
				strcat(path, "/");
				strcat(path, dirRead->d_name);
				
				printf("\033[0;33m");
				printf("	%s, thread %d\n", path, id);
				printf("\033[0m");
				
				pthread_t * thread = malloc(sizeof(pthread_t));
				
				//We need to check if the head is null, and if it is, initialise the head node
				if(threadHead == NULL){
					threadHead = malloc(sizeof(Node));
					
					//threadHead->data = malloc(sizeof(pthread_t));
					threadHead->data = thread;
					threadHead->next = NULL;
					
					/*pthread_create((pthread_t*)threadHead->data, NULL, &dirTravel, path);
					pthread_join(*(pthread_t*)threadHead->data, NULL);*/
					
					pthread_create(thread, NULL, dirTravel, path);
					pthread_join(*thread, NULL);
					//printf("Thread %s creating new thread %s\n", (char*)inputDir, path);
					
				} else{	//else we need to create a new node
					//printf("New node started : %s --- ", (char*)inputDir);
					Node * new = malloc(sizeof(Node));
					
					new->next = threadHead;
					threadHead = new;
					
					//new->data = malloc(sizeof(pthread_t));
					new->data = thread;
					new->next = NULL;
					
					/*pthread_create((pthread_t*)new->data, NULL, &dirTravel, path);
					pthread_join(*(pthread_t*)new->data, NULL);*/
					
					pthread_create(thread, NULL, dirTravel, path);
					pthread_join(*thread, NULL);
					//printf("Thread %s creating new thread %s\n", (char*)inputDir, path);
					
				}
			}
		}
	}
	
	Node * iterator;
	//Create a while loop to join every thread in the list as long as the head isn't NULL
	while(threadHead != NULL){
		iterator = threadHead;
		threadHead = threadHead->next;
		
		//pthread_join(*(pthread_t*)iterator->data, NULL);
		free((pthread_t*)iterator->data);
		free(iterator);
	}
	
	//Close the DIR pointer and free the path pointer
	closedir(dir);
	free(path);
	
	return NULL;
}

int main(int argc, char * argv[]){
	if(argc == 2){
		//Create a thread for the initial run; next threads will be opened by the function itself
		pthread_t thread;
		pthread_create(&thread, NULL, dirTravel, argv[1]);
		
		//Join the thread once it's done
		pthread_join(thread, NULL);
		
	} else printf("Incorrect number of inputs. Program expects a single directory path as stdin.\n");
	
	return 0;
}
