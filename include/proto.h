
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            proto.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* klib.asm */
PUBLIC void	out_byte(u16 port, u8 value);
PUBLIC u8	in_byte(u16 port);
PUBLIC void	disp_str(char * info);
PUBLIC void	disp_color_str(char * info, int color);

/* protect.c */
PUBLIC void	init_prot();
PUBLIC u32	seg2phys(u16 seg);

/* klib.c */
PUBLIC void	delay(int time);

/* kernel.asm */
void restart();

/* main.c */
void ReaderA();
void ReaderB();
void ReaderC();
void WriterD();
void WriterE();
void NormalF();

/* i8259.c */
PUBLIC void put_irq_handler(int irq, irq_handler handler);
PUBLIC void spurious_irq(int irq);

/* clock.c */
PUBLIC void clock_handler(int irq);
PUBLIC void milli_delay(int milli_sec);

/* SEMAPHORE */
typedef struct s_sem{
	int	value;
	int	queue[QUEUE_SIZE];//进程队列
	int     in;//队尾指针
	int 	out;//队首指针
}SEMAPHORE;

/* 以下是系统调用相关 */

/* proc.c */
PUBLIC  int     sys_get_ticks();        /* sys_call */
PUBLIC  void    sys_dispstr(char*);
PUBLIC  void    sys_color_dispstr(char*,int);
PUBLIC  void    sys_sleep(int);
PUBLIC  void    sys_P(SEMAPHORE*,int);
PUBLIC  void    sys_V(SEMAPHORE*);

/* syscall.asm */
PUBLIC  void    sys_call();             /* int_handler */
PUBLIC  int     get_ticks();
PUBLIC  void    dispstr(char*);
PUBLIC  void    color_dispstr(char*,int);
PUBLIC  void    sleep(int);
PUBLIC  void    P(SEMAPHORE*,int);
PUBLIC  void    V(SEMAPHORE*);
