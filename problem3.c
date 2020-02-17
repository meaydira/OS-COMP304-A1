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


void writeToPipe(int fd[2], char string[]){
       close(fd[READ_END]);

    
         write(fd[WRITE_END], string, strlen(string)+1);

         close(fd[WRITE_END]);
}



void readFromPipe(int pipefd[2], char input[]){
           close(pipefd[WRITE_END]);
           read(pipefd[READ_END], input, BUFFERSIZE);
           printf("child reads %d\n",strlen(input));
           close(pipefd[READ_END]);
}


static long readSize(int pipefd[2]){
       long size;
     close(pipefd[WRITE_END]);
     read(pipefd[READ_END], size, sizeof(size));
     printf("Size is %d",size);
     close(pipefd[READ_END]);
    return size;
}


void createSharedMemory(void *ptr, int size, char * name){
    int shmfd;

        shmfd=shm_open(name, O_CREAT| O_RDWR, 0);
        ftruncate(shmfd, size);
        ptr =mmap(0, size, PROT_WRITE|PROT_READ, MAP_SHARED, shmfd, 0);
    if (ptr == MAP_FAILED) {
           printf("Map failed --- create \n");
           exit(-1);
       }
}

void openSharedMemory(char * name, void *ptr, int SIZE){
    int shm_fd = shm_open(name, O_RDONLY, 0666);
    if (shm_fd == -1) {
        printf("shared memory failed\n");
        exit(-1);
    }

    /* now map the shared memory segment in the address space of the process */
    ptr = mmap(0,SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        printf("Map failed --- opening \n");
        exit(-1);
    }

}

void sendMessage(key_t key){
    char message[] = "Done";
       printf("HI IM GONNA SEND A MESSAGE");
    int msgid = msgget(key,  IPC_CREAT);
    
      
       // msgsnd to send message
       printf("Data send is : %s \n", message);
       msgsnd(msgid, &message, sizeof(message), 0);

      

}

void convertToHex(char* str, char* hex)
{
    int count=0;
    int i =0;
    
    while(str[count] != '\0')
    {
        sprintf((char*)(hex+i),"%X", str[count]);
        i=i+2;
        count+=1;
        
    }
    hex[i] = '\0';
}


int main()
{
    
    struct mymsg {
        long mesg_type;
        char mesg_text[MESSAGESIZE];
    } message;
    
    int pipefd[2];
    int pipefd2[2];
    int shmfd;
    void *ptr_a;
    void *ptr_b;
    void *ptr;
      int childA_pid;
      int childB_pid;
    char* const shm_nameA="sharedmemoryAs";
    char* const shm_nameB="sharedmemoryB";
    
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
        
             char input[BUFFERSIZE];
             readFromPipe(pipefd, input);
              printf("Child A is here, input read, size is: %d \n", strlen(input));
        
        
       // createSharedMemory(ptr_a,BUFFERSIZE,shm_nameA);
        if (shm_unlink(shm_nameA) == -1) {
                                 printf("Error removing %s\n",shm_nameA);
                                 exit(-1);
                             }
        int shmfd = shm_open(shm_nameA, O_CREAT| O_RDWR, 0);
               ftruncate(shmfd, BUFFERSIZE);
               ptr_a =mmap(0, BUFFERSIZE, PROT_WRITE|PROT_READ, MAP_SHARED, shmfd, 0);
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
                  int msgid = msgget(key,  IPC_CREAT);
                     printf("Message is : %s MSGİD is %d \n", message.mesg_text, msgid);
        if ( msgsnd(msgid, &message, sizeof(message), 0)==-1) {
            fprintf(stderr, "Message from A could not be sended.\n");
        }
       // sendMessage(key);
           // childA();
          
        }
    else{
        childB_pid = fork();
        if(childB_pid == 0){
             
            char input[BUFFERSIZE];
            readFromPipe(pipefd2, input);
              printf("Child B is here, input read, size is: %d \n",strlen(input));
            if (shm_unlink(shm_nameB) == -1) {
                           printf("Error removing %s\n",shm_nameA);
                           exit(-1);
                       }
             int shmfd=shm_open(shm_nameB, O_CREAT| O_RDWR, 0666);
                           ftruncate(shmfd, BUFFERSIZE*2);
                           ptr_b =mmap(0, BUFFERSIZE*2, PROT_WRITE|PROT_READ, MAP_SHARED, shmfd, 0);
                       if (ptr_b == MAP_FAILED) {
                              printf("Map failed --- create \n");
                              exit(-1);
                          }
            
         
            
            char result[BUFFERSIZE*2] ;
            
            convertToHex(input,result);
           //  printf("HEX FORMAT OF %s IS %s  \n",input,result);
            
            sprintf(ptr_a,"%s",result);
                     ptr_a += strlen(result);
            
            key_t key = ftok(shm_nameA, 65);
                  printf("key created");
                message.mesg_type = 1;
                char msg[MESSAGESIZE]="Done";
                for(int i = 0 ; i<strlen(msg)+1;i++){
                      message.mesg_text[i]=msg[i];
                }
            
            
        /*     int msgid = msgget(key,  IPC_CREAT);
                if ( msgsnd(msgid, &message, sizeof(message), 0)==-1) {
                    fprintf(stderr, "Message from B could not be sended.\n");
                    //exit(EXIT_FAILURE);
                } */
        
        
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
                     
                     writeToPipe(pipefd2, str);
                     writeToPipe(pipefd, str);
                             
                     
                     
                 }
    
            
           

            
      struct mymsg message;
            
            
            key_t key = ftok(shm_nameA, 65);
             int msgid= msgget(key,IPC_CREAT);
            wait(NULL);
            msgrcv(msgid, &message, sizeof(message), 1, 0);
            
            int shm_fd = shm_open(shm_nameA, O_RDONLY, 0666);
            if (shm_fd == -1) {
                printf("shared memory A failed\n");
                exit(-1);
            }
            ptr = mmap(0,BUFFERSIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
            if (ptr == MAP_FAILED) {
                printf("Map failed --- opening \n");
                exit(-1);
            }
            
            
            printf("reading from %s : %s", shm_nameA,ptr);
            
        
             wait(NULL);
            
          /*        struct mymsg msg;
            key = ftok(shm_nameB, 65);
            msgid = msgget(key,IPC_CREAT);
            msgrcv(msgid, &msg, sizeof(msg), 1, 0);
            
            
                   shm_fd = shm_open(shm_nameB, O_RDONLY, 0666);
                       if (shm_fd == -1) {
                           printf("shared memory failed\n");
                           exit(-1);
                       }
                       ptr = mmap(0,BUFFERSIZE*2, PROT_READ, MAP_SHARED, shm_fd, 0);
                       if (ptr == MAP_FAILED) {
                           printf("Map failed --- opening \n");
                           exit(-1);
                       }
                         fprintf(stderr,"Hex format is : %s \n",ptr);
                       
                       if (shm_unlink(shm_nameB) == -1) {
                           printf("Error removing %s\n",shm_nameB);
                           exit(-1);
                       }  */
            
           // parent();
      
            
        }
        
        }
    return 0;
}


