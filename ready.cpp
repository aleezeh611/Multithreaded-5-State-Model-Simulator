/*
	Submitted by: Aleezeh Usman 
	Roll #: 18i-0529
*/

#include"DataStructures.h"
using namespace std;
//Ready will not just extract the next process to run, but will also check the time for next process is right i.e a process cant run before the cpu time is equal to or more than its arrival time

int fd;                                 //file descriptor of pipe sending data from main file
cputime cpu_time;
general_info g;
/*
NOTE: Using alot of pipes, for each algorithm individually so we can easily use threads and work in parallel so no time is wasted and program is efficient
*/
int fcfs_fd[2];                            //file desc of pipe to return info about whether or not run FCFS process etc
int sjf_fd[2];                             //file desc of pipe to return info about whether or not run SJF process etc
int srtf_fd[2];                            //file desc of pipe to return info about whether or not run SRTF process etc
int rr_fd[2];                              //file desc of pipe to return info about whether or not run RR process etc


//==========================================THREADS===============================================================================
void* FCFS_Ready(void *thrdarg){
    proc* temp = new proc[g.FCFS];
    bool onlyonce = true;
    for(int i = 0; i < g.FCFS; i++){
        read(fcfs_fd[0],&temp[i],sizeof(temp[i]));
    }
    for(int i = 0; i < g.FCFS; i++){
        if(temp[i].strt <= cpu_time.FCFS){
            if(onlyonce){                                       //to let main know if any new processes are ready or not
                write(fcfs_fd[1], &onlyonce, sizeof(onlyonce));
                onlyonce = false;
            }
            write(fcfs_fd[1], &temp[i], sizeof(temp[i]));       //all processes ready to run coming from new state writted in pipe
        }
    }
    if(onlyonce == true) {                                      //if no process ready the send false
        onlyonce = false;
        write(fcfs_fd[1],&onlyonce, sizeof(onlyonce));
    }
}

void* SJF_Ready(void *thrdarg){
    proc* temp = new proc[g.SJF];
    bool onlyonce = true;
    for(int i = 0; i < g.SJF; i++){
        read(sjf_fd[0],&temp[i],sizeof(temp[i]));
    }
    for(int i = 0; i < g.SJF; i++){
        if(temp[i].strt <= cpu_time.SJF){
            if(onlyonce){                                       //need a bool to let main know if any processes or not 
                write(sjf_fd[1], &onlyonce, sizeof(onlyonce));
                onlyonce = false;
            }
            write(sjf_fd[1], &temp[i], sizeof(temp[i]));       //all processes ready to run coming from new state written in pipe
        }
    }

    if(onlyonce == true) {                                      //if no processes send false
        onlyonce = false;
        write(sjf_fd[1],&onlyonce, sizeof(onlyonce));
    }
}

void* RR_Ready(void *thrdarg){
    proc* temp = new proc[g.RR];
    bool onlyonce = true;
    for(int i = 0; i < g.RR; i++){
        read(rr_fd[0],&temp[i],sizeof(temp[i]));
    }
    for(int i = 0; i < g.RR; i++){
        if(temp[i].strt <= cpu_time.RR){
            if(onlyonce){                                       //to let main know if any new processes are ready or not
                write(rr_fd[1], &onlyonce, sizeof(onlyonce));
                onlyonce = false;
            }
            write(rr_fd[1], &temp[i], sizeof(temp[i]));       //all processes ready to run coming from new state writted in pipe
        }
    }
    if(onlyonce == true) {                                      //if no process ready the send false
        onlyonce = false;
        write(rr_fd[1],&onlyonce, sizeof(onlyonce));
    }
}

void* SRTF_Ready(void *thrdarg){
    proc* temp = new proc[g.SRTF];
    bool onlyonce = true;
    for(int i = 0; i < g.SRTF; i++){
        read(srtf_fd[0],&temp[i],sizeof(temp[i]));
    }
    for(int i = 0; i < g.SRTF; i++){
        if(temp[i].strt <= cpu_time.SRTF){
            if(onlyonce){                                      //to let main know if any processes ready yet or not
                write(srtf_fd[1], &onlyonce, sizeof(onlyonce));
                onlyonce = false;
            }
            write(srtf_fd[1], &temp[i], sizeof(temp[i]));       //all processes ready to run coming from new state written in pipe
        }
    }

    if(onlyonce == true) {                                      //if no ready processes then send false
        onlyonce = false;
        write(srtf_fd[1],&onlyonce, sizeof(onlyonce));
    }
}

//===============================================================================================================================
//                                          MAIN CODE
//===============================================================================================================================
int main(int argc, char* argv[]){
    pthread_t ready_thid[NUM_ALGOS];
    pthread_t srtfready;
    
    fd = int(argv[1][0]);                            //file descriptor of pipe sending data from main file
    read(fd, &fcfs_fd[1], sizeof(fcfs_fd[1]));       //both read and write file descriptors of each individual pipe
    read(fd, &fcfs_fd[0], sizeof(fcfs_fd[0]));
    read(fd,&sjf_fd[1],sizeof(sjf_fd[1]));
    read(fd,&sjf_fd[0],sizeof(sjf_fd[0]));
    read(fd,&srtf_fd[1],sizeof(srtf_fd[1]));
    read(fd,&srtf_fd[0],sizeof(srtf_fd[0]));
    read(fd,&rr_fd[1],sizeof(rr_fd[1]));
    read(fd,&rr_fd[0],sizeof(rr_fd[0]));
    read(fd,&cpu_time,sizeof(cpu_time) );           //current cpu times for each individual algos     
    read(fd,&g,sizeof(g) );                         //current general info for each individual algos     

    if(g.FCFS>0)                           //schedule FCFS procs and do all work if procs are there, if no more procs no need to waste time checking any further
		pthread_create(&ready_thid[0], NULL, FCFS_Ready, NULL);    
    if(g.SJF>0)
        pthread_create(&ready_thid[1], NULL, SJF_Ready, NULL);
    if(g.RR>0)
		pthread_create(&ready_thid[2], NULL, RR_Ready, NULL);
	if(g.SRTF>0)
        pthread_create(&srtfready, NULL, SRTF_Ready, NULL);

    pthread_join(srtfready, NULL);			    	//wait for each thread and get its return status

	for(int i = 0; i < 3 ; i++)
    pthread_join(ready_thid[i], NULL);			    	//wait for each thread and get its return status

}