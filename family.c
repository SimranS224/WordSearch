#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "family.h"

/* Number of word pointers allocated for a new family.
   This is also the number of word pointers added to a family
   using realloc, when the family is full.
*/
static int family_increment = 0;

/* Sets family_increment to size, and initializes random number generator.
*/
void init_family(int size) {
    family_increment = size;
    srand(time(0));
}

/* Prints each family's signature and word given a poitner to the head of a
   linked list of Family nodes.
*/
void print_families(Family* fam_list) {
    int i;
    Family *fam = fam_list;
    
    while (fam) {
        printf("***Family signature: %s Num words: %d\n",
               fam->signature, fam->num_words);
        for(i = 0; i < fam->num_words; i++) {
            printf("     %s\n", fam->word_ptrs[i]);
        }
        printf("\n");
        fam = fam->next;
    }
}


/* Returns a pointer to a new family whose signature is 
   a copy of str. Initializes word_ptrs to point to 
   family_increment+1 pointers, numwords to 0, 
   maxwords to family_increment, and next to NULL.
*/
Family *new_family(char *str) {
    Family *newfam = malloc(sizeof(Family));
    if (newfam == NULL) {
        perror("malloc");
        exit(1);
    }
    newfam->signature = malloc((strlen(str)+1) * sizeof(char));
    if ((newfam->signature) == NULL) {
        perror("malloc");
        exit(1);
    }
    strncpy(newfam->signature,str, (strlen(str) +1)) ;
    newfam->signature[ strlen(str)] = '\0';
    
    newfam->num_words = 0;
    newfam->word_ptrs = malloc((family_increment+1) * sizeof(char*));
    if ((newfam->word_ptrs) == NULL) {
        perror("malloc");
        exit(1);
    }
    newfam->word_ptrs[0] = NULL;
    newfam->max_words = family_increment;
    newfam->next = NULL;
    return newfam;
}


/* Adds word to the next free slot fam->word_ptrs.
   If fam->word_ptrs is full, first uses realloc to allocate family_increment
   more pointers and then adds the new pointer.
*/
void add_word_to_family(Family *fam, char *word) {
    if (fam->num_words  >= fam->max_words){
        fam->word_ptrs = realloc(fam->word_ptrs, (family_increment + fam->max_words + 1) * sizeof(char*));
        if ((fam->word_ptrs) == NULL) {
            perror("malloc");
            exit(1);
        }
        fam->max_words += family_increment;
    }
    fam->num_words++;
    fam->word_ptrs[fam->num_words-1] = word;
    fam->word_ptrs[fam->num_words] = NULL;
}

/* Returns a pointer to the family in the list with the most words;
   if the list is empty, returns NULL. If multiple families have the most words,
   returns a pointer to any of them.
   fam_list is a pointer to the head of a list of Family nodes.
*/
Family *find_biggest_family(Family *fam_list) {
    Family *curr = fam_list;
    int maxval = 0;
    Family *currentmax = NULL;
    while (curr != NULL) {
        if ((curr->num_words) > maxval){
            maxval = curr->num_words;
            currentmax = curr;
        }
        curr = curr->next;
    }
    return currentmax;
}

/* Deallocates all memory rooted in the List pointed to by fam_list. */
void deallocate_families(Family *fam_list) {
    Family *curr = fam_list;
    Family *prev = NULL;
    while (curr != NULL){
        prev = curr;
        curr = curr->next;
        free(prev->word_ptrs);
        free(prev->signature);
        free(prev);
    }
}

/* Returns a pointer to the family whose signature is sig;
   if there is no such family, returns NULL.
   fam_list is a pointer to the head of a list of Family nodes.
*/
Family *find_family(Family *fam_list, char *sig) {
    Family *curr = fam_list;
    while (curr != NULL) {
        if (strcmp(curr->signature,sig) == 0){
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}
/* Generates and returns a linked list of all families using words pointed to
   by word_list, using letter to partition the words.
*/
Family *generate_families(char **word_list, char letter) {
   
    Family *curr = NULL;
    int wordsize = 0;
    int numwords = 0;

    while (word_list[numwords] != NULL){
        numwords++;
    }
    if (numwords>0){
         // get size of one word 
        wordsize = strlen(*word_list);
    }else
    {
        return curr;
    }
    Family *currfam = NULL;
    int outerloopcounter = 0;
    char *newword = malloc((wordsize + 1) *sizeof(char));
    if ((newword) == NULL) {
        perror("malloc");
        exit(1);
    }
    while (word_list[outerloopcounter] != NULL){
        for (int i=0; i<wordsize;i++){
            if (word_list[outerloopcounter][i] == letter){
                newword[i] = letter;
            }else {
                newword[i] = '-';
            }
        }
        newword[wordsize] = '\0';
        if (outerloopcounter == 0){
            currfam = new_family(newword); 
            add_word_to_family(currfam, word_list[outerloopcounter]);
        }
        else {
            Family *toaddto = find_family(currfam, newword);
            if (toaddto == NULL){
                Family *tempfam = new_family(newword);
                add_word_to_family(tempfam, word_list[outerloopcounter]);
                tempfam->next = currfam;
                currfam = tempfam;
            }else{
                add_word_to_family(toaddto, word_list[outerloopcounter]);
            }
        }    
        outerloopcounter++;
    }
    free(newword);
    return currfam;
}


/* Returns the signature of the family pointed to by fam. */
char *get_family_signature(Family *fam) {
    return fam->signature;
}

/* Returns a pointer to word pointers, each of which
   points to a word in fam.
*/
char **get_new_word_list(Family *fam) {
    char **newwordptrs = malloc((fam->num_words+1) * sizeof(char*));
    if ((newwordptrs) == NULL) {
        perror("malloc");
        exit(1);
    }
    for(int i=0;i<fam->num_words;i++){
        newwordptrs[i] = fam->word_ptrs[i];
    }
    printf("in get new word list ");

    newwordptrs[fam->num_words] = NULL;
    return newwordptrs;
}


/* Returns a pointer to a random word from fam. 
   Uses rand (man 3 rand) to generate random integers.
*/
char *get_random_word_from_family(Family *fam) {
    srand(time(0)); 

    int randomnum = (rand() % (fam->num_words)); 
    
    return (fam->word_ptrs)[randomnum];
}
