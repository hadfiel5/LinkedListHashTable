#include"HashTable.h"
#include<stdlib.h>
#include<string.h>
#include<stdio.h>

//I made the range global so that I could access it in every function without having
//to type table->size
unsigned int range;

//each entry keeps its key, data and a next pointer for linking
typedef struct entry
{
	char *key;
	void *data;
	struct entry *next;

}entry;

unsigned int Hash(char *zard){

	//just simple string hash
	unsigned int sum = 0;
	int i = 0;

	while(zard[i] != '\0'){

		sum += (unsigned int)zard[i];
		i++;

	}

	return sum % range;
}

entry *createEntry(char *id, void *cont, void *col){

	//create an entry pointer to pass back
	entry *object = malloc(sizeof(entry));

	//give the key enough space (+1 is very neccessary for the
	//delimiting character /0)
	object->key = malloc(sizeof(char) * strlen(id) + 1);
	strcpy(object->key, id);

	//set other parts equal to their respective values
	(object->data) = cont;

	object->next = col;

	return object; 
}


int CreateHashTable(HashTablePTR *handle, unsigned int initialSize){

	//set global variable range to the inital size
	range = initialSize;

	//make sure we don't already have a pointer to something	
	if(handle != NULL){

		//give the handle some space
		*handle = malloc(sizeof(hashTable));
		
		//make sure malloc worked
		if(*handle != NULL){

			//assign everything to the table: sentinel, key number,
			//and give the buckets space and set them each to NULL
			(*handle)->sentinel = (int)0xDEADBEEF;
	
			(*handle)->keys = 0;

			(*handle)->buckets = malloc(range * sizeof(entry *));

			for(unsigned int i = 0; i < range; i++){

				((*handle)->buckets)[i] = NULL;
			}
		
		}else{return -1;}

	}else{return -1;}

	
	return 0;
}

//This function frees a linked list (thank you Lab 7)
void freeEverything(entry *head){
	
	//obviously if head is NULL there's nothing left to free
	while(head != NULL){

		entry *temp = head;

		//gotta check if temp is pointing to a list of only one
		if(temp->next == NULL){

			//if it is we're going to free all the pointers within the entry
			//and return 
			free(temp->key);
			free(temp);
			temp = NULL;
			return;
		}

		//if the list is longer than one, we're going to go to find the end, free
		//it and then restart with the list without the last entry, seems like 
		//there would be a sweet recursive solution
		while((temp->next)->next != NULL){

			temp = temp->next;
		}

		free((temp->next)->key);
		free(temp->next);
		temp->next = NULL;
		temp = NULL;

	}
	return;
}


int DestroyHashTable(HashTablePTR *table){

	//Classic Deadbeef check
	if((*table) == NULL || (*table)->sentinel != 0xDEADBEEF){return -1;}

	else{

		//go through the buckets and free every linked list compliments of 
		//freeEverything
		for(int i = 0; i < range; i++){

			freeEverything(((*table)->buckets)[i]);

		}

		//free the last pesky pointers
		free((*table)->buckets);
		free((*table));
		*table = NULL;
		return 0;
	}
}

int InsertEntry(HashTablePTR table, char *key, void *data, void **existing){


	//standard
	if((table) == NULL || table->sentinel != 0xDEADBEEF){return -1;}

	//check if the bucket is currently empty: best case
	if(((table->buckets)[Hash(key)]) == NULL){

		//temporary pointer to new entry
		entry *new = createEntry(key, data, NULL);

		if(new == NULL){return -2;}
		
		(((table)->buckets)[Hash(key)]) = new;

		table->keys++;

		return 0;
	
	}else{

		//Now we know there's a collision
		entry *temp = (((table)->buckets)[Hash(key)]);
		if(temp == NULL){return -2;}

		//check to make sure we don't have an exisiting key
		while(strcmp(temp->key, key)){

			//if we have the last one, we just pin the new one to the end
			if(temp->next == NULL){

				entry *new = createEntry(key, data, NULL);
				if(new == NULL){return -2;}
				temp->next = new;
				temp = NULL;
				table->keys++;
				return 1;
		
			}else{
				//looping through the linked entries my friend
				temp = temp->next;				
			}		
		}
	
		//if we break the loop it implies there was already an exisiting value
		//for that key, so we replace the data
		*existing = temp->data;
		temp->data = data;
		temp = NULL;
		return 2;
	}
}

