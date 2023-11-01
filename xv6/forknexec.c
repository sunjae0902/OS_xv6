#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"
#include "elf.h"

struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;

int forknexec(const char *path, const char **args) {
    int i, argc, off;
    uint sz, sp, ustack[3+MAXARG+1];
    struct proc *np;
    char *s,*last;
    struct proc *curproc = myproc();
    struct proghdr ph;
    struct inode *ip;
    struct elfhdr elf;
    pde_t *pgdir;

    if ((np = allocproc()) == 0) {
        cprintf("forknexec: fail_to_allocproc\n");
        return -2;
    }
   

    if ((np->pgdir = copyuvm(curproc->pgdir, curproc->sz)) == 0) {
        kfree(np->kstack);
        np->kstack = 0;
        np->state = UNUSED;
        return -2;
    }

    np->sz = curproc->sz;
    np->parent = curproc;
    *np->tf = *curproc->tf;
    np->tf->eax = 0;

    for (i = 0; i < NOFILE; i++) { //파일 디스크립터 복사
        if (curproc->ofile[i])
            np->ofile[i] = filedup(curproc->ofile[i]);
    }
    np->cwd = idup(curproc->cwd); //작업 디렉토리 복사, 프로세스 이름 복사
    safestrcpy(np->name, curproc->name, sizeof(curproc->name));
    
    pgdir = np->pgdir;

    begin_op();
    if ((ip = namei((char *)path)) == 0) { //파일 이름으로 해당하는 실행할 프로그램의 inode 구조체를 가져옴 ,ip: inode pointer
        end_op();
        cprintf("forknexec: fail_fileNotFound\n");
        return -2;
    }
   
    ilock(ip);
    
    if (readi(ip, (char*)&elf, 0, sizeof(elf)) != sizeof(elf)) // 실행할 프로그램 파일의 inode elf 헤더를 가져옴-
        goto bad;
    if (elf.magic != ELF_MAGIC)
        goto bad;
    if ((pgdir = setupkvm()) == 0) // 새 페이지 디렉터리 설정
        goto bad;
    
    sz = 0;
    for(i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)){
        if(readi(ip, (char*)&ph, off, sizeof(ph)) != sizeof(ph))
          goto bad;
        if(ph.type != ELF_PROG_LOAD) // elf 프로그램 세그먼트의 타입이 이 경우인 것만 로드
          continue;
        if(ph.memsz < ph.filesz)
          goto bad;
        if(ph.vaddr + ph.memsz < ph.vaddr)
          goto bad;
        if((sz = allocuvm(pgdir, sz, ph.vaddr + ph.memsz)) == 0)
          goto bad;
        if(ph.vaddr % PGSIZE != 0)
          goto bad;
        if(loaduvm(pgdir, (char*)ph.vaddr, ip, ph.off, ph.filesz) < 0)
          goto bad;
      }
    iunlockput(ip);
    end_op();
    ip = 0;
    
    sz = PGROUNDUP(sz); //주어진 페이지 크기의 배수로 올림(PGSIZE=4096)
    
    if ((sz = allocuvm(pgdir, sz, sz + 2 * PGSIZE)) == 0) //사용자 스택 할당
        goto bad;
    clearpteu(pgdir, (char*)(sz - 2 * PGSIZE));
    sp = sz;

    for (argc = 0; args[argc]; argc++) { //명령행인자복사
        if (argc >= MAXARG)
            goto bad;
        sp = (sp - (strlen(args[argc]) + 1)) & ~3;
        if (copyout(pgdir, sp, (char*)args[argc], strlen(args[argc]) + 1) < 0)
            goto bad;
        ustack[3 + argc] = sp;
    }
    ustack[3+argc] = 0;
    
    ustack[0] = 0xffffffff;
    ustack[1] = argc;
    ustack[2] = sp - (argc+1)*4;

    sp -= (3 + argc + 1) * 4;
    if (copyout(pgdir, sp, ustack, (3 + argc + 1) * 4) < 0)
        goto bad;
    
    for(last=s=(char *)path; *s; s++) // 프로그램 이름 업데이트
       if(*s == '/')
         last = s+1;
     safestrcpy(np->name, last, sizeof(np->name));

 
    np->pgdir = pgdir;
    np->tf->esp = sp;
    np->sz = sz;
    np->tf->eip = elf.entry;

    switchuvm(np);
    np->state = RUNNABLE;
 
    
    
    if(np->pid<0) return -2; // fork 실패
    else if (np->pid>0) {
        int res=wait();
        return res;
    }
    else exit();
    

bad:
    if (pgdir)
        freevm(pgdir);
    if (ip) {
        iunlockput(ip);
        end_op();
    }
    return -2;
}

