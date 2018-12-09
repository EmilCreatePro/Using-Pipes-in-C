#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

	/*
		Requirement:
		-from the terminal you will call: <exe> a0 r N
		-a child process will calculate the aN value where a(i+1) = ai + 1/r; -> when finished showcase the value of aN and the exit status 
		-the parent will write '+' for a second, and after a second the parrent will write '*' as long as the childs caluclate 
	*/

char char_to_write = '+';

void changeChar(int nothing)
{
	char_to_write = '*';
}

void send_usr1_sig_to_parent(int nothing)
{
	kill(getppid(), SIGUSR1);//send usr1 signal to parent to tell him that one second passed
}

void child_processes(float a0, float r, int N)
{
	double a;;
	
	struct sigaction timer;
	timer.sa_handler = send_usr1_sig_to_parent;
	sigaction(SIGALRM, &timer, NULL);// after one second SIGALRM signal will be sent

	alarm(1);//set the timer-> one second

	a = (double)a0;

	for(int i = 0; i < N; i++)
	{
		a += 1/r;
	}

	printf("a[%d] = %.2lf\n", N, a);

	exit(100);
}

void childDone(int nothing)
{
	int status;
	int wRetVal = wait(&status);

	if(wRetVal == -1)
	{
		printf("\nError when waiting for child process!\n");
		exit(3);
	}

	if(WIFEXITED(status))
	{
		printf("\nChild terminated! PID: %d with exit status: %d\n", getpid(), WEXITSTATUS(status));
	}

	exit(0); //-> if we reach this point the program will be terminated
	
}

int main(int nrArgs, char **args)
{

	int pid;

	if(nrArgs != 4)
	{
		printf("\nError! Illegal number of arguments: <%s> a0 r N\n", args[0]);
		exit(1);
	}

	struct sigaction childFinish;
	childFinish.sa_handler = childDone;
	sigaction(SIGCHLD, &childFinish, NULL);//if this signal is received then it means that the child finished his job

	struct sigaction oneSecPassed;
	oneSecPassed.sa_handler = changeChar;
	sigaction(SIGUSR1, &oneSecPassed, NULL);//if this signal is received then it means that child announced that one second passed
	
	if( (pid = fork()) < 0)
	{
		printf("\nError when creating child process!\n");
		exit(2);
	}

	if(pid == 0)
	{
		child_processes(atof(args[1]), atof(args[2]), atoi(args[3]));
	}
	else while(1)//this will happpen forever, until the child is finished and 'childDone' is called
		{
			printf("%c", char_to_write);
		}

	//return 0; -> we don't need this beacuse we will not reach this point
}
