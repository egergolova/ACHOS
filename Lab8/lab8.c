#include <sys/stat.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
//#include <fcnt1.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include<unistd.h>
#include <signal.h>

volatile sig_atomic_t amReceived1=0; //may be int
volatile sig_atomic_t amReceived2=0;
volatile sig_atomic_t amSent1=0;
volatile sig_atomic_t amSent2=0;
volatile sig_atomic_t CurrProc=1;
pid_t pid;
void handleSIGUSR(int SIG){
if(SIG==SIGUSR1){
__sync_fetch_and_add(&amReceived1,1);
}
if(SIG==SIGUSR2){
__sync_fetch_and_add(&amReceived2,1);
}
}

void handleSIGTERM(){
sleep(0.5);
printf("\nDONE!Number of proc=%d, pid=%d, ppid=%d, 
       USR1=%d,USR2=%d,received USR1=%d,received  
       USR2=%d\n",CurrProc,getpid(),
       getppid(),amSent1, amSent2,amReceived1,amReceived2);
if(CurrProc!=1){
kill(getpid(), SIGKILL);
}
else{
pid_t childpid=wait(NULL);
while(childpid!=-1){
childpid=wait(NULL);
}
exit(0);
}

}
void createProc(int numCurr, int numNext){
if(CurrProc==numCurr){
sleep(2);
pid=fork();
if(!pid){
CurrProc=numNext;
}
}
}

void createAllProc(){
createProc(1,2);
createProc(2,3);
createProc(2,4);
createProc(4,5);
createProc(3,6);
createProc(6,7);
createProc(7,8);
}

void sender(){
if(CurrProc==2){
amSent2=0;
while( amSent2<101){
kill(0, SIGUSR2);
sleep(0.5);
printf("\nNumber of proc=%d, pid=%d, ppid=%d, USR1=%d, 
         USR2=%d,received USR1=%d,received USR2=%d\n",
         CurrProc,getpid(),getppid(),amSent1, amSent2++,
         amReceived1,amReceived2);
}
}
else{
amSent1=0;
while( amSent1<101){
kill(0, SIGUSR1);
sleep(0.5);
printf("\nNumber of proc=%d, pid=%d, ppid=%d, USR1=%d, 
         USR2=%d,received USR1=%d,received USR2=%d\n",
         CurrProc,getpid(),getppid(),amSent1++, amSent2,
         amReceived1,amReceived2);

}
}
kill(0, SIGTERM);
}

int main(){
signal(SIGUSR1, handleSIGUSR);
signal(SIGUSR2, handleSIGUSR);
signal(SIGTERM, handleSIGTERM);
createAllProc();
amReceived1=0;
sender();

}
