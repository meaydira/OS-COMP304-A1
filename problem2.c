//
//  problem2.c
//  assignment1
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

void parent(int pipefd[2], int input,int pipefd3[2]){
    int result=input;
    close(pipefd[0]);          /* Close unused read end */
    write(pipefd[1], &result, sizeof(result));
    close(pipefd[1]);
    printf("I am parent. input : %d \n",input);
    fflush(stdout);

    wait(NULL);
    
    
    int final;
    close(pipefd3[1]);          /* Close unused write end */
      read(pipefd3[0], &final, sizeof(final));
     // printf("parent  reads from pipe3... %d \n", final);
      fflush(stdout);
      close(pipefd3[0]);
    
    
    printf("I am parent. Final result : %d \n", final);

    
    
    

}


void child(int pipefd[2],int pipefd2[2],int pipefd3[2],int pipefd4[2]){
    
    //read input
   int input;
    //wait(NULL);  // wait for parent
    close(pipefd[1]);          /* Close unused write end */
    read(pipefd[0], &input, sizeof(input));
   // printf("child  reads from pipe1... \n", input);
    fflush(stdout);
    close(pipefd[0]);
    
    int output = input*2;
    
    //write output to pipe 2 (send it to grandchild)
        close(pipefd2[0]);          /* Close unused read end */
            write(pipefd2[1], &output, sizeof(output));
            close(pipefd2[1]);
            printf("Hi I'm child! : \n");
            printf("input : %d output : %d  \n",input, output);
           // printf("I have written to pipe 2 , input : %d output : %d  \n",input, output);
            fflush(stdout);
  
   // wait(NULL);
    
    //get result from grandchild
    int final;
     close(pipefd4[1]);          /* Close unused write end */
     read(pipefd4[0], &final, sizeof(final));
     //printf("child  reads from pipe4... %d \n", final);
     fflush(stdout);
     close(pipefd4[0]);
    
        
    //write result to pipe1
    int result = final;
     close(pipefd3[0]);          /* Close unused read end */
         write(pipefd3[1], &result, sizeof(result));
         close(pipefd3[1]);
        // printf("Hi I'm child! : \n");
        // printf("I am writing to pipe3 , result : %d  \n",result);
         fflush(stdout);

    
    
}

void grandchild(int pipefd2[2],int pipefd4[2]){
    //read input
    int input;
    //wait(NULL);// wait for parent
     close(pipefd2[1]);          /* Close unused write end */
     read(pipefd2[0], &input, sizeof(input));
     //printf("grandchild  reads from parent... \n", input);
     fflush(stdout);
     close(pipefd2[0]);
     
     //write output
     int output = input*2;
      close(pipefd4[0]);          /* Close unused read end */
          write(pipefd4[1], &output, sizeof(output));
          close(pipefd4[1]);
          printf("Hi I'm grandchild! : \n");
          printf("input : %d output : %d  \n",input, output);
          fflush(stdout);
  
}



void passGrandchildPid(int pipefd5[2], int grand_child_pid){
    int result = grand_child_pid;
     close(pipefd5[0]);          /* Close unused read end */
         write(pipefd5[1], &result, sizeof(result));
         close(pipefd5[1]);
        // printf("Hi I'm child! : \n");
        // printf("I am writing to pipe3 , result : %d  \n",result);
         fflush(stdout);
}


int getGrandchildPid(int pipefd5[2]){
    int final;
      close(pipefd5[1]);          /* Close unused write end */
        read(pipefd5[0], &final, sizeof(final));
       // printf("parent  reads from pipe3... %d \n", final);
        fflush(stdout);
        close(pipefd5[0]);
    
    return final;
}



int main()

{         int pipefd[2];  // for sending messages from parent to child
          int pipefd2[2];  //for sending messages from child to grandchild
          int pipefd3[2];  // for sending messages from child to parent
           int pipefd4[2];  //for sending messages from grandchild to child
    int pipefd5[2];  //for sending messages from grandchild to child

          if (pipe(pipefd) == -1)
          {
              perror(" pipe");
              return 0;
          }
    
          if (pipe(pipefd2) == -1)
             {
                 perror(" pipe2");
                 return 0;
             }
    
        if (pipe(pipefd3) == -1)
               {
                   perror(" pipe2");
                   return 0;
               }
    
        if (pipe(pipefd4) == -1)
                   {
                       perror(" pipe2");
                       return 0;
                   }
    if (pipe(pipefd5) == -1)
    {
        perror(" pipe2");
        return 0;
    }
    
          int child_pid = fork();
          int grand_child;
    
    
    if(child_pid == 0){
        //child process
   
        grand_child = fork();
        
        if(grand_child == 0){
            //grandchild will execute
            sleep(5);
            grandchild(pipefd2, pipefd4);
            
        }else{
            //child process
            passGrandchildPid(pipefd5, grand_child);
            child(pipefd, pipefd2, pipefd3, pipefd4);
            
           //wait for parent
        }
       
    }else{
        //parent process
        grand_child = getGrandchildPid(pipefd5);
        int input;
        printf("Enter an integer: ");
        scanf("%d", &input);
        
         printf(" pid = %d ",getpid());
        printf("child pid = %d ",child_pid);
        printf("grandchild pid = %d ",grand_child);
        //printf("Integer is :  %d",input);
          sleep(5);
        parent(pipefd, input,pipefd3);
       
        kill(child_pid, SIGTERM);
        kill(grand_child, SIGTERM);
        
    }
    
    
    return 0;
}
