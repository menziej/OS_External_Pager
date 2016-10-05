#include <iostream>
#include "vm_app.h"
#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>
using namespace std;

int main(void)
{
	char *a = (char *) vm_extend(0);
    vm_extend(2);
	vm_extend(1);
    char *b = (char *) vm_extend(2);
	char *c = (char *) vm_extend(1);
	for(int i=0; i<4096*3; i++){
		a[i] = 'a' + (i*7)%26;
	}

	vm_syslog(a, 0);
	vm_syslog(a, 4096*1+1);
	vm_syslog(a, 4096*2+1);
	vm_syslog(a, 4096*3+1);
	vm_syslog(a, 4096*4+1);
	vm_syslog(b-10, 0);
	vm_syslog(b-10, 4096*1+1);
	vm_syslog(b, 4096*2+1);
	vm_syslog(b, 4096*3+1);
	vm_syslog(b, 4096*4+1);
	vm_syslog(c+10, 10);
	vm_syslog(c+100, 10);
	vm_syslog(c+4095, 1);
	vm_syslog(c+4097, 10);
	vm_syslog(c+4098, 10);
	vm_syslog(c+4099, 10000);

}
