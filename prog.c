#include<stdio.h>
#include <unistd.h>
#include <fcntl.h> 
#include <sys/types.h>
#include <sys/types.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/wait.h>


/*
	Requirement:
	Call in terminal: <exe> file
	A parent process will read a file
		while it's reading the file->the parrent will send to child1 the letters using a pipe
								   ->the parrent will send to child2 the lowerCase letters using a pipe
								   ->child3 will count the number of words that the file has
	Send the number of letters as the exit codes from the 2 children and find a way to get the number of upper letters
*/

int pipe1[2], pipe2[2], child1, child2, child3;

int countLetters(int fd)
{
	int count = 0;
	char aux[1];

	while(read(fd, aux, 1))
		count++;

	close(fd);

	return count;
}

void startProcesses(char *path)
{
	if( (child1 = fork()) < 0 )
	{
		printf("\nError when creting child1!\n");
		exit(-4);
	}

	if(child1 == 0)
	{
		close(pipe1[1]);
		close(pipe2[0]);
		close(pipe2[1]);

		exit(countLetters(pipe1[0]));
	}

	if( (child2 = fork()) < 0 )
	{
		printf("\nError when creting child2!\n");
		exit(-7);
	}

	if(child2 == 0)
	{
		close(pipe1[0]);
		close(pipe1[1]);
		close(pipe2[1]);

		exit(countLetters(pipe2[0]));
	}
	

	if( (child3 = fork()) < 0 )
	{
		printf("\nError when creting child3!\n");
		exit(-7);
	}

	if(child3 == 0)
	{
		close(pipe1[1]);
		close(pipe1[0]);
		close(pipe1[1]);
		close(pipe2[1]);

		execlp("wc", "wc", "-w", path, NULL);
		printf("\nError wehn using execlp!\n");
		exit(-8);
	}
}

void startParent(char *path)
{
	int fd;
	char buff[2];

	fd = open(path, O_RDWR);

	if(fd == -1)
	{
		printf("\nError when opening file!\n");
		exit(-5);
	}

	close(pipe1[0]);
	close(pipe2[0]);

	while(read(fd, buff, 1))
	{
		if(isalpha(buff[0]))
		{
			write(pipe1[1], buff, 1);

			if(islower(buff[0]))
				write(pipe2[1], buff, 1);
		}
	}

	close(pipe2[1]);
	close(pipe1[1]);

	int status, wExit, nrLetters, nrLowerLetters;

	for(int i = 0; i < 3; i++)
	{
		wExit = wait(&status);

		if(wExit == -1)
		{
			printf("\nError when doing wait!\n");
		}

		if(wExit == child1)
			if(WIFEXITED(status))
            	{
                	nrLetters = WEXITSTATUS(status);
                	printf("\nWe have %d number of letters\n", nrLetters);
            	}
		
		if(wExit == child2)
			if(WIFEXITED(status))
            	{
                	nrLowerLetters = WEXITSTATUS(status);
                	printf("\nWe have %d number of lower letters\n", nrLowerLetters);
            	}
	}


	printf("\nWe have %d upper letters.\n", nrLetters - nrLowerLetters);
	close(fd);

}

int main(int nrArgs, char **args)
{

	if(nrArgs != 2)
	{
		printf("\nError!\nWrong number of arguments! -> <%s> <file>\n", args[0]);
		exit(-1);
	}

	if(pipe(pipe1) < 0)
	{
		printf("\nError when creating pipe1!\n");
		exit(-2);
	}
	
	if(pipe(pipe2) < 0)
	{
		printf("\nError when creating pipe2\n");
		exit(-3);
	}
	
	startProcesses(args[1]);
	startParent(args[1]);
	
	return 0;
}
