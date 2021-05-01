/*
	Submitted by: Aleezeh Usman 
	Roll #: 18i-0529

NOTE:
1.	Since I ran all algorithms in parallel therefore writing to file in the loop wouldn't allow me to write the final statistics of each algorithm independantly
	all data would've been mixed up, therefore, for a more efficient system I had to compromise and divide exit state in two parts, 
	the exit state within the loop will temporarily store all exited procs info in a queue (same order as they are executed) while
	the final exec does exactly as the assignment states, use fork to exec and write all stats into a file.
2.	Indeed, this can very easily be done within the loop but that would require us to also run each algorithm one by one which would mean this entire process 
	would run for the sum of each algos total burst time of all processes, while with my program it will approximately run for only 
	the time equal to the algorithm whose total burst time for all processes is highest, since they are all running together. 
3.	Furthermore, i would also like to address the fact that ALOT of exra variables also have to be used due to parallel running since
	I wanted to avoid any confusions or errors and make sure they are all running independantly but in a grander scheme the memory usage would be negligible 
	compared to time saved. 
4.  An important point while building this program was that I imagined that time will only be controlled by the Running State, as according to my understanding
	Ready State and Blocked State are working along side Running State and we can check how much time has passed by which processes have been completed etc, in the
	case that no process is ready yet, running state will check if any processes are ready after 'every second'.
5.  I am not checking whether it is time to unblock a process yet while another process is running because it would have to go to ready state anyways,
	which means overall waiting time would be same, and right after running we show that the process is unblocked, it is ambigous at what 
	exact time it was unblocked, but since time is only changing in Running State it can be imagined that it was unblocked at its exact time 
	and put in ready state. That greatly helps with synchronisation, as the primary component of that is 'time' which is being handled efficiently
	using this method of linear coordination between states and parallel execution between scheduling algorithms.
*/

#include"Utility.h"
using namespace std;

//========================================================================================================================================================
//
//														MAIN CODE BEGINS AFTER THIS 
//
//========================================================================================================================================================

