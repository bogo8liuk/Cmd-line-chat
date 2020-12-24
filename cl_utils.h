#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <sys/socket.h>

#define NO_PROTOCOL	0

#define bad_exit(...)				fprintf(stderr, __VA_ARGS__);	\
									exit(EXIT_FAILURE)

struct cl_init_struct {
	struct sockaddr_in entry;
	int domain;
	int type;
	int protocol;
};

extern struct cl_init_struct *cl_init(char *const address, char *const port, int domain, int type, int protocol);

extern bool port_conversion(char *const port, uint16_t *res);
