//
//  problem1.c
//  problem1
//
//  Created by Dilara on 15.02.2020.
//

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
  
void printtime(){
    time_t timer;
       char buffer[26];
       struct tm* tm_info;

       timer = time(NULL);
       tm_info = localtime(&timer);

       strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
       puts(buffer);
}


int main()
{
      int child_pid=fork();
  //  printf("Child pid is %d  \n", pid);
           
        if (child_pid == 0){
         printf("Child pid is in the for loop , pid : %d \n", getpid());
        for(;;){
                sleep(1);
            printtime();  
        }
          
        }
    else{
             printf("Parent is sleeping , pid : %d \n", getpid());
        sleep(5);
        int r;
        r=kill(child_pid, SIGTERM);
        if (r==0)
              printf("Child with pid : %d is killed.\ n", child_pid);
        else
            perror("Kill failed");
     
                
        }
    return 0;
}

