#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#define BUFSIZE 8

int main(int argc, char* argv[]) {
	int rxpid = atoi(argv[1]);
	union {
		char buf[BUFSIZE];
		void* ptr;
	} convert;

	memcpy(convert.buf, argv[2], BUFSIZE);
	union sigval sv;
	sv.sival_ptr = convert.ptr;
	sigqueue(rxpid, SIGUSR1, sv);
}
