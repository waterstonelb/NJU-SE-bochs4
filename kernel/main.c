
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
PRIVATE void start(int);
PRIVATE void read(char);
PRIVATE void write(char);
PRIVATE void end(int);
// PRIVATE void print_current();

/*======================================================================*
                            kernel_main
 *======================================================================*/
PUBLIC int kernel_main()
{
	disp_str("-----\"kernel_main\" begins-----\n");

	TASK *p_task = task_table;
	PROCESS *p_proc = proc_table;
	char *p_task_stack = task_stack + STACK_SIZE_TOTAL;
	u16 selector_ldt = SELECTOR_LDT_FIRST;
	int i;
	for (i = 0; i < NR_TASKS; i++)
	{
		strcpy(p_proc->p_name, p_task->name); // name of the process
		p_proc->pid = i;					  // pid

		p_proc->ldt_sel = selector_ldt;

		memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3],
			   sizeof(DESCRIPTOR));
		p_proc->ldts[0].attr1 = DA_C | PRIVILEGE_TASK << 5;
		memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3],
			   sizeof(DESCRIPTOR));
		p_proc->ldts[1].attr1 = DA_DRW | PRIVILEGE_TASK << 5;
		p_proc->regs.cs = ((8 * 0) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
		p_proc->regs.ds = ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
		p_proc->regs.es = ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
		p_proc->regs.fs = ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
		p_proc->regs.ss = ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
		p_proc->regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK) | RPL_TASK;

		p_proc->regs.eip = (u32)p_task->initial_eip;
		p_proc->regs.esp = (u32)p_task_stack;
		p_proc->regs.eflags = 0x1202; /* IF=1, IOPL=1 */
		p_proc->ticks = p_proc->block = 0;
		//proc_table[i].ticks = proc_table[i].block = 15;
		p_task_stack -= p_task->stacksize;
		p_proc++;
		p_task++;
		selector_ldt += 1 << 3;
	}

	//实验初始化开始
	clear();
	// proc_table[0].ticks = proc_table[0].block = 0;
	// proc_table[1].ticks = proc_table[1].block = 0;
	// proc_table[2].ticks = proc_table[2].block = 0;
	// proc_table[3].ticks = proc_table[3].block = 0;
	// proc_table[4].ticks = proc_table[4].block = 0;
	// proc_table[5].ticks = proc_table[5].block = 0;

	num_reader = 0;
	num_writer = 0;
	pid = -1;
	readers.in = 0;
	readers.out = 0;
	readers.value = 1;
	writer.in = 0;
	writer.out = 0;
	writer.value = 1;
	mutex.in = 0;
	mutex.out = 0;
	mutex.value = 1;
	wmutex.in = 0;
	wmutex.out = 0;
	wmutex.value = 1;
	read_max.in = 0;
	read_max.out = 0;
	read_max.value = MAX_READER3;
	Hungry.in = 0;
	Hungry.out = 0;
	Hungry.value = 1;
	//实验初始化结束
	k_reenter = 0;
	ticks = 0;

	p_proc_ready = proc_table;

	/* 初始化 8253 PIT */
	out_byte(TIMER_MODE, RATE_GENERATOR);
	out_byte(TIMER0, (u8)(TIMER_FREQ / HZ));
	out_byte(TIMER0, (u8)((TIMER_FREQ / HZ) >> 8));

	put_irq_handler(CLOCK_IRQ, clock_handler); /* 设定时钟中断处理程序 */
	enable_irq(CLOCK_IRQ);					   /* 让8259A可以接收时钟中断 */

	restart();

	while (1)
	{
	}
}

/*======================================================================*
                               TestA
 *======================================================================*/
void ReaderA()
{
	int i = 0;
	while (1)
	{
		P(&read_max, i);
		if(HUNGRY==1)
			P(&Hungry, i);
		start(i);
		
		if (WRITER_FIRST==1)
		{
			P(&readers, i);
		}
		P(&mutex, i);
		if(WRITER_FIRST==0)
		{
			
			if (num_reader == 0)
			{
				P(&writer, i);
			}
		}
		num_reader++;
		V(&mutex);
		if (WRITER_FIRST)
			V(&readers);
		
		if(HUNGRY==1)
		    V(&Hungry);
		
		pid = i;
		read('A' + i);

		end(i);
		P(&mutex, i);
		num_reader--;
		if(WRITER_FIRST==0)
		{
			
			if (num_reader == 0)
			{
				V(&writer);
			}
		}
		V(&mutex);
		V(&read_max);
	}
}

/*======================================================================*
                               TestB
 *======================================================================*/
