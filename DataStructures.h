/*
	Submitted by: Aleezeh Usman 
	Roll #: 18i-0529
*/

#include<iostream>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<stdio.h>
#include<pthread.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<fcntl.h> 
#include<errno.h> 
#include<pthread.h>
#include<climits>
#include<time.h>
using namespace std;
#define NUM_ALGOS 4

struct general_info{
	int FCFS;					//will store number of process not yet in ready
	int SJF;
	int SRTF;
	int RR;
	int Quantum_time;
};
struct cputime{
	int FCFS;
	int SJF;
	int SRTF;
	int RR;
	cputime(){
		FCFS = SJF = SRTF = RR = 0;		//CPU TIME FOR ALL 0 IN THE START
	}	
};
struct proc{
    int procnum;
	int strt;
	int total;
	int waiting_time;
	int burst;
	int turnaround;

	proc(){
		procnum = strt = total = waiting_time = 0;
		turnaround = burst = 0;
	}
	proc(int num, int st, int ed){
		procnum = num;
		strt = st;
		total = ed;
	}
};
ostream& operator<<(ostream &out, const proc& p ){
		out<<"PROC# "<<p.procnum<<" || ARRIVAL: "<<p.strt<<" - BURST: "<<p.total<<endl;
		return out;
}

//Node for queues of all procs
struct qnode{
	proc curr_proc;
	qnode* next;
	qnode(){
		next = NULL;
	}
};
//Queue of Processes
class queue{
	public:
	qnode* head;
	qnode* tail;
	int count;

	queue()	{
		head = NULL;
		tail = NULL;
		count = 0;
	}
	void enqueue(proc np){
		qnode *newNode = new qnode;
		newNode->curr_proc.procnum = np.procnum; 
		newNode->curr_proc.strt = np.strt;
		newNode->curr_proc.waiting_time = np.waiting_time;
		newNode->curr_proc.total = np.total;
		newNode->curr_proc.burst = np.burst;
		newNode->curr_proc.turnaround = np.turnaround;
		if(head == NULL){
			head = newNode;
			tail = newNode;
		}
		else{
			if(head == tail){
				head->next = newNode;
				tail = newNode;
			}
			else{
				tail->next = newNode;
				tail = newNode; 
			}
		}
		count++;
	}
	proc dequeue(){
		proc to_ret;
		if(head!=NULL){
			if(head == tail){
				to_ret = head->curr_proc;
				delete head;
				head = NULL;
				tail = NULL;
			}
			else{
				to_ret = head->curr_proc;
				qnode* temp = head;
				head = head->next;
				delete temp;
			}	
		count--;
		}
		return to_ret;
	}
	void deletenode(proc p){
		qnode* temp = head;
		if(head->curr_proc.procnum == p.procnum){	//in case first node to delete or only 1 node present
			qnode* todelete = head;
			if(head == tail){
				tail = NULL;
				head = NULL;
			}
			else{
				qnode* todelete = head;
				head = head->next;
			}
			delete todelete;
			count--;
		}
		else{
			while(temp->next!=NULL){				//any other node in queue
				if(temp->next->curr_proc.procnum == p.procnum) break;
				temp = temp->next;
			}
			if(temp->next!=NULL){
				qnode* todelete = temp->next;
				if(temp->next == tail) tail = temp;
				temp->next = temp->next->next;
				delete todelete;
			}	
			count--;
		}
		//cout<<p<<" node deleted\n";
	}
	void display_queue(){
		qnode *curr=head;
		while(curr!=NULL){
			cout << "PROC# " << curr->curr_proc.procnum  << " || START: "<<curr->curr_proc.strt <<" - BURST: "<<curr->curr_proc.total<<endl;
			curr=curr->next;
		}
	}
	void display_exitqueue(){
		qnode *curr=head;
		while(curr!=NULL){
			cout << "Process # " << curr->curr_proc.procnum  << " || ARRIVAL: "<<curr->curr_proc.strt <<" - BURST: "<<curr->curr_proc.burst
			<<" - WAITING: "<<curr->curr_proc.waiting_time<<" - TURNAROUND : "<<curr->curr_proc.turnaround<<endl;
			curr=curr->next;
		}
	}
	bool isEmpty(){
		if(head == NULL && tail == NULL)	return true;
		else	return false;
	}
	proc top(){
		proc to_ret = head->curr_proc;
		return to_ret;
	}
	~queue(){
		while(!isEmpty()){
			dequeue();
		}
	}
};
void sort(queue& prev, queue& newq){
		int min = 9999;
		proc store;
		qnode* temp;
		while(!prev.isEmpty()){
			temp = prev.head;
			while(temp != NULL){
				if(temp->curr_proc.strt < min){
					store = temp->curr_proc;
					min = store.strt;
				}
				temp = temp->next;
			}
			newq.enqueue(store);
			prev.deletenode(store);
			min = 9999;
		}
}

