#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <arpa/inet.h>

#define bad_exit(message, ...)		if (NULL != message) {	\
										fprintf(stderr, message, __VA_ARGS__);	\
									}	\
									exit(EXIT_FAILURE)

static bool port_conversion(char *port, uint16_t *res) {
	uintmax_t value;
	char *end;
	errno = 0;
	value = strtoumax(arg, &end, 10);
	if (ERANGE == errno || UINT16_MAX < value)
		return false;
	else {
		*res = htons((uint16_t) value);
		return true;
	}
}

int main(int argc, char *argv[]) {
	if (3 != argc) {
		bad_exit("Usage: %s ip_address port_number");
	}

	
}
