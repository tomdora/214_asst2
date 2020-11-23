#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

//struct for the linked list Node
typedef struct Node{
	char * data;
	char * type;
	struct Node * next;
} Node;

int startLoc = 0;			//stores starting location
int currentLoc = 0;			//stores current iteration location


///////////////////////////////////////////////////////////////////////////////////////////////////////// ""Beep boop" beep"


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


//Function for testing specific words
Node* isKeyword(char * input, Node * head){
	//printf("startLoc: %d, currentLoc: %d\n", startLoc, currentLoc);
	
	//Test to see if the token can possibly be any of these operators.
	if(currentLoc-startLoc+1 > 1 && currentLoc-startLoc+1 < 9){
		//printf("Between 1 and 9\n");
		
		//Split the sizes in half for 2,3,4 or 5,6,7,8
		//This reduces the number of comparisons necessary from 7 (==2,==3,==4,==5,==6,==7,==8) to either 4 (if,==2,==3,==4) or 5 (if,==5,==6,==7,==8)
		if(currentLoc-startLoc+1 < 5){
			//Check for 2
			if(currentLoc-startLoc+1 == 2){
				if(strncmp(input + startLoc, "do", currentLoc-startLoc+1) == 0){
					head = createNewNode(input, "do", head);
				} else if(strncmp(input + startLoc, "if", currentLoc-startLoc+1) == 0){
					head = createNewNode(input, "if", head);
				} 
				
				else{
					head = createNewNode(input, "word", head);
				}
			}
			//Check for 3
			else if(currentLoc-startLoc+1 == 3){
				if(strncmp(input + startLoc, "for", currentLoc-startLoc+1) == 0){
					head = createNewNode(input, "for", head);
				} else if(strncmp(input + startLoc, "int", currentLoc-startLoc+1) == 0){
					head = createNewNode(input, "int", head);
				} 
				
				else{
					head = createNewNode(input, "word", head);
				}
			} 
			//If not 2 or 3 it must be 4
			else{
				if(strncmp(input + startLoc, "auto", currentLoc-startLoc+1) == 0){
					head = createNewNode(input, "auto", head);
				} else if(strncmp(input + startLoc, "case", currentLoc-startLoc+1) == 0){
					head = createNewNode(input, "case", head);
				} else if(strncmp(input + startLoc, "char", currentLoc-startLoc+1) == 0){
					head = createNewNode(input, "char", head);
				} else if(strncmp(input + startLoc, "else", currentLoc-startLoc+1) == 0){
					head = createNewNode(input, "else", head);
				} else if(strncmp(input + startLoc, "enum", currentLoc-startLoc+1) == 0){
					head = createNewNode(input, "enum", head);
				} else if(strncmp(input + startLoc, "goto", currentLoc-startLoc+1) == 0){
					head = createNewNode(input, "goto", head);
				} else if(strncmp(input + startLoc, "long", currentLoc-startLoc+1) == 0){
					head = createNewNode(input, "long", head);
				} else if(strncmp(input + startLoc, "Node*", currentLoc-startLoc+1) == 0){
					head = createNewNode(input, "Node*", head);
				} 
				
				else{
					head = createNewNode(input, "word", head);
				}
			}
		} else{
			//Split 4(==5,==6,==7,==8) into 2/3 (if,==5/,==6) or 2/3 (if,==7/,==8)
			if(currentLoc-startLoc+1 < 7){
				//Check if it's 5
				if(currentLoc-startLoc+1 == 5){
					if(strncmp(input + startLoc, "break", currentLoc-startLoc+1) == 0){
						head = createNewNode(input, "break", head);
					} else if(strncmp(input + startLoc, "const", currentLoc-startLoc+1) == 0){
						head = createNewNode(input, "const", head);
					} else if(strncmp(input + startLoc, "float", currentLoc-startLoc+1) == 0){
						head = createNewNode(input, "float", head);
					} else if(strncmp(input + startLoc, "short", currentLoc-startLoc+1) == 0){
						head = createNewNode(input, "short", head);
					} else if(strncmp(input + startLoc, "union", currentLoc-startLoc+1) == 0){
						head = createNewNode(input, "union", head);
					} else if(strncmp(input + startLoc, "while", currentLoc-startLoc+1) == 0){
						head = createNewNode(input, "while", head);
					}
					
					else{
						head = createNewNode(input, "word", head);
					}
				}
				//If it's not 5, it's 6.
				else{
					if(strncmp(input + startLoc, "double", currentLoc-startLoc+1) == 0){
						head = createNewNode(input, "double", head);
					} else if(strncmp(input + startLoc, "extern", currentLoc-startLoc+1) == 0){
						head = createNewNode(input, "extern", head);
					} else if(strncmp(input + startLoc, "return", currentLoc-startLoc+1) == 0){
						head = createNewNode(input, "return", head);
					} else if(strncmp(input + startLoc, "signed", currentLoc-startLoc+1) == 0){
						head = createNewNode(input, "signed", head);
					} else if(strncmp(input + startLoc, "sizeof", currentLoc-startLoc+1) == 0){
						head = createNewNode(input, "sizeof", head);
					} else if(strncmp(input + startLoc, "static", currentLoc-startLoc+1) == 0){
						head = createNewNode(input, "static", head);
					} else if(strncmp(input + startLoc, "struct", currentLoc-startLoc+1) == 0){
						head = createNewNode(input, "struct", head);
					} else if(strncmp(input + startLoc, "switch", currentLoc-startLoc+1) == 0){
						head = createNewNode(input, "switch", head);
					} 
					
					else{
						head = createNewNode(input, "word", head);
					}
				}
			} 
			else{
				//Check if it's 7
				if(currentLoc-startLoc+1 == 7){
					if(strncmp(input + startLoc, "default", currentLoc-startLoc+1) == 0){
						head = createNewNode(input, "default", head);
					} else if(strncmp(input + startLoc, "typedef", currentLoc-startLoc+1) == 0){
						head = createNewNode(input, "typedef", head);
					} 
					
					else{
						head = createNewNode(input, "word", head);
					}
				}
				//If it's not 7, it's 8
				else{
					if(strncmp(input + startLoc, "continue", currentLoc-startLoc+1) == 0){
						head = createNewNode(input, "continue", head);
					} else if(strncmp(input + startLoc, "register", currentLoc-startLoc+1) == 0){
						head = createNewNode(input, "register", head);
					} else if(strncmp(input + startLoc, "unsigned", currentLoc-startLoc+1) == 0){
						head = createNewNode(input, "unsigned", head);
					} else if(strncmp(input + startLoc, "volatile", currentLoc-startLoc+1) == 0){
						head = createNewNode(input, "volatile", head);
					} 
					
					else{
						head = createNewNode(input, "word", head);
					}
				}
			}
		}
	}
	//If it's too small or too big to be any of them, it's a word.
	else{
		head = createNewNode(input, "word", head);
	}
	
	return head;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////


//Function for creating a word. Due to the greedy nature of the algorithm, only needs a space or punctuation to terminate rather than any numbers.
Node* isWord(char * input, Node * head){
	//printf("word\n");
	while(currentLoc < strlen(input)){
		//printf("currentLoc: %d\n", currentLoc);
		
		//Check to see if the character is alphanumeric. 
		if(currentLoc < strlen(input)-1 && !isalpha(input[currentLoc+1]) && !isdigit(input[currentLoc+1])){
			//printf("End of word.\n");
			
			head = isKeyword(input, head);
			
			break;
		}
		
		//Check to see if we're at the end of the string, since the function won't automatically send the rest of the string.
		else if(currentLoc == strlen(input) - 1){
			//printf("End of string.\n");
			
			head = isKeyword(input, head);
			
			break;
		}
		
		currentLoc++;
	}
	//printf("Returning\n");
	return head;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////


//Function for block comments; looks for the closing block
Node* isCommentBlock(char * input, Node * head){
	while(currentLoc < strlen(input)){
		if(currentLoc < strlen(input)-1 && input[currentLoc] == '*' && input[currentLoc+1] == '/'){
			currentLoc += 2;
			startLoc = currentLoc;
			
			break;
		}
		
		else if(currentLoc == strlen(input)-1){
			currentLoc++;
			startLoc = currentLoc;
			
			break;
		}
		
		currentLoc++;
	}
	
	return head;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////


//Function for // comments; looks for a '\n' char
Node* isCommentLine(char * input, Node * head){
	while(currentLoc < strlen(input)){
		if(currentLoc < strlen(input)-1 && input[currentLoc+1] == '\n'){
			currentLoc++;
			startLoc = currentLoc;
			
			break;
		}
		
		else if(currentLoc == strlen(input)-1){
			currentLoc++;
			startLoc = currentLoc;
			
			break;
		}
		
		currentLoc++;
	}
	
	return head;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////


//Function to detect an entire string. Will not do anything if there exists an invalid string.
Node* isString(char * input, Node * head){
	while(currentLoc < strlen(input)){
		//Detect if an excaped ''' or '"' exists before we check for an ending mark
		if(currentLoc < strlen(input)-2 && input[currentLoc+1] == '\\' && input[currentLoc+2] == input[startLoc]){
			currentLoc += 2;
		}
		//Detect if the string ends
		else if(currentLoc < strlen(input)-1 && input[currentLoc+1] == input[startLoc]){
			currentLoc++;
			head = createNewNode(input, "string", head);
			
			break;
		}
		//Else check for an end to the entire input; if so, none of this is valid.
		else if(currentLoc == strlen(input)-1){
			printf("String invalid, NULL returned.\n");
			
			currentLoc = strlen(input);
			startLoc = currentLoc;
			
			return NULL;
		}
		
		currentLoc++;
	}
	
	return head;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////


//Function to test for operators. 
Node* isPunctuation(char * input, Node * head){
	//Unfortunately, we have to check each and every single one of these one by one
	//We can't break it up by size, because we don't know the size coming into this, and we can't blindly check the size first
	
	//Check for block comment before anything else
	if(currentLoc < strlen(input)-1 && input[currentLoc] == '/' && input[currentLoc+1] == '*'){
		currentLoc += 2;
		isCommentBlock(input, head);
	}
	//Check for line comment before anything else
	else if(currentLoc < strlen(input)-1 && input[currentLoc] == '/' && input[currentLoc+1] == '/'){
		currentLoc += 2;
		isCommentLine(input, head);
	}
	//Check for quotes; there must be space for an ending quote.
	else if(currentLoc < strlen(input)-2 && (input[currentLoc] == '\'' || input[currentLoc] == '\"')){
		currentLoc++;
		isString(input, head);
	}
	
	//All operators of size 3
	else if(currentLoc < strlen(input)-2 && input[currentLoc] == '>' && input[currentLoc+1] == '>' && input[currentLoc+2] == '='){		
		currentLoc++;
		
		head = createNewNode(input, "shift right equals", head);
	} else if(currentLoc < strlen(input)-2 && input[currentLoc] == '<' && input[currentLoc+1] == '<' && input[currentLoc+2] == '='){		
		currentLoc++;
		
		head = createNewNode(input, "shift left equals", head);
	} 
	
	
	
	//All operators of size 2
	else if(currentLoc < strlen(input)-1 && input[currentLoc] == '<' && input[currentLoc+1] == '<'){		
		currentLoc++;
		
		head = createNewNode(input, "shift left", head);
	} else if(currentLoc < strlen(input)-1 && input[currentLoc] == '>' && input[currentLoc+1] == '>'){		
		currentLoc++;
		
		head = createNewNode(input, "shift right", head);
	} 
	
	else if(currentLoc < strlen(input)-1 && input[currentLoc] == '+' && input[currentLoc+1] == '='){		
		currentLoc++;
		
		head = createNewNode(input, "plusequals", head);
	} else if(currentLoc < strlen(input)-1 && input[currentLoc] == '-' && input[currentLoc+1] == '='){		
		currentLoc++;
		
		head = createNewNode(input, "minusequals", head);
	}
	
	else if(currentLoc < strlen(input)-1 && input[currentLoc] == '<' && input[currentLoc+1] == '='){		
		currentLoc++;
		
		head = createNewNode(input, "less than or equal test", head);
	} else if(currentLoc < strlen(input)-1 && input[currentLoc] == '>' && input[currentLoc+1] == '='){		
		currentLoc++;
		
		head = createNewNode(input, "greater than or equal test", head);
	}
	
	else if(currentLoc < strlen(input)-1 && input[currentLoc] == '=' && input[currentLoc+1] == '='){		
		currentLoc++;
		
		head = createNewNode(input, "equality test", head);
	} else if(currentLoc < strlen(input)-1 && input[currentLoc] == '!' && input[currentLoc+1] == '='){		
		currentLoc++;
		
		head = createNewNode(input, "inequality test", head);
	}
	
	else if(currentLoc < strlen(input)-1 && input[currentLoc] == '*' && input[currentLoc+1] == '='){		
		currentLoc++;
		
		head = createNewNode(input, "times equals", head);
	} else if(currentLoc < strlen(input)-1 && input[currentLoc] == '/' && input[currentLoc+1] == '='){		
		currentLoc++;
		
		head = createNewNode(input, "divide equals", head);
	} else if(currentLoc < strlen(input)-1 && input[currentLoc] == '%' && input[currentLoc+1] == '='){		
		currentLoc++;
		
		head = createNewNode(input, "mod equals", head);
	} else if(currentLoc < strlen(input)-1 && input[currentLoc] == '&' && input[currentLoc+1] == '='){		
		currentLoc++;
		
		head = createNewNode(input, "bitwise AND equals", head);
	} else if(currentLoc < strlen(input)-1 && input[currentLoc] == '^' && input[currentLoc+1] == '='){		
		currentLoc++;
		
		head = createNewNode(input, "bitwise XOR equals", head);
	} else if(currentLoc < strlen(input)-1 && input[currentLoc] == '|' && input[currentLoc+1] == '='){		
		currentLoc++;
		
		head = createNewNode(input, "bitwise OR equals", head);
	} 
	
	else if(currentLoc < strlen(input)-1 && input[currentLoc] == '+' && input[currentLoc+1] == '+'){		
		currentLoc++;
		
		head = createNewNode(input, "increment", head);
	} else if(currentLoc < strlen(input)-1 && input[currentLoc] == '-' && input[currentLoc+1] == '-'){		
		currentLoc++;
		
		head = createNewNode(input, "decrement", head);
	} 
	
	else if(currentLoc < strlen(input)-1 && input[currentLoc] == '-' && input[currentLoc+1] == '>'){		
		currentLoc++;
		
		head = createNewNode(input, "structure pointer", head);
	} 
	
	else if(currentLoc < strlen(input)-1 && input[currentLoc] == '&' && input[currentLoc+1] == '&'){		
		currentLoc++;
		
		head = createNewNode(input, "logical AND", head);
	} else if(currentLoc < strlen(input)-1 && input[currentLoc] == '|' && input[currentLoc+1] == '|'){		
		currentLoc++;
		
		head = createNewNode(input, "logical OR", head);
	} 
	
	
	
	//All operators of size 1
	else if(input[currentLoc] == '+'){	
		head = createNewNode(input, "addition", head);
	} else if(input[currentLoc] == '-'){
		head = createNewNode(input, "minus/subtract operator", head);
	} else if(input[currentLoc] == '*'){
		head = createNewNode(input, "multiply/dereference operator", head);
	} else if(input[currentLoc] == '/'){
		head = createNewNode(input, "division", head);
	} 
	
	else if(input[currentLoc] == '('){
		head = createNewNode(input, "left parenthesis", head);
	} else if(input[currentLoc] == ')'){
		head = createNewNode(input, "right parenthesis", head);
	} else if(input[currentLoc] == '['){
		head = createNewNode(input, "left bracket", head);
	} else if(input[currentLoc] == ']'){
		head = createNewNode(input, "right bracket", head);
	} else if(input[currentLoc] == '{'){
		head = createNewNode(input, "left brace", head);
	} else if(input[currentLoc] == '}'){
		head = createNewNode(input, "right brace", head);
	} 
	
	else if(input[currentLoc] == '.'){
		head = createNewNode(input, "structure member", head);
	} else if(input[currentLoc] == ','){
		head = createNewNode(input, "comma", head);
	} 
	
	else if(input[currentLoc] == '!'){
		head = createNewNode(input, "negate", head);
	} else if(input[currentLoc] == '~'){
		head = createNewNode(input, "is complement", head);
	} else if(input[currentLoc] == '^'){
		head = createNewNode(input, "bitwise XOR", head);
	} else if(input[currentLoc] == '|'){
		head = createNewNode(input, "bitwise OR", head);
	} 
	
	else if(input[currentLoc] == '?'){
		head = createNewNode(input, "conditional true", head);
	} else if(input[currentLoc] == ':'){
		head = createNewNode(input, "conditional false", head);
	}
	
	else if(input[currentLoc] == '<'){
		head = createNewNode(input, "less than test", head);
	} else if(input[currentLoc] == '>'){
		head = createNewNode(input, "greater than test", head);
	} 
	
	else if(input[currentLoc] == '='){
		head = createNewNode(input, "assignment", head);
	} else if(input[currentLoc] == '&'){
		head = createNewNode(input, "AND/address operator", head);
	}
	
	
	
	//If all else fails, it's punctuation
	else{
		head = createNewNode(input, "punctuation", head);
	}
	
	return head;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////


//Helper function for isFloat for if a float also has scientific notation.
Node* isScien(char * input, Node * head){
	//printf("Scientific notation.\n");
	
	while(currentLoc < strlen(input)){
		if(currentLoc < strlen(input)-1 && !isdigit(input[currentLoc+1])){
			head = createNewNode(input, "floating point", head);
			
			break;
		}
		
		//Check to see if we're at the end of the string, since the function won't automatically send the rest of the string.
		else if(currentLoc == strlen(input) - 1){
			//printf("End of string.\n");
			
			head = createNewNode(input, "floating point", head);
			
			break;
		}
		
		currentLoc++;
	}
	
	return head;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////


//If a token has a period AND a number afterwards, it's a float and passed here.
//This function checks for scientific notation, or the end of the token.
Node* isFloat(char * input, Node * head){
	//printf("Float.\n");
	
	while(currentLoc < strlen(input)){
		//First check for scientific notation with signs
		if(currentLoc < strlen(input)-3 && (input[currentLoc+1] == 'e' || input[currentLoc+1] == 'E') && (input[currentLoc+2] == '-' || input[currentLoc+2] == '+') && isdigit(input[currentLoc+3])){
			currentLoc += 3;
			
			head = isScien(input, head);
			
			break;
		}
		
		//Checks for scientific notation without any signs
		else if(currentLoc < strlen(input)-2 && (input[currentLoc+1] == 'e' || input[currentLoc+1] == 'E') && isdigit(input[currentLoc+2])){
			currentLoc += 2;
			
			head = isScien(input, head);
			
			break;
		}
		
		//Check for a non-numerical character
		else if(currentLoc < strlen(input)-1 && !isdigit(input[currentLoc+1])){
			head = createNewNode(input, "floating point", head);
			
			break;
		}
		
		//Check to see if we're at the end of the string, since the function won't automatically send the rest of the string.
		else if(currentLoc == strlen(input) - 1){
			//printf("End of string.\n");
			
			head = createNewNode(input, "floating point", head);
			
			break;
		}
		
		currentLoc++;
	}
	
	return head;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////


Node* isDecimal(char * input, Node * head){
	//printf("Decimal.\n");
	
	while(currentLoc < strlen(input)){
		//Check to see if the string is a float with a decimal point.
		if(currentLoc < strlen(input)-2 && (input[currentLoc+1] == '.' && isdigit(input[currentLoc + 2]))){
			//If there exists a decimal point and a digit after, increment currentLoc and send to isFloat.
			currentLoc++;
			
			head = isFloat(input, head);
			
			break;
		}
		
		//If it's not a float, it's a decimal
		else if(currentLoc < strlen(input)-1 && !isdigit(input[currentLoc+1])){
			head = createNewNode(input, "decimal integer", head);
			
			break;
		}
		
		//Check to see if we're at the end of the string, since the function won't automatically send the rest of the string.
		else if(currentLoc == strlen(input) - 1){
			//printf("End of string.\n");
			
			head = createNewNode(input, "decimal integer", head);
			
			break;
		}
		
		currentLoc++;
	}
	
	return head;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////


//If a token starts with a 0, there exists the possibility it's an octal. Function checks to see if it the token is instead a float or decimal
Node* isOctal(char * input, Node * head){
	//printf("Number at %d\n", currentLoc);
	
	while(currentLoc < strlen(input)){
		//Check to see if the string is a float with a decimal point.
		if(currentLoc < strlen(input)-2 && (input[currentLoc+1] == '.' && isdigit(input[currentLoc + 2]))){
			//If there exists a decimal point and a digit after, increment currentLoc and send to isFloat.
			currentLoc++;
			
			head = isFloat(input, head);
			
			break;
		}
		
		//Also checks to see if the string is a decimal, with a number too large for an octal.
		else if(currentLoc < strlen(input)-1 && input[currentLoc+1] > '7' && input[currentLoc+1] <= '9'){
			currentLoc++;
			
			head = isDecimal(input, head);
			
			break;
		}
		
		else if(currentLoc < strlen(input)-1 && !isdigit(input[currentLoc+1])){
			//printf("Octal.\n");
			
			head = createNewNode(input, "octal integer", head);
			
			break;
		}
		
		//Check to see if we're at the end of the string, since the function won't automatically send the rest of the string.
		else if(currentLoc == strlen(input) - 1){
			//printf("End of string.\n");
			
			head = createNewNode(input, "octal integer", head);
			
			break;
		}
		
		currentLoc++;
	}
	
	return head;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////


//Function for a known hex token. Goes until it finds a non-hex character.
Node* isHex(char * input, Node * head){
	//printf("Hex.\n");
	while(currentLoc < strlen(input)){
		//printf("i: %d, currentLoc: %d\n", i, currentLoc);
		
		//Checks for a non-hex character
		if(currentLoc < strlen(input)-1 && !isxdigit(input[currentLoc+1])){
			//printf("Non-hex found\n");
			
			head = createNewNode(input, "hexadecimal integer", head);
			
			break;
		}
		
		//Check to see if we're at the end of the string, since the function won't automatically send the rest of the string.
		else if(currentLoc == strlen(input) - 1){
			//printf("End of string.\n");
			
			head = createNewNode(input, "hexadecimal integer", head);
			
			break;
		}
		
		currentLoc++;
	}
	
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
	
	while(currentLoc < strlen(input)){
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
	}
	
	return head;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////





//Main function. Takes argv[1] and sends it to tokenize()
int main(int argc, char * argv[]){
	//Check for exactly two arguments; one is not enough and more is too many
	if(argc == 2){
		
		Node * tokenList = tokenize(argv[1]);
		
		printTokens(tokenList);
		freeTokens(tokenList);
		
	} else if(argc <= 1){
		printf("There are no strings to tokenize.\n");
	} else if(argc > 2){
		printf("There are too many strings to tokenize.\n");
	}
	
	return EXIT_SUCCESS;
}
