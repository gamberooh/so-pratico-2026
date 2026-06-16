#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define BUFSIZE 8

volatile sig_atomic_t ack = 0;

void ack_handler(int signo, siginfo_t* info, void* context) {
	ack = 1;
}

int main(int argc, char* argv[]) {

	struct sigaction sa = {
		.sa_sigaction = ack_handler,
		.sa_flags = SA_SIGINFO
	};

	int rxpid = atoi(argv[1]);

	union {
		char buf[BUFSIZE];
		void* ptr;
	} convert;

	sigaction(SIGUSR2, &sa, NULL);		
		
	union sigval sv;
	int chunk_len;
	char* iter = argv[2];

	do {
		memset(convert.buf, 0, BUFSIZE);
			
		chunk_len = (BUFSIZE == strnlen(iter, BUFSIZE)) ? BUFSIZE : strnlen(iter, BUFSIZE);
		memcpy(convert.buf, iter, chunk_len);

		sv.sival_ptr = convert.ptr;
		ack = 0;
		sigqueue(rxpid, SIGUSR1, sv);
		
		while(!ack)
			pause();
		iter += chunk_len; //iter trasla ai prossimi 8 byte da scrivere

	} while(chunk_len == BUFSIZE || *iter != '\0');

	printf("Trasmissone ternminata\n");
	return 0;
}
