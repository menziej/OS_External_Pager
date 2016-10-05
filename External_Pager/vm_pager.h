/*
 * vm_pager.h
 *
 * Header file for the external pager
 */

#ifndef _VM_PAGER_H_
#define _VM_PAGER_H_

#include <sys/types.h>

/*
 * ****************************************************
 * * Interface for student portion of Project 2 pager *
 * ****************************************************
 */

/*
 * vm_init
 *
 * Called when the pager starts.  It should set up any internal data structures
 * needed by the pager, e.g., physical page bookkeeping, process table, disk
 * usage table, etc.
 *
 * vm_init is passed both the number of physical memory pages and the number
 * of disk blocks in the raw disk.
 */
extern void vm_init(unsigned int memory_pages, unsigned int disk_blocks);

/*
 * vm_create
 * Called when a new process, with process identifier "pid", is added to the
 * system.  It should create whatever new elements are required for each of
 * your data structures.  The new process will only run when it's switched
 * to via vm_switch().
 */
extern void vm_create(pid_t pid);

/*
 * vm_switch
 *
 * Called when the kernel is switching to a new process, with process
 * identifier "pid".  This allows the pager to do any bookkeeping needed to
 * register the new process.
 */
extern void vm_switch(pid_t pid);

/*
 * vm_fault
 *
 * Called when current process has a fault at virtual address addr.  write_flag
 * is true if the access that caused the fault is a write.
 * Returns 0 on success, -1 on failure.
 */
extern int vm_fault(void *addr, bool write_flag);

/*
 * vm_destroy
 *
 * Called when current process exits.  It should deallocate all resources
 * held by the current process (page table, physical pages, disk blocks, etc.)
 */
extern void vm_destroy();

/*
 * vm_extend
 *
 * A request by the current process for the lowest invalid virtual page in the
 * process's arena to be declared valid.  If share_id is zero, this virtual
 * page is private, i.e., not shared with any other virtual page.  If share_id
 * is non-zero, this virtual page is shared with other virtual pages with the
 * same share_id.
 * Returns the lowest-numbered byte of the new virtual page.
 * Returns nullptr if disk is out of swap space.
 */
extern void *vm_extend(unsigned int share_id);

/*
 * vm_syslog
 *
 * A request by the current process to log a message.  The message is stored
 * in the process' arena at address "message" and is of length "len".
 *
 * Returns 0 on success, -1 on failure.
 */
extern int vm_syslog(void *message, size_t len);

/*
 * *********************************************
 * * Public interface for the disk abstraction *
 * *********************************************
 *
 * Disk blocks are numbered from 0 to (disk_blocks-1), where disk_blocks
 * is the parameter passed in vm_init().
 */

/*
 * disk_read
 *
 * read block "block" from the disk into buf.
 */
extern void disk_read(unsigned int block, void *buf);

/*
 * disk_write
 *
 * write the contents of buf to disk block "block".
 */
extern void disk_write(unsigned int block, void *buf);

/*
 * ********************************************************
 * * Public interface for the physical memory abstraction *
 * ********************************************************
 *
 * Physical memory pages are numbered from 0 to (memory_pages-1), where
 * memory_pages is the parameter passed in vm_init().
 *
 * Your pager accesses the data in physical memory through the variable
 * vm_physmem, e.g., ((char *)vm_physmem)[5] is byte 5 in physical memory.
 */
extern void * const vm_physmem;

/*
 * ***********************
 * * Definition of arena *
 * ***********************
 */

/* pagesize for the machine */
#define VM_PAGESIZE 4096

/* virtual address at which application's arena starts */
#define VM_ARENA_BASEADDR    ((void *) 0x60000000)

/* virtual page number at which application's arena starts */
#define VM_ARENA_BASEPAGE    ((uintptr_t) VM_ARENA_BASEADDR / VM_PAGESIZE)

/* size (in bytes) of arena */
#define VM_ARENA_SIZE    0x20000000

/*
 * **************************************
 * * Definition of page table structure *
 * **************************************
 */

/*
 * Format of page table entry.
 *
 * read_enable=0 ==> loads to this virtual page will fault
 * write_enable=0 ==> stores to this virtual page will fault
 * ppage refers to the physical page for this virtual page (unused if
 * both read_enable and write_enable are 0)
 */
typedef struct {
    unsigned int ppage : 20;		/* bit 0-19 */
    unsigned int read_enable : 1;	/* bit 20 */
    unsigned int write_enable : 1;	/* bit 21 */
} page_table_entry_t;

/*
 * Format of page table.  Entries start at virtual page VM_ARENA_BASEPAGE,
 * i.e., ptes[0] is the page table entry for virtual page VM_ARENA_BASEPAGE.
 */
typedef struct {
    page_table_entry_t ptes[VM_ARENA_SIZE/VM_PAGESIZE];
} page_table_t;

/*
 * MMU's page table base register.  This variable is defined by the
 * infrastructure, but it is controlled completely by the student's pager code.
 */
extern page_table_t *page_table_base_register;

#endif /* _VM_PAGER_H_ */