void ReaderB()
{
	int i = 1;
	while (1)
	{
		
		P(&read_max, i);
		if(HUNGRY==1)
			P(&Hungry, i);
		start(i);
		
		if (WRITER_FIRST==1)
		{
			P(&readers, i);
		}
		P(&mutex, i);
		if(WRITER_FIRST==0)
		{
			
			if (num_reader == 0)
			{
				P(&writer, i);
			}
		}
		num_reader++;
		V(&mutex);
		if (WRITER_FIRST)
			V(&readers);
		
		if(HUNGRY==1)
		    V(&Hungry);
		
		pid = i;
		read('A' + i);

		end(i);
		P(&mutex, i);
		num_reader--;
		if(WRITER_FIRST==0)
		{
			
			if (num_reader == 0)
			{
				V(&writer);
			}
		}
		V(&mutex);
		V(&read_max);
	}
}

/*======================================================================*
                               TestB
 *======================================================================*/
void ReaderC()
{
	int i = 2;
	while (1)
	{
		P(&read_max, i);
		if(HUNGRY==1)
			P(&Hungry, i);
		start(i);
		
		if (WRITER_FIRST==1)
		{
			P(&readers, i);
		}
		P(&mutex, i);
		if(WRITER_FIRST==0)
		{
			
			if (num_reader == 0)
			{
				P(&writer, i);
			}
		}
		num_reader++;
		V(&mutex);
		if (WRITER_FIRST)
			V(&readers);
		
		if(HUNGRY==1)
		    V(&Hungry);
		
		pid = i;
		read('A' + i);

		end(i);
		P(&mutex, i);
		num_reader--;
		if(WRITER_FIRST==0)
		{
			if (num_reader == 0)
			{
				V(&writer);
			}
		}
		V(&mutex);
		V(&read_max);
	}
}
void WriterD()
{
	int i = 3;
	while (1)
	{
		start(i);
		if(HUNGRY==1)
		    P(&Hungry, i);
		
		if (WRITER_FIRST==1)
		{
			P(&wmutex,i);
			num_writer++;
			if (num_writer == 1)
			{
				P(&readers,i);
			}
			V(&wmutex);
		}
		
		P(&writer, i);
		
		pid = i;
		write('A' + i);
		end(i);
		
		V(&writer);
		
		if (WRITER_FIRST)
		{
			P(&wmutex,i);
			num_writer--;
			if(num_writer==0)
				V(&readers);
			V(&wmutex);
		}
		
		if(HUNGRY==1)
		    V(&Hungry);
	}
}
void WriterE()
{
	int i = 4;
	while (1)
	{
		start(i);
		if(HUNGRY==1)
		    P(&Hungry, i);
		
		if (WRITER_FIRST==1)
		{
			P(&wmutex,i);
			num_writer++;
			if (num_writer == 1)
			{
				P(&readers,i);
			}
			V(&wmutex);
		}
		
		P(&writer, i);
		
		pid = i;
		write('A' + i);
		end(i);
		
		V(&writer);
		
		if (WRITER_FIRST)
		{
			P(&wmutex,i);
			num_writer--;
			if(num_writer==0)
				V(&readers);
			V(&wmutex);
		}
		
		if(HUNGRY==1)
		    V(&Hungry);
	}
}
void NormalF()
{
	while (1)
	{
		char writer[] = "WRITING\n";
		char reader[40] = "READING the number of reader is ";
		if (pid < 3)
		{
			char c = num_reader + 48;
			char cc = 20;
			strcpy(reader + 32, &c);
			strcpy(reader + 33, &cc);
			dispstr(reader);
		}
		else
		{
			dispstr(writer);
		}
		sleep(1 * 1000 / HZ);
	}
}
PRIVATE void clear()
{
	disp_pos = 0;

	for (int i = 0; i < 80 * 25; i++)
		disp_str(" ");

	disp_pos = 0;
}
PRIVATE void start(int i)
{
	char p = 'A' + i;
	char t[20] = {0};
	t[0] = p;
	strcpy(t + 1, " is ready\n");
	color_dispstr(t, GREEN);
}
PRIVATE void end(int i)
{
	char p = 'A' + i;
	char t[20] = {0};
	t[0] = p;
	strcpy(t + 1, " is end\n");
	color_dispstr(t, RED);
}
PRIVATE void read(char c)
{
	char t[20] = {0};
	t[0] = c;
	strcpy(t + 1, " is reading\n");
	color_dispstr(t, BLUE);
	if (c == 'A')
	{
		
		milli_delay(2 * 1000 / HZ);
	}
	else
	{
		milli_delay(3 * 1000 / HZ);
	}
}
PRIVATE void write(char c)
{
	char t[20] = {0};
	t[0] = c;
	strcpy(t + 1, " is writing\n");
	color_dispstr(t, BLUE);
	if (c == 'D')
	{
		milli_delay(3 * 1000 / HZ);
	}
	else
	{
		milli_delay(4 * 1000 / HZ);
	}
}