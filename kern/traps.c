#include <env.h>
#include <pmap.h>
#include <printk.h>
#include <trap.h>

extern void handle_int(void);
extern void handle_tlb(void);
extern void handle_sys(void);
extern void handle_mod(void);
extern void handle_reserved(void);
extern void handle_ri(void);

void (*exception_handlers[32])(void) = {
    [0 ... 31] = handle_reserved,
    [0] = handle_int,
    [2 ... 3] = handle_tlb,
    [10]= handle_ri,
#if !defined(LAB) || LAB >= 4
    [1] = handle_mod,
    [8] = handle_sys,
#endif
};

/* Overview:
 *   The fallback handler when an unknown exception code is encountered.
 *   'genex.S' wraps this function in 'handle_reserved'.
 */
void do_reserved(struct Trapframe *tf) {
	print_tf(tf);
	panic("Unknown ExcCode %2d", (tf->cp0_cause >> 2) & 0x1f);
}
void do_ri(struct Trapframe *tf) {
	    u_long va=tf->cp0_epc;
    Pte* pte;
    page_lookup(curenv->env_pgdir,va,&pte);
    u_long pa=PTE_ADDR(*pte)|(va&0xfff);
    u_long kva=KADDR(pa);
    int* instr=(int*)kva;

    u_int rs=(*instr>>21)&0b11111;
    u_int rt=(*instr>>16)&0b11111;
    u_int rd=(*instr>>11)&0b11111;

    if((*instr>>26)==0&&(*instr&0b11111111111)==0b00000111111)
    {
        tf->regs[rd] = 0;
        for (int i = 0; i < 32; i += 8) {
            u_int rs_i = tf->regs[rs] & (0xff << i);
            u_int rt_i = tf->regs[rt] & (0xff << i);
            if (rs_i < rt_i) {
                tf->regs[rd] = tf->regs[rd] | rt_i;
            } else {
                tf->regs[rd] = tf->regs[rd] | rs_i;
            }
        }
    }
    else if((*instr>>26)==0&&(*instr&0b11111111111)==0b00000111110)
    {
        u_int tmp = *((u_int*)tf->regs[rs]);
        if (*((u_int*)tf->regs[rs]) == tf->regs[rt]) {
            *((u_int*)tf->regs[rs]) = tf->regs[rd];
        }
        tf->regs[rd] = tmp;
    }
	tf->cp0_epc+=4;
}
