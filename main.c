#include <stdlib.h>

#include "processor.h"
#include "display.h"
#include "file.h"

void run()
{
	sp = 0xFF;
	p_reset();

	while (p_run()) {}
	
}

void* worker_thread(void * file_name)
{
	load((char *) file_name);
	run();
}

int main (int argc, char ** argv)
{
	if (argc != 2)
	{
		printf("Usage: %s <binary file>\n", argv[0]);
		exit(-1);
	}

	pthread_t thread;
	pthread_create(&thread, NULL, worker_thread, argv[1]);

	display_init(argv[0]);
}

