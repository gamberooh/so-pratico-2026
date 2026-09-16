#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

#define BUFSIZE 9 // mex_size + str_terminator 

void handler(int signo, siginfo_t* info, void* context) {
	union {
		char buf[BUFSIZE];
		void* ptr;
	} convert ;

	memset(convert.buf, 0, BUFSIZE);
	convert.ptr = info->si_value.sival_ptr;
	printf("sig: %d\n", signo);
	printf("mex %s\n", convert.buf);
}

int main(int argc, char* argv[]) {

	struct sigaction sa = {
		.sa_sigaction = handler,
		.sa_flags = SA_SIGINFO
	};

	printf("pid: %d\n", getpid());
	
	sigaction(SIGUSR1, &sa, NULL);

	for(int i = 0;; i++) 
		pause();

	return 0;
}

