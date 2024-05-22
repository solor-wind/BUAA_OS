#include <lib.h>

void strace_barrier(u_int env_id) {
	int straced_bak = straced;
	straced = 0;
	while (envs[ENVX(env_id)].env_status == ENV_RUNNABLE) {
		syscall_yield();
	}
	straced = straced_bak;
}

void strace_send(int sysno) {
	if (!((SYS_putchar <= sysno && sysno <= SYS_set_tlb_mod_entry) ||
	      (SYS_exofork <= sysno && sysno <= SYS_panic)) ||
	    sysno == SYS_set_trapframe) {
		return;
	}

	// Your code here. (1/2)
	if(straced==0)
		return;
	int tmp=straced;
	straced=0;
	ipc_send(env->env_parent_id,sysno,0,0);
	syscall_set_env_status(env->env_id,ENV_NOT_RUNNABLE);
	straced=tmp;
}

void strace_recv() {
	// Your code here. (2/2)
	int sysno=0;
	while(sysno!=SYS_env_destroy)
	{
		u_int child=0;
		sysno=ipc_recv(&child,0,0);
		//sysno=e->env_ipc_value;
		strace_barrier(child);//?
		recv_sysno(child,sysno);
		syscall_set_env_status(child,ENV_RUNNABLE);
	}
}
