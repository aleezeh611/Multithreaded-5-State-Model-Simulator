/*
	Submitted by: Aleezeh Usman 
	Roll #: 18i-0529
*/

#include"DataStructures.h"
using namespace std;

general_info g;
bool fcfs_ifrun;        //whether or not to run, if any process or not 
bool sjf_ifrun;
bool rr_ifrun;
bool srtf_ifrun;
cputime cpu_time;
int rr_fd;
int srtf_fd;
int srtf_next_arrivaltime;

//==========================================THREADS=============================================
void* FCFS_Running(void *thrdarg){
    struct proc* p = (struct proc*)thrdarg;
    while(p->total > 0){
        sleep(1);
        p->total--;                                 //since non-preemptive continue running until process ends
        cpu_time.FCFS++;
    }
    pthread_exit(0);
}

void* SJF_Running(void *thrdarg){
    struct proc* p = (struct proc*)thrdarg;
    while(p->total > 0){
        sleep(1);
        p->total--;                                //since non-preemptive continue running until process ends
        cpu_time.SJF++;
    }
    pthread_exit(0);
}

void* RR_Running(void* thrdarg){
    struct proc* p = (struct proc*) thrdarg;
    bool procended;                             //will let simulator know if processes has completed execution within the Quantum time or not
    bool again = 1;                             //randomly decide whether to let same process run again or let other process from queue run
    srand(time(0));
    while(again){
        if(p->total <= g.Quantum_time){
            procended = true;
            while(p->total>0){
                sleep(1);
                p->total--;
                cpu_time.RR++;
            }
            break;
        }
        else {
            procended = false;                      //will show that proc is not ended and needs to be returned to RR queue
            for(int i = 0; i < g.Quantum_time; i++){
                sleep(1);
                p->total--;
                cpu_time.RR++;
            }
        }
        again = rand()%2 ;                          //if zero next process if one same process will get another time slice
        if(again) cout<<" [time slice given to same RR process again]\n";
    }

    proc temp = *p;
    write(rr_fd, &procended, sizeof(bool));
    bool blockornot;                                //if process has not ended we will decide if it needs to be blocked or not, blocking only happens in multiples of 5 since at 5 ticks of time(as told by assignment pdf)         
    if(procended == false) {
        cout<<" [RR proc not ended]\n";
        if(cpu_time.RR%5==0 || (cpu_time.RR+1)%5==0 || (cpu_time.RR-1)%5==0){
            int timetostayblocked = rand()%10 + 15;
            blockornot = true;                     //true means it needs to be blocked
            write(rr_fd, &blockornot, sizeof(bool));
            write(rr_fd, &timetostayblocked, sizeof(int));
        }
        else{
            blockornot = false;                     //do not block the processes
            write(rr_fd, &blockornot, sizeof(bool));
        }
    }    
    write(rr_fd, &temp, sizeof(temp));    
}


void* SRTF_Running(void *thrdarg){
    struct proc* p = (struct proc*)thrdarg;
    bool procended;
    if(p->total + cpu_time.SRTF < srtf_next_arrivaltime || srtf_next_arrivaltime == 0){   //check if a process will be coming from new while this process running
        procended = true;                               //if no process coming run normally as if non-preemptive
        while(p->total > 0){
            sleep(1);
            p->total--;                                
            cpu_time.SRTF++;
        }
    }
    else{                                               //else run until it is time for another process to enter ready state
        procended = false;
        for(int i = 0; cpu_time.SRTF < srtf_next_arrivaltime ; i++){
            sleep(1);
            p->total--;                               
            cpu_time.SRTF++;
        }
    }
    proc temp = *p;
    write(srtf_fd, &procended, sizeof(bool));
    if(procended == false){
        srand(time(NULL));
        cout<<"[SRTF PROC NOT ENDED]\n";
        bool blockornot = false;
        if(cpu_time.SRTF%5 == 0){
            blockornot = true;
            write(srtf_fd, &blockornot, sizeof(bool));
            int timetoblock = rand()%10 + 15;
            write(srtf_fd, &timetoblock, sizeof(int));
            write(srtf_fd, &cpu_time.SRTF, sizeof(int));
        }
        else 
            write(srtf_fd, &blockornot, sizeof(bool));
        write(srtf_fd, &temp, sizeof(temp));    
    }
    pthread_exit(0);
}

//================================================================================================================================
//                                          MAIN CODE
//================================================================================================================================
int main(int argc, char* argv[]){
    pthread_t running_thid[NUM_ALGOS];
    int writeback;
    int fd = int(argv[1][0]);
    read(fd, &writeback, sizeof(writeback));        //to write back shared structure that all processes require like cpu_time
    read(fd, &g,sizeof(g) );                        //current general info for each individual algos     
    read(fd, &cpu_time, sizeof(cpu_time));
    read(fd, &rr_fd,sizeof(int) );                  //to write back info about whether process completed or not etc     
    read(fd, &srtf_fd,sizeof(int) );                //write back info about whether process completed or not     
    
    proc fcfs_run;
    proc sjf_run;
    proc rr_run;
    proc srtf_run;

//RUNNING FCFS
    read(fd, &fcfs_ifrun, sizeof(fcfs_ifrun));
    if(fcfs_ifrun){                                 //check if any process present to run
        read(fd,&fcfs_run, sizeof(fcfs_run));
        proc* passtothread = &fcfs_run;
        cout<<"FCFS -> "<<*passtothread;
        pthread_create(&running_thid[0], NULL, FCFS_Running, (void*)passtothread); 
    }    
//RUNNING SJF
    read(fd, &sjf_ifrun, sizeof(sjf_ifrun));
    if(sjf_ifrun){                                //check if any processes ready to run                   
        read(fd,&sjf_run, sizeof(sjf_run));
        proc* passtothread = &sjf_run;
        cout<<"SJF -> "<<*passtothread;
        pthread_create(&running_thid[1], NULL, SJF_Running, (void*)passtothread); 
   }
    
//RUNNING RR
    read(fd, &rr_ifrun, sizeof(rr_ifrun));
    if(rr_ifrun){                                 //check if any process present to run
        read(fd,&rr_run, sizeof(rr_run));
        proc* passtothread = &rr_run;
        cout<<"RR -> "<<*passtothread;
        pthread_create(&running_thid[2], NULL, RR_Running, (void*)passtothread); 
    }
    
//RUNNING SRTF
    read(fd, &srtf_ifrun, sizeof(srtf_ifrun));
    if(srtf_ifrun){                                //check if any processes ready to run                   
        read(fd,&srtf_run, sizeof(srtf_run));
        read(fd, &srtf_next_arrivaltime, sizeof(int));
        proc* passtothread = &srtf_run;
        cout<<"SRTF -> "<<*passtothread;
        pthread_create(&running_thid[3], NULL, SRTF_Running, (void*)passtothread); 
   }

//wait for each thread    
	if(fcfs_ifrun) pthread_join(running_thid[0], NULL);
    if(sjf_ifrun) pthread_join(running_thid[1], NULL);
    if(rr_ifrun) pthread_join(running_thid[2], NULL);
    if(srtf_ifrun) pthread_join(running_thid[3], NULL);
    
    write(writeback, &cpu_time, sizeof(cpu_time));              //since time changed so send back new time to main 
}