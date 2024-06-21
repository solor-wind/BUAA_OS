#ifndef SYSCALL_H
#define SYSCALL_H

#ifndef __ASSEMBLER__

enum {
	SYS_putchar,
	SYS_print_cons,
	SYS_getenvid,
	SYS_yield,
	SYS_env_destroy,
	SYS_set_tlb_mod_entry,
	SYS_mem_alloc,
	SYS_mem_map,
	SYS_mem_unmap,
	SYS_exofork,
	SYS_set_env_status,
	SYS_set_trapframe,
	SYS_panic,
	SYS_ipc_try_send,
	SYS_ipc_recv,
	SYS_cgetc,
	SYS_write_dev,
	SYS_read_dev,
	SYS_add_job,
	SYS_set_job_status,
	SYS_get_job,
	SYS_get_job_envid,
	SYS_get_job_status,
	SYS_get_jobs,
	SYS_print_jobs,
	SYS_mykill,
	SYS_env_set_return_value,
	MAX_SYSNO,
};

#endif

#endif
