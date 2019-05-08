#include <iostream>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>     
#include <math.h>

                    
int main(int argc, char* argv[])
{
	  int lines = atoi(argv[1]);   // the first argument represents length and width
	    printf("%d,\n",lines); 
	      for(int i = 0; i < lines; i++){
		          for(int j = 0; j < lines; j++){
				        printf("%d,",lines-abs(j-i));
					    }
			      printf("\n");
			        }
	        printf("\n");
}
