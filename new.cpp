/*
	Submitted by: Aleezeh Usman 
	Roll #: 18i-0529
*/

#include"Utility.h"
using namespace std;

int main(int argc, char* argv[]){
    int file;
    int fd = int(argv[1][0]);
    char temp[1024];            //everything read from files will be filtered through checks and stored in one single char array
    int j = 0;

//reading FCFS file
    file = open("FCFS.txt", O_RDONLY);
    char *c = (char *) calloc(200, sizeof(char));
    if(file < 0){
        cout<<"ERROR WHILE OPENING FILE\n";
        exit(-1);
    }
    read(file,c,200);                                     //since normal fstream not allowed read all text into c
    temp[j++] = 'F'; temp[j++] = 'C'; temp[j++] = 'F'; temp[j++] = 'S'; 
    for (int i = 0; i < strlen(c); i++)
    {
        if(!(c[i] > 'A' && c[i] < 'Z') && !(c[i-1] > 'A' && c[i-1] < 'Z'))
        if(!(c[i]>'a' && c[i]<'z') && c[i]!=' '){
            temp[j] = c[i];
            j++;
        }
    }

//reading SJF file
    char *c2 = (char *) calloc(200, sizeof(char));
    file = open("SJF.txt", O_RDONLY);
    if(file < 0){
        cout<<"ERROR WHILE OPENING FILE\n";
        exit(-1);
    }
    read(file,c2,200);                                     //since normal fstream not allowed read all text into c
    temp[j++] = 'S'; temp[j++] = 'J'; temp[j++] = 'F';
    for (int i = 0; i < strlen(c2); i++)
    {
        if(!(c2[i] > 'A' && c2[i] < 'Z') && !(c2[i-1] > 'A' && c2[i-1] < 'Z'))
        if(!(c2[i]>'a' && c2[i]<'z') && c2[i]!=' '){
            temp[j] = c2[i];
            j++;
        }
    }
    
//reading SRTF file
    char *c3 = (char *) calloc(200, sizeof(char));
    file = open("SRTF.txt", O_RDONLY);
    if(file < 0){
        cout<<"ERROR WHILE OPENING FILE\n";
        exit(-1);
    }
    read(file,c3,200);                                     //since normal fstream not allowed read all text into c
    temp[j++] = 'S'; temp[j++] = 'R'; temp[j++] = 'T'; temp[j++] = 'F';
    for (int i = 0; i < strlen(c3); i++)
    {
        if(!(c3[i] > 'A' && c3[i] < 'Z') && !(c3[i-1] > 'A' && c3[i-1] < 'Z'))
        if(!(c3[i]>'a' && c3[i]<'z') && c3[i]!=' ') {
            temp[j] = c3[i];
            j++;
        }
    }

//reading RR file
    char *c4 = (char *) calloc(200, sizeof(char));
    file = open("RR.txt", O_RDONLY);
    if(file < 0){
        cout<<"ERROR WHILE OPENING FILE\n";
        exit(-1);
    }
    read(file,c4,200);                                     //since normal fstream not allowed read all text into c
    temp[j++] = 'R'; temp[j++] = 'R';
    for (int i = 0; i < strlen(c4); i++)
    {
        if(!(c4[i] > 'A' && c4[i] < 'Z') && !(c4[i-1] > 'A' && c4[i-1] < 'Z'))
        if(!(c4[i]>'a' && c4[i]<'z') && c4[i]!=' '){
            temp[j] = c4[i];
            j++;
        }
    }
    
//organize all data and compute required info
    //variables to store important information that will be computed and then sent back to main process
    int FCFS_p, SJF_p, SRTF_p, RR_p, Qt;
    j = j/6;                                            //an approximate number of processes to decrease waste of space
    proc* p = new proc[j];
    general_info n;
    OrganizeData(p,j,temp,n.FCFS, n.SJF, n.SRTF, n.RR, n.Quantum_time);
    write(fd, &n, sizeof(n));

    for(int i = 0; i < n.FCFS+n.SJF+n.SRTF+n.RR; i++) write(fd,&p[i], sizeof(p[i]));
    
    delete [] c;
    delete [] c2;
    delete [] c3;
    delete [] c4;
//cout<<"\nNEW STATE HAS BEEN COMPLETED LETS MOVE ON\n";

}