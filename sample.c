#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

#define BUFFER_SIZE 25
#define READ_END    0
#define WRITE_END    1

int main(void)
{
    char write_msg[BUFFER_SIZE] = "Hello Mama";
    char read_msg[BUFFER_SIZE];
    pid_t pid;
    int fd[2];

    /* create the pipe */
    if (pipe(fd) == -1) {
        fprintf(stderr,"Pipe failed");
        return 1;
    }

    pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Fork failed");
        return 1;
    }

    if (pid > 0) {  /* parent process */
        /* close the unused end of the pipe */
        close(fd[READ_END]);

   
        write(fd[WRITE_END], write_msg, strlen(write_msg)+1);

        close(fd[WRITE_END]);
    }
    else { /* child process */
        close(fd[WRITE_END]);

        read(fd[READ_END], read_msg, BUFFER_SIZE);
        printf("child reads %s\n",read_msg);

        close(fd[READ_END]);
    }

    return 0;
}
