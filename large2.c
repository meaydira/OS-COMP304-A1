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
#include <limits.h>


#define MESSAGESIZE 10
#define READ_END    0
#define WRITE_END    1
#define BUFFERSIZE PIPE_BUF

int fsize(FILE *fp){
    int prev=ftell(fp);
    fseek(fp, 0L, SEEK_END);
    int sz=ftell(fp);
    fseek(fp,prev,SEEK_SET); //go back to where we were
    return sz;
}

void convertToHex(char str[], char output[], int size)
{
    int count=0;
    int i =0;
    
    for (int i = 0, j = 0; i < size; ++i, j += 2)
        sprintf(output + j, "%02x", str[i] & 0xff);
    
    //  printf("'%s' in hex is %s.\n", str, output);
}



void writeToPipe(int fd[2], char string[], long stell){
    
    close(fd[READ_END]);
    write(fd[WRITE_END], &stell, sizeof(stell));
    char * str = string;
    if(stell> PIPE_BUF){
        printf("size is : %d \n", stell);
        int count = stell+1;
        
        while(count > PIPE_BUF){
            //   printf("count is : %d \n", count);
            write(fd[WRITE_END], str, PIPE_BUF);
            count= count - PIPE_BUF;
            str+= PIPE_BUF;
        }
        //   printf("count is : %d \n", count);
        write(fd[WRITE_END], str, count+1);
        
    }
    
    close(fd[WRITE_END]);
}



static long readSize(int pipefd[2]){
    long size;
    close(pipefd[WRITE_END]);
    read(pipefd[READ_END], &size, sizeof(size));
    // printf("Size is %d \n",size);
    close(pipefd[READ_END]);
    return size;
}


struct message {
    long mtype;
    char mtext[MESSAGESIZE];
};

int main()
{
    
    
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
    int msqid = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    if (msqid == -1) {
        perror("msgget");
        
        exit(-1);
    }
    
    int msq2id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    if (msqid == -1) {
        perror("msgget");
        
        exit(-1);
    }
    
    
    
    
    childA_pid = fork();
    
        if(childA_pid == 0){
        //childA
        
        long SIZE;
        close(pipefd[WRITE_END]);
        read(pipefd[READ_END], &SIZE, sizeof(SIZE));
        // printf("Size is %d \n",SIZE);
        // printf("Buffer size is %d \n",BUFFERSIZE);
        char input[SIZE];
        int stell = SIZE;
        char *p = input;
        if(stell> PIPE_BUF){
            int count = stell-1;
            
            while(count > PIPE_BUF){
                read(pipefd[READ_END], p, PIPE_BUF);
                count= count - PIPE_BUF;
                p+= PIPE_BUF;
            }
            
            read(pipefd[READ_END],p, count);
            
        }
        printf("child reads input, size : %d \n",stell);
        close(pipefd[READ_END]);
        
        
        printf("Child A here, input is read. input is : %d \n", strlen(input));
        // printf("Size is %d : %s \n", strlen(input));
        
        int shmfd = shm_open(shm_nameA, O_CREAT| O_RDWR, 0666);
        ftruncate(shmfd, SIZE);
        ptr_a =mmap(0,SIZE, PROT_WRITE|PROT_READ, MAP_SHARED, shmfd, 0);
        if (ptr_a == MAP_FAILED) {
            printf("Map failed --- create \n");
            exit(-1);
        }
        
        ///
        printf("shared memory A created \n");
        
        sprintf(ptr_a,"%s",input);
        ptr_a += strlen(input);
        
        
        
        
        struct message message;
        message.mtype = 1;
        memset(&(message.mtext), 0,MESSAGESIZE * sizeof(char));
        (void)strcpy(message.mtext, "Done");
        
        /* send message to queue */
        if (msgsnd(msqid, &message,MESSAGESIZE, 0) == -1) {
            perror("msgsnd");
            
        }
        
        printf("Message: %s is sent from Child A. \n", message.mtext);
        
        
    }
    else{
        childB_pid = fork();
        if(childB_pid == 0){
            long SIZE;
            close(pipefd2[WRITE_END]);
            read(pipefd2[READ_END], &SIZE, sizeof(SIZE));
            // printf("Size is %d \n",SIZE);
            // printf("Buffer size is %d \n",BUFFERSIZE);
            char input[SIZE];
            int stell = SIZE;
            char *p = input;
            if(stell> PIPE_BUF){
                int count = stell-1;
                
                while(count > PIPE_BUF){
                    read(pipefd2[READ_END], p, PIPE_BUF);
                    count= count - PIPE_BUF;
                    p+= PIPE_BUF;
                }
                
                read(pipefd2[READ_END],p, count);
                
            }
            
            printf("Child B here, input is read. length is : %d , input is : %s \n", strlen(input), input);
            
            
            int shmfd = shm_open(shm_nameB, O_CREAT| O_RDWR, 0666);
            ftruncate(shmfd, SIZE*2);
            ptr_b =mmap(0,SIZE*2, PROT_WRITE|PROT_READ, MAP_SHARED, shmfd, 0);
            if (ptr_b == MAP_FAILED) {
                perror("Map failed --- create \n");
                exit(-1);
            }
            
            ///
            printf("shared memory B created \n");
            
            char output[SIZE*2];
            convertToHex(input, output, SIZE);
            
            sprintf(ptr_b,"%s",output);
            ptr_b += strlen(output);
            
            
            struct message message;
            message.mtype = 1;
            memset(&(message.mtext), 0,MESSAGESIZE * sizeof(char));
            (void)strcpy(message.mtext, "Done");
            
            /* send message to queue */
            if (msgsnd(msq2id, &message,MESSAGESIZE, 0) == -1) {
                perror("msgsnd");
                
            }
            
            printf("Message: %s is sent from Child B. \n", message.mtext);
            
            
        }else{
            //parent
            char *buffer;
            FILE *fp = stdin;
            
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
                
                
                writeToPipe(pipefd2, str,stell);
                writeToPipe(pipefd, str,stell);
                //   printf("Input is written to pipes: size %d \n", stell);
                
                struct message message;
                if (msgrcv(msqid, &message, MESSAGESIZE, 0, 0) == -1) {
                    perror("msgrcv");
                    
                    exit(-1);
                }
                
                printf("Messsage received from A is : %s\n", message.mtext);
                
                
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
                
                
                printf("reading from  A : %s", ptr);
                
                if (shm_unlink(shm_nameA) == -1) {
                    printf("Error removing %s\n",shm_nameA);
                    exit(-1);
                }
                
                
            
                              if (msgrcv(msq2id, &message, MESSAGESIZE, 0, 0) == -1) {
                                  perror("msgrcv");
                                  
                                  exit(-1);
                              }
                
                 printf("Messsage received from B is : %s\n", message.mtext);
                
                shm_fd = shm_open(shm_nameB, O_RDONLY, 0666);
                if (shm_fd == -1) {
                    printf("shared memory B failed\n");
                    perror("memory fail");
                    exit(-1);
                }
                
                ptr = mmap(0,stell*2, PROT_READ, MAP_SHARED, shm_fd, 0);
                if (ptr == MAP_FAILED) {
                    printf("Map failed --- opening \n");
                    exit(-1);
                }
                
                
                fprintf(stderr,"Hex format from Child B is : %s \n",ptr);
                
                // printf("reading from  B : %s", ptr);
                
                if (shm_unlink(shm_nameB) == -1) {
                    printf("Error removing %s\n",shm_nameB);
                    exit(-1);
                }
            }
            
        }
           
    }
     return 0;
}


