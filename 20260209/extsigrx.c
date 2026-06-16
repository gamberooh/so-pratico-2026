#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <wait.h>

#define BUFSIZE 8
#define MAXSIZE 10124

static char full_mex[MAXSIZE];
static int current_pos = 0;

void handler(int signo, siginfo_t* info, void* context) {
	union {
		char buf[BUFSIZE];
		void* ptr;
	} convert;

	convert.ptr = info->si_value.sival_ptr;
	memcpy(full_mex + current_pos, convert.buf, BUFSIZE);


	int chunk_size = strnlen(convert.buf, BUFSIZE);
	
	printf("sig: %d\n", signo);
	printf("chunk %s\n", convert.buf);
	
	current_pos += chunk_size;

	if (chunk_size < BUFSIZE) {
		printf("FullMex: %s\n", full_mex);
		memset(full_mex, 0, MAXSIZE);
		current_pos = 0;
	}

	sleep(1);
	kill(info->si_pid, SIGUSR2);
	
}

int main(int argc, char* argv[]) {
	struct sigaction sa = {
		.sa_sigaction = handler,
		.sa_flags = SA_SIGINFO
	};

	printf("pid: %d\n", getpid());
	
	sigaction(SIGUSR1, &sa, NULL);
	
	while(1)
		pause();
	

	return 0;
}

