#include "types.h" 
#include "stat.h" 
#include "user.h" 


int main(int argc, char *argv[]) {
    int pid;
    
    set_proc_priority(getpid(),7);
    pid = fork(); // fork 실행
    
    
    if(pid < 0) return -1;
    
    else if(pid == 0){
        pid=getpid();
        set_proc_priority(pid,10); // 자식 프로세스의 우선순위를 10으로 변경
        printf(1, "Changed Child's Priority -> pid: %d, priority: %d\n",pid,get_proc_priority(pid));
        exit();
    }
    else{
        printf(1, "Created Child -> pid: %d, priority: %d\n",pid,get_proc_priority(pid));
       // set_proc_priority(pid,10); // 자식 프로세스의 우선순위를 10으로 변경
        wait();
    }
    exit();
    
}
