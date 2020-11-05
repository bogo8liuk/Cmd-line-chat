#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

#define bad_exit(...)				fprintf(stderr, __VA_ARGS__);	\
									exit(EXIT_FAILURE)

extern bool port_conversion(char *port, uint16_t *res);
