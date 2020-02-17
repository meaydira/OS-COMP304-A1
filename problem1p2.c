//
//  problem1p2.c
//  assignment1
//
//  Created by Dilara on 15.02.2020.
//

#include "problem1p2.h"

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

void act(){
    printf("Child pid is in the for loop , pid : %d \n", getpid());
        for(;;){
                sleep(1);
            printtime();
        }
}

/*
             printf("Parent is sleeping , pid : %d \n", getpid());
        sleep(5);
        int r;
        r=kill(child_pid, SIGTERM);
        if (r==0)
              printf("Child with pid : %d is killed.\ n", child_pid);
        else
            perror("Kill failed");
 */
int main()
{
      int pid=fork();
      int pid1,pid2,pid3;
  //  printf("Child pid is %d  \n", pid);
    
        if (pid == 0){
        act();
          
        }
        else{
            int pid1 = fork();
            
            if(pid1==0){
                act();
            }else{
                  int pid2 = fork();
                
                if(pid2==0){
                    act();
                }else{
                      int pid3 = fork();
                    if(pid3==0){
                        act();
                    }else{
                         sleep(5);
                        int r;
                              r=kill(pid, SIGTERM);
                              if (r==0)
                                    printf("Child with pid : %d is killed.\n", pid);
                              else
                                  perror("Kill failed");
                             r=kill(pid1, SIGTERM);
                                                        if (r==0)
                                                              printf("Child with pid : %d is killed.\n", pid1);
                                                        else
                                                            perror("Kill failed");
                         r=kill(pid2, SIGTERM);
                                                        if (r==0)
                                                            printf("Child with pid : %d is killed.\n", pid2);
                                                                              else
                                                                                  perror("Kill failed");
                        r=kill(pid3, SIGTERM);
                                            if (r==0)
                                                                                    printf("Child with pid : %d is killed.\n", pid3);
                                            else
                                                                                  perror("Kill failed");
                    }
                }
            }
            
            
            
        
     
                
        }
    return 0;
}

