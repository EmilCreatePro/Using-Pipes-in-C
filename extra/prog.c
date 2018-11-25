#include<stdio.h>
#include <unistd.h>
#include <fcntl.h> 
#include <sys/types.h>
#include <sys/types.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_BLOCK 4096

int scanRow(char *row)
{
	for(int i = 0; i < strlen(row); i++)
		if(isdigit(row[i])) return 1;

	return 0;
}

void childThreeDuty(int pipe3[2])
{
	int child3;


	if((child3 = fork()) < 0)
	{
		printf("\nError when creating child3!");
		exit(-7);
	}

	if(child3 == 0)
	{
		usleep(250);//delay this process for a bit in order for the other processes to have time to finish
		close(pipe3[1]);


		int nrLines = 0;
		char buffer[MAX_BLOCK];

		/*
		execlp("wc", "wc", "-l", pipe3[0], NULL);
		printf("\nError when using execlp!\n");
		exit(-8);
		*/ //->this is not working because you need to give a file path to wc
		while(read(pipe3[0], buffer, MAX_BLOCK));

		for(int i = 0; i < strlen(buffer); i++)
		{
			if(buffer[i] == '\n') nrLines++;
		}

		printf("\nNumber of rows that have at least one digit: %d\n", nrLines);
		close(pipe3[0]);
		exit(3);
	}
	
}

void childTwoDuty(int pipe2[2])
{
	int child2;

	char buffer[MAX_BLOCK];

	if((child2 = fork()) < 0)
		{
			printf("\nError when creating child2!");
			exit(-5);
		}

	if(child2 == 0)
	{
		close(pipe2[1]);

		while(read(pipe2[0], buffer, MAX_BLOCK));

		printf("\n\nRows that contain at least one digit:\n%s", buffer);

		/*
		printf("\n\nNumber of rows that have at least one digit:\n");
		
		execlp("wc", "wc", "-l", pipe2[0], NULL);
		printf("\nError when using execlp!\n");
		exit(-6);

		//unfortunately these lines will never be reacehd :(
		*/
		close(pipe2[0]);

		int pipe3[2];

		if(pipe(pipe3) < 0)
		{
			printf("\nError when creating third pipe!\n");
			exit(-6);
		}

		childThreeDuty(pipe3);

		close(pipe3[0]);

		write(pipe3[1], buffer, strlen(buffer));

		close(pipe3[1]);

		exit(2);
	}
}

void childOneDuty(int pipe1[2])
{
	int child1;

	char *buffer = malloc(MAX_BLOCK + 1);
	int len;

	if((child1 = fork()) < 0)
	{
		printf("\nError when creating child1!");
		exit(-3);
	}

	if(child1 == 0)
	{
		int pipe2[2], child2;

		close(pipe1[1]);

		while(read(pipe1[0], buffer, MAX_BLOCK))
		printf("%s", buffer);

		close(pipe1[0]);
		
		if(pipe(pipe2) < 0)
		{
			printf("\nError when creating second pipe!\n");
			exit(-4);
		}

		childTwoDuty(pipe2);

		close(pipe2[0]);

		char row[MAX_BLOCK];
		int lenRow = 0;

		for(int i = 0; i < strlen(buffer); i++)
		{
			row[lenRow++] = buffer[i];
			row[lenRow] = '\0';

			if(row[lenRow - 1] == '\n')
			{
				if(scanRow(row))
					write(pipe2[1], row, lenRow);
				
				lenRow = 0;
				row[lenRow] = '\0';
			}
		}

		close(pipe2[1]);
		
		int status;

		wait(&status);

		exit(1);
	}
}

int main(int nrArgs, char **args)
{

	
	char *buffer = malloc(MAX_BLOCK + 1);
	int len;

	if(nrArgs != 2)
	{
		printf("\nError!\nWrong number of arguments! -> <%s> <file>\n", args[0]);
		exit(-1);
	}

	int pipe1[2];

	if(pipe(pipe1) < 0)
	{
		printf("\nError when creating pipe1!\n");
		exit(-2);
	}

	childOneDuty(pipe1);

	close(pipe1[0]);

	int fd = open(args[1], O_RDWR);

	if(fd == -1)
	{
		printf("\nError when reading file!\n");
		exit(-1);
	}

	while((len = read(fd, buffer, MAX_BLOCK)) )
	{
		write(pipe1[1], buffer, len);//we assume that the file has less than 4096 characters
	}

	close(pipe1[1]);
	close(fd);

	int status;

	wait(&status);

	return 0;
}
