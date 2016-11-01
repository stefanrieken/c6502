#include <stdlib.h>

#include "display.h"
#include "file.h"

void* worker_thread(void * app_name)
{
	load((char *) app_name);
}

int main (int argc, char ** argv)
{
	if (argc != 2)
	{
		printf("Usage: %s <binary file>\n", argv[0]);
		exit(-1);
	}

	pthread_t thread;
	pthread_create(&thread, NULL, worker_thread, argv[0]);

	display_init(argv[0]);
}

