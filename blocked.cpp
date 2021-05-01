/*
	Submitted by: Aleezeh Usman 
	Roll #: 18i-0529
*/

#include"DataStructures.h"
using namespace std;

int rr_ifd[2];                                       //input queue file descriptors
int rr_ofd[2];                                       //output queue file descriptors
int rr_pfd[2];                                       //printer queue file descriptors
int srtf_ifd[2];
int srtf_ofd[2];
int srtf_pfd[2];
cputime cpu_time; 

//========================================THREADS  (PART C)===================================================
void* RR_INPUT(void *thrdarg){
    bnode bproc;
    b_queue rr_blockedqueue;
    bool onlyonce;
    int count;
    read(rr_ifd[0], &count, sizeof(int));
    for(int i = 0 ; i < count; i++){
        read(rr_ifd[0], &bproc, sizeof(bproc));
        rr_blockedqueue.enqueue(bproc.curr_proc, bproc.arrival, bproc.blocked);
    }
    bnode* temp = rr_blockedqueue.head;
    for(int i = 0; i < count ; i++){
        if(cpu_time.RR - temp->arrival >= temp->blocked){
            if(!onlyonce){                          //onlyonce will let main know if any process is being blocked or not
                onlyonce = true;
                write(rr_ifd[1], &onlyonce, sizeof(bool));
            }
            bproc = *temp;
            write(rr_ifd[1], &bproc, sizeof(bnode));
        }
        temp = temp->next;
    }
    if(!onlyonce)   write(rr_ifd[1], &onlyonce, sizeof(bool));
}
void* RR_OUTPUT(void *thrdarg){
    bnode bproc;
    b_queue rr_blockedqueue;
    bool onlyonce;
    int count;
    read(rr_ofd[0], &count, sizeof(int));
    for(int i = 0 ; i < count; i++){
        read(rr_ofd[0], &bproc, sizeof(bproc));
        rr_blockedqueue.enqueue(bproc.curr_proc, bproc.arrival, bproc.blocked);
    }
    bnode* temp = rr_blockedqueue.head;
    for(int i = 0; i < count ; i++){
        if(cpu_time.RR - temp->arrival >= temp->blocked){
            if(!onlyonce){                          //onlyonce will let main know if any process is being blocked or not
                onlyonce = true;
                write(rr_ofd[1], &onlyonce, sizeof(bool));
            }
            bproc = *temp;
            write(rr_ofd[1], &bproc, sizeof(bnode));
        }
        temp = temp->next;
    }
    if(!onlyonce)   write(rr_ofd[1], &onlyonce, sizeof(bool));
}
void* RR_PRINTER(void *thrdarg){
    bnode bproc;
    b_queue rr_blockedqueue;
    bool onlyonce;
    int count;
    read(rr_pfd[0], &count, sizeof(int));
    for(int i = 0 ; i < count; i++){
        read(rr_pfd[0], &bproc, sizeof(bproc));
        rr_blockedqueue.enqueue(bproc.curr_proc, bproc.arrival, bproc.blocked);
    }
    bnode* temp = rr_blockedqueue.head;
    for(int i = 0; i < count ; i++){
        if(cpu_time.RR - temp->arrival >= temp->blocked){
            if(!onlyonce){                          //onlyonce will let main know if any process is being blocked or not
                onlyonce = true;
                write(rr_pfd[1], &onlyonce, sizeof(bool));
            }
            bproc = *temp;
            write(rr_pfd[1], &bproc, sizeof(bnode));
        }
        temp = temp->next;
    }
    if(!onlyonce)   write(rr_pfd[1], &onlyonce, sizeof(bool));
}
void* SRTF_INPUT(void *thrdarg){
    bnode bproc;
    b_queue srtf_blockedqueue;
    bool onlyonce;
    int count;
    read(srtf_ifd[0], &count, sizeof(int));
    for(int i = 0 ; i < count; i++){
        read(srtf_ifd[0], &bproc, sizeof(bproc));
        srtf_blockedqueue.enqueue(bproc.curr_proc, bproc.arrival, bproc.blocked);
    }
    bnode* temp = srtf_blockedqueue.head;
    for(int i = 0; i < count ; i++){
        if(cpu_time.SRTF - temp->arrival >= temp->blocked){
            if(!onlyonce){                          //onlyonce will let main know if any process is being blocked or not
                onlyonce = true;
                write(srtf_ifd[1], &onlyonce, sizeof(bool));
            }
            bproc = *temp;
            write(srtf_ifd[1], &bproc, sizeof(bnode));
        }
        temp = temp->next;
    }
    if(!onlyonce)   write(srtf_ifd[1], &onlyonce, sizeof(bool));
}

