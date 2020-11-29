#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>



//Create a Node struct for linked lists; node is as generic as possible to apply to as many types of data as possible
typedef struct Node{
	void * data;
	struct Node * next;
} Node;

//Create a Token struct for holding each token's string and number of occurrences
typedef struct Token{
	char * data;
	int number;
	struct Token * next;
} Token;

int runtimes = 0;

//	tokenizing code starts here

/////////////////////////////////////////////////////////////////////////////////////////////////////////

//Function to actually create a Token node
Token * createToken(char * token, Token * head){
	runtimes++;
	//If no head exists, we create the head node
	if(head == NULL){
		//printf("Head is null.\n");
		
		//Malloc the head for the size of a Token struct, calloc enough space for the string + 1 for null terminator at the end of the string
		//If malloc returns NULL, program ends and returns NULL
		if((head = malloc(sizeof(Token))) == NULL){
			return NULL;
		} else{
			//Malloc space for data + 1; also need to catch if malloc returns NULL
			if((head->data = malloc(sizeof(token) + 1)) == NULL){
				return NULL;
			} else{
				//Copy data over to the head and set the number of times the token's shown up to 1
				strcpy(head->data, token);
				head->number = 1;
				head->next = NULL;
			}
		}
	}
	
	//Else if a head exists, the code will create a new node
	else{
		//First thing we want to do is run through the list and see if the token already exists
		//We're going to check for alphabetical order and make our list alphabetical from the start
		Token * next = head;
		Token * prev = head;
		while(next != NULL && strcmp(token, next->data) > 0){
			prev = next;
			next = next->next;
		}
		
		//Check to see if they're the same token; if so, we just need to increment the counter and return
		if(next != NULL && strcmp(token, next->data) == 0){
			next->number++;
			
			return head;
		}
		
		//Else we know that the token needs to go in-between the "prev" token and the "next" token
		else{
			Token * new = malloc(sizeof(Token));
			if(new == NULL){
				return NULL;
			}
			else{
				//Malloc space for data + 1, catch if malloc returns NULL
				if((new->data = malloc(sizeof(token) + 1)) == NULL){
					return NULL;
				} else{
					//Make sure that the head node doesn't need to change
					
					//Copy data over to the new and set the number of times the token's shown up to 1
					strcpy(new->data, token);
					new->number = 1;
					
					//Point previous node to new node, new node to next node. Make sure next node isn't NULL
					prev->next = new;
					
					if(next != NULL){
						new->next = next;
					} else{
						new->next = NULL;
					}
					
					return head;
				}
			}
		}
	}
	
	return head;
}

//Function for looking for the rest of the token
Token * isToken(FILE * fd, int size, char * c, Token * head){
	char * temp = malloc(size+1);
	strcpy(temp, c);
	
	while((c[0] = tolower(fgetc(fd)))){
		//If we reach the end of the file or a delimiter, we can complete the token
		if(c[0] == EOF || isspace(c[0])){
			head = createToken(temp, head);
			
			free(temp);
			return head;
		}
		//Check each character to see if it's alphanumeric or a hyphen
		else if(isalpha(c[0]) || isdigit(c[0]) || c[0] == '-'){
			strcat(temp, c);
			
			//printf("%c", c[0]);
		}
	}
	
	free(temp);
	return head;
}

//Function creates a head Token and starts iterating through the characters
Token * tokenize(FILE * fd, int size){
	Token * head = NULL;
	
	char c[2] = {0,0};
	while((c[0] = tolower(fgetc(fd))) != EOF){
		if(isalpha(c[0]) || isdigit(c[0]) || c[0] == '-'){
			//printf("%c", c[0]);
			head = isToken(fd, size, c, head);
		}
	}
	//printf("\n");
	
	return head;
}

//Function to print the entire linked list.
void printTokens(Token * head){
	//printf("printing\n");
	
	if(head == NULL){ printf("Head is null.\n"); }
	Token * l = head;
	while(l != NULL){
		printf("		%s occurs %d times\n", l->data, l->number);
		
		l = l->next;
	}
	
	return;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////

//	tokenizing code ends here, directory travel and file management code starts here

/////////////////////////////////////////////////////////////////////////////////////////////////////////



//Create a Node struct for passing data between threads
typedef struct threadData{
	char * path;
} threadData;

void * openFile(void * path){
	int id = rand();
	
	/*printf("\033[1;31m");
	printf("Thread %d opening file %s\n", id, (char*)path);
	printf("\033[0m");*/
	
	//Open the file and lseek to find the file length
	FILE * fd = fopen((char*)path, "r");
	if(fd == NULL){
		printf("Problem opening file %s\n", (char*)path);
		return NULL;
	}
	
	fseek(fd, 0, SEEK_END);
	int fileSize = ftell(fd);
	
	rewind(fd);
	
	printf("\033[0;36m");
	printf("	%s : %dB, thread %d\n", (char*)path, fileSize, id);
	printf("\033[0m");
	
	Token * tokenHead = tokenize(fd, fileSize);
	printTokens(tokenHead);
	
	fclose(fd);
	
	return NULL;
}

//Function to actually recursively look through directories
void * dirTravel(void * inputDir){
	//char * inputPath = malloc(1000);
	//strcpy(inputPath, inputDir);
	int id = rand();
	
	printf("\033[1;31m");
	printf("%d opening directory %s\n", id, (char*)inputDir);
	//printf("Thread %d opening directory %s\n", id, inputPath);
	printf("\033[0m");
	
	DIR * dir = opendir((char*)inputDir);
	
	//Check to make sure the opendir() worked; if it didn't, returns
	if(dir == NULL){
		printf("- %d directory %s is inaccessible\n", id, (char*)inputDir);
		//printf("-Directory %s is inaccessible %d\n", inputPath, id);
		return NULL;
	}
	
	/*printf("\033[1;31m");
	printf("-%s opened by thread %d\n", (char*)inputDir, id);
	printf("\033[0m");*/
	
	//Create a linked list to store all threads
	Node * threadHead = NULL;
	
	//Create a string to hold the path, a dirent struct for readdir, and loop while there's more to read within the directory
	char * path = malloc(1000);
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
