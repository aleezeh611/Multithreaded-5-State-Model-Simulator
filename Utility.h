/*
	Submitted by: Aleezeh Usman 
	Roll #: 18i-0529
*/

#include"DataStructures.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~GLOBAL VARIABLES~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
general_info g;						//general information e.g number of processes read for each algorithm stored
int total_procs;	 				//total number of procs of all algorithms
queue FCFS_queue;					//all processes of FCFS in separate queue
queue SRTF_queue;					//all processes of SRTF in separate heap
queue SJF_queue;					//all processes of SJF in separate heap
queue RR_queue;						//all processes of RR in separate queue
queue FCFS_readyqueue;				//all processes of FCFS ready to run
MinHeap SRTF_readyheap;				//all processes of SRTF ready to run
MinHeap SJF_readyheap;				//all processes of SJF ready to run
queue RR_readyqueue;				//all processes of RR ready to run
b_queue RR_inputblockedqueue;		//input blocked processes of RR 
b_queue RR_outputblockedqueue;		//output blocked processes of RR 
b_queue RR_printerblockedqueue;		//printer blocked processes of RR 
b_queue SRTF_inputblockedqueue;		//input blocked processes of SRTF 
b_queue SRTF_outputblockedqueue;	//output blocked processes of SRTF
b_queue SRTF_printerblockedqueue;	//printer blocked processes of SRTF
bool fcfs_run;						//will tell if any process to run rn
bool sjf_run;			
bool srtf_run;
bool rr_run;
bool rr_procended;					//will let simulator know if process ended before next time slice or not
bool srtf_procended;
cputime cpu_time;					//will keep track of CPU times of each scheduling algorithm
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



//utility function to convert from text file to procs etc
//what it will do is: according to each heading it will make the corresponding check true and count the number of processes, in that way processes will all be stores in one proc array in a definite order and that order will be known by computer according to computations 
void OrganizeData(proc p[],int n, char buffer[], int& FCFS_proc, int& SJF_proc, int& SRTF_proc, int& RR_proc, int& Quantum_time){
	int counter;
	bool FCFS_check = false, SJF_check = false, SRTF_check = false, RR_check = false;
	for(int i = 0, j = 0; i < n*6 ; j++){						//reorganizing data extracted from file into a proper data structure to perform scheduling on
		if(buffer[i] == 'F' && buffer[i+1]=='C' && buffer[i+2] == 'F' && buffer[i+3] == 'S'){
			FCFS_check = true;
			i += 4;
			FCFS_proc = 0; 
		}

		if(buffer[i] == 'S' && buffer[i+1]=='J' && buffer[i+2] == 'F'){
			SJF_check = true;
			FCFS_check = false;
			i += 3;
			SJF_proc = 0; 
		}

		if(buffer[i] == 'S' && buffer[i+1]=='R' && buffer[i+2] == 'T' && buffer[i+3] == 'F'){
			SJF_check = false;
			FCFS_check = false;
			SRTF_check = true;
			i += 4; 
			SRTF_proc = 0;
		}

		if(buffer[i] == 'R' && buffer[i+1]=='R'){
			SJF_check = false;
			FCFS_check = false;
			SRTF_check = false;
			RR_check = true;
			i += 2; 
			RR_proc = 0;
			Quantum_time = buffer[i] - '0';							//store quantum time which will always be given right after RR heading
			i+=2;
		}

			if(buffer[i+1] == '\n')	{
				p[j].procnum = buffer[i] - '0';
			}
			else{
				p[j].procnum = (buffer[i] - '0')*10;
				p[j].procnum += (buffer[++i] - '0');
			}
			i+=2;
			if(buffer[i+1] == '\n'){
				p[j].strt = buffer[i] - '0';
			}
			else{
				p[j].strt = (buffer[i] - '0')*10;
				p[j].strt += buffer[++i] - '0';
			}
			i+=2;
			if(buffer[i+1] == '\n'){
				p[j].total = buffer[i] - '0';
			}
			else{
				p[j].total = (buffer[i] - '0')*10;
				p[j].total += (buffer[++i] - '0');
			}
			i+=2;
			//setting rest of the attributes as well
			p[j].burst = p[j].total;
			p[j].waiting_time = 0; 			// we will calculate this later when running and blocking etc 

			if(FCFS_check == true) FCFS_proc++;
			if(SJF_check == true) SJF_proc++;
			if(SRTF_check == true) SRTF_proc++;
			if(RR_check == true) RR_proc++;
	}
}


//==================================================================================================================================
//
//													Functions for Threads
//
//===================================================================================================================================

void* CreateFCFSQueue(void* thrdarg){
	proc** p = (proc **)thrdarg;
	queue temp;
	for(int i = 0; i < g.FCFS; i++){
		temp.enqueue(p[0][i]);
	}
	sort(temp,FCFS_queue);
	pthread_exit(0);
}
void* CreateSJFQueue(void* thrdarg){
	proc** p = (proc **)thrdarg;
	queue temp;
	SJF_readyheap.Initialize(g.SJF);
	for(int i = g.FCFS; i < g.FCFS + g.SJF; i++){
		temp.enqueue(p[0][i]);
	}
	sort(temp, SJF_queue);
	pthread_exit(0);
}
void* CreateSRTFQueue(void* thrdarg){
	proc** p = (proc **)thrdarg;
	queue temp;
	SRTF_readyheap.Initialize(g.SRTF);
	for(int i = g.FCFS + g.SJF; i < g.FCFS + g.SJF + g.SRTF; i++){
		temp.enqueue(p[0][i]);
	}
	sort(temp, SRTF_queue);
	pthread_exit(0);
}
void* CreateRRQueue(void* thrdarg){
	proc** p = (proc **)thrdarg;
	queue temp;
	for(int i = g.FCFS+g.SRTF+g.SJF; i < g.FCFS+g.SRTF+g.SJF+g.RR; i++){
		temp.enqueue(p[0][i]);
	}
	sort(temp, RR_queue);
	pthread_exit(0);
}