//Node for blocked queue
struct bnode{
	int arrival;
	int blocked;
	proc curr_proc;
	bnode* next;
	bnode(){
		next = NULL;
	}
};

ostream& operator<<(ostream &out, const bnode& p ){
		out<<"PROC# "<<p.curr_proc.procnum<<" -> "<<p.arrival<<" "<<p.blocked<<endl;
		return out;
}

class b_queue{
	public:
	bnode* head;
	bnode* tail;
	int count;

	b_queue()	{
		head = NULL;
		tail = NULL;
		count = 0;
	}
	void enqueue(proc np, int arr, int blckd){
		bnode *newNode = new bnode;
		newNode->curr_proc.procnum = np.procnum; 
		newNode->curr_proc.strt = np.strt;
		newNode->curr_proc.waiting_time = np.waiting_time;
		newNode->curr_proc.total = np.total;
		newNode->curr_proc.burst = np.burst;
		newNode->curr_proc.turnaround = np.turnaround;
		newNode->arrival = arr;
		newNode->blocked = blckd;
		if(head == NULL){
			head = newNode;
			tail = newNode;
		}
		else{
			if(head == tail){
				head->next = newNode;
				tail = newNode;
			}
			else{
				tail->next = newNode;
				tail = newNode; 
			}
		}
		count++;
	}
	proc dequeue(){
		proc to_ret;
		if(head!=NULL){
			if(head == tail){
				to_ret = head->curr_proc;
				delete head;
				head = NULL;
				tail = NULL;
			}
			else{
				to_ret = head->curr_proc;
				bnode* temp = head;
				head = head->next;
				delete temp;
			}	
		count--;
		}
		return to_ret;
	}
	void deletenode(proc p){
		bnode* temp = head;
		if(head->curr_proc.procnum == p.procnum){	//in case first node to delete or only 1 node present
			bnode* todelete = head;
			if(head == tail){
				tail = NULL;
				head = NULL;
			}
			else{
				bnode* todelete = head;
				head = head->next;
			}
			delete todelete;
			count--;
		}
		else{
			while(temp->next!=NULL){				//any other node in queue
				if(temp->next->curr_proc.procnum == p.procnum) break;
				temp = temp->next;
			}
			if(temp->next!=NULL){
				bnode* todelete = temp->next;
				if(temp->next == tail) tail = temp;
				temp->next = temp->next->next;
				delete todelete;
			}	
			count--;
		}
		//cout<<p<<" node deleted\n";
	}
	void display_queue(){
		bnode *curr=head;
		while(curr!=NULL){
			cout << "Process # " << curr->curr_proc.procnum  << " || Arrival in Blocked: "<<curr->arrival<<" - Stay: "<<curr->blocked<<endl;
			curr=curr->next;
		}
	}
	bool isEmpty(){
		if(head == NULL && tail == NULL)	return true;
		else	return false;
	}
	proc top(){
		proc to_ret = head->curr_proc;
		return to_ret;
	}
	~b_queue(){
		while(!isEmpty()){
			dequeue();
		}
	}
};



