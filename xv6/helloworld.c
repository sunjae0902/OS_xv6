#include "types.h" 
#include "stat.h" 
#include "user.h" 


int main(int argc, char *argv[]) {
    int pid;
    
    pid = fork(); // fork 실행
    
    
    if(pid < 0) return -1;
    
    else if(pid == 0){
        pid=getpid();
        printf(1,"\n복제된 자식 프로세스\n");
        printf(1, "우선순위 변경 전-> pid: %d, priority: %d\n",pid,get_proc_priority(pid));
        set_proc_priority(pid,10); // 자식 프로세스의 우선순위를 10으로 변경
        printf(1, "우선순위 변경 후-> pid: %d, priority: %d\n",pid,get_proc_priority(pid));
        exit();
    }
    else{
        pid = getpid();
        printf(1, "부모 프로세스 pid: %d, priority: %d\n",pid,get_proc_priority(pid));
        wait();
    }
    exit();
    
}
