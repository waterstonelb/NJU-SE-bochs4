
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            main.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "string.h"
#include "proc.h"
#include "global.h"


PRIVATE void clear();
// PRIVATE void start(int index, int is_read);
// PRIVATE void end(int index, int is_read);
// PRIVATE void print_current();

/*======================================================================*
                            kernel_main
 *======================================================================*/
PUBLIC int kernel_main()
{
	disp_str("-----\"kernel_main\" begins-----\n");

	TASK*		p_task		= task_table;
	PROCESS*	p_proc		= proc_table;
	char*		p_task_stack	= task_stack + STACK_SIZE_TOTAL;
	u16		selector_ldt	= SELECTOR_LDT_FIRST;
	int i;
	for (i = 0; i < NR_TASKS; i++) {
		strcpy(p_proc->p_name, p_task->name);	// name of the process
		p_proc->pid = i;			// pid

		p_proc->ldt_sel = selector_ldt;

		memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3],
		       sizeof(DESCRIPTOR));
		p_proc->ldts[0].attr1 = DA_C | PRIVILEGE_TASK << 5;
		memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3],
		       sizeof(DESCRIPTOR));
		p_proc->ldts[1].attr1 = DA_DRW | PRIVILEGE_TASK << 5;
		p_proc->regs.cs	= ((8 * 0) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.ds	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.es	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.fs	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.ss	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.gs	= (SELECTOR_KERNEL_GS & SA_RPL_MASK)
			| RPL_TASK;

		p_proc->regs.eip = (u32)p_task->initial_eip;
		p_proc->regs.esp = (u32)p_task_stack;
		p_proc->regs.eflags = 0x1202; /* IF=1, IOPL=1 */
		p_proc->ticks=p_proc->block=0;
		//proc_table[i].ticks = proc_table[i].block = 15;
		p_task_stack -= p_task->stacksize;
		p_proc++;
		p_task++;
		selector_ldt += 1 << 3;
	}

	//proc_table[0].ticks = proc_table[0].block = 15;
	// proc_table[1].ticks = proc_table[1].block =  5;
	// proc_table[2].ticks = proc_table[2].priority =  3;
	//实验初始化开始
	proc_table[0].ticks = proc_table[0].block = 0;
	proc_table[1].ticks = proc_table[1].block = 0;
	proc_table[2].ticks = proc_table[2].block = 0;
	proc_table[3].ticks = proc_table[3].block = 0;
	proc_table[4].ticks = proc_table[4].block = 0;
	proc_table[5].ticks = proc_table[5].block = 0;
	clear();
	disp_pos = 0;
	for(i=0;i<80*25;i++){
		disp_str(" ");	
	}
	disp_pos = 0;
	readers.in=0;
	readers.out=0;
	readers.value=1;
	writer.in=0;
	writer.out=0;
	writer.value=1;
	mutex.in=0;
	mutex.out=0;
	mutex.value=1;
	//实验初始化结束
	k_reenter = 0;
	ticks = 0;

	p_proc_ready	= proc_table;

        /* 初始化 8253 PIT */
        out_byte(TIMER_MODE, RATE_GENERATOR);
        out_byte(TIMER0, (u8) (TIMER_FREQ/HZ) );
        out_byte(TIMER0, (u8) ((TIMER_FREQ/HZ) >> 8));

        put_irq_handler(CLOCK_IRQ, clock_handler); /* 设定时钟中断处理程序 */
        enable_irq(CLOCK_IRQ);                     /* 让8259A可以接收时钟中断 */

	restart();

	while(1){}
}

/*======================================================================*
                               TestA
 *======================================================================*/
void ReaderA()
{
	while (1) {
		P(&readers,0);
		P(&writer,0);
		char a[] = "Reader A START\n";
		dispstr(a);
		//V(&readers);
		milli_delay(20*HZ/1000);
		char b[] = "Reader A END\n";
		color_dispstr(b,BLUE);
		char c={'\0','\0'};

		V(&writer);
		V(&readers);
	}
}

/*======================================================================*
                               TestB
 *======================================================================*/
void ReaderB()
{
	while (1) {
		P(&readers,1);
		char a[] = "Reader B START\n";
		dispstr(a);
		milli_delay(30*HZ/1000);

		V(&readers);
	}
}

/*======================================================================*
                               TestB
 *======================================================================*/
void ReaderC()
{
	while (1) {
		P(&readers,2);
		char a[] = "Reader C START\n";
		color_dispstr(a,RED);
		milli_delay(30*HZ/1000);
		V(&readers);

	}
}
void WriterD(){
	while (1) {
		P(&writer,3);
		char a[] = "Writer D START\n";
		color_dispstr(a,GREEN);
		milli_delay(30*HZ/1000);
		V(&writer);
	}
}
void WriterE(){
	while (1) {
		P(&writer,4);
		char a[] = "Writer E START\n";
		color_dispstr(a,GREEN);
		milli_delay(40*HZ/1000);
		V(&writer);
		
	}
}
void NormalF(){
	while (1) {
		char a[]="ffffffff\n";
		dispstr(a);
		milli_delay(300);
	}
}
PRIVATE void clear()
{
	disp_pos = 0;
        
        for(int i = 0; i < 80 * 25; i++)
                disp_str(" ");
        
        disp_pos = 0;
}