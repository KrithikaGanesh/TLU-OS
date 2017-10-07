
// #; -*- fundamental -*-


#define save_registers \
	pushq %rbx;    \
	pushq %rcx;    \
	pushq %rdx;    \
	pushq %rdi;    \
	pushq %rsi;    \
	pushq %r8;     \
	pushq %r9;     \
	pushq %r10;    \
	pushq %r11;    \
	pushq %r12;    \
	pushq %r13;    \
	pushq %r14;    \
	pushq %r15;    \
	pushq %rbp;




#define restore_registers \
	popq %rbp;	  \
	popq %r15;	  \
	popq %r14;	  \
	popq %r13;	  \
	popq %r12;	  \
	popq %r11;	  \
	popq %r10;	  \
	popq %r9;	  \
	popq %r8;	  \
	popq %rsi;	  \
	popq %rdi;	  \
	popq %rdx;	  \
	popq %rcx;	  \
	popq %rbx;	  \


//
// from ( http://6.035.scripts.mit.edu/sp17/x86-64-architecture-guide.html )
// %rdi - 1st argument
// %rsi - 2nd argument
// %rdx - 3rd argument
// %rcx - 4th argument
// %r8  - 5th argument
// %r9 -  6th argument



.align 8
.globl __switch_to_stack
__switch_to_stack:       // %rdi %rsi %rdx %rcx

	save_registers;
	movq %rsp,   (%rdi); // Save Current Stack Pointer to 1st Org Dereferenced
	movq (%rsi), %rsp;   // Retrieve Next Stack Pointer from 2nd Org Dereferenced

	// movq %rdx,   %rdi;   // Current Thread Id as the first arg to cleanup
	// movq %rcx,   %rsi;   // Next Thread Id as the 2nd arg to cleanup
	// call _pet_thread_cleanup;

	restore_registers;
	ret;

/*
.align 8
.globl _switch_to_stack_first
_switch_to_stack_first:       // %rdi %rsi %rdx %rcx

	save_registers;
	movq %rsp,   (%rdi); // Save Current Stack Pointer to 1st Org Dereferenced
	movq (%rsi), %rsp;   // Retrieve Next Stack Pointer from 2nd Org Dereferenced
	ret;
*/

.align 8
.globl __abort_to_stack
__abort_to_stack:
	movq (%rdi), %rsp;
	restore_registers;
	ret;


.align 8
.globl __push_context
__push_context:       // %rdi %rsi %rdx %rcx
	movq %rsp,  %r9 ; // save calling stack to %r9
    movq (%rsi), %rsp;  // switch tcb stack

	save_registers;    // do dummy push
	movq %rsp, (%rsi)   // update tcb stack
	movq %r9, %rsp     // retrieve calling stack from %r9
	ret;



