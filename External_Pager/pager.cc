#include "vm_pager.h"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <map>
#include <string>
#include <deque>
#include <vector>

using namespace std;


struct page_table_status_entry_t {
	bool resident = false;
	bool dirty = false;
	bool reference = false;
	bool ondisk = false;
	int share_id = 0;
	int disk_block_n;
};

struct page_table_status_t {
	page_table_status_entry_t pts[VM_ARENA_SIZE/VM_PAGESIZE];
};

class process_t {
public:
	pid_t pid;
	page_table_t page_table;
	page_table_status_t page_table_status;
	int n_of_pages = 0;

	process_t(){

	}

	void set_entry(int vpn, page_table_entry_t page_table_entry){
		this->page_table.ptes[vpn] = page_table_entry;
	}

	page_table_entry_t get_entry(int vpn){
		return this->page_table.ptes[vpn];
	}

	void set_status_entry(int vpn, page_table_status_entry_t page_table_status_entry){
		this->page_table_status.pts[vpn] = page_table_status_entry;
	}

	page_table_status_entry_t get_status_entry(int vpn){
		return this->page_table_status.pts[vpn];
	}

	void set_resident(int vpn, bool resident){
		page_table_status.pts[vpn].resident = resident;
	}

	bool get_resident(int vpn){
		return page_table_status.pts[vpn].resident;
	}

	void set_dirty(int vpn, bool dirty){
		page_table_status.pts[vpn].dirty = dirty;
	}

	bool get_dirty(int vpn){
		return page_table_status.pts[vpn].dirty;
	}

	void set_reference(int vpn, bool reference){
		page_table_status.pts[vpn].reference = reference;
	}

	bool get_reference(int vpn){
		return page_table_status.pts[vpn].reference;
	}

	void set_ondisk(int vpn, bool ondisk){
		page_table_status.pts[vpn].ondisk = ondisk;
	}

	bool get_ondisk(int vpn){
		return page_table_status.pts[vpn].ondisk;
	}

	void set_share_id(int vpn, int share_id){
		page_table_status.pts[vpn].share_id = share_id;
	}

	int get_share_id(int vpn){
		return page_table_status.pts[vpn].share_id;
	}

	void set_disk_block_n(int vpn, int disk_block_n){
		page_table_status.pts[vpn].disk_block_n = disk_block_n;
	}

	int get_disk_block_n(int vpn){
		return page_table_status.pts[vpn].disk_block_n;
	}

	void set_read_enable(int vpn, bool read_enable){
		page_table.ptes[vpn].read_enable = read_enable;
	}

	bool get_read_enable(int vpn){
		return page_table.ptes[vpn].read_enable;
	}

	void set_write_enable(int vpn, bool write_enable){
		page_table.ptes[vpn].write_enable = write_enable;
	}

	bool get_write_enable(int vpn){
		return page_table.ptes[vpn].write_enable;
	}

	void set_ppage(int vpn, int ppage){
		page_table.ptes[vpn].ppage = ppage;
	}

	int get_ppage(int vpn){
		return page_table.ptes[vpn].ppage;
	}

};

class page_entry {
public:
	process_t* process_ptr;
	int vpn;

	page_entry(){

	}

	page_table_entry_t get_entry(){
		return process_ptr->get_entry(vpn);
	}

	page_table_status_entry_t get_status_entry(){
		return process_ptr->get_status_entry(vpn);
	}

	void set_resident(bool resident){
		process_ptr->set_resident(vpn,resident);
	}

	bool get_resident(){
		return process_ptr->get_resident(vpn);
	}

	void set_dirty(bool dirty){
		process_ptr->set_dirty(vpn,dirty);
	}

	bool get_dirty(){
		return process_ptr->get_dirty(vpn);
	}

	void set_reference(bool reference){
		process_ptr->set_reference(vpn,reference);
	}

	bool get_reference(){
		return process_ptr->get_reference(vpn);
	}

	void set_ondisk(bool ondisk){
		process_ptr->set_ondisk(vpn,ondisk);	
	}

	bool get_ondisk(){
		return process_ptr->get_ondisk(vpn);
	}

	void set_share_id(int share_id){
		process_ptr->set_share_id(vpn,share_id);
	}

