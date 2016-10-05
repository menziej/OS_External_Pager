#include <iostream>
#include "vm_app.h"
#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>
#include <vector>

using namespace std;

int main()
{
    vector <char*> v0(2);
    vector <char*> v1(0);
    vector <char*> v2(1);
    vector <char*> v3(2);

    for(int i=0;i<8;i++){
        v0[i] = (char *) vm_extend(0);
        v0[i][0] = 'w';
        v0[i][1] = 't';
        v0[i][2] = 'f';
    }
	vm_syslog(v0[2],100);
	vm_syslog(v0[1],100);
	vm_syslog(v2[0],100);
	vm_syslog(v2[2],100);
	fork();
    for(int i=0;i<7;i++){
        v1[i] = (char *) vm_extend(2);
        v1[i][0] = 't';
        v1[i][1] = 'w';
        v1[i][2] = 'e';
    }
	vm_syslog(v0[2],100);
	vm_syslog(v0[1],100);
	vm_syslog(v2[0],100);
	vm_syslog(v2[2],100);
    for(int i=0;i<6;i++){
        v2[i] = (char *) vm_extend(1);
        v2[i][0] = 't';
        v2[i][1] = 'y';
        v2[i][2] = 'u';
    }
    for(int i=0;i<9;i++){
        v3[i] = (char *) vm_extend(1);
        v2[i][0] = 'i';
        v2[i][1] = 'o';
        v2[i][2] = 'p';
    }
	vm_syslog(v0[2],100);
	vm_syslog(v0[1],100);
	vm_syslog(v2[0],100);
	vm_syslog(v2[2],100);
}
