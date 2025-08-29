#include <stdlib.h>

#include "processor.h"
#include "display.h"
#include "file.h"

bool slow;
void run()
{
	sp = 0xFF;
	p_reset();

	if (slow) while (p_run()) { usleep(50); }
	else while (p_run()) {}
	
}

void* worker_thread(void * file_name)
{
	load((char *) file_name);
	run();
}

int main (int argc, char ** argv)
{
	slow = false;
	if (argc == 3) {
		slow=true;
	}
	else if (argc != 2)
	{
		printf("Usage: %s <binary file> [slow]\n", argv[0]);
		exit(-1);
	}

	pthread_t thread;
	pthread_create(&thread, NULL, worker_thread, argv[1]);

	display_init(argv[0]);
}

