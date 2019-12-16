
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                               syscall.asm
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                                                     Forrest Yu, 2005
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

%include "sconst.inc"

_NR_get_ticks       equ 0 ; 要跟 global.c 中 sys_call_table 的定义相对应！
_NR_dispstr			equ 1
_NR_color_dispstr	equ 2
_NR_delay			equ 3
_NR_P 				equ 4
_NR_V				equ 5
INT_VECTOR_SYS_CALL equ 0x90

; 导出符号
global	get_ticks
global  dispstr
global  color_dispstr
global  P 
global  V 
global  my_delay


bits 32
[section .text]

; ====================================================================
;                              get_ticks
; ====================================================================
get_ticks:
	mov	eax, _NR_get_ticks
	int	INT_VECTOR_SYS_CALL
	ret
dispstr:
	mov	eax, _NR_dispstr
	mov	ebx, [esp+4]
	int INT_VECTOR_SYS_CALL
	ret
color_dispstr:
	mov eax, _NR_color_dispstr
	mov	ebx, [esp+4]
	int INT_VECTOR_SYS_CALL
	ret
my_delay:
	mov	eax, _NR_delay
	mov ebx, [esp+4]
	int INT_VECTOR_SYS_CALL
	ret
P:
	mov eax, _NR_P
	mov ecx,[esp+8]
	mov	ebx,[esp+4]
	int	INT_VECTOR_SYS_CALL
	ret
V:
	mov eax, _NR_V
	mov ebx,[esp+4]
	int	INT_VECTOR_SYS_CALL
	ret
