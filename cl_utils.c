#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <cl_utils.h>

bool port_conversion(const char *port, uint16_t *res) {
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