int main(){
	proc* p;
	int fd[2];															//main pipe that will give general variable communication between processes
	int check = pipe(fd);
	if(check < 0 ) {
		cout<<"--- ERROR WHILE CREATING PIPE ---\n";
		exit(0);
	}
	if (fcntl(fd[0], F_SETFL, O_NONBLOCK) < 0) 	exit(2); 
	int x = dup(fd[0]);												//pipe read file descriptor stored so we can pass as exec parameter
	int* ptrx = &x;													//pass pointer to variable in exec 
	int fcfs_fd[2];														//will communicate about fcfs proc specifically 
	check = pipe(fcfs_fd);
	if(check < 0 ) {
		cout<<"--- ERROR WHILE CREATING PIPE ---\n";
		exit(0);
	}
	if (fcntl(fcfs_fd[0], F_SETFL, O_NONBLOCK) < 0) exit(2); 
	int sjf_fd[2];														//will communicate about sjf proc specifically 
	check = pipe(sjf_fd);
	if(check < 0 ) {
		cout<<"--- ERROR WHILE CREATING PIPE ---\n";
		exit(0);
	}
	if (fcntl(sjf_fd[0], F_SETFL, O_NONBLOCK) < 0) exit(2); 
	int srtf_fd[2];														//will communicate about srtf proc specifically 
	check = pipe(srtf_fd);
	if(check < 0 ) {
		cout<<"--- ERROR WHILE CREATING PIPE ---\n";
		exit(0);
	}
	if (fcntl(srtf_fd[0], F_SETFL, O_NONBLOCK) < 0) exit(2); 
	int rr_fd[2];														//will communicate about rr proc specifically 
	check = pipe(rr_fd);
	if(check < 0 ) {
		cout<<"--- ERROR WHILE CREATING PIPE ---\n";
		exit(0);
	}
	if (fcntl(rr_fd[0], F_SETFL, O_NONBLOCK) < 0) exit(2); 



//->STATE 1 = NEW----------------------------------------------------------------------------------------------------------------
	pid_t pid = fork();
	if(pid==0){
	//read from files and store in pipes
		int y = dup(fd[1]);												//pipe write file descriptor stored so we can pass as exec parameter
		int* ptry = &y;													//pass pointer to variable in exec
		execlp("./new","./new", ptry, "/usr", NULL);
	    cout<<"--- ERROR :: NEW EXEC FAILED ---\n";
	}
	else if(pid>0){
		wait(NULL);
		read(fd[0], &g, sizeof(g));									//read general info e.g number of processes etc
		total_procs = g.FCFS + g.SJF + g.SRTF + g.RR;
		p = new proc[total_procs];
		for(int i = 0; i < total_procs ; i++) read(fd[0],&p[i],sizeof(p[i])); //read each process info individually
	
		
		//Use threads to increase efficiency since following jobs can be done in parallel since simply creating heaps and queues
		pthread_t create_thid[NUM_ALGOS];
		pthread_create(&create_thid[0], NULL, CreateFCFSQueue, &p);
		pthread_create(&create_thid[1], NULL, CreateSJFQueue, &p);
		pthread_create(&create_thid[2], NULL, CreateSRTFQueue, &p);
		pthread_create(&create_thid[3], NULL, CreateRRQueue, &p);
		for(int i = 0 ; i < NUM_ALGOS; i++)
		pthread_join(create_thid[i], NULL);				//wait for each thread and get its return status

	}

	int rr_binput[2];												//create pipes for communication between blocked process and the three blocked queues
	int rr_boutput[2];
	int rr_bprinter[2];
	if(pipe(rr_binput)<0) exit(1);
	if (fcntl(rr_binput[0], F_SETFL, O_NONBLOCK) < 0) 	exit(2);
	if(pipe(rr_boutput)<0) exit(1);
	if (fcntl(rr_boutput[0], F_SETFL, O_NONBLOCK) < 0) 	exit(2);
	if(pipe(rr_bprinter)<0) exit(1);
	if (fcntl(rr_bprinter[0], F_SETFL, O_NONBLOCK) < 0) 	exit(2);
	int rr_inputwrite = dup(rr_binput[1]);
	int rr_inputread = dup(rr_binput[0]);
	int rr_outputwrite = dup(rr_boutput[1]);
	int rr_outputread = dup(rr_boutput[0]);
	int rr_printerwrite = dup(rr_bprinter[1]);
	int rr_printerread = dup(rr_bprinter[0]);
	int srtf_binput[2];
	int srtf_boutput[2];
	int srtf_bprinter[2];
	if(pipe(srtf_binput)<0) exit(1);
	if (fcntl(srtf_binput[0], F_SETFL, O_NONBLOCK) < 0) 	exit(2);
	if(pipe(srtf_boutput)<0) exit(1);
	if (fcntl(srtf_boutput[0], F_SETFL, O_NONBLOCK) < 0) 	exit(2);
	if(pipe(srtf_bprinter)<0) exit(1);
	if (fcntl(srtf_bprinter[0], F_SETFL, O_NONBLOCK) < 0) 	exit(2);
	int srtf_inputwrite = dup(srtf_binput[1]);
	int srtf_inputread = dup(srtf_binput[0]);
	int srtf_outputwrite = dup(srtf_boutput[1]);
	int srtf_outputread = dup(srtf_boutput[0]);
	int srtf_printerwrite = dup(srtf_bprinter[1]);
	int srtf_printerread = dup(srtf_bprinter[0]);
	

	int xfcfs = dup(fcfs_fd[1]);									//pipe write file descriptor stored so we can pass as exec parameter
	int yfcfs = dup(fcfs_fd[0]);									//pipe write file descriptor stored so we can pass as exec parameter
	int xsjf = dup(sjf_fd[1]);
	int ysjf = dup(sjf_fd[0]);
	int xsrtf = dup(srtf_fd[1]);
	int ysrtf = dup(srtf_fd[0]);
	int xrr = dup(rr_fd[1]);
	int yrr = dup(rr_fd[0]);
	proc fcfs_toexit;												//to store the current process that has ran and needs to exit
	proc sjf_toexit;
	proc srtf_toexit;
	queue fcfs_storeexit;											//to temporarily store all data of the completed processes
	queue sjf_storeexit;
	queue rr_storeexit;
	queue srtf_storeexit;
	int fcfs_total_procs = g.FCFS;									//to store the actual number of processes in start to send to exit exec in end to write in file
	int sjf_total_procs = g.SJF;
	int srtf_total_procs = g.SRTF;
	int rr_total_procs = g.RR;
	proc rr_returnfromrunning;										//will store the proc returning from running													//will decide whether to block or not

while(total_procs > 0){
	cout<<"FCFS CPUTIME   :"<<cpu_time.FCFS<<endl;
	cout<<"SJF  CPUTIME   :"<<cpu_time.SJF<<endl;
	cout<<"RR  CPUTIME    :"<<cpu_time.RR<<endl;
	cout<<"SRTF  CPUTIME  :"<<cpu_time.SRTF<<endl;
	cout<<"CURRENT NUMBER OF TOTAL PROCESSES: "<<total_procs<<endl<<endl;
//->STATE 2 = READY -------------------------------------------------------------------------------------------------------------
	cout<<"<<---------------IN READY STATE--------------->>\n";
	pid_t pidready = fork();

	if(pidready == 0){
		write(fd[1], &xfcfs, sizeof(xfcfs));							//send file descriptor for FCFS individual pipe
		write(fd[1], &yfcfs, sizeof(yfcfs));							
		write(fd[1], &xsjf, sizeof(xsjf));								//send file descriptor for SJF individual pipe
		write(fd[1], &ysjf, sizeof(ysjf));								
		write(fd[1], &xsrtf, sizeof(xsrtf));							//send file descriptor for SRTF individual pipe
		write(fd[1], &ysrtf, sizeof(ysrtf));							
		write(fd[1], &xrr, sizeof(xrr));								//send file descriptor for RR individual pipe
		write(fd[1], &yrr, sizeof(yrr));								
		write(fd[1],&cpu_time,sizeof(cpu_time) );           			//current cpu times for each individual algos     
		write(fd[1], &g, sizeof(g)); 									//send current general info
		qnode* temp = FCFS_queue.head;
		for(int i = 0; i < g.FCFS; i++){
			proc p_temp = temp->curr_proc;								//send FCFS new procs queue 
			write(fcfs_fd[1], &p_temp, sizeof(p_temp));
			temp = temp->next;
		}
		temp = SJF_queue.head;
		for(int i = 0; i < g.SJF; i++){
			proc p_temp = temp->curr_proc;								//send SJF new procs queue 
			write(sjf_fd[1], &p_temp, sizeof(p_temp));
			temp = temp->next;
		}
		temp = RR_queue.head;
		for(int i = 0; i < g.RR ; i++){
			proc p_temp = temp->curr_proc;								//send RR new procs queue 
			write(rr_fd[1], &p_temp, sizeof(p_temp));
			temp = temp->next;	
		}
		temp = SRTF_queue.head;
		for(int i = 0; i < g.SRTF; i++){
			proc p_temp = temp->curr_proc;								//send SJF new procs queue 
			write(srtf_fd[1], &p_temp, sizeof(p_temp));
			temp = temp->next;
		}
		execlp("./ready","./ready", ptrx,"/usr", NULL);
	    cout<<"--- ERROR :: EXEC FAILED ---\n";
	}	
	else if(pidready > 0){
		wait(NULL);
		proc temp;
		read(fcfs_fd[0],&fcfs_run, sizeof(fcfs_run));				//to see if anything to read even present
		if(fcfs_run)
			while(read(fcfs_fd[0],&temp, sizeof(temp)) > 0){
				FCFS_readyqueue.enqueue(temp);	//directly enqueue into FCFS ready queue from pipe so no extra storage wasted
				FCFS_queue.deletenode(temp);
				g.FCFS -=1;
			}
		read(sjf_fd[0], &sjf_run, sizeof(sjf_run));
		if(sjf_run){
			while(read(sjf_fd[0],&temp, sizeof(temp)) > 0){
				 SJF_readyheap.insertKey(temp);						//directly insert into SJF ready heap from pipe so no extra storage wasted
				 SJF_queue.deletenode(temp);
				 g.SJF--;											//decrease counter that stores number of processes that still need to be admitted in cpu
			}
		}
		read(rr_fd[0],&rr_run, sizeof(rr_run));				//to see if anything to read even present
		if(rr_run){
			while(read(rr_fd[0],&temp, sizeof(temp)) > 0){
				RR_readyqueue.enqueue(temp);	//directly enqueue into FCFS ready queue from pipe so no extra storage wasted
				RR_queue.deletenode(temp);
				g.RR --;
			}
		}
		read(srtf_fd[0], &srtf_run, sizeof(srtf_run));
		if(srtf_run){
			while(read(srtf_fd[0],&temp, sizeof(temp)) > 0){
				if((temp.total != 0 && temp.total < 100) && temp.procnum < 100){
				 SRTF_readyheap.insertKey(temp);						//directly insert into SRTF ready heap from pipe so no extra storage wasted
				 SRTF_queue.deletenode(temp);
				 g.SRTF--;												//decrease counter that stores number of processes that still need to be admitted in cpu
				}
			}
		}
		if(!FCFS_readyqueue.isEmpty()) fcfs_run = true;
			else fcfs_run = false;
		if(!SJF_readyheap.isEmpty()) sjf_run = true;
			else sjf_run = false;
		if(!RR_readyqueue.isEmpty()) rr_run = true;
			else rr_run = false;
		if(!SRTF_readyheap.isEmpty()) srtf_run = true;
			else srtf_run = false;
		cout<<"FCFS READY QUEUE:\n";
		FCFS_readyqueue.display_queue();
		cout<<"FCFS ADMIT QUEUE: \n";
		FCFS_queue.display_queue();
		cout<<endl;
		cout<<"SJF READY HEAP:\n";
		SJF_readyheap.display_heap();
		cout<<"SJF ADMIT QUEUE:\n";
		SJF_queue.display_queue();
		cout<<endl;
		cout<<"RR READY QUEUE:\n";
		RR_readyqueue.display_queue();
		cout<<"RR ADMIT QUEUE: \n";
		RR_queue.display_queue();
		cout<<endl;
		cout<<"SRTF READY HEAP:\n";
		SRTF_readyheap.display_heap();
		cout<<"SRTF ADMIT QUEUE:\n";
		SRTF_queue.display_queue();
		cout<<endl;
	}
//->STATE 3 = RUNNING-------------------------------------------------------------------------------------------------------------
	cout<<"\n<<--------------IN RUNNING STATE--------------->>\n";
	pid_t pid_running = fork();
	if(pid_running==0){
		int y = dup(fd[1]);
		write(fd[1], &y, sizeof(y));									//send write file desc of fd
		write(fd[1], &g, sizeof(g)); 									//send current general info
		write(fd[1], &cpu_time,sizeof(cpu_time) );           			//current cpu times for each individual algos     
		write(fd[1], &xrr, sizeof(xrr));								//send write file descriptor for RR individual pipe(so we can get back incomplete processes and info regarding that stuff)
		write(fd[1], &xsrtf, sizeof(xsrtf));							//send write file descriptor for SRTF individual pipe
		proc temp;
		write(fd[1], &fcfs_run, sizeof(fcfs_run));
		if(fcfs_run){													//if ready queue not empty fcfs_run will be true else false
			temp = FCFS_readyqueue.dequeue();							//send current processes that need to be run to exec file
			write(fd[1], &temp, sizeof(temp));
		}
		write(fd[1], &sjf_run, sizeof(sjf_run));
		if(sjf_run){													//if sjf process ready
			temp = SJF_readyheap.extractMin();							//send to running state
			write(fd[1], &temp, sizeof(temp));
		}
		write(fd[1], &rr_run, sizeof(rr_run));
		if(rr_run){													//if ready queue not empty rr_run will be true else false
			temp = RR_readyqueue.dequeue();							//send current processes that need to be run to exec file
			write(fd[1], &temp, sizeof(temp));
		}
		write(fd[1], &srtf_run, sizeof(srtf_run));
		if(srtf_run){													//if srtf process ready
			temp = SRTF_readyheap.extractMin();							//send to running state
			int num;
			if(SRTF_queue.head != NULL) num = SRTF_queue.head->curr_proc.strt;		//send arrival time of next process in admit queue which will enter ready queue rn 
			else num = 0;
			write(fd[1], &temp, sizeof(temp));
			write(fd[1], &num, sizeof(int));
		}
		execlp("./running","./running", ptrx,"/usr", NULL);
		cout<<"====:: EXEC FAILED ::====\n";
	}
	else if(pid_running > 0){
		wait(NULL);
		if(fcfs_run){
			fcfs_toexit = FCFS_readyqueue.dequeue();					//dequeue the process that has completed it's execution and send to exit state 
		}
		if(sjf_run){
			sjf_toexit = SJF_readyheap.extractMin();
		}
		if(rr_run){
			rr_returnfromrunning = RR_readyqueue.dequeue();
			read(rr_fd[0], &rr_procended, sizeof(rr_procended));
			if(rr_procended==false){
				bool blockornot;										//check if blocked or not
				read(rr_fd[0],&blockornot, sizeof(bool));				
				if(blockornot){
					int timetostayblocked;
					read(rr_fd[0], &timetostayblocked, sizeof(int));	//read time to stay blocked
					read(rr_fd[0], &rr_returnfromrunning, sizeof(proc));
					srand(time(NULL));
					int decidewhichblockedqueue = rand()%3;
					if(decidewhichblockedqueue == 0)
						RR_inputblockedqueue.enqueue(rr_returnfromrunning, (cpu_time.RR+g.Quantum_time), timetostayblocked); //enqueue process, arrival time of proc into blocked queue and the time it needs to stay there in input queue
					if(decidewhichblockedqueue == 1)
						RR_outputblockedqueue.enqueue(rr_returnfromrunning, (cpu_time.RR+g.Quantum_time), timetostayblocked); //enqueue process, arrival time of proc into blocked queue and the time it needs to stay there in printer queue
					if(decidewhichblockedqueue == 2)
						RR_printerblockedqueue.enqueue(rr_returnfromrunning, (cpu_time.RR+g.Quantum_time), timetostayblocked); //enqueue process, arrival time of proc into blocked queue and the time it needs to stay there in printer queue
				}
				else{
					read(rr_fd[0], &rr_returnfromrunning, sizeof(proc));
					RR_readyqueue.enqueue(rr_returnfromrunning);		
				}
			}
			else{
				read(rr_fd[0], &rr_returnfromrunning, sizeof(proc));
			}
		}
		if(srtf_run){
			bool checkblocked;
			srtf_toexit = SRTF_readyheap.extractMin();
			read(srtf_fd[0], &srtf_procended, sizeof(srtf_procended));
			if(srtf_procended == false){								//if process has not ended
				read(srtf_fd[0], &checkblocked, sizeof(bool) );
				if(checkblocked){
					int timetoblock;
					int arrivalinblock;
					read(srtf_fd[0], &timetoblock, sizeof(int));
					read(srtf_fd[0], &arrivalinblock, sizeof(int));
					read(srtf_fd[0], &srtf_toexit, sizeof(proc));
					srand(time(NULL));
					int decidewhichblockqueue = rand()%3;
					if(decidewhichblockqueue == 0)
						SRTF_inputblockedqueue.enqueue(srtf_toexit, arrivalinblock, timetoblock); //enqueue process, arrival time of proc into blocked queue and the time it needs to stay there in input queue
					if(decidewhichblockqueue == 1)
						SRTF_outputblockedqueue.enqueue(srtf_toexit, arrivalinblock, timetoblock); //enqueue process, arrival time of proc into blocked queue and the time it needs to stay there in printer queue
					if(decidewhichblockqueue == 2)
						SRTF_printerblockedqueue.enqueue(srtf_toexit, arrivalinblock, timetoblock); //enqueue process, arrival time of proc into blocked queue and the time it needs to stay there in printer queue
				}
				else{
					read(srtf_fd[0], &srtf_toexit, sizeof(proc));
					SRTF_readyheap.insertKey(srtf_toexit);
				}
			}
		}
		read(fd[0], &cpu_time, sizeof(cpu_time));
	}

//->STATE 4 = BLOCKED-------------------------------------------------------------------------------------------------------------
	bnode bproc_info;
	cout<<"\n<<--------------IN BLOCKED STATE--------------->>\n";
	pid_t pidblocked = fork();
	if (pidblocked == 0){
		write(fd[1], &rr_inputread, sizeof(int));								
		write(fd[1], &rr_inputwrite, sizeof(int));								//send file descriptor for RR input blocked queue pipe
		write(fd[1], &rr_outputread, sizeof(int));								
		write(fd[1], &rr_outputwrite, sizeof(int));								//send file descriptor for RR output blocked queue pipe
		write(fd[1], &rr_printerread, sizeof(int));								
		write(fd[1], &rr_printerwrite, sizeof(int));							//send file descriptor for RR printer queue pipe
		write(fd[1], &srtf_inputread, sizeof(int));							
		write(fd[1], &srtf_inputwrite, sizeof(int));							//send file descriptor for SRTF input blocked queue pipe
		write(fd[1], &srtf_outputread, sizeof(int));								
		write(fd[1], &srtf_outputwrite, sizeof(int));							//send file descriptor for SRTF output blocked queue pipe
		write(fd[1], &srtf_printerread, sizeof(int));								
		write(fd[1], &srtf_printerwrite, sizeof(int));							//send file descriptor for SRTF printer queue pipe
		write(fd[1],&cpu_time,sizeof(cpu_time) );           					//current cpu times for each individual algos     
		cout<<"[RR BLOCKED QUEUES]\n";
	//RR input queue	
		bnode* temp = RR_inputblockedqueue.head;
		int num = RR_inputblockedqueue.count;
		write(rr_binput[1], &num, sizeof(int));
		cout<<"INPUT BLOCKED QUEUE: \n";
		RR_inputblockedqueue.display_queue();
		if(!RR_inputblockedqueue.isEmpty()){
			for(int i = 0; i < RR_inputblockedqueue.count ; i++){
				bproc_info = *temp;
				write(rr_binput[1], &bproc_info, sizeof(bproc_info));
				temp = temp->next;
			}
		}
	//RR output queue
		temp = RR_outputblockedqueue.head;
		cout<<"OUTPUT BLOCKED QUEUE:\n";
		RR_outputblockedqueue.display_queue();
		num = RR_outputblockedqueue.count;
		write(rr_boutput[1], &num, sizeof(int));
		if(!RR_outputblockedqueue.isEmpty()){
			for(int i = 0; i < RR_outputblockedqueue.count ; i++){
				bproc_info = *temp;
				write(rr_boutput[1], &bproc_info, sizeof(bproc_info));
				temp = temp->next;
			}
		}
	//RR printer queue
		temp = RR_printerblockedqueue.head;
		cout<<"PRINTER BLOCKED QUEUE:\n";
		RR_printerblockedqueue.display_queue();
		num = RR_printerblockedqueue.count;
		write(rr_bprinter[1], &num, sizeof(int));
		if(!RR_printerblockedqueue.isEmpty()){
			for(int i = 0; i < RR_printerblockedqueue.count ; i++){
				bproc_info = *temp;
				write(rr_bprinter[1], &bproc_info, sizeof(bproc_info));
				temp = temp->next;
			}
		}
		cout<<"\n[SRTF BLOCKED QUEUES]\n";
	//SRTF input queue
		cout<<"INPUT BLOCKED QUEUE:\n";
		SRTF_inputblockedqueue.display_queue();
		temp = SRTF_inputblockedqueue.head;
		num = SRTF_inputblockedqueue.count;
		write(srtf_binput[1], &num, sizeof(int));
		if(!SRTF_inputblockedqueue.isEmpty()){
			for(int i = 0; i < num; i++){
				bproc_info = *temp;
				write(srtf_binput[1], &bproc_info, sizeof(bproc_info));
				temp = temp->next;
			}
		}
	//SRTF output queue
		cout<<"OUTPUT BLOCKED QUEUE:\n";
		SRTF_outputblockedqueue.display_queue();
		temp = SRTF_outputblockedqueue.head;
		num = SRTF_outputblockedqueue.count;
		write(srtf_boutput[1], &num, sizeof(int));
		if(!SRTF_outputblockedqueue.isEmpty()){
			for(int i = 0; i < num; i++){
				bproc_info = *temp;
				write(srtf_boutput[1], &bproc_info, sizeof(bproc_info));
				temp = temp->next;
			}
		}
	//SRTF printer queue
		cout<<"PRINTER QUEUE:\n";
		SRTF_printerblockedqueue.display_queue();
		temp = SRTF_printerblockedqueue.head;
		num = SRTF_printerblockedqueue.count;
		write(srtf_bprinter[1], &num, sizeof(int));
		if(!SRTF_printerblockedqueue.isEmpty()){
			for(int i = 0; i < num; i++){
				bproc_info = *temp;
				write(srtf_bprinter[1], &bproc_info, sizeof(bproc_info));
				temp = temp->next;
			}
		}

		execlp("./blocked","./blocked", ptrx,"/usr", NULL);
		cout<<"====:: EXEC FAILED ::====\n";
	}
	else if(pidblocked > 0){
		wait(NULL);
		bool rrcheckunblock;											//check if any processes need unblocking or not
		read(rr_binput[0], &rrcheckunblock,sizeof(rrcheckunblock));
		if(rrcheckunblock){
			while(read(rr_binput[0], &bproc_info, sizeof(bproc_info)) >= 0){
				RR_readyqueue.enqueue(bproc_info.curr_proc);
				RR_inputblockedqueue.deletenode(bproc_info.curr_proc);
				cout<<"UNBLOCKING : [RR] "<<bproc_info<<endl;
			}
		}								//check if any processes need unblocking or not
		read(rr_boutput[0], &rrcheckunblock,sizeof(rrcheckunblock));
		if(rrcheckunblock){
			while(read(rr_boutput[0], &bproc_info, sizeof(bproc_info)) >= 0){
				RR_readyqueue.enqueue(bproc_info.curr_proc);
				RR_outputblockedqueue.deletenode(bproc_info.curr_proc);
				cout<<"UNBLOCKING : [RR] "<<bproc_info<<endl;
			}
		}								//check if any processes need unblocking or not
		read(rr_bprinter[0], &rrcheckunblock,sizeof(rrcheckunblock));
		if(rrcheckunblock){
			while(read(rr_bprinter[0], &bproc_info, sizeof(bproc_info)) >= 0){
				RR_readyqueue.enqueue(bproc_info.curr_proc);
				RR_printerblockedqueue.deletenode(bproc_info.curr_proc);
				cout<<"UNBLOCKING : [RR] "<<bproc_info<<endl;
			}
		}
		bool srtfcheckunblock;
		read(srtf_binput[0], &srtfcheckunblock, sizeof(bool));
		if(srtfcheckunblock){
			while(read(srtf_binput[0], &bproc_info, sizeof(bproc_info))>=0){
				SRTF_readyheap.insertKey(bproc_info.curr_proc);
				SRTF_inputblockedqueue.deletenode(bproc_info.curr_proc);
				cout<<"UNBLOCKING: [SRTF] "<<bproc_info<<endl;
			}
		}
		read(srtf_boutput[0], &srtfcheckunblock, sizeof(srtfcheckunblock));
		if(srtfcheckunblock){
			while(read(srtf_boutput[0], &bproc_info, sizeof(bproc_info))>=0){
				SRTF_readyheap.insertKey(bproc_info.curr_proc);
				SRTF_outputblockedqueue.deletenode(bproc_info.curr_proc);
				cout<<"UNBLOCKING: [SRTF] "<<bproc_info<<endl;	
			}
		}
		read(srtf_bprinter[0], &srtfcheckunblock, sizeof(srtfcheckunblock));
		if(srtfcheckunblock){
			while(read(srtf_bprinter[0], &bproc_info, sizeof(bproc_info))>=0){
				SRTF_readyheap.insertKey(bproc_info.curr_proc);
				SRTF_printerblockedqueue.deletenode(bproc_info.curr_proc);
				cout<<"UNBLOCKING: [SRTF] "<<bproc_info<<endl;	
			}
		}
	}
	if(fcfs_run == false) cpu_time.FCFS++;							//if no process in ready yet then cpu time still moves forward
	if(sjf_run == false) cpu_time.SJF++;							//assuming if no process free yet 1 tic would be used to check and all until checked again
	if(rr_run == false) cpu_time.RR++;
	if(srtf_run == false) cpu_time.SRTF++;
//->STATE 4 = EXIT STATE-----------------------------------------------------------------------------------------------------------
	cout<<"\n<<----------------IN EXIT STATE---------------->>\n\n";
	if(fcfs_run){ 
		total_procs--;
		fcfs_toexit.waiting_time = cpu_time.FCFS - fcfs_toexit.burst - fcfs_toexit.strt;
		fcfs_toexit.turnaround = fcfs_toexit.burst + fcfs_toexit.waiting_time;
		fcfs_storeexit.enqueue(fcfs_toexit);
	}
	if(sjf_run){ 
		total_procs--;
		sjf_toexit.waiting_time = cpu_time.SJF - sjf_toexit.burst - sjf_toexit.strt;
		sjf_toexit.turnaround = sjf_toexit.burst + sjf_toexit.waiting_time;
		sjf_storeexit.enqueue(sjf_toexit);
	}
	if(rr_procended){
		rr_procended = false;
		rr_returnfromrunning.waiting_time = cpu_time.RR - rr_returnfromrunning.burst - rr_returnfromrunning.strt;
		rr_returnfromrunning.turnaround = rr_returnfromrunning.burst + rr_returnfromrunning.waiting_time;
		rr_storeexit.enqueue(rr_returnfromrunning);
		total_procs--;
	}
	if(srtf_procended && srtf_run){
		total_procs--;
		srtf_toexit.waiting_time = cpu_time.SRTF - srtf_toexit.burst - srtf_toexit.strt;
		srtf_toexit.turnaround = srtf_toexit.burst + srtf_toexit.waiting_time;
		srtf_storeexit.enqueue(srtf_toexit);
	}
}
pid_t pid_exit = fork();
	if(pid_exit == 0){
		proc temp;
		write(fd[1], &fcfs_total_procs, sizeof(int));
		while(!fcfs_storeexit.isEmpty()){
			temp = fcfs_storeexit.dequeue();
			write(fd[1], &temp, sizeof(temp));
		}

		write(fd[1], &sjf_total_procs, sizeof(int));
		while(!sjf_storeexit.isEmpty()){
			temp = sjf_storeexit.dequeue();
			write(fd[1], &temp, sizeof(temp));
		}
		write(fd[1], &rr_total_procs, sizeof(int));
		while(!rr_storeexit.isEmpty()){
			temp = rr_storeexit.dequeue();
			write(fd[1],&temp,sizeof(temp));			
		}

		write(fd[1], &srtf_total_procs, sizeof(int));
		while(!srtf_storeexit.isEmpty()){
			temp = srtf_storeexit.dequeue();
			write(fd[1], &temp, sizeof(temp));
		}

		execlp("./exit","./exit", ptrx,"/usr", NULL);
		cout<<"====:: EXEC FAILED ::====\n";

	}
	else if(pid_exit > 0){
		wait(NULL);
		cout<<"\n\nPROGRAM TERMINATED :D\n";
	}

delete [] p;
return 0;
}