void* SRTF_OUTPUT(void *thrdarg){
    bnode bproc;
    b_queue srtf_blockedqueue;
    bool onlyonce;
    int count;
    read(srtf_ofd[0], &count, sizeof(int));
    for(int i = 0 ; i < count; i++){
        read(srtf_ofd[0], &bproc, sizeof(bproc));
        srtf_blockedqueue.enqueue(bproc.curr_proc, bproc.arrival, bproc.blocked);
    }
    bnode* temp = srtf_blockedqueue.head;
    for(int i = 0; i < count ; i++){
        if(cpu_time.SRTF - temp->arrival >= temp->blocked){
            if(!onlyonce){                          //onlyonce will let main know if any process is being blocked or not
                onlyonce = true;
                write(srtf_ofd[1], &onlyonce, sizeof(bool));
            }
            bproc = *temp;
            write(srtf_ofd[1], &bproc, sizeof(bnode));
        }
        temp = temp->next;
    }
    if(!onlyonce)   write(srtf_ofd[1], &onlyonce, sizeof(bool));
}

void* SRTF_PRINTER(void *thrdarg){
    bnode bproc;
    b_queue srtf_blockedqueue;
    bool onlyonce;
    int count;
    read(srtf_pfd[0], &count, sizeof(int));
    for(int i = 0 ; i < count; i++){
        read(srtf_pfd[0], &bproc, sizeof(bproc));
        srtf_blockedqueue.enqueue(bproc.curr_proc, bproc.arrival, bproc.blocked);
    }
    bnode* temp = srtf_blockedqueue.head;
    for(int i = 0; i < count ; i++){
        if(cpu_time.SRTF - temp->arrival >= temp->blocked){
            if(!onlyonce){                          //onlyonce will let main know if any process is being blocked or not
                onlyonce = true;
                write(srtf_pfd[1], &onlyonce, sizeof(bool));
            }
            bproc = *temp;
            write(srtf_pfd[1], &bproc, sizeof(bnode));
        }
        temp = temp->next;
    }
    if(!onlyonce)   write(srtf_pfd[1], &onlyonce, sizeof(bool));
}

//==========================================MAIN CODE=========================================================
int main(int argc, char* argv[]){
    pthread_t b_thid[6];
    int fd = int(argv[1][0]);                        //file descriptor of pipe sending data from main file
    read(fd,&rr_ifd[0],sizeof(int));
    read(fd,&rr_ifd[1],sizeof(int));
    read(fd,&rr_ofd[0],sizeof(int));
    read(fd,&rr_ofd[1],sizeof(int));
    read(fd,&rr_pfd[0],sizeof(int));
    read(fd,&rr_pfd[1],sizeof(int));
    read(fd,&srtf_ifd[0],sizeof(int));
    read(fd,&srtf_ifd[1],sizeof(int));
    read(fd,&srtf_ofd[0],sizeof(int));
    read(fd,&srtf_ofd[1],sizeof(int));
    read(fd,&srtf_pfd[0],sizeof(int));
    read(fd,&srtf_pfd[1],sizeof(int));
    read(fd,&cpu_time,sizeof(cpu_time) );            //current cpu times for each individual algos     
    
    pthread_create(&b_thid[0], NULL, RR_INPUT, NULL); 
    pthread_create(&b_thid[1], NULL, RR_OUTPUT, NULL);
    pthread_create(&b_thid[2], NULL, RR_PRINTER, NULL);
    pthread_create(&b_thid[3], NULL, SRTF_INPUT, NULL); 
    pthread_create(&b_thid[4], NULL, SRTF_OUTPUT, NULL);
    pthread_create(&b_thid[5], NULL, SRTF_PRINTER, NULL);
   
    pthread_join(b_thid[0], NULL);
    pthread_join(b_thid[1], NULL);
    pthread_join(b_thid[2], NULL);
    pthread_join(b_thid[3], NULL);
    pthread_join(b_thid[4], NULL);
    pthread_join(b_thid[5], NULL);
    
    return 1;
}