#include<stdio.h>
#include <unistd.h>
#include <fcntl.h> 
#include <sys/types.h>
#include <sys/types.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sched.h>
#include <pthread.h>

int matrice[10][10], vectorSume[10], n;

int aux[10];

void *calcSuma(void *nr)
{
    int *linie = (int *)nr;
    for(int i = 0; i < n; i++)
        vectorSume[*linie] += matrice[*linie][i]; 
}

int main(int nrArgs, char **args)
{
    FILE *fis;
    
	if(nrArgs != 3)
	{
		printf("\nError!\nWrong number of arguments! -> <%s> N <file>\n", args[0]);
		exit(-1);
        
    }
    
    n = atoi(args[1]);
    
    printf("n = %d\n", n);
    
    fis = fopen(args[2], "r");
    
    if(!fis)
    {
        printf("Could not open file!");
        exit(-2);
    }
    
    for(int i = 0; i < n; i++)
        for(int j = 0; j < n; j++)
            fscanf(fis, "%d", &matrice[i][j]);
        
    for(int i = 0; i < n; i++)
        {
            for(int j = 0; j < n; j++)
                printf("%d ", matrice[i][j]);
            
            printf("\n");
        }
    
    int th;
    
    pthread_t Threads[10];
    
    for(int i = 0; i < n; i++)
    {
        aux[i] = i;
        th = pthread_create(&Threads[i], NULL, calcSuma, &aux[i]);
                            
        
        if(th < 0)
        {
            printf("Error creating thread.");
            exit(-3);
        }
    }
    
    int sum = 0;
    
    for(int k = 0; k < n; k++)
    {
        pthread_join(Threads[k], NULL);
        printf("The sum of line %d is: %d\n", k+1, vectorSume[k]);
        sum += vectorSume[k];
    }
    
    
    printf("Total sum is %d", sum);
    
    
    return 0;
}
