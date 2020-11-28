#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

//struct for the linked list Node
typedef struct Node{
	void * data;
	struct Node * next;
} Node;

static int startLoc = 0;			//stores starting location
static int currentLoc = 0;			//stores current iteration location


/////////////////////////////////////////////////////////////////////////////////////////////////////////


//Function to create a new node. Checks to see if the head exists first.
Node* createNewNode(char * input, char * inputType, Node * head){
	//printf("type: %s, startLoc: %d, currentLoc: %d\n", inputType, startLoc, currentLoc);
	
	//If no head exists, function passes to the function to create a head.
	if(head == NULL){
		//printf("Head is null.\n");
		
		//Malloc the head for the size of a Node struct, calloc enough space for the string + 1 for null terminator at the end of the string
		//If malloc returns NULL, program ends and returns NULL
		if((head = malloc(sizeof(Node))) == NULL){
			currentLoc = strlen(input);
			startLoc = currentLoc;
			
			return NULL;
		} else{
			//calloc space for data; also need to catch if calloc returns NULL
			if((head->data = calloc((currentLoc - startLoc + 2), sizeof(char))) == NULL){
				currentLoc = strlen(input);
				startLoc = currentLoc;
				
				return NULL;
			} else{
				//Copy data over to the head and set the type
				strncpy(head->data, input + startLoc, currentLoc-startLoc+1);
				head->type = inputType;
				
				currentLoc++;
				startLoc = currentLoc;
			}
		}
	}
	
	//Else if a head exists, the code will run as normal to create a new node.
	else{
		//Malloc the head for the size of a Node struct, calloc enough space for the string + 1 for null terminator at the end of the string
		//Catch if malloc returns NULL
		Node * new = malloc(sizeof(Node));
		if(new == NULL){
			currentLoc = strlen(input);
			startLoc = currentLoc;
			
			return NULL;
		} else{
			//calloc space for data; also need to catch if calloc returns NULL
			if((new->data = calloc((currentLoc - startLoc + 2), sizeof(char))) == NULL){
				currentLoc = strlen(input);
				startLoc = currentLoc;
				
				return NULL;
			} else{
				//Iterate pointer "iterator" until we get to the end of the list
				Node * iterator = head;
				while(iterator->next != NULL){
					iterator = iterator->next;
				}
				
				//Make the last element's pointer point to the address of the new Node struct
				iterator->next = new;
				
				//Copy data over to the head and set the type
				strncpy(new->data, input + startLoc, currentLoc-startLoc+1);
				new->type = inputType;
				
				currentLoc++;
				startLoc = currentLoc;
			}
		}
	}
	
	return head;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////


//Function for creating a word. Due to the greedy nature of the algorithm, only needs a space or punctuation to terminate rather than any numbers.
Node* isWord(char * input, Node * head){
	//printf("word\n");
	while(currentLoc < strlen(input)){
		//printf("currentLoc: %d\n", currentLoc);
		
		currentLoc++;
	}
	//printf("Returning\n");
	return head;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////


//Function to print the entire linked list.
void printTokens(Node * head){
	//printf("printing\n");
	
	if(head == NULL){ printf("Head is null.\n"); }
	Node * l = head;
	while(l != NULL){
		printf("%s: \"%s\"\n", l->type, l->data);
		
		l = l->next;
	}
	
	return;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////


//Function to free every element of the list.
void freeTokens(Node * head){
	Node * x;
	
	while(head != NULL){
		x = head;
		head = head->next;
		
		free(x->data);
		free(x);
	}
	
	return;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////


//Function creates a head Node and starts iterating through the characters, detecting character types and sending the input to the function for the most likely token
Node* tokenize(char * input){
	Node * head = NULL;
	
	/*while(currentLoc < strlen(input)){
		if(isspace(input[currentLoc])){
			currentLoc++;
			startLoc = currentLoc;
		} 
		else if(isalpha(input[currentLoc])){
			head = isWord(input, head);
		} 
		else if(ispunct(input[currentLoc])){
			head = isPunctuation(input, head);
		} 
		
		else if(currentLoc < strlen(input)-1 && input[currentLoc] == '0' && (input[currentLoc+1] == 'x' || input[currentLoc+1] == 'X')){
			currentLoc++;
			head = isHex(input, head);
		}
		else if(isdigit(input[currentLoc]) && input[currentLoc] == '0'){
			head = isOctal(input, head);
		}
		else if(isdigit(input[currentLoc])){
			head = isDecimal(input, head);
		}
		//If none of these conditions are satisfied, somehow, the code is broken and NULL is returned
		else{
			return NULL;
		}
	}*/
	
	return head;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
