//
//  problem3.c
//  assignment1
//
//  Created by Dilara on 16.02.2020.
//


#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>


#define MESSAGESIZE 10
#define READ_END    0
#define WRITE_END    1
#define BUFFERSIZE 64000

int fsize(FILE *fp){
    int prev=ftell(fp);
    fseek(fp, 0L, SEEK_END);
    int sz=ftell(fp);
    fseek(fp,prev,SEEK_SET); //go back to where we were
    return sz;
}



void writeToPipe(int fd[2], char string[], long stell){
       close(fd[READ_END]);
       write(fd[WRITE_END], &stell, sizeof(stell));
         write(fd[WRITE_END], string, strlen(string)+1);
         close(fd[WRITE_END]);
}

static void readFromPipe(int pipefd[2], char input[],long size){
         // wait(NULL);
           close(pipefd[WRITE_END]);
           read(pipefd[READ_END], input, size);
           printf("child reads input, size : %d \n",size);
           close(pipefd[READ_END]);
}

static long readSize(int pipefd[2]){
       long size;
     close(pipefd[WRITE_END]);
     read(pipefd[READ_END], &size, sizeof(size));
     printf("Size is %d",size);
     close(pipefd[READ_END]);
    return size;
}




int main()
{
    
    struct mymsg {
          long mesg_type;
          char mesg_text[MESSAGESIZE];
      } message;
    int shmfd;
    void *ptr_a;
    void *ptr_b;
    void *ptr;
    char* const shm_nameA="memoryA";
    char* const shm_nameB="memoryB";
    int pipefd[2];
    int pipefd2[2];
      int childA_pid;
      int childB_pid;
    
    if (pipe(pipefd) == -1)
           {
               perror(" pipe");
               return 0;
           }
    
    if (pipe(pipefd2) == -1)
           {
               perror(" pipe");
               return 0;
           }
    
    
       childA_pid = fork();
           
        if(childA_pid == 0){
            //childA
        
        long SIZE = readSize(pipefd);
        char input[SIZE];
        readFromPipe(pipefd, input, SIZE);
          printf("Child A here, input read, size is: %d \n",SIZE);
        
       /* if (shm_unlink(shm_nameA) == -1) {
                                        printf("Error removing %s\n",shm_nameA);
                                        exit(-1);
                                    }  */
               int shmfd = shm_open(shm_nameA, O_CREAT| O_RDWR, 0666);
                      ftruncate(shmfd, SIZE);
                      ptr_a =mmap(0,SIZE, PROT_WRITE|PROT_READ, MAP_SHARED, shmfd, 0);
                  if (ptr_a == MAP_FAILED) {
                         printf("Map failed --- create \n");
                         exit(-1);
                     }
               
               ///
               printf("shared memory created");
               sprintf(ptr_a,"%s",input);
               ptr_a += strlen(input);
               
               key_t key = ftok(shm_nameA, 65);
                 printf("key created");
               message.mesg_type = 1;
               char msg[MESSAGESIZE]="Done";
               for(int i = 0 ; i<strlen(msg)+1;i++){
                     message.mesg_text[i]=msg[i];
               }
                 
                            printf("HI IM GONNA SEND A MESSAGE");
        int msgid;
        if( msgget(key,  IPC_CREAT | 0666)==-1){
              perror("msgid error");
        }
                            printf("Message is : %s MSGİD is %d \n", message.mesg_text, msgid);
               if ( msgsnd(msgid, &message, sizeof(message), 0)==-1) {
                    perror("msgsnd error");
                   fprintf(stderr, "Message from A could not be sended.\n");
               }
     
        }
    else{
        childB_pid = fork();
        if(childB_pid == 0){
             
             long SIZE = readSize(pipefd2);
             char input[SIZE];
             readFromPipe(pipefd2, input, SIZE);
              printf("Child B here, input read, size is: %d \n",SIZE);
          
        
        }else{
            //parent
               char *buffer;
                       FILE *fp = fopen("try.txt", "r");
                 if (fp != NULL)
                 {
                     fseek(fp, 0L, SEEK_END);
                     long stell = ftell(fp);
                     rewind(fp);
                     char str[stell];
                     buffer = (char *)malloc(stell);
           
                     if (buffer != NULL)
                     {
                         //fread(buffer, stell, 1, fp);
                      
                             fread(str,stell,1, fp);
                          // printf("%s", str);
                         fclose(fp);
                         fp = NULL;
                         free(buffer);
                     }
                     
                       printf("SIZE: %d", stell);
                     
                     writeToPipe(pipefd2, str,stell);
                     writeToPipe(pipefd, str,stell);
                             struct mymsg message;
                                        
                                        
                                        key_t key = ftok(shm_nameA, 65);
                                         int msgid= msgget(key,IPC_CREAT);
                                        msgrcv(msgid, &message, sizeof(message), 1, 0);
                                        
                                        int shm_fd = shm_open(shm_nameA, O_RDONLY, 0666);
                                        if (shm_fd == -1) {
                                            printf("shared memory A failed\n");
                                            exit(-1);
                                        }
                                        ptr = mmap(0,stell, PROT_READ, MAP_SHARED, shm_fd, 0);
                                        if (ptr == MAP_FAILED) {
                                            printf("Map failed --- opening \n");
                                            exit(-1);
                                        }
                                        
                                        
                                        printf("reading from %s : %s", shm_nameA,ptr);
                     
                     
                 }
    
        }
   
}
    return 0;
}


