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
	pthread_t * thread;
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
	float mean;
	struct Token * next;
} Token;

//Create a Distance struct for holding the two file names, the actual JSD, and the number of tokens for sorting
typedef struct Distance{
	char * file1;
	char * file2;
	double distance;
	int shared;
} Distance;

FileList * listHead = NULL;



/////////////////////////////////////////////////////////////////////////////////////////////////////////

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



//Function to update the mean of every single node within the list
Token * tokenMeans(Token * head){
	//Now we need to iterate through the entire list and calculate the mean distribution for each token using the total stored in the head
	Token * i = head;
	while(i != NULL){
		i->mean = (float)i->number / (float)head->total;
		
		i = i->next;
	}
	
	return head;
}



//Function to print the entire linked list.
void printTokens(Token * head){
	//printf("printing\n");
	
	if(head == NULL){
		printf("Head is null.\n");
		return;
	}
	
	printf("	There are %d tokens.\n", head->total);
	
	/*Token * l = head;
	while(l != NULL){
		printf("		\"%s\" occurs %d times with a mean of %f\n", l->data, l->number, l->mean);
		
		l = l->next;
	}*/
	
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



//Function to add a FileList node; nodes are added in order of total number of tokens, from least to greatest
FileList * addFile(Token * tokenHead, Master * master){
	//If no head exists, we create the head node
	if(listHead == NULL){
		//printf("Head is null.\n");
		
		listHead = malloc(sizeof(FileList));
		listHead->path = malloc(strlen(master->path) + 1);
		strcpy(listHead->path, master->path);
		
		listHead->token = malloc(sizeof(Token));
		listHead->token = tokenHead;
		listHead->next = NULL;
		
		return listHead;
	}
	
	//Else if a head exists, the code will create a new node
	else{
		//First thing we want to do is run through the list and see if the token already exists
		//We're going to check for alphabetical order and make our list alphabetical from the start
		FileList * next = listHead;
		FileList * prev = listHead;
		
		while(next != NULL && tokenHead->total >= next->token->total){
			//printf("token %d next->token %d\n", tokenHead->total, next->token->total);
			prev = next;
			next = next->next;
		}
		
		//Insert token into list
		FileList * new = malloc(sizeof(FileList));
		if(new == NULL){
			return NULL;
		}
		else{
			//Check if the next node is the same as the head node; if so, they're the same node
			if(next != NULL && strcmp(next->path, listHead->path) == 0){
				//We need to insert the new node to the "left" of the head node and make it the new head node
				//In other words, we need to turn the new node into the head node, and make the head node the next node
				new->path = malloc(strlen(master->path) + 1);
				strcpy(new->path, master->path);
				
				new->token = malloc(sizeof(Token));
				new->token = tokenHead;
				new->next = listHead;
				
				return new;
			}
			//Head node is fine as the first node, we can keep moving
			else{
				//Malloc space for data + 1, catch if malloc returns NULL
				if((new->path = malloc(strlen(master->path) + 1)) == NULL){
					return NULL;
				} else{
					//Copy data over to the new and set the number of times the token's shown up to 1
					strcpy(new->path, master->path);
					
					new->token = malloc(sizeof(Token));
					new->token = tokenHead;
					
					//Point previous node to new node, new node to next node. Make sure next node isn't NULL
					prev->next = new;
					
					if(next != NULL){
						new->next = next;
					} else{
						new->next = NULL;
					}
					
					return listHead;
				}
			}
		}
	}
}



//Function to open a file and deal with the contents
void * openFile(void * master){
	//int id = rand();
	
	//printf("\033[1;31m	Thread %d opening file %s \033[0m\n", id, ((Master*)master)->path);
	
	//Open the file and lseek to find the file length
	FILE * fd = fopen(((Master*)master)->path, "r");
	if(fd == NULL){
		printf("Problem opening file %s\n", ((Master*)master)->path);
		return NULL;
	}
	
	fseek(fd, 0, SEEK_END);
	int fileSize = ftell(fd);
	
	rewind(fd);
	
	//printf("\033[0;36m	%s : %dB, thread %d \033[0m\n", ((Master*)master)->path, fileSize, id);
	
	Token * tokenHead = malloc(sizeof(Token));
	tokenHead = tokenize(fd, fileSize);
	tokenHead = tokenMeans(tokenHead);
	//printTokens(tokenHead);
	
	fclose(fd);
	
	//If the head token is NULL, the file is empty and we don't care about it
	if(tokenHead == NULL){
		printf("- Empty file: %s\n", ((Master*)master)->path);
		
		return NULL;
	}
	
	//Lock the mutex
	pthread_mutex_lock(((Master*)master)->mutex);
	
	//Add the new file's Token list to a new FileList object while mutex locked
	listHead = addFile(tokenHead, master);
	
	//Unlock the mutex
	pthread_mutex_unlock(((Master*)master)->mutex);
	
	return NULL;
}




//Function to create a copy of a Master node 
Master * mascpy(Master * master, Master * cpy, char * name){
	cpy->mutex = ((Master*)master)->mutex;
	cpy->path = malloc(1000);
	cpy->next = NULL;
	
	strcpy(cpy->path, ((Master*)master)->path);
	if(cpy->path[strlen(cpy->path) - 1] != '/') strcat(cpy->path, "/");
	strcat(cpy->path, name);
	
	return cpy;
}





//Function to actually recursively look through directories
void * dirTravel(void * master){
	//int id = rand();
	
	//printf("\033[1;31m%d opening directory %s \033[0m\n", id, ((Master*)master)->path);
	//printf("Thread %d opening directory %s\n", id, inputPath);
	
	DIR * dir = opendir(((Master*)master)->path);
	
	//Check to make sure the opendir() worked; if it didn't, returns
	if(dir == NULL){
		printf("- Problem opening directory %s\n", ((Master*)master)->path);
		//printf("-Directory %s is inaccessible %d\n", inputPath, id);
		return NULL;
	}
	
	//printf("\033[1;31m- %d opened %s \033[0m\n", id, ((Master*)master)->path);
	
	//Create a linked list to store all threads
	ThreadList * threadHead = NULL;
	
	struct dirent * dirRead = malloc(sizeof(struct dirent));
	if(dirRead == NULL) return NULL;
	while((dirRead = readdir(dir)) != NULL){
		//printf("Top of while loop\n");
		//Check to see if the file is a regular file, print the path, file name, and file length in green
		if(dirRead->d_type == DT_REG){
			//Create a copy of the master node for the new thread
			Master * newMaster = malloc(sizeof(Master));
			newMaster = mascpy(master, newMaster, dirRead->d_name);
			
			pthread_t * thread = malloc(sizeof(pthread_t));
			
			//Check to see if the thread head exists
			if(threadHead == NULL){
				threadHead = malloc(sizeof(ThreadList));
				
				threadHead->thread = thread;
				threadHead->next = NULL;
				
				pthread_create(thread, NULL, openFile, newMaster);
				
			} else{	//else we need to create a new node
				ThreadList * newThread = malloc(sizeof(ThreadList));
				
				ThreadList * i = threadHead;
				while(i->next != NULL) i = i->next;
				
				i->next = newThread;
				
				newThread->thread = thread;
				newThread->next = NULL;
				
				pthread_create(thread, NULL, openFile, newMaster);
			}
			
			//Check to see if it's a directory at all
		} else if(dirRead->d_type == DT_DIR && strcmp(dirRead->d_name, ".") != 0 && strcmp(dirRead->d_name, "..") != 0){
			//Create a copy of the master node for the new thread
			Master * newMaster = malloc(sizeof(Master));
			newMaster = mascpy(master, newMaster, dirRead->d_name);
			
			//printf("\033[0;33m	%s, thread %d \033[0m\n", newMaster->path, id);
			
			pthread_t * thread = malloc(sizeof(pthread_t));
			
			//We need to check if the head is null, and if it is, initialise the head node
			if(threadHead == NULL){
				threadHead = malloc(sizeof(ThreadList));
				
				//threadHead->data = malloc(sizeof(pthread_t));
				threadHead->thread = thread;
				threadHead->next = NULL;
				
				pthread_create(thread, NULL, dirTravel, newMaster);
				
			} else{	//else we need to create a new node
				ThreadList * newThread = malloc(sizeof(ThreadList));
				
				ThreadList * i = threadHead;
				while(i->next != NULL) i = i->next;
				
				i->next = newThread;
				
				newThread->thread = thread;
				newThread->next = NULL;
				
				pthread_create(thread, NULL, dirTravel, newMaster);
				
			}
		}
	}
	
	ThreadList * iterator;
	//Create a while loop to join every thread in the list as long as the head isn't NULL
	while(threadHead != NULL){
		iterator = threadHead;
		threadHead = iterator->next;
		
		pthread_join(*iterator->thread, NULL);
		free(iterator->thread);
		free(iterator);
	}
	
	//Close the DIR pointer and free the path pointer
	free(dirRead);
	closedir(dir);
	//free(path);
	
	return NULL;
}



//Function to return the number of shared tokens between two files
int sharedTokens(Token * token1, Token * token2){
	int counter = 0;
	
	Token * i = token1;		//iterator for outer loop
	//Outer loop to loop the first Token list through all the tokens in the first file
	while(i != NULL){
		
		Token * j = token2;		//iterator for inner loop
		//Inner loop to loop the second Token list through all tokens in the second file
		while(j != NULL){
			//If we get a match for the token...
			if(strcmp(i->data, j->data) == 0){
				//Add it to the counter
				counter++;
				
				break;
			}
			
			j = j->next;
		}
		
		i = i->next;
	}
	
	return counter;
}



//Function to return the KLD for the first file of two files
double calcKLD(Token * token1, Token * token2){
	double kld = 0;
	
	Token * i = token1;		//iterator for outer loop
	//Outer loop to loop the first Token list through all the tokens in the first file
	while(i != NULL){
		
		Token * j = token2;		//iterator for inner loop
		//Inner loop to loop the second Token list through all tokens in the second file
		while(j != NULL){
			//If we get a match for the token...
			if(strcmp(i->data, j->data) == 0){
				//Add it to the KLD
				kld += i->mean * log10(i->mean / ((i->mean + j->mean) / 2));
				
				break;
			}
			
			j = j->next;
		}
		//If we didn't get a match for the token, j will be NULL. The token exists within the first, but not the second, distribution
		//Add this to the KLD as well
		if(j == NULL) kld += i->mean * log10(i->mean / (i->mean/2));
		
		i = i->next;
	}
	
	return kld;
}



//Funtion to calculate the JSD distance and insert it into array
Distance * calcJSD(Distance * jsd){
	FileList * file1 = listHead;
	FileList * file2;
	int i = 0;
	
	//Outer loop to loop the first file through all the files
	while(file1 != NULL){
		file2 = file1->next;
		
		//Inner loop to loop the second file through all files after the first file
		while(file2 != NULL){
			//Calculate the KSD of the two files and then set it as the JSD in the array
			jsd[i].distance = (calcKLD(file1->token, file2->token) + calcKLD(file2->token, file1->token)) / 2;
			
			//jsd[i].file1 = malloc(strlen(file1->path) + 1);
			//jsd[i].file2 = malloc(strlen(file1->path) + 1);
			
			jsd[i].file1 = malloc(1000);
			jsd[i].file2 = malloc(1000);
			
			strcpy(jsd[i].file1, file1->path);
			strcpy(jsd[i].file2, file2->path);
			
			jsd[i].shared = sharedTokens(file1->token, file2->token);
			
			i++;
			file2 = file2->next;
		}
		
		file1 = file1->next;
	}
	
	return jsd;
}



void printJSD(Distance * jsd, unsigned int numberComp){
	for(int i = 0; i < numberComp; i++){
		if(jsd[i].distance <= .1){
			printf("\033[31m%f %s and %s; shared tokens = %d\033[0m\n", jsd[i].distance, jsd[i].file1, jsd[i].file2, jsd[i].shared);
		}
		else if(jsd[i].distance <= .15){
			printf("\033[33m%f %s and %s; shared tokens = %d\033[0m\n", jsd[i].distance, jsd[i].file1, jsd[i].file2, jsd[i].shared);
		}
		else if(jsd[i].distance <= .2){
			printf("\033[32m%f %s and %s; shared tokens = %d\033[0m\n", jsd[i].distance, jsd[i].file1, jsd[i].file2, jsd[i].shared);
		}
		else if(jsd[i].distance <= .25){
			printf("\033[36m%f %s and %s; shared tokens = %d\033[0m\n", jsd[i].distance, jsd[i].file1, jsd[i].file2, jsd[i].shared);
		}
		else if(jsd[i].distance <= .3){
			printf("\033[34m%f %s and %s; shared tokens = %d\033[0m\n", jsd[i].distance, jsd[i].file1, jsd[i].file2, jsd[i].shared);
		}
		else{
			printf("%f %s and %s; shared tokens = %d\n", jsd[i].distance, jsd[i].file1, jsd[i].file2, jsd[i].shared);
		}
	}
	
	return;
}



int sortComp(const void * a, const void * b){
	const Distance * file1 = a;
	const Distance * file2 = b;
	
	return (file1->shared - file2->shared);
}




int main(int argc, char * argv[]){
	if(argc == 2){
		pthread_mutex_t mutex;
		pthread_mutex_init(&mutex, NULL);
		
		Master * master = malloc(sizeof(Master));
		master->mutex = &mutex;
		
		master->path = malloc(strlen(argv[1]) + 1);
		strcpy(master->path, argv[1]);
		
		master->next = NULL;
		
		//Create a thread for the initial run; next threads will be opened by the function itself
		pthread_t thread;
		pthread_create(&thread, NULL, dirTravel, master);
		
		//sleep(1);
		
		//Join the thread once it's done
		pthread_join(thread, NULL);
		
		//printList();
		
		//Find the number of files so we can malloc an array for each pair
		unsigned int numberFiles = 0;
		FileList * i = listHead;
		while(i != NULL){
			numberFiles++;
			i = i->next;
		}
		
		//If we have fewer than 2 files, we don't need to do any of the rest of the program
		if(numberFiles < 2){
			printf("Not enough tokens to sort.\n");
			return 0;
		}
		
		unsigned int numberComp = numberFiles * (numberFiles-1)/2;
		printf("There are %u files; %u total comparisons.\n", numberFiles, numberComp);
		
		//Calculate the JSD for each file comparison and store it in an array of Distance Structs
		Distance * jsd = malloc(sizeof(Distance) * numberComp);
		jsd = calcJSD(jsd);
		
		//Now we need to sort the jsd array
		qsort(jsd, numberComp, sizeof(Distance), sortComp);
		
		printJSD(jsd, numberComp);
		
	} else printf("Incorrect number of inputs. Program expects a single directory path in stdin.\n");
	
	return 0;
}