int DeleteEntry(HashTablePTR table, char *key, void **handle){

	//#oneliners
	if(table == NULL || table->sentinel != 0xDEADBEEF){return -1;}

	entry *temp = ((table->buckets)[Hash(key)]);

	//if there are no values yet in this bucket
	if(temp == NULL){
		return -2;

	//if there is only one key in this bucket
	}else if(temp->next == NULL){

		//if its the key you're looking for
		if(!strcmp(temp->key, key)){

			((table->buckets)[Hash(key)]) = NULL;

			//This may not be good, accessing freed memory
			*handle = temp->data;
			free(temp->key);
			free(temp);
			temp = NULL;
			table->keys--;
			return 0;

		//if not		
		}else{
			return -2;
		}

	//if there's more than one key but the first one is the right one
	}else if(!strcmp(temp->key, key)){
		
		((table->buckets)[Hash(key)]) = temp->next;

		*handle = temp->data;
		free(temp->key);
		free(temp);
		temp = NULL;
		table->keys--;
		return 0;	
	
	//more than one key and the one you are looking for is not the first one	
	}else{

		//second pointer goes in front of the first
		entry* temp2 = temp->next;

		//while the second one doesn't point to the correct key
		while(strcmp(temp2->key, key)){

			//if the second one is the last entry then the key
			//isn't in the table
			if(temp2->next == NULL){

				return -2;

			}else{

				temp2 = temp2->next;
				temp = temp->next;

			}

		}
		
		//breaking the loop implies the second pointer is at the
		//correct key, therefore point the first past the second
		//and free memory
		temp->next = temp2->next;
		*handle = temp2->data;
		free(temp2->key);
		free(temp2);
		temp2 = NULL;
		temp = NULL;
		table->keys--;
		return 0;
	
	}
}

int FindEntry(HashTablePTR table, char *key, void **handle){

	//this one line check is dedicated to TYS
	if(table == NULL || table->sentinel != 0xDEADBEEF){return -1;}

	//pointer to the correct bucket
	entry *temp = ((table->buckets)[Hash(key)]);

	//if temp is NULL then there is no list and the key doesn't exist
	if(temp == NULL){

		return -2;

	}else{

		//If there is a linked list we're going to loop through until we
		//find the key
		while(strcmp(temp->key, key)){

			//if we get to the end without finding the key it's not there
			if(temp->next == NULL){

				return -2;
			}else{
				temp = temp->next;
			}
		}

		//we break the loop, we found the key
		*handle = temp->data;
		temp = NULL;
		return 0;	
	}
}


int GetLoadFactor(HashTablePTR table, float *loadfactor){
	
	//I usually just call it a cow if the beef is living
	if(table == NULL || table->sentinel != 0xDEADBEEF){return -1;}

	else{

		//pretty self explanitory
		float fact = (float)table->keys / (float)range;

		*loadfactor = fact;

		return 0;	
	}
}


int GetKeys(HashTablePTR table, char ***keysarray, unsigned int *keycount){

	//I'll stop commenting this line next function
	if(table == NULL || table->sentinel != 0xDEADBEEF){return -1;}

	//malloc the correct amount of space using the number of
	//keys which I've been conveniently tracking	
	*keysarray = malloc(sizeof(char *) * (unsigned long)table->keys);

	if(*keysarray == NULL){return -2;}

	*keycount = (unsigned int)table->keys; 

	//this variable helps me address the array of strings
	int keyindex = 0;

	//gotta loop through all the buckets
	for(int i = 0; i < range; i++){

		entry *temp = ((table->buckets)[i]);

		//check there are even keys to explore
		if(temp != NULL){		

			//loop through all the keys in the bucket
			while(temp->next != NULL){

				(*keysarray)[keyindex] = malloc(strlen(temp->key) * sizeof(char) + 1);
				if((*keysarray)[keyindex] == NULL){return -2;}

				strcpy((*keysarray)[keyindex], temp->key);
				temp = temp->next;
				keyindex++;
			}
			
			//the last one will still have a key, don't forget that!
			(*keysarray)[keyindex] = malloc(strlen(temp->key) * sizeof(char) + 1);
			if((*keysarray)[keyindex] == NULL){return -2;}

			strcpy((*keysarray)[keyindex], temp->key);
			keyindex++;	
		
		}

		temp = NULL;
	}
		

	return 0;
}




