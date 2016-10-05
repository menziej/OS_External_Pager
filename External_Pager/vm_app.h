/*
 * vm_app.h
 *
 * Public routines for clients of the external pager
 */

#ifndef _VM_APP_H_
#define _VM_APP_H_

/*
 * vm_extend
 *
 * Ask for the lowest invalid virtual page in the process's arena to be
 * declared valid.  If share_id is zero, this virtual page is private, i.e.,
 * not shared with any other virtual page.  If share_id is non-zero, this
 * virtual page is shared with other virtual pages with the same share_id.
 * Returns the lowest-numbered byte of the new virtual page.
 * Returns nullptr if disk is out of swap space.
 */
extern void *vm_extend(unsigned int share_id);

/* 
 * vm_syslog
 *
 * Ask external pager to log a message (message data must be in address space
 * controlled by external pager).  Logs message of length len.
 * Returns 0 on success, -1 on failure.
 */
extern int vm_syslog(void *message, unsigned long len);

/* 
 * vm_yield
 *
 * Ask operating system to yield the CPU to another process.
 * The infrastructure's scheduler is non-preemptive, so a process runs until
 * it calls vm_yield() or exits.
 */
extern void vm_yield(void);

#define VM_PAGESIZE 4096

#endif /* _VM_APP_H_ */
