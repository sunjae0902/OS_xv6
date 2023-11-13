#include "types.h"
#include "stat.h"
#include "user.h"


#define MAX 10


int childProcess(){
    int i;
    int parr[10]={6,7,8,5,5,5,5,5,5,5};
  
    for(i=0;i<MAX;i++){
        int pid = fork();
        if(pid == 0){
           
            return parr[i%10]; // 우선순위 부여
        }
    }
    return -1;
}
void exitChild(int p){
    if (p == -1){
        while (wait() != -1); // wait for all child processes to finish
    }
     else
       exit();
}


int main(void) {
    int res,i;

    printf(1,"===========<test>============\n");
    res = childProcess(); // 자식 프로세스 생성
    
    if (res != -1) {
       
        int pid = getpid();
        
        for (i = 0; i < 50000; i++) {
            set_proc_priority(pid, res);
    }
    printf(1,"\n[COMPLETED] PID: %d, Priority: %d\n",pid,get_proc_priority(pid));
         
    }
    // 부모 프로세스
  
   exitChild(res);
   
    exit();
}

