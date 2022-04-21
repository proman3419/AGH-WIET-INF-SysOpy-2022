#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>


int main(int argc, char* argv[])
{

 if(argc !=4){
    printf("Not a suitable number of program parameters\n");
    return(1);
 }

    //utworz proces potomny w ktorym wykonasz program ./calc z parametrami argv[1], argv[2] oraz argv[3]
    //odpowiednio jako pierwszy operand, operacja (+-x/) i drugi operand 
    //uzyj tablicowego sposobu przekazywania parametrow do programu 

 if (fork() == 0) // child
 {
   char* args[5] = {"./calc", argv[1], argv[2], argv[3], NULL}; 
   execvp(args[0], args);
 }
 else // parent
   while (wait(NULL) < 0);
    
 return 0;
}
