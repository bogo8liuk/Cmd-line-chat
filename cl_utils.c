#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <cl_utils.h>

struct cl_init_struct *cl_init(char *const address, char *const port, int domain, int type, int protocol) {
	struct cl_init_struct *init = (struct cl_init_struct *) malloc(sizeof(struct cl_init_struct));
	struct sockaddr_in entry;

	if (inet_pton(domain, address, &entry.sin_addr) != 1) {
		bad_exit("Address conversion error\n");
	}

	if (!port_conversion(port, (uint16_t *) &entry.sin_port)) {
		bad_exit("Port conversion error\n");
	}

	entry.sin_family = AF_INET;

	init->entry = entry;
	init->domain = domain;
	init->type = type;
	init->protocol = protocol;

	return init;
}

bool port_conversion(char *const port, uint16_t *res) {
	uintmax_t value;
	char *end;
	errno = 0;
	value = strtoumax(port, &end, 10);
	if (ERANGE == errno || UINT16_MAX < value)
		return false;
	else {
		*res = htons((uint16_t) value);
		return true;
	}
}
