#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>

#define bad_exit(...)				fprintf(stderr, __VA_ARGS__);	\
									exit(EXIT_FAILURE)

static bool port_conversion(const char *port, uint16_t *res) {
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

static int connect_request(const struct sockaddr *addr, int domain, int type, int protocol) {
	int sockfd;

	while (true) {
		if ((sockfd = socket(domain, type, protocol)) < 0) {
			bad_exit("Socket creation error%s", strerror(errno));
		}

		if (connect(sockfd, addr, sizeof(struct sockaddr)) == 0)
			return sockfd;

		close(sockfd);
	}

	return(-1);
}

int main(int argc, char *argv[]) {
	if (3 != argc) {
		bad_exit("Usage: %s ip_address port_number", argv[0]);
	}

	struct sockaddr_in entry;
	int sockfd;
	int domain = AF_INET;
	int type = SOCK_STREAM;
	int protocol = 0;

	if (inet_pton(AF_INET, argv[1], &entry.sin_addr) != 1) {
		bad_exit("Address conversion error");
	}

	if (!port_conversion(argv[2], (uint16_t *) &entry.sin_port)) {
		bad_exit("Port conversion error");
	}

	entry.sin_family = AF_INET;

	sockfd = connect_request((struct sockaddr *) &entry, domain, type, protocol);
}
