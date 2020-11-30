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
#include <math.h>



//Create the Master struct for overall Master nodes; only struct to move between threads
typedef struct Master{
	pthread_mutex_t * mutex;
	char * path;
	struct Master * next;
} Master;

//Create a struct for holding linked list for threads
typedef struct ThreadList{
	pthread_t * thread;
	struct ThreadList * next;
} ThreadList;

//Create a struct for the linked list of each file's list of tokens
typedef struct FileList{
	char * path;
	struct Token * token;
	struct FileList * next;
} FileList;

//Create a Token struct for holding each token's string and number of occurrences
typedef struct Token{
	char * data;
	int number;
	int total;
	struct Token * next;
} Token;

FileList * listHead = NULL;

//	tokenizing code starts here

/////////////////////////////////////////////////////////////////////////////////////////////////////////

//Function to actually create a Token node
Token * createToken(char * token, Token * head){
	//If no head exists, we create the head node
	if(head == NULL){
		//printf("Head is null.\n");
		
		//Malloc the head for the size of a Token struct, calloc enough space for the string + 1 for null terminator at the end of the string
		//If malloc returns NULL, program ends and returns NULL
		if((head = malloc(sizeof(Token))) == NULL){
			return NULL;
		} else{
			//Malloc space for data + 1; also need to catch if malloc returns NULL
			if((head->data = malloc(strlen(token) + 1)) == NULL){
				return NULL;
			} else{
				//Copy data over to the head and set the number of times the token's shown up to 1
				strcpy(head->data, token);
				head->number = 1;
				head->total = 1;
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
			//printf("Token: %s, next->data: %s, strcmp: %d\n", token, next->data, strcmp(token, next->data));
			prev = next;
			next = next->next;
		}
		
		//Check to see if they're the same token; if so, we just need to increment the counter and return
		if(next != NULL && strcmp(token, next->data) == 0){
			next->number++;
			head->total++;
			
			return head;
		}
		
		//Else we know that the token needs to be inserted into the list
		else{
			Token * new = malloc(sizeof(Token));
			if(new == NULL){
				return NULL;
			}
			else{
				//Check if the next node is the same as the head node; if so, they're the same node
				if(next != NULL && strcmp(next->data, head->data) == 0){
					//We need to insert the new node to the "left" of the head node and make it the new head node
					//In other words, we need to turn the new node into the head node, and make the head node the next node
					new->data = malloc(strlen(token) + 1);
					strcpy(new->data, token);
					
					new->number = 1;
					new->total = head->total + 1;
					
					new->next = head;
					
					return new;
				}
				//Head node is fine as the first node, we can keep moving
				else{
					//Malloc space for data + 1, catch if malloc returns NULL
					if((new->data = malloc(strlen(token) + 1)) == NULL){
						return NULL;
					} else{
						//Copy data over to the new and set the number of times the token's shown up to 1
						strcpy(new->data, token);
						new->number = 1;
						head->total++;
						
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
	
	if(head == NULL){
		printf("Head is null.\n");
		return;
	}
	
	printf("		There are %d tokens.\n", head->total);
	
	Token * l = head;
	while(l != NULL){
		printf("		\"%s\" occurs %d times\n", l->data, l->number);
		
		l = l->next;
	}
	
	return;
}

//Function to print the entire FileList
void printList(){
	//printf("printing\n");
	
	if(listHead == NULL){
		printf("Head is null.\n");
		return;
	}
	
	FileList * l = listHead;
	while(l != NULL){
		printf("Tokens for file %s:\n", l->path);
		printTokens(l->token);
		
		l = l->next;
	}
	
	return;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////

//	tokenizing code ends here, directory travel and file management code starts here

/////////////////////////////////////////////////////////////////////////////////////////////////////////



//Function to open a file and deal with the contents
void * openFile(void * master){
	int id = rand();
	
	printf("\033[1;31m");
	printf("	Thread %d opening file %s\n", id, ((Master*)master)->path);
	printf("\033[0m");
	
	//Open the file and lseek to find the file length
	FILE * fd = fopen(((Master*)master)->path, "r");
	if(fd == NULL){
		printf("Problem opening file %s\n", ((Master*)master)->path);
		return NULL;
	}
	
	fseek(fd, 0, SEEK_END);
	int fileSize = ftell(fd);
	
	rewind(fd);
	
	printf("\033[0;36m");
	printf("	%s : %dB, thread %d\n", ((Master*)master)->path, fileSize, id);
	printf("\033[0m");
	
	Token * tokenHead = malloc(sizeof(Token));
	tokenHead = tokenize(fd, fileSize);
	//printTokens(tokenHead);
	
	pthread_mutex_lock(((Master*)master)->mutex);
	
	if(listHead == NULL){
		listHead = malloc(sizeof(FileList));
		listHead->path = malloc(strlen(((Master*)master)->path) + 1);
		strcpy(listHead->path, ((Master*)master)->path);
		
		listHead->token = malloc(sizeof(Token));
		listHead->token = tokenHead;
		listHead->next = NULL;
		
		//printTokens(listHead->token);
	} else{
		FileList * i = listHead;
		while(i->next != NULL) i = i->next;
		
		FileList * newList = malloc(sizeof(FileList));
		i->next = newList;
		
		newList->path = malloc(strlen(((Master*)master)->path) + 1);
		strcpy(newList->path, ((Master*)master)->path);
		
		newList->token = malloc(sizeof(Token));
		newList->token = tokenHead;
		newList->next = NULL;
		
		//printTokens(newList->token);
	}
	pthread_mutex_unlock(((Master*)master)->mutex);
	
	fclose(fd);
	
	return NULL;
}





//Function to actually recursively look through directories
void * dirTravel(void * master){
	//char * inputPath = malloc(1000);
	//strcpy(inputPath, inputDir);
	int id = rand();
	
	printf("\033[1;31m");
	printf("%d opening directory %s\n", id, ((Master*)master)->path);
	//printf("Thread %d opening directory %s\n", id, inputPath);
	printf("\033[0m");
	
	DIR * dir = opendir(((Master*)master)->path);
	
	//Check to make sure the opendir() worked; if it didn't, returns
	if(dir == NULL){
		printf("- %d directory %s is inaccessible\n", id, ((Master*)master)->path);
		//printf("-Directory %s is inaccessible %d\n", inputPath, id);
		return NULL;
	}
	
	printf("\033[1;31m");
	printf("- %d opened %s\n", id, ((Master*)master)->path);
	printf("\033[0m");
	
	//Create a linked list to store all threads
	ThreadList * threadHead = NULL;
	
	//Create a string to hold the path, a dirent struct for readdir, and loop while there's more to read within the directory
	Master * newMaster = malloc(sizeof(Master));
	newMaster->mutex = ((Master*)master)->mutex;
	newMaster->path = malloc(1000);
	newMaster->next = NULL;
	
	struct dirent * dirRead = malloc(sizeof(struct dirent));
	while((dirRead = readdir(dir)) != NULL){
		//printf("Top of while loop\n");
		//Check to see if the file is a regular file, print the path, file name, and file length in green
		if(dirRead->d_type == DT_REG){
			//printf("if\n");
			strcpy(newMaster->path, ((Master*)master)->path);
			if(newMaster->path[strlen(newMaster->path)] != '/') strcat(newMaster->path, "/");
			strcat(newMaster->path, dirRead->d_name);
			
			pthread_t * thread = malloc(sizeof(pthread_t));
			
			//Check to see if the thread head exists
			if(threadHead == NULL){
				threadHead = malloc(sizeof(ThreadList));
				
				//threadHead->data = malloc(sizeof(pthread_t));
				threadHead->thread = thread;
				threadHead->next = NULL;
				
				pthread_create(thread, NULL, openFile, newMaster);
				pthread_join(*thread, NULL);
				//printf("Thread %s creating new thread %s\n", (char*)inputDir, path);
				
			} else{	//else we need to create a new node
				//printf("New node started : %s --- ", (char*)inputDir);
				ThreadList * newThread = malloc(sizeof(ThreadList));
				
				newThread->next = threadHead;
				threadHead = newThread;
				
				//new->data = malloc(sizeof(pthread_t));
				newThread->thread = thread;
				newThread->next = NULL;
				
				pthread_create(thread, NULL, openFile, newMaster);
				pthread_join(*thread, NULL);
				//printf("Thread %s creating new thread %s\n", (char*)inputDir, path);
				
			}
			
			//Check to see if it's a directory at all
		} else if(dirRead->d_type == DT_DIR){
			//printf("else if\n");
			//If it is, check to see if it's the current or previous directory and ignore them (".", "..", respectively), then print the path
			if(strcmp(dirRead->d_name, ".") != 0 && strcmp(dirRead->d_name, "..") != 0){
				//printf("else if if\n");
				strcpy(newMaster->path, ((Master*)master)->path);
				if(newMaster->path[strlen(newMaster->path)] != '/') strcat(newMaster->path, "/");
				strcat(newMaster->path, dirRead->d_name);
				
				printf("\033[0;33m");
				printf("	%s, thread %d\n", newMaster->path, id);
				printf("\033[0m");
				
				pthread_t * thread = malloc(sizeof(pthread_t));
				
				//We need to check if the head is null, and if it is, initialise the head node
				if(threadHead == NULL){
					threadHead = malloc(sizeof(ThreadList));
					
					//threadHead->data = malloc(sizeof(pthread_t));
					threadHead->thread = thread;
					threadHead->next = NULL;
					
					pthread_create(thread, NULL, dirTravel, newMaster);
					pthread_join(*thread, NULL);
					//printf("Thread %s creating new thread %s\n", (char*)inputDir, path);
					
				} else{	//else we need to create a new node
					//printf("New node started : %s --- ", (char*)inputDir);
					ThreadList * newThread = malloc(sizeof(ThreadList));
					
					newThread->next = threadHead;
					threadHead = newThread;
					
					//new->data = malloc(sizeof(pthread_t));
					newThread->thread = thread;
					newThread->next = NULL;
					
					pthread_create(thread, NULL, dirTravel, newMaster);
					pthread_join(*thread, NULL);
					//printf("Thread %s creating new thread %s\n", (char*)inputDir, path);
					
				}
			}
		}
	}
	
	ThreadList * iterator;
	//Create a while loop to join every thread in the list as long as the head isn't NULL
	while(threadHead != NULL){
		iterator = threadHead;
		threadHead = threadHead->next;
		
		//pthread_join(*iterator->thread, NULL);
		free(iterator->thread);
		free(iterator);
	}
	
	//Close the DIR pointer and free the path pointer
	free(dirRead);
	closedir(dir);
	//free(path);
	
	return NULL;
}





int main(int argc, char * argv[]){
	if(argc == 2){
		pthread_mutex_t mutex;
		pthread_mutex_init(&mutex, NULL);
		
		Master * master = malloc(sizeof(Master));
		master->mutex = &mutex;
		
		master->path = malloc(strlen(argv[1] + 1));
		strcpy(master->path, argv[1]);
		
		master->next = NULL;
		
		//Create a thread for the initial run; next threads will be opened by the function itself
		pthread_t thread;
		pthread_create(&thread, NULL, dirTravel, master);
		
		//Join the thread once it's done
		pthread_join(thread, NULL);
		
		//printList();
		
	} else printf("Incorrect number of inputs. Program expects a single directory path as stdin.\n");
	
	return 0;
}
