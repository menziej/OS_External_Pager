#include <iostream>
#include "vm_app.h"

using namespace std;

int main()
{
    char *p;
    p = (char *) vm_extend(0);
   	while(1){
		if(vm_extend(0) == nullptr){
			vm_extend(0);
			vm_extend(0);
			break;
		}
    }    
    p[0] = 'h';
    p[1] = 'e';
    p[2] = 'l';
    p[3] = 'l';
    p[4] = 'o';
    vm_syslog(p, 5);
}
