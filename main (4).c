/*-------------------------------------------
Program 2: Dynamic Food Web Analysis
    A program to build, modify, and analyze a food web
    using dynamic memory allocation. It allows users to
    define organisms and predator-prey relationships,
    simulate species expansion or extinction, and display
    various food web characteristics.
Course: CS 211, Fall 2025, UIC
Author: Run Wang
------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct Org_struct {
    char name[20];
    int* prey; //dynamic array of indices  
    int numPrey;
} Org;


void addOrgToWeb(Org** pWeb, int* pNumOrgs, char* newOrgName) {
    //add a new organism to the food web.    
    Org *newWeb = (Org*)malloc((*pNumOrgs + 1) * sizeof(Org));
    
    for (int i = 0; i < *pNumOrgs; i++){
        newWeb[i] = (*pWeb)[i];
    }

    if (*pNumOrgs > 0){
        free(*pWeb);
    }

    strcpy(newWeb[*pNumOrgs].name, newOrgName);
    newWeb[*pNumOrgs].prey = NULL;
    newWeb[*pNumOrgs].numPrey = 0;

    *pWeb = newWeb;
    (*pNumOrgs)++;
}

bool addRelationToWeb(Org* web, int numOrgs, int predInd, int preyInd) { 
    //add a predator-prey relation to the food web.
    if (predInd < 0 || predInd >= numOrgs || preyInd < 0 || preyInd >= numOrgs || predInd == preyInd){
        return false;
    }

    for(int i = 0; i < web[predInd].numPrey; i++){
        if (web[predInd].prey[i] == preyInd){
            return false;
        }
    }

    int newSize = web[predInd].numPrey + 1;
    int* newPrey = (int*)malloc(newSize * sizeof(int));

    for (int i = 0; i < web[predInd].numPrey; i++){
        newPrey[i] = web[predInd].prey[i];
    }
    newPrey[newSize - 1] = preyInd;

    if(web[predInd].prey != NULL){
        free(web[predInd].prey);
    }

    web[predInd].prey =newPrey;
    web[predInd].numPrey = newSize;

    return true;
}
    
bool removeOrgFromWeb(Org** pWeb, int* pNumOrgs, int index) {
    //remove the organism associated with [index] from web.
    if (index < 0 || index >= *pNumOrgs){
        printf("Invalid extinction index. No organism removed from the food");
        return false;
    }
    for (int i = 0; i < *pNumOrgs; i++){
        if(i == index){
            continue;
        }
        int preyFoundCount = 0;
        for(int j = 0; j < (*pWeb)[i].numPrey; j++){
            if((*pWeb)[i].prey[j] == index){
                preyFoundCount++;
            }
        }
        if(preyFoundCount > 0){
            int newNumPrey = (*pWeb)[i].numPrey - preyFoundCount;
            int* newPreyArr = NULL;
            if (newNumPrey > 0){
                newPreyArr = (int*)malloc(newNumPrey * sizeof(int));
            }
            int k = 0;
            for(int j = 0; j < (*pWeb)[i].numPrey; j++){
                if ((*pWeb)[i].prey[j] != index){
                    newPreyArr[k++] = (*pWeb)[i].prey[j];
                }
            }
            free((*pWeb)[i].prey);
            (*pWeb)[i].prey = newPreyArr;
            (*pWeb)[i].numPrey = newNumPrey;
        }
        for (int j = 0; j < (*pWeb)[i].numPrey; j++){
            if((*pWeb)[i].prey[j] > index){
                (*pWeb)[i].prey[j]--;
            }
        }
    }
    if ((*pWeb)[index].numPrey > 0){
        free((*pWeb)[index].prey);
    }
    int newNumOrgs = *pNumOrgs - 1;
    Org * newWeb = NULL;
    if(newNumOrgs > 0){
        newWeb = (Org*)malloc(newNumOrgs * sizeof(Org));
        int newIdx = 0;
        for (int i = 0; i < *pNumOrgs; i++){
            if(i != index){
                newWeb[newIdx++] = (*pWeb)[i];
            }
        }
    }
    free(*pWeb);
    *pWeb = newWeb;
    *pNumOrgs = newNumOrgs;
    return true;
}

void freeWeb(Org* web, int numOrgs) {
    //free all malloc'd memory associated with web, 
    //               to prevent potential leaks before program terminates
    for (int i = 0; i < numOrgs; i++){
        if(web[i].prey != NULL){
            free(web[i].prey);
        }
        
    }
    if(web != NULL){
        free(web); 
    }
 
}

void printWeb(Org* web, int numOrgs) {
    //print the Food Web Organisms with what they eat (i.e. prey)
    for (int i = 0; i < numOrgs; i++){
        printf("  (%d) %s", i, web[i].name);
        if(web[i].numPrey > 0){
            printf(" eats ");
            for (int j = 0; j < web[i].numPrey; j++){
                printf("%s", web[web[i].prey[j]].name);
                if(j < web[i].numPrey - 1){
                    printf(", ");
                }
            }
        }
        printf("\n");
    }
}

void displayAll(Org* web, int numOrgs, bool modified) {
    if (modified) printf("UPDATED ");
    printf("Food Web Predators & Prey:\n");
    printWeb(web,numOrgs); 
    printf("\n");

    if (modified) printf("UPDATED ");
    printf("Apex Predators:\n");
    //identify and print the organisms not eaten by any others
    bool* isEaten = (bool*)calloc(numOrgs, sizeof(bool));
    for (int i = 0; i <numOrgs; i++){
        for (int j = 0; j < web[i].numPrey; j++){
            isEaten[web[i].prey[j]] = true;
        }
    }

    for (int i = 0; i < numOrgs; i++){
        if (!isEaten[i]){
            printf("  %s\n", web[i].name);
        }
    }
    free(isEaten);
    printf("\n");

    if (modified) printf("UPDATED ");
    printf("Producers:\n");
    //identify and print the organisms that eat no other organisms
    for (int i = 0; i < numOrgs; i++){
        if(web[i].numPrey == 0){
            printf("  %s\n", web[i].name);
        }
    }
    printf("\n");

    if (modified) printf("UPDATED ");
    printf("Most Flexible Eaters:\n");
    //identity and print the organism(s) with the most prey
    int maxPrey = 0;
    for (int i = 0; i < numOrgs; i++){
        if(web[i].numPrey > maxPrey){
            maxPrey = web[i].numPrey;
        }
    }
    for (int i = 0; i < numOrgs; i++){
        if(web[i].numPrey == maxPrey){
            printf("  %s\n", web[i].name);
        }
    }
    printf("\n");

    if (modified) printf("UPDATED ");
    printf("Tastiest Food:\n");
    //identity and print organism(s) eaten by the most other organisms
    int *eatenBy = (int*)calloc(numOrgs, sizeof(int));
    for(int i = 0; i < numOrgs; i++){
        for (int j = 0; j < web[i].numPrey; j++){
            eatenBy[web[i].prey[j]]++;
        }
    }
    int maxEatenBy = 0;
    for(int i = 0; i < numOrgs; i++){
        if(eatenBy[i] > maxEatenBy){
            maxEatenBy = eatenBy[i];
        }
    }
    for(int i = 0; i < numOrgs; i++){
        if (eatenBy[i] == maxEatenBy){
            printf("  %s\n", web[i].name);
        }
    }
    free(eatenBy);
    printf("\n");

    if (modified) printf("UPDATED ");
    printf("Food Web Heights:\n");
    //calculate and print the length of the longest chain from any 
    //producer to each organism
    int* heights = (int*)calloc(numOrgs, sizeof(int));
    bool changed = true;
    while(changed){
        changed = false;
        for ( int i = 0; i < numOrgs; i++){
            for(int j = 0; j < web[i].numPrey; j++){
                if (heights[i] <= heights[web[i].prey[j]]){
                    heights[i] = heights[web[i].prey[j]] + 1;
                    changed = true;
                }
            }
        }
    }
    for (int i = 0; i < numOrgs; i++){
        printf("  %s: %d\n", web[i].name, heights[i]);
    }
    free(heights);
    printf("\n");

    if (modified) printf("UPDATED ");
    printf("Vore Types:\n");
    //classify all organisms and print each group
    //(producers, herbivores, omnivores, & carnivores)
    bool* isProducer = (bool*)calloc(numOrgs, sizeof(bool));
    for (int i = 0; i < numOrgs; i++){
        if(web[i].numPrey == 0){
            isProducer[i] = true;
        }
    }

    printf("  Producers:\n");
    for (int i = 0; i < numOrgs; i++){
        if (isProducer[i]){
            printf("    %s\n", web[i].name);
        }
    }

    printf("  Herbivores:\n");
    for (int i = 0; i < numOrgs; i++){
        if (web[i].numPrey > 0){
            bool onlyEatsProducers = true;
            for (int j = 0; j < web[i].numPrey; j++){
                if(!isProducer[web[i].prey[j]]){
                    onlyEatsProducers = false;
                    break;
                }
            }
            if (onlyEatsProducers){
                printf("    %s\n", web[i].name);
            }
        }
    }

    printf("  Omnivores:\n");
    for (int i = 0; i < numOrgs; i++){
        if(web[i].numPrey > 0 && !isProducer[i]){
            bool eatsProducers = false;
            bool eatsNonProducers = false;
            for(int j = 0; j < web[i].numPrey; j++){
                if(isProducer[web[i].prey[j]]){
                    eatsProducers = true;
                }
                else{
                    eatsNonProducers = true;
                }
            }
            if(eatsProducers && eatsNonProducers){
                printf("    %s\n", web[i].name);
            }
        }
    }

    printf("  Carnivores:\n");
    for (int i = 0; i < numOrgs; i++){
        if(web[i].numPrey > 0 && !isProducer[i]){
            bool onlyEatsNonProducers = true;
            for ( int j = 0; j < web[i].numPrey; j++){
                if(isProducer[web[i].prey[j]]){
                    onlyEatsNonProducers = false;
                    break;
                }
            }
            if(onlyEatsNonProducers){
                printf("    %s\n", web[i].name);
            }
        }
    }

    free(isProducer);
    printf("\n");
}

bool setModes(int argc, char* argv[], bool* pBasicMode, bool* pDebugMode, bool* pQuietMode) {
    //process command-line arguments & update basicMode, debugMode, and quietMode
    //using the passed-by-pointer parameters pBasicMode, pDebugMode, and pQuietMode
    *pBasicMode = false;
    *pDebugMode = false;
    *pQuietMode = false;
    
    bool basicSet = false;
    bool debugSet = false;
    bool quietSet = false;

    for (int i = 1; i < argc; i++){
        if (argv[i][0] == '-'){
            if (argv[i][1] == 'b'){
                if (basicSet){
                    return false;
                }
                *pBasicMode = true;
                basicSet = true;
            }
            else if(argv[i][1] == 'd'){
                if(debugSet){
                    return false;
                    }
                *pDebugMode = true;
                debugSet = true;
            }
            else if(argv[i][1] == 'q'){
                if(quietSet){
                    return false;
                }
                *pQuietMode = true;
                quietSet = true;
            }
            else{
                return false;
            }
        }
        else{
            return false;
        }
    }
    return true;
}

void printONorOFF(bool mode) {
    if (mode) {
        printf("ON\n");
    } else {
        printf("OFF\n");
    }
}

int main(int argc, char* argv[]) {  
    // -----------------------------------------------------------------------------------------------------
    //add comments to the main application to show understanding
    //initialize mode flags
    bool basicMode = false;
    bool debugMode = false;
    bool quietMode = false;

    //parse command-line arguments tpo set modes
    if (!setModes(argc, argv, &basicMode, &debugMode, &quietMode)) {
        printf("Invalid command-line argument. Terminating program...\n");
        return 1;
    }

    //print the final program mode setting
    printf("Program Settings:\n");
    printf("  basic mode = ");
    printONorOFF(basicMode);
    printf("  debug mode = ");
    printONorOFF(debugMode);
    printf("  quiet mode = ");
    printONorOFF(quietMode);
    printf("\n");

    //initialize organism counter and the main web array
    int numOrgs = 0;
    printf("Welcome to the Food Web Application\n\n");
    printf("--------------------------------\n\n");

    Org* web = NULL;

    //build the initial food web
    printf("Building the initial food web...\n");
    
    //loop to read organism names until "DONE" is entered
    if (!quietMode) printf("Enter the name for an organism in the web (or enter DONE): ");
    char tempName[20] = "";
    scanf("%s",tempName); 
    if (!quietMode) printf("\n");
    while (strcmp(tempName,"DONE") != 0) {
        addOrgToWeb(&web,&numOrgs,tempName);
        if (debugMode) {
            printf("DEBUG MODE - added an organism:\n");
            printWeb(web,numOrgs);
            printf("\n");
        }
        if (!quietMode) printf("Enter the name for an organism in the web (or enter DONE): ");
        scanf("%s",tempName); 
        if (!quietMode) printf("\n");
    }
    if (!quietMode) printf("\n");

    //loop to read predator_prey relations until an invalid index is entered
    if (!quietMode) printf("Enter the pair of indices for a predator/prey relation.\n");
    if (!quietMode) printf("Enter any invalid index when done (-1 2, 0 -9, 3 3, etc.).\n");
    if (!quietMode) printf("The format is <predator index> <prey index>: ");
        
    int predInd, preyInd;
    scanf("%d %d",&predInd, &preyInd);
    if (!quietMode) printf("\n");

    while (predInd >= 0 && preyInd >= 0 && predInd < numOrgs &&  preyInd < numOrgs && predInd != preyInd) {
        addRelationToWeb(web,numOrgs,predInd,preyInd);
        if (debugMode) {
            printf("DEBUG MODE - added a relation:\n");
            printWeb(web,numOrgs);
            printf("\n");
        }
        if (!quietMode) printf("Enter the pair of indices for a predator/prey relation.\n");
        if (!quietMode) printf("Enter any invalid index when done (-1 2, 0 -9, 3 3, etc.).\n");
        if (!quietMode) printf("The format is <predator index> <prey index>: ");
        
        scanf("%d %d",&predInd, &preyInd);  
        if (!quietMode) printf("\n");
    }
    printf("\n");

    //initialize food web anaylysis
    printf("--------------------------------\n\n");
    printf("Initial food web complete.\n");
    printf("Displaying characteristics for the initial food web...\n");
    
    displayAll(web,numOrgs,false);

    //main loop for modifing the food web
    if (!basicMode) {
        printf("--------------------------------\n\n");
        printf("Modifying the food web...\n\n");
        char opt = '?';

        while (opt != 'q') {
            //display menu and get user's option
            if (!quietMode) {
                printf("Web modification options:\n");
                printf("   o = add a new organism (expansion)\n");
                printf("   r = add a new predator/prey relation (supplementation)\n");
                printf("   x = remove an organism (extinction)\n");
                printf("   p = print the updated food web\n");
                printf("   d = display ALL characteristics for the updated food web\n");
                printf("   q = quit\n");
                printf("Enter a character (o, r, x, p, d, or q): ");
            }
            scanf(" %c", &opt);
            if (!quietMode) printf("\n\n");

            //exeute action based on user's choice
            if (opt == 'o') {
                char newName[20];
                if (!quietMode) printf("EXPANSION - enter the name for the new organism: ");
                scanf("%s",newName);
                if (!quietMode) printf("\n");
                printf("Species Expansion: %s\n", newName);
                addOrgToWeb(&web,&numOrgs,newName);
                printf("\n");

                if (debugMode) {
                    printf("DEBUG MODE - added an organism:\n");
                    printWeb(web,numOrgs);
                    printf("\n");
                }

            } else if (opt == 'x') {
                int extInd;
                if (!quietMode) printf("EXTINCTION - enter the index for the extinct organism: ");
                scanf("%d",&extInd);
                if (!quietMode) printf("\n");
                if (extInd >= 0 && extInd < numOrgs) {
                    printf("Species Extinction: %s\n", web[extInd].name);
                    removeOrgFromWeb(&web,&numOrgs,extInd);
                } else {
                    printf("Invalid index for species extinction\n");
                }
                printf("\n");
                
                if (debugMode) {
                    printf("DEBUG MODE - removed an organism:\n");
                    printWeb(web,numOrgs);
                    printf("\n");
                }

            } else if (opt == 'r') {
                if (!quietMode) printf("SUPPLEMENTATION - enter the pair of indices for the new predator/prey relation.\n");
                if (!quietMode) printf("The format is <predator index> <prey index>: ");
                scanf("%d %d",&predInd, &preyInd);
                if (!quietMode) printf("\n");

                if (addRelationToWeb(web,numOrgs,predInd,preyInd)) {
                    printf("New Food Source: %s eats %s\n", web[predInd].name, web[preyInd].name);
                };
                printf("\n");
                if (debugMode) {
                    printf("DEBUG MODE - added a relation:\n");
                    printWeb(web,numOrgs);
                    printf("\n");
                }

            } else if (opt == 'p') {
                printf("UPDATED Food Web Predators & Prey:\n");
                printWeb(web,numOrgs);
                printf("\n");
                
            } else if (opt == 'd') {
                printf("Displaying characteristics for the UPDATED food web...\n\n");
                displayAll(web,numOrgs,true);

            }
            printf("--------------------------------\n\n");
        
        }
        
    }

    //free all dynamically allocated memory before exiting
    freeWeb(web,numOrgs);

    //printf("\n\n");

    return 0;
}