	int get_share_id(){
		return process_ptr->get_share_id(vpn);
	}

	void set_disk_block_n(int disk_block_n){
		process_ptr->set_disk_block_n(vpn,disk_block_n);
	}

	int get_disk_block_n(){
		return process_ptr->get_disk_block_n(vpn);
	}

	void set_read_enable(bool read_enable){
		process_ptr->set_read_enable(vpn,read_enable);
	}

	bool get_read_enable(){
		return process_ptr->get_read_enable(vpn);
	}

	void set_write_enable(bool write_enable){
		process_ptr->set_write_enable(vpn,write_enable);
	}

	bool get_write_enable(){
		return process_ptr->get_write_enable(vpn);
	}

	void set_ppage(int ppage){
		process_ptr->set_ppage(vpn,ppage);
	}

	int get_ppage(){
		return process_ptr->get_ppage(vpn);
	}

};

map<int, vector<page_entry>> shareid_shareq_map;//map between share id and the share virtual page entries
deque<page_entry> clock_queue;//the clock queue of virtual pages
deque<int> free_ppage_queue;//the physical page available
deque<int> free_disk_block_queue;//the disk blocks available
map<pid_t, process_t*> pid_process_map;//map between pid and process

process_t* current_process;//pointer to current process

void vm_init(unsigned int memory_pages, unsigned int disk_blocks) {

	for (int i=0; i<memory_pages; i++) {
		free_ppage_queue.push_back(i);
	}
	for (int i=0; i<disk_blocks; i++) {
		free_disk_block_queue.push_back(i);
	}
}


void vm_create(pid_t pid) {
	process_t* process_ptr = new process_t;
	process_ptr -> pid = pid;
	for (int i=0; i<VM_ARENA_SIZE/VM_PAGESIZE; i++) {
		process_ptr->set_read_enable(i,false);
		process_ptr->set_write_enable(i,false);
	}
	pid_process_map[pid] = process_ptr;
}

void vm_switch(pid_t pid) {
	auto it = pid_process_map.find(pid);
	if (it == pid_process_map.end()) {
		return;
	} else {
		page_table_base_register = &(it->second->page_table);
		current_process = it->second;
	}
	return;
}

void *vm_extend(unsigned int share_id) {
	
	if (VM_PAGESIZE * current_process->n_of_pages < VM_ARENA_SIZE 
	&& free_disk_block_queue.size() > 0) {
		int VPN = current_process->n_of_pages;
		current_process->n_of_pages ++;
		
		if(share_id > 0){
			auto it = shareid_shareq_map.find(share_id);
			if(it == shareid_shareq_map.end()){
				current_process->set_disk_block_n(VPN,free_disk_block_queue.front());
				free_disk_block_queue.pop_front();
			}else{
				current_process->set_entry(VPN, it->second[0].get_entry());
				current_process->set_status_entry(VPN, it->second[0].get_status_entry());
			}
			page_entry temp_entry;
			temp_entry.process_ptr = current_process;
			temp_entry.vpn = VPN;
			shareid_shareq_map[share_id].push_back(temp_entry);
		}else{
			current_process->set_disk_block_n(VPN,free_disk_block_queue.front());
			free_disk_block_queue.pop_front();
		}
		current_process->set_share_id(VPN,share_id);
		return (void *)(VPN * VM_PAGESIZE + VM_ARENA_BASEADDR);
	} else {
		return nullptr;
	}
}


