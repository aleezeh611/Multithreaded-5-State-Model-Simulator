/*
	Submitted by: Aleezeh Usman 
	Roll #: 18i-0529
*/

#include"DataStructures.h"
using namespace std;

int main(int argc, char* argv[]){
    queue all_the_procs;
    proc temp;
    qnode* currnode;
    int fd = int(argv[1][0]);
    int fcfs_procs, sjf_procs, srtf_procs, rr_procs;
    read(fd, &fcfs_procs, sizeof(int));
    for(int i = 0; i < fcfs_procs ; i++){
        read(fd,&temp, sizeof(temp));
        all_the_procs.enqueue(temp);
    }
    read(fd, &sjf_procs, sizeof(int));
    for(int i = 0; i < sjf_procs ; i++){
        read(fd,&temp, sizeof(temp));
        all_the_procs.enqueue(temp);
    }
    read(fd, &rr_procs, sizeof(int));
    for(int i = 0; i < rr_procs; i++){
        read(fd,&temp, sizeof(temp));
        all_the_procs.enqueue(temp);
    }
    read(fd, &srtf_procs, sizeof(int));
    for(int i = 0; i < srtf_procs ; i++){
        read(fd,&temp, sizeof(temp));
        all_the_procs.enqueue(temp);
    }
    
    int throughput = 0;
    int avgwait = 0;
    int avgturnaround = 0 ;
    int file = open("processes_stats.txt", O_WRONLY);
    if(file < 0) {
        cout<<"------ERROR OPENING EXIT FILE-------\n";
        exit(1);
    }
    dup2(file, 1);
    currnode = all_the_procs.head;
    
    cout<<"FCSF PROCESSES:\n";
    for(int i = 0; i < fcfs_procs ; i++){
        throughput += currnode->curr_proc.burst;
        avgwait += currnode->curr_proc.waiting_time;
        avgturnaround += currnode->curr_proc.turnaround;
        cout << "Process # " << currnode->curr_proc.procnum  << " || ARRIVAL: "<<currnode->curr_proc.strt <<" - BURST: "<<currnode->curr_proc.burst
		<<" - WAITING: "<<currnode->curr_proc.waiting_time<<" - TURNAROUND : "<<currnode->curr_proc.turnaround<<endl;
        currnode = currnode->next;
    }
    cout<<endl;
    cout<<"FCFS STATISTICS: \n";
    cout<<"THROUGHPUT = "<<throughput/fcfs_procs<<"  ||  ";
    cout<<"AVERAGE WAITING TIME = "<<avgwait/fcfs_procs<<"  ||  ";
    cout<<"AVERAGE TURNAROUND TIME = "<<avgturnaround/fcfs_procs<<endl;
    cout<<"-----------------------------------------------------------------------------------\n\n";
    
    throughput = 0;                 //reset all variables to zero so they can be reused
    avgturnaround = 0;
    avgwait = 0;
    cout<<"SJF PROCESSES:\n";
    for(int i = 0; i < sjf_procs ; i++){
        throughput += currnode->curr_proc.burst;
        avgwait += currnode->curr_proc.waiting_time;
        avgturnaround += currnode->curr_proc.turnaround;
        cout << "Process # " << currnode->curr_proc.procnum  << " || ARRIVAL: "<<currnode->curr_proc.strt <<" - BURST: "<<currnode->curr_proc.burst
		<<" - WAITING: "<<currnode->curr_proc.waiting_time<<" - TURNAROUND : "<<currnode->curr_proc.turnaround<<endl;
        currnode = currnode->next;
    }
    cout<<endl;
    cout<<"SJF STATISTICS: \n";
    cout<<"THROUGHPUT = "<<throughput/sjf_procs<<"  ||  ";
    cout<<"AVERAGE WAITING TIME = "<<avgwait/sjf_procs<<"  ||  ";
    cout<<"AVERAGE TURNAROUND TIME = "<<avgturnaround/sjf_procs<<endl;
    cout<<"-----------------------------------------------------------------------------------\n\n";

    throughput = 0;                 //reset all variables to zero so they can be reused
    avgturnaround = 0;
    avgwait = 0;
    cout<<"RR PROCESSES:\n";
    for(int i = 0; i < rr_procs ; i++){
        throughput += currnode->curr_proc.burst;
        avgwait += currnode->curr_proc.waiting_time;
        avgturnaround += currnode->curr_proc.turnaround;
        cout << "Process # " << currnode->curr_proc.procnum  << " || ARRIVAL: "<<currnode->curr_proc.strt <<" - BURST: "<<currnode->curr_proc.burst
		<<" - WAITING: "<<currnode->curr_proc.waiting_time<<" - TURNAROUND : "<<currnode->curr_proc.turnaround<<endl;
        currnode = currnode->next;
    }
    cout<<endl;
    cout<<"RR STATISTICS: \n";
    cout<<"THROUGHPUT = "<<throughput/rr_procs<<"  ||  ";
    cout<<"AVERAGE WAITING TIME = "<<avgwait/rr_procs<<"  ||  ";
    cout<<"AVERAGE TURNAROUND TIME = "<<avgturnaround/rr_procs<<endl;
    cout<<"-----------------------------------------------------------------------------------\n\n";

    throughput = 0;                 //reset all variables to zero so they can be reused
    avgturnaround = 0;
    avgwait = 0;
    cout<<"SRTF PROCESSES:\n";
    for(int i = 0; i < srtf_procs ; i++){
        throughput += currnode->curr_proc.burst;
        avgwait += currnode->curr_proc.waiting_time;
        avgturnaround += currnode->curr_proc.turnaround;
        cout << "Process # " << currnode->curr_proc.procnum  << " || ARRIVAL: "<<currnode->curr_proc.strt <<" - BURST: "<<currnode->curr_proc.burst
		<<" - WAITING: "<<currnode->curr_proc.waiting_time<<" - TURNAROUND : "<<currnode->curr_proc.turnaround<<endl;
        currnode = currnode->next;
    }
    cout<<endl;
    cout<<"SRTF STATISTICS: \n";
    cout<<"THROUGHPUT = "<<throughput/srtf_procs<<"  ||  ";
    cout<<"AVERAGE WAITING TIME = "<<avgwait/srtf_procs<<"  ||  ";
    cout<<"AVERAGE TURNAROUND TIME = "<<avgturnaround/srtf_procs<<endl;
    cout<<"-----------------------------------------------------------------------------------\n\n";

}