// A utility function to swap two elements 
void swap(proc *x, proc *y) 
{ 
    proc temp = *x; 
    *x = *y; 
    *y = temp; 
} 

// A class for Min Heap 
class MinHeap 
{ 
	public:
    proc *harr; // pointer to array of elements in heap 
    int capacity; // maximum possible size of min heap 
    int heap_size; // Current number of elements in min heap 
    // Constructor 
    MinHeap(int cap = 0){
	    heap_size = 0; 
    	capacity = cap;
		if(cap!=0) 
    	harr = new proc[cap]; 
	}

	void Initialize(int cap){
	    heap_size = 0; 
    	capacity = cap;
		if(cap!=0) 
    	harr = new proc[cap]; 
		for(int i = 0; i < cap; i++) harr[i].procnum = -1;			//to show that it is empty rn
	}
	
    // to heapify a subtree with the root at given index 
    void MinHeapify(int i){
	    int l = left(i); 
	    int r = right(i); 
    	int smallest = i; 
	    if (l < heap_size && harr[l].total < harr[i].total) 
    	    smallest = l; 
	    if (r < heap_size && harr[r].total < harr[smallest].total) 
    	    smallest = r; 
	    if (smallest != i) 
    	{ 
        	swap(&harr[i], &harr[smallest]); 
	        MinHeapify(smallest); 
    	} 

	} 
  
    int parent(int i) { return (i-1)/2; } 
  
    // to get index of left child of node at index i 
    int left(int i) { return (2*i + 1); } 
  
    // to get index of right child of node at index i 
    int right(int i) { return (2*i + 2); } 
  
    // to extract the root which is the minimum element 
    proc extractMin(){
	    if (heap_size == 1) 
    	{ 
	        heap_size--; 
        	return harr[0]; 
    	} 
		proc root; 
		if (heap_size <= 0){
		    cout<<"HEAP IS EMPTY";
		    return root ; 
		}
    	// Store the minimum value, and remove it from heap 
    	root = harr[0]; 
    	harr[0] = harr[heap_size-1]; 
    	heap_size--; 
    	MinHeapify(0); 
  
	    return root; 
	} 
  
    // Decreases key value of key at index i to new_val 
    void decreaseKey(proc p, int new_val){
	    int i;
		for(int j = 0; j < heap_size; j++){
			if(harr[j].procnum == p.procnum ){
				i = j;
				break;
			}
		}
		harr[i].total = new_val; 
	    while (i != 0 && harr[parent(i)].total > harr[i].total) 
    	{ 
    	   swap(&harr[i], &harr[parent(i)]); 
        	i = parent(i); 
    	} 
	} 
  
    // Returns the minimum key (key at root) from min heap 
    proc getMin() { return harr[0]; } 
  
    // Deletes a key stored at index i 
    void deleteKey(proc p){
	    decreaseKey(p, INT_MIN); 
	    extractMin(); 

	}
  
    // Inserts a new key 'k' 
    void insertKey(proc k){

    	if (heap_size == capacity) 
    	{ 
        	cout << "\nOverflow: Could not insertKey\n"; 
	        return; 
    	} 
  
    	// First insert the new key at the end 
	    heap_size++; 
    	int i = heap_size - 1; 
    	harr[i] = k; 
  
	    // Fix the min heap property if it is violated 
    	while (i != 0 && harr[parent(i)].total > harr[i].total) 
    	{ 
       		swap(&harr[i], &harr[parent(i)]); 
        	i = parent(i); 
    	} 
	}

	void display_heap(){
		MinHeap temp(capacity);
		for(int i = 0; i < heap_size ; i++) temp.insertKey(harr[i]);
		while(temp.heap_size>0){
			cout<<temp.extractMin();
		}
	}

	bool isEmpty(){
		if(heap_size == 0) return true;
		else return false;
	}
}; 