int vm_fault(void *addr, bool write_flag){

	if ((intptr_t)addr < (intptr_t)VM_ARENA_BASEADDR || 
	(intptr_t)addr >= current_process->n_of_pages * VM_PAGESIZE + (intptr_t)VM_ARENA_BASEADDR) {
		return -1;
	}
	int start_vaddr = (intptr_t) addr;
	int VPN = (start_vaddr - (intptr_t) VM_ARENA_BASEADDR) / VM_PAGESIZE;



	if(!current_process->get_resident(VPN)) {
		int victim_ppage;
		if(free_ppage_queue.size() > 0) {
			victim_ppage = free_ppage_queue.front();
			free_ppage_queue.pop_front();
		} else {
			while(true){
				page_entry temp_entry = clock_queue.front();
				clock_queue.pop_front();
				int temp_shareid = temp_entry.get_share_id();
				if(temp_entry.get_reference()){
					if(temp_shareid > 0){
						for(int i = 0; i < shareid_shareq_map[temp_shareid].size(); i++){
							shareid_shareq_map[temp_shareid][i].set_reference(false);
							shareid_shareq_map[temp_shareid][i].set_read_enable(false);
							shareid_shareq_map[temp_shareid][i].set_write_enable(false);
						}	
					}else{
						temp_entry.set_reference(false);
						temp_entry.set_read_enable(false);
						temp_entry.set_write_enable(false);
					}
					clock_queue.push_back(temp_entry);
				} else {
					if(temp_shareid > 0){
						for(int i = 0; i < shareid_shareq_map[temp_shareid].size(); i++){
							shareid_shareq_map[temp_shareid][i].set_resident(false);
							shareid_shareq_map[temp_shareid][i].set_read_enable(false);
							shareid_shareq_map[temp_shareid][i].set_write_enable(false);
						}
					}else{
						temp_entry.set_resident(false);
						temp_entry.set_read_enable(false);
						temp_entry.set_write_enable(false);
					}
	
					victim_ppage = temp_entry.get_ppage();

					if(temp_entry.get_dirty()){
						if(temp_shareid > 0){
							for(int i = 0; i < shareid_shareq_map[temp_shareid].size(); i++){
								shareid_shareq_map[temp_shareid][i].set_dirty(false);
								shareid_shareq_map[temp_shareid][i].set_ondisk(true);
							}
						}else{
							temp_entry.set_dirty(false);
							temp_entry.set_ondisk(true);
						}
						disk_write(temp_entry.get_disk_block_n(), 
						&(((char*)vm_physmem)[victim_ppage * VM_PAGESIZE]));
					}
					break;
				}
			}
		}

		int temp_shareid = current_process->get_share_id(VPN);

		if(temp_shareid > 0){
			for(int i = 0; i < shareid_shareq_map[temp_shareid].size(); i++){
				shareid_shareq_map[temp_shareid][i].set_ppage(victim_ppage);
			}
		}else{
			current_process->set_ppage(VPN,victim_ppage);
		}

		memset (&(((char*)vm_physmem)[victim_ppage * VM_PAGESIZE]), 0, VM_PAGESIZE);
		
		if(current_process->get_ondisk(VPN)) {
			disk_read(current_process->get_disk_block_n(VPN),
			&(((char*)vm_physmem)[victim_ppage * VM_PAGESIZE]));
		}

		page_entry the_entry;
		the_entry.vpn = VPN;
		the_entry.process_ptr = current_process;
		clock_queue.push_back(the_entry);

		if(temp_shareid > 0){
			for(int i = 0; i < shareid_shareq_map[temp_shareid].size(); i++){
				shareid_shareq_map[temp_shareid][i].set_resident(true);
			}
		}else{
			current_process->set_resident(VPN,true);
		}
	}

	int temp_shareid = current_process->get_share_id(VPN);

	if(temp_shareid > 0){
		for(int i = 0; i < shareid_shareq_map[temp_shareid].size(); i++){
			shareid_shareq_map[temp_shareid][i].set_reference(true);
			shareid_shareq_map[temp_shareid][i].set_read_enable(true);
			if(shareid_shareq_map[temp_shareid][i].get_dirty() || write_flag){
				shareid_shareq_map[temp_shareid][i].set_write_enable(true);
			}
			if(write_flag) {
				shareid_shareq_map[temp_shareid][i].set_dirty(true);
			}
		}
	}else{
		current_process->set_reference(VPN,true);
		current_process->set_read_enable(VPN,true);
		if(current_process->get_dirty(VPN) || write_flag){
			current_process->set_write_enable(VPN,true);
		}
		if(write_flag) {
			current_process->set_dirty(VPN,true);
		}
	}
	return 0;
}

