#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define RAND (rand() % 10000)
//credit for how the random number generator code was implemented

/* description: got the idea to use srand and time to generate a random number 
 
 Generating random numbers using srand. (n.d.). Retrieved April 25, 2016, 
 from http://stackoverflow.com/questions/33182924/generating­random­numbers­using­srand
 */

int main(){
    srand(time(NULL));
    FILE * fp = NULL;
    int start;
    int end;
    int diff;
    int edges;
    int i;
    printf("Enter starting node\n");
    scanf("%d", &start);
    printf("Enter ending node\n");
    scanf("%d", &end);
    printf("Enter number of edges\n");
    diff = end - start + 1;
    scanf("%d", &edges);
    
    //scanf("%d", &start);
    fp = fopen("output.txt", "w");
    fprintf(fp, "%d %d\n", diff, edges);
    
    for(i = start; i < end + 1; i++) {
        fprintf(fp, "%d ", i);
        fprintf(fp, "%.1lf ", (RAND + (0.1 * (RAND % 10))));
        fprintf(fp, "%.1lf\n", (RAND + (0.1 * (RAND % 10))));
    }
    
    for(i = 0; i < edges; ++i) {
        fprintf(fp, "%d ", (((RAND % diff) + start)));
        fprintf(fp, "%d\n", (((RAND % diff) + start)));
    }
    fclose(fp);
    fp = fopen("output2.txt", "w");
    fprintf(fp, "%d %d", (((RAND % (diff)) + start)), (((RAND % (diff)) + start))); 
    fclose(fp);
}
