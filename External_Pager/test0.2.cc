#include <iostream>
#include "vm_app.h"
#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>
using namespace std;

int main(void)
{
	char *a = (char *) vm_extend(0);
	char *c = (char *) vm_extend(1);
    char *d = (char *) vm_extend(1);
	char *e;
	for(int i=0; i<10; i++){
		a[i] = 'a' + (i*7)%26;
		vm_syslog(a, 20);
		c[i] = 'a'+(a[i]*2)%26;
		if(i == 0 || i == 3 || i == 7){
			vm_yield();
		}
		d[i] = 'a'+(c[i]*6)%26;
		a[i] = 'a' + (i*9)%26;
		vm_syslog(c, 4097);
		vm_syslog(d, 20);
		vm_syslog(a, 20);
		if(i == 2 || i == 9){
			cout << " call fork!" << endl;
			fork();
			e = (char *) vm_extend(1);
			vm_extend(0);
			a[i] = 'a' + (i*9)%26;
			vm_syslog(e, 4097);
		}

	}
	vm_syslog(c, 20);
	vm_syslog(d, 20);
	vm_syslog(a, 20);
}