void vm_destroy() {

	for (int i = 0; i < current_process->n_of_pages; i++) {
		int temp_shareid = current_process->get_share_id(i);
		if (temp_shareid == 0){
			if(current_process->get_resident(i)){
				for(int j=0; j < clock_queue.size(); ) {
					page_entry temp_entry = clock_queue.front();
					clock_queue.pop_front();
					if(current_process == temp_entry.process_ptr && temp_entry.vpn == i){
						free_ppage_queue.push_back(current_process->get_ppage(i));
					}else{
						clock_queue.push_back(temp_entry);
						j += 1;
					}
				}
			}
			free_disk_block_queue.push_back(current_process->get_disk_block_n(i));
		} else if (shareid_shareq_map[temp_shareid].size() == 1){
			if(current_process->get_resident(i)){
				for(int j=0; j < clock_queue.size(); ) {
					page_entry temp_entry = clock_queue.front();
					clock_queue.pop_front();
					if(current_process == temp_entry.process_ptr && temp_entry.vpn == i){
						free_ppage_queue.push_back(current_process->get_ppage(i));
					}else{
						clock_queue.push_back(temp_entry);
						j += 1;
					}
				}
			}
			free_disk_block_queue.push_back(current_process->get_disk_block_n(i));
			shareid_shareq_map.erase(temp_shareid);
		} else {
			for(auto it = shareid_shareq_map[temp_shareid].begin();
			it != shareid_shareq_map[temp_shareid].end(); ++it){
				if(it->process_ptr == current_process && it->vpn == i){
					shareid_shareq_map[temp_shareid].erase(it);
					break;
				}
			}
			if(current_process->page_table_status.pts[i].resident){
				for(int j=0; j < clock_queue.size(); j++) {
					page_entry temp_entry = clock_queue.front();
					clock_queue.pop_front();					
					if(current_process == temp_entry.process_ptr && temp_entry.vpn == i){
						temp_entry.process_ptr = shareid_shareq_map[temp_shareid][0].process_ptr;
						temp_entry.vpn = shareid_shareq_map[temp_shareid][0].vpn;
					}
					clock_queue.push_back(temp_entry);
				}
			}
		}
	}
	pid_process_map.erase(current_process->pid);
	delete current_process;
	return;
}


int vm_syslog(void *message, size_t len) {

	if ((intptr_t)message < (intptr_t)VM_ARENA_BASEADDR ||
	(intptr_t)message >= current_process->n_of_pages * VM_PAGESIZE + (intptr_t)VM_ARENA_BASEADDR ||
	(intptr_t)message + len - 1 >= current_process->n_of_pages * VM_PAGESIZE + (intptr_t)VM_ARENA_BASEADDR ||
	len <= 0 ||
	len -1 >= current_process->n_of_pages * VM_PAGESIZE + (intptr_t)VM_ARENA_BASEADDR - (intptr_t)message) {
		return -1;
	}

	int start_vaddr = (intptr_t) message;
	int VPN = (start_vaddr - (intptr_t)VM_ARENA_BASEADDR) / VM_PAGESIZE;
	int offset = start_vaddr & (VM_PAGESIZE - 1);

	string result;

	if(!current_process->get_read_enable(VPN)){
		vm_fault((void*)start_vaddr, false);
	}

	int start_paddr = (current_process->get_ppage(VPN) * VM_PAGESIZE) + offset;

	if(len < VM_PAGESIZE - offset) {
		result.append(&(((char*)vm_physmem)[start_paddr]), len);
	} else {
		result.append(&(((char*)vm_physmem)[start_paddr]), VM_PAGESIZE - offset);
		len -= VM_PAGESIZE - offset;
		while(len > VM_PAGESIZE) {
			VPN += 1;
			start_vaddr += VM_PAGESIZE;
			if(!current_process->get_read_enable(VPN)){
				vm_fault((void*)start_vaddr, false);
			}
			start_paddr = (current_process->get_ppage(VPN) * VM_PAGESIZE);
			result.append(&(((char*)vm_physmem)[start_paddr]), VM_PAGESIZE);
			len -= VM_PAGESIZE;
		}
		VPN += 1;
		start_vaddr += VM_PAGESIZE;
		if(!current_process->get_read_enable(VPN)){
			vm_fault((void*)start_vaddr, false);
		}
		start_paddr = (current_process->get_ppage(VPN) * VM_PAGESIZE);
		result.append(&(((char*)vm_physmem)[start_paddr]), len);
	}
	cout << "syslog\t\t\t";
	cout << result << endl;
	return 0;
}


